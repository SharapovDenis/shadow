#pragma once

#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>

#include "solar.hpp"

/**
 * Reads lines from a csv file.
 * @param filename filename
 * @return vector vector@<string>
 */
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

/**
 * Writes a vector of strings in csv format.
 * @param filename filename
 * @param data vector vector@<string>
 */
void write_csv(const std::string &filename, const std::vector<std::vector<std::string>> &data) {
    std::fstream file("/home/maxim/Рабочий стол/code/shadows/shadows_calc/" + filename, std::ios::out);

    unsigned long data_len = data.size();
    unsigned long row_len = data[0].size();

    for (auto i = 0; i < data_len - 1; i++) {
        for (auto j = 0; j < row_len - 1; j++)
            file << data[i][j] << ",";

        file << data[i][row_len - 1] << "\n";
    }

    for (int j = 0; j < row_len - 1; j++)
        file << data[data_len - 1][j] << ",";

    file << data[data_len - 1][row_len - 1];
}

/**
 * Converts doubles to string with the specified precision.
 * @param x double
 * @param n number of digits after the decimal point
 * @return string
 */
std::string to_string_with_precision(double x, const int n = 7) {
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << x;
    return out.str();
}

void junk_2() {
    std::vector<std::vector<std::string>> data = read_csv("output_data.csv");

    std::vector<std::vector<std::string>> new_data = {data[0]};

    double height = 3, elev = 5, azim = 90;
    height = height / (2 * M_PI * 6378 * 1000 / 360);

    for (int i = 1; i < data.size(); i++) {
        auto &row = data[i];
        double new_lat = std::stod(row[3]) + (-height * std::stoi(row[2]) * cos(to_rad(azim)) / tan(to_rad(elev)));
        double new_lon = std::stod(row[4]) - (+height * std::stoi(row[2]) * sin(to_rad(azim)) / tan(to_rad(elev)));

        std::vector<std::string> new_row = {row[0], row[1], row[2], to_string_with_precision(new_lat),
                                            to_string_with_precision(new_lon)};
        new_data.push_back(new_row);
    }

    write_csv("output_data_2.csv", new_data);
}

void junk_3() {
    std::vector<std::vector<std::string>> data = read_csv("output_data.csv");

    std::vector<std::vector<std::string>> new_data = {data[0]};

    double height = 3, elev = 5, azim = 90;
    height = height / (2 * M_PI * 6378 * 1000 / 360);

    for (int i = 1; i < data.size(); i++) {
        auto &row = data[i];
        double new_lat = std::stod(row[3]) + (-height * std::stoi(row[2]) * cos(to_rad(azim)) / tan(to_rad(elev)));
        double new_lon = std::stod(row[4]) - (+height * std::stoi(row[2]) * sin(to_rad(azim)) / tan(to_rad(elev)));

        std::vector<std::string> temp_row;

        temp_row = {std::to_string(2 * (i - 1)), row[1], row[2], row[3], row[4]};
        new_data.push_back(temp_row);

        temp_row = {std::to_string(2 * (i - 1) + 1), row[1], row[2], to_string_with_precision(new_lat),
                    to_string_with_precision(new_lon)};
        new_data.push_back(temp_row);
    }

    write_csv("output_data_3.csv", new_data);
}
