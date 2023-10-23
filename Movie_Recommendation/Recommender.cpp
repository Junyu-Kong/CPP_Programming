#include "Recommender.h"
#include "UserDatabase.h"
#include "MovieDatabase.h"
#include "Movie.h"
#include "User.h"
#include "treemm.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

Recommender::Recommender(const UserDatabase& user_database,
                         const MovieDatabase& movie_database)
{
    m_udb = &user_database;
    m_mdb = &movie_database;
}

vector<MovieAndRank> Recommender::recommend_movies(const string& user_email, int movie_count) const
{
    // return an empty vector if user not found
    User* user = m_udb->get_user_from_email(user_email);
    if (user == nullptr)
        return vector<MovieAndRank>();

    // store all the movies the user has watched into a vector
    vector<string> movieIDs = user->get_watch_history();
    vector<Movie*> watchedMovies;
    for (string movieID : movieIDs)
        watchedMovies.push_back(m_mdb->get_movie_from_id(movieID));

    // gather all directors, actors, and genres related to watched movies, duplicates allowed
    vector<string> watched_directors;
    vector<string> watched_actors;
    vector<string> watched_genres;
    for (Movie* movie : watchedMovies)
    {
        const vector<string>& directors = movie->get_directors();
        watched_directors.insert(watched_directors.end(), directors.begin(), directors.end());
        const vector<string>& actors = movie->get_actors();
        watched_actors.insert(watched_actors.end(), actors.begin(), actors.end());
        const vector<string>& genres = movie->get_genres();
        watched_genres.insert(watched_genres.end(), genres.begin(), genres.end());
    }

    vector<int> score(100000, 0);


    // for each watched_director, find directed movies and add their score
    for (string director : watched_directors)
    {
        vector<Movie*> temp = m_mdb->get_movies_with_director(director);
        for (Movie* movie : temp)
        {
            string id = movie->get_id().substr(2, 5);
            int key = stoi(id);
            score[key] += 20;
        }
    }

    // for each watched_actor, find acted movies and add their score
    for (string actor : watched_actors)
    {
        vector<Movie*> temp = m_mdb->get_movies_with_actor(actor);
        for (Movie* movie : temp)
        {
            string id = movie->get_id().substr(2, 5);
            int key = stoi(id);
            score[key] += 30;
        }
    }

    // for each watched_genre, find related movies and add their score
    for (string genre : watched_genres)
    {
        vector<Movie*> temp = m_mdb->get_movies_with_genre(genre);
        for (Movie* movie : temp)
        {
            string id = movie->get_id().substr(2, 5);
            int key = stoi(id);
            score[key] += 1;
        }
    }

    // initialize the MovieAndScore vector
    vector<MovieAndScore> recommandations;
    for (int i = 0; i < 99999; i++)
    {
        if (score[i] > 0)
        {
            string id = "ID";
            if (i < 10)
                id += "0000" + to_string(i);
            else if (i < 100)
                id += "000" + to_string(i);
            else if (i < 1000)
                id += "00" + to_string(i);
            else if (i < 10000)
                id += "0" + to_string(i);
            else
                id += to_string(i);
            recommandations.push_back(MovieAndScore(m_mdb->get_movie_from_id(id), score[i]));
        }
    }

    // remove watched movies
    vector<MovieAndScore>::iterator p = recommandations.begin();
    while (p != recommandations.end())
    {
        for (string mv : movieIDs)
        {
            if (p->m_movie->get_id() == mv)
            {
                p = recommandations.erase(p);
                p--;
                break;
            }
        }
        p++;
    }

    // sort it
    sort(recommandations.begin(), recommandations.end());


    // produce a MovieAndRank vector output
    vector<MovieAndRank> output;
    for (int i = recommandations.size() - 1; i >= 0 && movie_count > 0; i--, movie_count--)
    {
        output.push_back(MovieAndRank(recommandations[i].m_movie->get_id(), recommandations[i].m_score));
    }

    return output;
}
