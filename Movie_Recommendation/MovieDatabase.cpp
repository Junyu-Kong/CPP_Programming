#include "MovieDatabase.h"
#include "Movie.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

MovieDatabase::MovieDatabase()
{
    ;
}

MovieDatabase::~MovieDatabase()
{
    for (Movie* m : m_movies)
        delete m;
    m_movies.clear();
}

bool MovieDatabase::load(const string& filename)
{
    // check if this file has been loaded
    if (find(m_loadedFiles.begin(), m_loadedFiles.end(), filename) != m_loadedFiles.end())
        return false;
    m_loadedFiles.push_back(filename);

    // check if loading this file is successful
    ifstream infile(filename);
    if (!infile)
        return false;
    string movieID, movieName, movieYear, movieDirectors, movieActors, movieGenres;
    float movieRating;
    vector<string> directors;
    vector<string> actors;
    vector<string> genres;
    while (true)
    {
        getline(infile, movieID);

        // check if at the end of file
        if (infile.eof())
            break;

        // get information from input file
        getline(infile, movieName);
        getline(infile, movieYear);
        getline(infile, movieDirectors);
        getline(infile, movieActors);
        getline(infile, movieGenres);
        infile >> movieRating;
        infile.ignore(10000, '\n');

        // store the information into directors vectors 
        directors.clear();
        while (true)
        {
            int index = movieDirectors.find(',');
            if (index == string::npos)
            {
                directors.push_back(movieDirectors);
                break;
            }
            else
            {
                directors.push_back(movieDirectors.substr(0, index));
                movieDirectors = movieDirectors.substr(index + 1);
            }
        }

        // store the information into actors vectors 
        actors.clear();
        while (true)
        {
            int index = movieActors.find(',');
            if (index == string::npos)
            {
                actors.push_back(movieActors);
                break;
            }
            else
            {
                actors.push_back(movieActors.substr(0, index));
                movieActors = movieActors.substr(index + 1);
            }
        }

        // store the information into genres vectors 
        genres.clear();
        while (true)
        {
            int index = movieGenres.find(',');
            if (index == string::npos)
            {
                genres.push_back(movieGenres);
                break;
            }
            else
            {
                genres.push_back(movieGenres.substr(0, index));
                movieGenres = movieGenres.substr(index + 1);
            }
        }

        Movie* temp = new Movie(movieID, movieName, movieYear, directors, actors, genres, movieRating);
        m_movies.push_back(temp);

        // insert the movie into different maps
        m_IDMap.insert(movieID, temp);
        for (string director : directors)
            m_directorMap.insert(director, temp);
        for (string actor : actors)
            m_actorMap.insert(actor, temp);
        for (string genre : genres)
            m_genreMap.insert(genre, temp);

        // get rid of the empty line after each user's information
        getline(infile, movieID);
    }
    return true;
}

Movie* MovieDatabase::get_movie_from_id(const string& id) const
{
    // make id all upper case becasue want case-insensitive search
    string temp = id;
    for (int i = 0; i < id.size(); i++)
        temp[i] = toupper(id[i]);

    TreeMultimap<string, Movie*>::Iterator it = m_IDMap.find(temp);
    if (!it.is_valid())
        return nullptr;
    return it.get_value();
}

vector<Movie*> MovieDatabase::get_movies_with_director(const string& director) const
{
    vector<Movie*> temp;
    TreeMultimap<string, Movie*>::Iterator it = m_directorMap.find(director);
    if (!it.is_valid())
        return temp;
    
    while (it.is_valid())
    {
        temp.push_back(it.get_value());
        it.advance();
    }
    return temp;
}

vector<Movie*> MovieDatabase::get_movies_with_actor(const string& actor) const
{
    vector<Movie*> temp;
    TreeMultimap<string, Movie*>::Iterator it = m_actorMap.find(actor);
    if (!it.is_valid())
        return temp;

    while (it.is_valid())
    {
        temp.push_back(it.get_value());
        it.advance();
    }
    return temp;
}

vector<Movie*> MovieDatabase::get_movies_with_genre(const string& genre) const
{
    vector<Movie*> temp;
    TreeMultimap<string, Movie*>::Iterator it = m_genreMap.find(genre);
    if (!it.is_valid())
        return temp;

    while (it.is_valid())
    {
        temp.push_back(it.get_value());
        it.advance();
    }
    return temp;
}
