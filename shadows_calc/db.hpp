#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>
#include <cstdio>

using namespace std;

/*
Used functions from sqlite3.h:

1. sqlite3_open() --- open connection with db;

2. sqlite3_prepare_v2() --- transforms sql querry to bin querry;

3. typedef struct sqlite3_stmt sqlite3_stmt;

An instance of this object represents a single SQL statement that has been compiled into binary form and is ready to be evaluated.

Think of each SQL statement as a separate computer program. The original SQL text is source code. A prepared statement object is the compiled object code. All SQL must be converted into a prepared statement before it can be run.

4. sqlite3_step(stmt) --- execute bin querry (step by step)

5. sqlite3_column_type(stmt, i) --- defines i column's type on any step

6. sqlite3_column_text(stmt, i) --- returns text from i col on any step

7. sqlite3_column_int(stmt, i) --- returns int from i col on any step

8. sqlite3_column_double(stmt, i) --- returns double from i col on any step

if the query returns an integer but the sqlite3_column_text() interface is used to extract the value, then an automatic type conversion is performed.
*/

struct node {
    unsigned long long int id{};
    double lat{};
    double lon{};
    map<string, string> tags;

};

struct way {
    unsigned long long int id{};
    vector<node> seq;
    map<string, string> tags;
};


int buildings_receive(const char *db_path,
                      const string &lat_low, const string &lon_left, const string &lat_up, const string &lon_right,
                      vector<way> &build);

void buildings_receive_test();