#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "sqlite3.h"
#include <stdio.h>

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
    unsigned long long int id;
    double lat;
    double lon;
    map<string, string> tags;

};

struct way {
    unsigned long long int id;
    vector<node> seq;
    map<string, string> tags;
};


int buildings_receive(const char *db_path, string lat_low, string lon_left, string lat_up, string lon_right,
                      vector<way> &build) {
    /* LU --- left upper; RL --- right lower; */

    sqlite3 *db;
    sqlite3_stmt *stmt, *stmt1, *stmt2;
    string query, query_tags, query_nodes, pattern_tags, pattern_nodes;
    string between, tag_key, tag_val;
    way mid_way;
    node mid_node;
    unsigned int i, counter = 0;
    int flag;

    between =
            "("
            "lat BETWEEN " + lat_low + " AND " + lat_up + ")" + " AND (lon BETWEEN " + lon_left + " AND " + lon_right +
            ")";

    query = "SELECT way_id "
            "FROM WayBuildings "
            "WHERE tag_key = 'building' AND " + between + " GROUP BY way_id;";

    pattern_tags = "SELECT way_tags.tag_key, way_tags.tag_val "
                   "FROM way_tags "
                   "WHERE way_tags.tag_key = 'building:levels' AND way_tags.way_id = ";

    pattern_nodes = "SELECT nodes.node_id, nodes.lat, nodes.lon "
                    "FROM nodes "
                    "JOIN way_nodes ON way_nodes.node_id = nodes.node_id "
                    "WHERE way_nodes.way_id = ";

    cout << "Openning db " + string(db_path) + ": ";

    flag = sqlite3_open(db_path, &db);

    if (flag != SQLITE_OK) {
        cerr << "failed" << endl;
        return 1;
    }

    cout << "done" << endl;

    flag = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (flag != SQLITE_OK) {
        cerr << "failed preparation sql data to binary" << endl;
        return 1;
    }


    cout << "selecting ways by coordinates: ";

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        mid_way.id = sqlite3_column_int64(stmt, 0);
        build.push_back(mid_way);
        counter++;
    }

    cout << "total " + to_string(counter - 1) << endl;

    sqlite3_finalize(stmt);

    counter = 0;

    cout << "selecting building:levels by way.id: ";

    for (i = 0; i < build.size(); ++i) {
        query_tags = pattern_tags + to_string(build[i].id) + ";";
        query_nodes = pattern_nodes + to_string(build[i].id) + ";";

        flag = sqlite3_prepare_v2(db, query_tags.c_str(), -1, &stmt1, NULL);

        if (flag != SQLITE_OK) {
            cerr << "failed preparation by tag" << endl;
            return 1;
        }

        while (sqlite3_step(stmt1) != SQLITE_DONE) {
            tag_key = string((char *) sqlite3_column_text(stmt1, 0));
            tag_val = string((char *) sqlite3_column_text(stmt1, 1));
            build[i].tags.insert(pair<string, string>(tag_key, tag_val));
        }

        flag = sqlite3_prepare_v2(db, query_nodes.c_str(), -1, &stmt2, NULL);

        if (flag != SQLITE_OK) {
            cerr << "failed preparation by node" << endl;
            return 1;
        }

        while (sqlite3_step(stmt2) != SQLITE_DONE) {
            mid_node.id = sqlite3_column_int64(stmt2, 0);
            mid_node.lat = sqlite3_column_double(stmt2, 1);
            mid_node.lon = sqlite3_column_double(stmt2, 2);
            build[i].seq.push_back(mid_node);
            counter++;
        }

        sqlite3_finalize(stmt1);
        sqlite3_finalize(stmt2);
    }

    cout << "total " + to_string(i - 1) << endl;
    cout << "total nodes " + to_string(counter - 1) << endl;


    sqlite3_close(db);
    return 0;
}

void buildings_receive_test() {
    string lat_low, lon_left, lat_up, lon_right;

    lat_low = "55.7274074";
    lon_left = "37.5765893";
    lat_up = "55.8322596";
    lon_right = "37.7582195";

    vector<way> a;

    buildings_receive("../database/shadow.db", lat_low, lon_left, lat_up, lon_right, a);
    cout.precision(8);
    for (int i = 0; i < a.size(); ++i) {
        for (int j = 0; j < a[i].seq.size(); ++j) {
            cout << "id: " << a[i].seq[j].id << " ";
            cout << "lat: " << a[i].seq[j].lat << " ";
            cout << "lon: " << a[i].seq[j].lon << endl;
        }
        if (i == 1) {
            break;
        }
    }
}
