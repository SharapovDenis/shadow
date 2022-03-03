#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <utility>
#include <ctime>
#include <vector>
#include <array>

#include "delaunator.hpp"

/**
 * Converts degrees to radians.
 */
double to_rad(double x) {
    return x * M_PI / 180;
}

/**
 * Converts radians to degrees.
 */
double to_deg(double x) {
    return x * 180 / M_PI;
}

/**
 * Calculates the altitude and azimuth of the sun.
 * @param latit [-90, 90]
 * @param longit [-90, 90]
 * @param year current year
 * @param mon [1, 12]
 * @param mday [1, how_many_days_in_month]
 * @param hour [0, 23]
 * @param min [0, 59]
 * @param sec [0, 59]
 * @param time_zone in hours
 * @return elevation and azimuth
 */
std::pair<double, double>
solar_coord(double latit, double longit, int year, int mon, int mday, int hour, int min, int sec, double time_zone) {
    int a = (14 - mon) / 12;
    int y = year + 4800 - a;
    int m = mon + 12 * a - 3;
    int jul_day_n = mday + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    double jul_day =
            jul_day_n + (hour - 12) / (double) 24 + min / (double) 1440 + sec / (double) 86400 - time_zone / 24;
    double jul_cen = (jul_day - 2451545) / 36525;

    double mean_long = fmod(280.46646 + jul_cen * (36000.76983 + jul_cen * 0.0003032), 360);
    double mean_anom = 357.52911 + jul_cen * (35999.05029 - 0.0001537 * jul_cen);
    double eq_of_ctr = sin(to_rad(mean_anom)) * (1.914602 - jul_cen * (0.004817 + 0.000014 * jul_cen)) +
                       sin(to_rad(2 * mean_anom)) * (0.019993 - 0.000101 * jul_cen) +
                       sin(to_rad(3 * mean_anom)) * 0.000289;
    double true_long = mean_long + eq_of_ctr;
    double app_long = true_long - 0.00569 - 0.00478 * sin(to_rad(125.04 - 1934.136 * jul_cen));

    double obliq_mean =
            23 + (26 + ((21.448 - jul_cen * (46.815 + jul_cen * (0.00059 - jul_cen * 0.001813)))) / 60) / 60;
    double obliq_corr = obliq_mean + 0.00256 * cos(to_rad(125.04 - 1934.136 * jul_cen));

    double dec = to_deg(asin(sin(to_rad(obliq_corr)) * sin(to_rad(app_long))));

    double eccent = 0.016708634 - jul_cen * (0.000042037 + 0.0000001267 * jul_cen);
    double var_y = tan(to_rad(obliq_corr / 2)) * tan(to_rad(obliq_corr / 2));
    double eq_of_time = 4 * to_deg(var_y * sin(2 * to_rad(mean_long)) - 2 * eccent * sin(to_rad(mean_anom)) +
                                   4 * eccent * var_y * sin(to_rad(mean_anom)) * cos(2 * to_rad(mean_long)) -
                                   0.5 * var_y * var_y * sin(4 * to_rad(mean_long)) -
                                   1.25 * eccent * eccent * sin(2 * to_rad(mean_anom)));
    double local_time = ((sec / (double) 60 + min) / 60 + hour) / 24;
    double true_solar_time = fmod(local_time * 1440 + eq_of_time + 4 * longit - 60 * time_zone, 1440);
    double hour_angle;
    if (true_solar_time / 4 < 0)
        hour_angle = true_solar_time / 4 + 180;
    else
        hour_angle = true_solar_time / 4 - 180;

    double zenith = to_deg(acos(sin(to_rad(latit)) * sin(to_rad(dec)) +
                                cos(to_rad(latit)) * cos(to_rad(dec)) * cos(to_rad(hour_angle))));
    double elev = 90 - zenith;
    double refr;
    if (elev > 85)
        refr = 0;
    else if (elev > 5)
        refr = 58.1 / tan(to_rad(elev)) - 0.07 / pow(tan(to_rad(elev)), 3) + 0.000086 / pow(tan(to_rad(elev)), 5);
    else if (elev > -0.575)
        refr = 1735 + elev * (-518.2 + elev * (103.4 + elev * (-12.79 + elev * 0.711)));
    else
        refr = 20.772 / tan(to_rad(elev));
    refr /= 3600;
    double elev_corr = elev + refr;

    double azim;
    if (hour_angle > 0)
        azim = fmod(to_deg(acos(((sin(to_rad(latit)) * cos(to_rad(zenith))) - sin(to_rad(dec))) /
                                (cos(to_rad(latit)) * sin(to_rad(zenith))))) + 180, 360);
    else
        azim = fmod(540 - to_deg(acos(((sin(to_rad(latit)) * cos(to_rad(zenith))) - sin(to_rad(dec))) /
                                      (cos(to_rad(latit)) * sin(to_rad(zenith))))), 360);

    return {elev_corr, azim};
}

/**
 * Some test.
 */
void solar_coord_test() {
    double latit = (55.7263 + 55.7816) / 2;
    double longit = (37.6503 + 37.7864) / 2;
    double time_zone = 3;

    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    std::pair<double, double> coord = solar_coord(latit, longit, 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
                                                  ltm->tm_hour, ltm->tm_min, ltm->tm_sec, time_zone);

    printf("Solar elevation = %f \nSolar azimuth = %f\n", coord.first, coord.second);
}

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

/**
 * Less code.
 */
using Grid = std::vector<std::vector<int>>;

