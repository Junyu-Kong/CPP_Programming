#include "UserDatabase.h"
#include "User.h"
#include "treemm.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

UserDatabase::UserDatabase()
{
    ;
}

UserDatabase::~UserDatabase()
{
    vector<User*>::iterator p = m_users.begin();
    while (p != m_users.end())
    {
        delete *p;
        p = m_users.erase(p);
    }
}

bool UserDatabase::load(const string& filename)
{
    // check if this file has been loaded
    if (find(m_loadedFiles.begin(), m_loadedFiles.end(), filename) != m_loadedFiles.end())
        return false;
    m_loadedFiles.push_back(filename);

    // check if loading this file is successful
    ifstream infile(filename);
    if (!infile)
        return false;

    string userName, userEmail, movie;
    int movieCount;
    vector<string> movieHistory;
    while (true)
    {
        getline(infile, userName);

        // check if at the end of file
        if (infile.eof())
            break;

        getline(infile, userEmail);

        infile >> movieCount;
        infile.ignore(10000, '\n');

        movieHistory.clear();
        for (int i = 0; i < movieCount; i++)
        {
            getline(infile, movie);
            movieHistory.push_back(movie);
        }

        User* temp = new User(userName, userEmail, movieHistory);
        m_map.insert(userEmail, temp);
        m_users.push_back(temp);

        // get rid of the empty line after each user's information
        getline(infile, userName);
    }
    return true;
}

User* UserDatabase::get_user_from_email(const string& email) const
{
    TreeMultimap<string, User*>::Iterator it = m_map.find(email);
    if (!it.is_valid())
        return nullptr;
    return it.get_value();
}
