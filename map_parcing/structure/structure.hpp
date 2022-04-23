#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>
#include <stdio.h>

#include "dejkstra.hpp"

#define UNDEFINED_PATH  120 // empty string received
#define ABANDONED       1
#define BRIDLEWAY       2
#define BUS_STOP        3
#define CONSTRUCTION    4
#define CORRIDOR        5
#define CYCLEWAY        6
#define ELEVATOR        7
#define FOOTWAY         8
#define LIVING_STREET   9
#define MOTORWAY        10
#define PATH            11
#define PEDESTRIAN      12
#define PLATFORM        13
#define PRIMARY         14
#define PRIMARY_LINK    15
#define PROPOSED        16
#define RACEWAY         17
#define RESIDENTIAL     18
#define REST_AREA       19
#define ROAD            20
#define SECONDARY       21
#define SECONDARY_LINK  22
#define SERVICE         23
#define STEPS           24
#define STREET_LAMP     25
#define TERTIARY        26
#define TERTIARY_LINK   27
#define TRACK           28
#define TRUNK           29
#define TRUNK_LINK      30
#define UNCLASSIFIED    31
#define VIA_FERRATA     32

std::map<std::string, int> price_list = {

    {"abandoned", ABANDONED},
    {"bridleway", BRIDLEWAY},
    {"bus_stop", BUS_STOP},
    {"construction", CONSTRUCTION},
    {"corridor", CORRIDOR},
    {"cycleway", CYCLEWAY},
    {"elevator", ELEVATOR},
    {"footway", FOOTWAY},
    {"living_street", LIVING_STREET},
    {"motorway", MOTORWAY},
    {"path", PATH},
    {"pedestrian", PEDESTRIAN},
    {"platform", PLATFORM},
    {"primary", PRIMARY},
    {"primary_link", PRIMARY_LINK},
    {"proposed", PROPOSED},
    {"raceway", RACEWAY},
    {"residential", RESIDENTIAL},
    {"rest_area", REST_AREA},
    {"road", ROAD},
    {"secondary", SECONDARY},
    {"secondary_link", SECONDARY_LINK},
    {"service", SERVICE},
    {"steps", STEPS},
    {"street_lamp", STREET_LAMP},
    {"tertiary", TERTIARY},
    {"tertiary_link", TERTIARY_LINK},
    {"track", TRACK},
    {"trunk", TRUNK},
    {"trunk_link",TRUNK_LINK},
    {"unclassified", UNCLASSIFIED},
    {"via_ferrata", VIA_FERRATA},
    {"", UNDEFINED_PATH}
};

namespace resp {

    struct node {
        unsigned long long int                  id;
        double                                  lat;
        double                                  lon;
        std::map<std::string, std::string>      tags;
    };

    struct way {
        unsigned long long int                  id;
        std::vector<node>                       seq;
        std::map<std::string, std::string>      tags;
    };

    struct mate {
        unsigned long long int      prev;           // previous node in the array
        unsigned long long int      next;           // next node in the array
        std::string                 path_type;      // way_tag for classifying roads
    };

    int buildings_receive(const char *db_path, std::string lat_low, std::string lon_left, std::string lat_up, std::string lon_right, std::vector<resp::way> &build);

    int neighbours_receive(const char *db_path, std::string node_id, std::vector<resp::mate> &mates);

    int define_fine(std::string path_type);

    int node_coord(const char *db_path, std::string node_id, resp::node &ret);

    void buildings_receive_test();

};

std::vector<graph::weightNode> getadjacencyMatrix(uint64_t Node);
graph::graphNode getNode(uint64_t Node);
void getadjacencyMatrix_test();
void getNode_test();

class DataBase {

};