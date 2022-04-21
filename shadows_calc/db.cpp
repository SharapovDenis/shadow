#include "db.hpp"

int buildings_receive(const char *db_path,
                      const string &lat_low, const string &lon_left, const string &lat_up, const string &lon_right,
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

    flag = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

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

        flag = sqlite3_prepare_v2(db, query_tags.c_str(), -1, &stmt1, nullptr);

        if (flag != SQLITE_OK) {
            cerr << "failed preparation by tag" << endl;
            return 1;
        }

        while (sqlite3_step(stmt1) != SQLITE_DONE) {
            tag_key = string((char *) sqlite3_column_text(stmt1, 0));
            tag_val = string((char *) sqlite3_column_text(stmt1, 1));
            build[i].tags.insert(pair<string, string>(tag_key, tag_val));
        }

        flag = sqlite3_prepare_v2(db, query_nodes.c_str(), -1, &stmt2, nullptr);

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

    buildings_receive("../db/shadow.db", lat_low, lon_left, lat_up, lon_right, a);
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
