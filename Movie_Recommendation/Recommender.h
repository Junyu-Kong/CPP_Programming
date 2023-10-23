#ifndef RECOMMENDER_INCLUDED
#define RECOMMENDER_INCLUDED

#include <string>
#include <vector>
#include "Movie.h"

class UserDatabase;
class MovieDatabase;

struct MovieAndRank
{
    MovieAndRank(const std::string& id, int score)
     : movie_id(id), compatibility_score(score)
    {}

    std::string movie_id;
    int compatibility_score;
};

class Recommender
{
  public:
    Recommender(const UserDatabase& user_database,
                const MovieDatabase& movie_database);
    std::vector<MovieAndRank> recommend_movies(const std::string& user_email,
                                               int movie_count) const;

  private:
      const UserDatabase* m_udb;
      const MovieDatabase* m_mdb;

      struct MovieAndScore
      {
          MovieAndScore(Movie* movie, int score)
              : m_movie(movie), m_score(score)
          {}
          bool operator<(const MovieAndScore& MS)
          {
              if (m_score < MS.m_score)
                  return true;
              else if (m_score > MS.m_score)
                  return false;
              else if (m_movie->get_rating() < MS.m_movie->get_rating())
                  return true;
              else if (m_movie->get_rating() > MS.m_movie->get_rating())
                  return false;
              else if (m_movie->get_title() > MS.m_movie->get_title())
                  return true;
              return false;
          }

          Movie* m_movie;
          int m_score;
      };
};

#endif // RECOMMENDER_INCLUDED
