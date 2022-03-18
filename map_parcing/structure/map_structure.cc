#include "json.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

using json = nlohmann::json;
using namespace std;

struct building {
    unsigned int        index;
    unsigned long int   id;
    unsigned int        levels;
    double              latitude;
    double              longitude;
};

void json_to_buildings(const json data, vector<struct building> &buildings) {
    assert(!data.empty());
    assert(data.find("elements") != data.end());
    struct building middle;
    unsigned int i;
    for(i = 0; i < data["elements"].size(); ++i) {
        middle.index = i;
        middle.id =              data["elements"][i].at("id").get<unsigned long int>();
        middle.levels =     stoi(data["elements"][i]["tags"].at("building:levels").get<string>());
        middle.latitude =        data["elements"][i]["bounds"].at("minlat").get<double>();
        middle.longitude =       data["elements"][i]["bounds"].at("minlon").get<double>();
        buildings.push_back(middle);
    }
}

int main() {
    fstream file("output_data.json");
    json data;
    vector<building> build;
    int i;
    file >> data;
    json_to_buildings(data, build);
    for(i = 0; i < build.size(); ++i) {
        printf("%d %ld %d %f %f \n", build[i].index, build[i].id, build[i].levels, build[i].latitude, build[i].longitude);
    }
    file.close();
}
