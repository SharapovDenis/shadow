#pragma once

#include <vector>
#include <map>
#include <string>

typedef struct node {
    unsigned long long int id;
    double lat;
    double lon;
    std::map<std::string, std::string> tag;
} nodes;

typedef struct way {
    unsigned long long int id;
    std::vector<nodes> seq;
    std::map<std::string, std::string> tag;
} ways;
