#if defined(_MSC_VER)  &&  !defined(_DEBUG)
#include <iostream>
#include <windows.h>
#include <conio.h>

struct KeepWindowOpenUntilDismissed
{
	~KeepWindowOpenUntilDismissed()
	{
		DWORD pids[1];
		if (GetConsoleProcessList(pids, 1) == 1)
		{
			std::cout << "Press any key to close this window . . . ";
			_getch();
		}
	}
} keepWindowOpenUntilDismissed;
#endif

#include "UserDatabase.h"
#include "User.h"
#include "Movie.h"
#include "MovieDatabase.h"
#include <iostream>
#include <string>
#include "treemm.h"
#include "Recommender.h"
#include <assert.h>
using namespace std;



//////////////////////////i/////////////////////////////////////////////////////
//
// You are free to do whatever you want with this file, since you won't
// be turning it in.  Presumably, you will make changes to help you test
// your classes.  For example, you might try to implement only some User
// member functions to start out with, and so replace our main routine with
// one that simply creates a User and verifies that the member functions you
// implemented work correctly.
//
//////////////////////////i/////////////////////////////////////////////////////


  // If your program is having trouble finding these files. replace the
  // string literals with full path names to the files.  Also, for test
  // purposes, you may want to create some small, simple user and movie
  // data files to makde debuggiing easier, so you can replace the string
  // literals with the names of those smaller files.

const string USER_DATAFILE  = "users.txt";
const string MOVIE_DATAFILE = "movies.txt";

void findMatches(const Recommender& r,
	const MovieDatabase& md,
	const string& user_email,
	int num_recommendations) {
	// get up to ten movie recommendations for the user
	vector<MovieAndRank> recommendations =
		r.recommend_movies(user_email, 10);
	if (recommendations.empty())
		cout << "We found no movies to recommend :(.\n";
	else {
		for (int i = 0; i < recommendations.size(); i++) {
			const MovieAndRank& mr = recommendations[i];
			Movie* m = md.get_movie_from_id(mr.movie_id);
			cout << i << ". " << m->get_title() << " ("
				<< m->get_release_year() << ")\n Rating: "
				<< m->get_rating() << "\n Compatibility Score: "
				<< mr.compatibility_score << "\n";
		}
	}
}
int main()
{

	vector<string> history = { "mv1", "mv2" };
	User user = User("joe", "joe@gmail.com", history);
	assert(user.get_full_name() == "joe" && user.get_email() == "joe@gmail.com");
	assert(user.get_watch_history()[1] == "mv2");

	vector<string> director = { "d1", "d2" };
	vector<string> actor = { "a1", "a2" };
	vector<string> genre = { "g1", "g2" };
	Movie movie = Movie("id", "mv", "2023", director, actor, genre, 5);
	assert(movie.get_id() == "id" && movie.get_title() == "mv");
	assert(movie.get_release_year() == "2023" && movie.get_rating() == 5);
	assert(movie.get_directors()[0] == "d1");
	assert(movie.get_actors()[1] == "a2");
	assert(movie.get_genres()[0] == "g1");

	UserDatabase udb;
	udb.load(USER_DATAFILE);
	assert(!udb.load(USER_DATAFILE));
	User* user1 = udb.get_user_from_email("AbFow2483@charter.net");
	assert(user1->get_full_name() == "Abdullah Fowler");
	assert(user1->get_watch_history()[0] == "ID22937");


	MovieDatabase mdb;
	mdb.load(MOVIE_DATAFILE);
	assert(!mdb.load(MOVIE_DATAFILE));
	Movie* mv = mdb.get_movie_from_id("ID25779");
	assert(mv->get_title() == "The Carpet of Horror");
	vector<Movie*> mv1 = mdb.get_movies_with_actor("Joachim Fuchsberger");
	assert(mv1[0]->get_title() == "The Carpet of Horror");
	vector<Movie*> mv2 = mdb.get_movies_with_director("M. Raja");
	assert(mv2[0]->get_title() == "Santosh Subramaniam");
	vector<Movie*> mv3 = mdb.get_movies_with_genre("Animation");
	assert(mv3[0]->get_title == "Monkey Love Experiments");


	Recommender r(udb, mdb);
	findMatches(r, mdb, "RhBonner51535@ymail.com", 5);

}

//int main()
//{
//	MovieDatabase udb;
//	
//	if (!udb.load(MOVIE_DATAFILE))  // In skeleton, load always return false
//	{
//		cout << "Failed to load user data file " << USER_DATAFILE << "!" << endl;
//		return 1;
//	}
//	for (;;)
//	{
//		cout << "Enter movieID (or quit): ";
//		string movieID;
//		getline(cin, movieID);
//		if (movieID == "quit")
//			break;
//		Movie* u = udb.get_movie_from_id(movieID);
//		if (u == nullptr)
//			cout << "No user in the database has that email address." << endl;
//		else
//			cout << "Found " << u->get_id() << endl;
//	}
//	for (;;)
//	{
//		cout << "Enter genre (or quit): ";
//		string director;
//		getline(cin, director);
//		if (director == "quit")
//			return 0;
//		vector<Movie*> v = udb.get_movies_with_genre(director);
//		if (v.size() == 0)
//			cout << "No user in the database has that email address." << endl;
//		else
//		{
//			for (Movie* movie : v)
//				cout << movie->get_id();
//			cout << endl;
//		}
//
//	}
//}

//int main()
//{
//	UserDatabase udb;
//	if (!udb.load(USER_DATAFILE))  // In skeleton, load always return false
//	{
//		cout << "Failed to load user data file " << USER_DATAFILE << "!" << endl;
//		return 1;
//	}
//	for (;;)
//	{
//		cout << "Enter user email address (or quit): ";
//		string email;
//		getline(cin, email);
//		if (email == "quit")
//			return 0;
//		User* u = udb.get_user_from_email(email);
//		if (u == nullptr)
//			cout << "No user in the database has that email address." << endl;
//		else
//			cout << "Found " << u->get_full_name() << endl;
//	}
//}
