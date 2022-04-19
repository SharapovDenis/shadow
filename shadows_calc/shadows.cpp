#include <iostream>

#include <vector>
#include <map>

#include "csv.hpp"
#include "solar.hpp"
#include "grid.hpp"

#include <chrono>

auto start = std::chrono::high_resolution_clock::now();

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

std::vector<ways> get_ways() {
    auto data = read_csv("output_data.csv");

    way temp_way;
    node temp_node;
    std::vector<ways> ways_arr;

    auto last_id = data[1][1];
    temp_way.id = std::stoi(data[1][1]);
    temp_way.tag["levels"] = data[1][2];
    for (int i = 1; i < data.size(); i++) {

        if (data[i][1] != last_id) {
            ways_arr.push_back(temp_way);

            temp_way = {};
            temp_way.id = std::stoi(data[i][1]);
            temp_way.tag["levels"] = data[i][2];

            last_id = data[i][1];
        }

        temp_node = {};
        temp_node.id = std::stoi(data[i][1]);
        temp_node.lat = std::stod(data[i][3]);
        temp_node.lon = std::stod(data[i][4]);

        temp_way.seq.push_back(temp_node);
    }

    ways_arr.push_back(temp_way);

    return ways_arr;
}

/**
 * y (lat)
 * ^
 * |
 * |
 * |
 * |
 * |
 * | - - - - - - - > x (lon)
 */
void grid_filling(Grid grid, std::vector<ways> ways_arr, double min_lat, double min_lon, double dlat, double dlon,
                  double elev, double azim, double height, int n_lon, int n_lat) {
    for (auto &temp_way: ways_arr) {
        double dlat_shadow = (-height * std::stoi(temp_way.tag["levels"]) * cos(to_rad(azim)) / tan(to_rad(elev)));
        double dlon_shadow = (-height * std::stoi(temp_way.tag["levels"]) * sin(to_rad(azim)) / tan(to_rad(elev)));

        int dx_shadow = (int) round(dlat_shadow / dlat);
        int dy_shadow = (int) round(dlon_shadow / dlon);

        for (int i = 1; i < temp_way.seq.size(); i++) {
            double lat1 = temp_way.seq[i - 1].lat;
            double lon1 = temp_way.seq[i - 1].lon;
            double lat2 = temp_way.seq[i].lat;
            double lon2 = temp_way.seq[i].lon;

            int y1 = (int) ((lat1 - min_lat) / dlat);
            int x1 = (int) ((lon1 - min_lon) / dlon);
            int y2 = (int) ((lat2 - min_lat) / dlat);
            int x2 = (int) ((lon2 - min_lon) / dlon);

            auto marked = grid.plotLine({x1, y1}, {x2, y2});

            for (auto p: marked) {
                int x_new = p.x + dx_shadow;
                x_new = std::max(0, x_new);
                x_new = std::min(x_new, n_lon - 1);
                int y_new = p.y + dy_shadow;
                y_new = std::max(0, y_new);
                y_new = std::min(y_new, n_lat - 1);

                grid.plotLine(p, {x_new, y_new});
            }
        }
    }
}

int main() {
    auto ways_arr = get_ways();

    double min_lat = 90, min_lon = 180;
    double max_lat = -90, max_lon = -180;
    for (auto &temp_way: ways_arr) {
        for (auto &temp_node: temp_way.seq) {
            min_lat = std::min(min_lat, temp_node.lat);
            max_lat = std::max(max_lat, temp_node.lat);
            min_lon = std::min(min_lon, temp_node.lon);
            max_lon = std::max(max_lon, temp_node.lon);
        }
    }

    int n_lat = 35 * 100;
    int n_lon = 214 * 100;
    double alpha = 0.01;

    double dlat = (max_lat - min_lat) / n_lat * (1 + alpha / 100);
    double dlon = (max_lon - min_lon) / n_lon * (1 + alpha / 100);

    double elev = 7, azim = 122;
    double height = 3;
    height = height / (2 * M_PI * 6378 * 1000 / 360);

    Grid grid(n_lon, n_lat);

    grid_filling(grid, ways_arr, min_lat, min_lon, dlat, dlon, elev, azim, height, n_lon, n_lat);

//    grid.print_grid();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    std::cout << duration.count() / (1000 * 1000 * 1000) << "."
              << duration.count() / (1000 * 1000) % 1000 << " "
              << duration.count() / 1000 % 1000 << " "
              << duration.count() % 1000 << "s" << std::endl;
}
