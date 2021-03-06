#include "grid.hpp"
#include "solar.hpp"
#include "db.hpp"

#include <iostream>
#include <cmath>
#include <thread>

//Grid::Grid() = default;

Grid::Grid(std::vector<way> &ArrOfWays, double gridStep) : waysArr{ArrOfWays}, step{gridStep} {
    min_lat = 90;
    max_lat = -90;
    min_lon = 180;
    max_lon = -180;
    for (auto &temp_way: waysArr) {
        for (auto &temp_node: temp_way.seq) {
            min_lat = std::min(min_lat, temp_node.lat);
            max_lat = std::max(max_lat, temp_node.lat);
            min_lon = std::min(min_lon, temp_node.lon);
            max_lon = std::max(max_lon, temp_node.lon);
        }
    }

    double dlat_meters = (max_lat - min_lat) * (EarthPerimeter / 360);
    double dlon_meters = (max_lon - min_lon) * (EarthPerimeter / 360) * cos(to_rad(max_lat));

    n_lat = (int) ceil(dlat_meters / step);
    n_lon = (int) ceil(dlon_meters / step);
    n_x = n_lon;
    n_y = n_lat;

    dlat = (max_lat - min_lat) / n_lat;
    dlon = (max_lon - min_lon) / n_lon;

    for (int i = 0; i < n_y; i++) {
        std::vector<int> temp(n_x, 0);
        grid.push_back(temp);
    }
}

void Grid::plot(iPnt p, int value) {
    grid[n_y - 1 - p.y][p.x] = value;
}

std::vector<iPnt> Grid::plotLineLow(iPnt p1, iPnt p2) {
    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);
    int error = 0;
    int slope = (dy + 1);

    int slope_sign;
    if (p2.y - p1.y >= 0)
        slope_sign = 1;
    else
        slope_sign = -1;

    std::vector<iPnt> marked;

    int y = p1.y;
    for (int x = p1.x; x <= p2.x; x++) {
        plot({x, y}, 1);
        marked.push_back({x, y});

        error += slope;

        if (error >= dx + 1) {
            y += slope_sign;
            error -= dx + 1;
        }
    }

    return marked;
}

std::vector<iPnt> Grid::plotLineHigh(iPnt p1, iPnt p2) {
    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);
    int error = 0;
    int slope = (dx + 1);

    int slope_sign;
    if (p2.x - p1.x >= 0)
        slope_sign = 1;
    else
        slope_sign = -1;

    std::vector<iPnt> marked;

    int x = p1.x;
    for (int y = p1.y; y <= p2.y; y++) {
        plot({x, y}, 1);
        marked.push_back({x, y});

        error += slope;

        if (error >= dy + 1) {
            x += slope_sign;
            error -= dy + 1;
        }
    }

    return marked;
}

std::vector<iPnt> Grid::plotLine(iPnt p1, iPnt p2) {
    if (p1.x == p2.x) {
        std::vector<iPnt> marked;

        int x = p1.x;
        for (int y = std::min(p1.y, p2.y); y <= std::max(p1.y, p2.y); y++) {
            plot({x, y}, 1);
            marked.push_back({x, y});
        }

        return marked;
    }

    if (abs(p1.x - p2.x) >= abs(p1.y - p2.y)) {
        if (p1.x > p2.x)
            return plotLineLow(p2, p1);

        return plotLineLow(p1, p2);
    }

    if (p1.y > p2.y)
        return plotLineHigh(p2, p1);

    return plotLineHigh(p1, p2);
}

void Grid::_fillIn(int start_ind, int final_ind) {
    for (int j = start_ind; j < final_ind; j++) {
        auto temp_way = waysArr[j];

        double dlat_shadow = (-height / EarthPerimeter * 360 * std::stoi(temp_way.tags["levels"])
                              * cos(to_rad(azim)) / tan(to_rad(elev)));
        double dlon_shadow = (-height / EarthPerimeter * 360 * std::stoi(temp_way.tags["levels"])
                              * sin(to_rad(azim)) / tan(to_rad(elev)));

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

            y1 = std::min(y1, n_y - 1);
            x1 = std::min(x1, n_x - 1);
            y2 = std::min(y2, n_y - 1);
            x2 = std::min(x2, n_x - 1);

            auto marked = plotLine({x1, y1}, {x2, y2});

            for (auto p: marked) {
                int x_new = p.x + dx_shadow;
                x_new = std::max(0, x_new);
                x_new = std::min(x_new, n_lon - 1);
                int y_new = p.y + dy_shadow;
                y_new = std::max(0, y_new);
                y_new = std::min(y_new, n_lat - 1);

                plotLine(p, {x_new, y_new});
            }
        }
    }
}

void Grid::fillIn(int numberOfThreads) {
    if (numberOfThreads == -1) {
        numberOfThreads = (int) std::thread::hardware_concurrency(); // number of processor threads
        if (numberOfThreads == 0)
            numberOfThreads = 8;
    }

    std::vector<std::thread> threads;

    for (int i = 0; i < numberOfThreads; i++) {
        int start_ind = i * (int) waysArr.size() / numberOfThreads;
        int final_ind = (i + 1) * (int) waysArr.size() / numberOfThreads;

        if (i == numberOfThreads - 1)
            final_ind = (int) waysArr.size();

        threads.emplace_back(&Grid::_fillIn, this, start_ind, final_ind);
    }

    for (auto &t: threads) {
        t.join();
    }
}

[[maybe_unused]] void Grid::print_grid() {
    for (auto &row: grid) {
        for (auto &cell: row) {
            if (cell == 0)
                std::cout << ".";
            else
                std::cout << "@";
        }
        std::cout << std::endl;
    }
}
