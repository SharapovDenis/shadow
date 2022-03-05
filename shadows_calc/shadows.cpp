#include <iostream>
#include <vector>

#include "csv.hpp"
#include "solar.hpp"
#include "triangulation.hpp"
#include "grid.hpp"

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
