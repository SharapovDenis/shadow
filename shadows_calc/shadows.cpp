#include "grid.hpp"
#include "bd.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>

#include <fstream>

std::vector<std::vector<std::string>> read_csv(const std::string &filename) {
    std::fstream file("/home/maxim/Рабочий стол/code/shadows/shadows_calc/" + filename, std::ios::in);

    if (not file.is_open())
        std::perror("Can't open file");

    std::vector<std::vector<std::string>> data;
    std::vector<std::string> row;
    std::string line, word, temp;

    while (getline(file, line)) {
        row.clear();

        std::istringstream line_stream(line);

        while (getline(line_stream, word, ','))
            row.push_back(word);

        data.push_back(row);
    }

    return data;
}

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

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    auto ways_arr = get_ways();

    double step = 1;
    Grid grid(ways_arr, step);
    grid.fillIn();
//    grid.print_grid();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    std::cout << duration.count() / (1000 * 1000 * 1000) << "."
              << std::setfill('0') << std::setw(3) << duration.count() / (1000 * 1000) % 1000 << " "
              << std::setfill('0') << std::setw(3) << duration.count() / 1000 % 1000 << " "
              << std::setfill('0') << std::setw(3) << duration.count() % 1000 << "s" << std::endl;
}