/**
 * Integer point.
 */
struct iPoint {
    int x;
    int y;
};

/**
 * Double point.
 */
struct dPoint {
    double x;
    double y;
};

Grid create_grid(int n) {
    Grid grid;
    for (int i = 0; i < n; i++) {
        std::vector<int> temp(n, 0);
        grid.push_back(temp);
    }

    return grid;
}

/**
 * Fills a triangle whose bottom side is perfectly horizontal.
 * Precondition is that v2 and v3 perform the flat side and that v1.y @< v2.y, v3.y.
 * @param grid grid
 * @param v1 first vertex, has the smallest y-coordinate
 * @param v2 second vertex
 * @param v3 third vertex
 */
void fill_bottom_flat_triangle(Grid &grid, iPoint v1, iPoint v2, iPoint v3) {
    double slope1 = (v2.x - v1.x) / (double) (v2.y - v1.y);
    double slope2 = (v3.x - v1.x) / (double) (v3.y - v1.y);

    double x1 = v1.x;
    double x2 = v1.x + 0.5;

    for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++) {
        for (int scanlineX = (int) fmin(x1, x2); scanlineX <= (int) fmax(x1, x2); scanlineX++)
            grid[scanlineX][scanlineY] = 1;

        x1 += slope1;
        x2 += slope2;

    }
}

/**
   * Fills a triangle whose top side is perfectly horizontal.
   * Precondition is that v1 and v2 perform the flat side and that v3.y > v1.y, v2.y.
   * @param grid grid
   * @param v1 first vertex
   * @param v2 second vertex
   * @param v3 third vertex, has the largest y-coordinate
   */
void fill_top_flat_triangle(Grid &grid, iPoint v1, iPoint v2, iPoint v3) {
    double slope1 = (v3.x - v1.x) / (double) (v3.y - v1.y);
    double slope2 = (v3.x - v2.x) / (double) (v3.y - v2.y);

    double x1 = v3.x;
    double x2 = v3.x + 0.5;

    for (int scanlineY = v3.y; scanlineY >= v1.y; scanlineY--) {
        for (int scanlineX = (int) fmin(x1, x2); scanlineX <= (int) fmax(x1, x2); scanlineX++)
            grid[scanlineX][scanlineY] = 1;

        x1 -= slope1;
        x2 -= slope2;
    }
}

/**
   * Fills a triangle.
   * @param grid grid
   * @param v1 first vertex
   * @param v2 second vertex
   * @param v3 third vertex, has the largest y-coordinate
   */
void fill_triangle(Grid &grid, iPoint v1, iPoint v2, iPoint v3) {

    /* at first sort the three vertices by y-coordinate ascending,
     * so p1 is the topmost vertice */
    if (v2.y < v1.y)
        std::swap(v1, v2);
    if (v3.y < v2.y) {
        std::swap(v2, v3);
        if (v2.y < v1.y)
            std::swap(v1, v2);
    }

    /* here we know that v1.y <= v2.y <= v3.y */
    if (v2.y == v3.y)
        /* check for trivial case of bottom-flat triangle */
        fill_bottom_flat_triangle(grid, v1, v2, v3);

    else if (v1.y == v2.y)
        /* check for trivial case of top-flat triangle */
        fill_top_flat_triangle(grid, v1, v2, v3);

    else {
        /* general case - split the triangle in a top-flat and bottom-flat one */
        iPoint new_v = {(int) (v1.x + ((v2.y - v1.y) / (double) (v3.y - v1.y)) * (v3.x - v1.x)), v2.y};
        fill_bottom_flat_triangle(grid, v1, v2, new_v);
        fill_top_flat_triangle(grid, v2, new_v, v3);
    }
}

/**
 * Does the magic and returns the result of Delaunay triangulation.
 * @param data vector of dPoints = {x, y}
 * @return vector of vertices of triangles in the form of an array of three dPoints
 */
std::vector<std::array<dPoint, 3>> triangulation(const std::vector<dPoint> &data) {
    std::vector<double> coords;

    for (auto &point: data) {
        coords.push_back(point.x);
        coords.push_back(point.y);
    }

    delaunator::Delaunator _tri_data(coords);
    std::vector<std::array<dPoint, 3>> tri_data;

    for (int i = 0; i < _tri_data.triangles.size(); i += 3) {
        std::array<dPoint, 3> temp{};
        temp[0] = {_tri_data.coords[2 * _tri_data.triangles[i]],
                   _tri_data.coords[2 * _tri_data.triangles[i] + 1]};
        temp[1] = {_tri_data.coords[2 * _tri_data.triangles[i + 1]],
                   _tri_data.coords[2 * _tri_data.triangles[i + 1] + 1]};
        temp[2] = {_tri_data.coords[2 * _tri_data.triangles[i + 2]],
                   _tri_data.coords[2 * _tri_data.triangles[i + 2] + 1]};

        tri_data.push_back(temp);
    }

    return tri_data;
}

void print_grid(const Grid &grid) {
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

int main() {
    std::vector<dPoint> coord = {
            {1,    7},
            {3,    1},
            {8,    5},
            {10,   2},
            {13,   11},
            {6,    10},
            {4,    6},
            {2.2,  5.5},
            {4.2,  -0.5},
            {9.2,  3.5},
            {11.2, 0.5},
            {14.2, 9.5},
            {7.2,  8.5},
            {5.2,  4.5}
    };

    auto tri_data = triangulation(coord);
}
