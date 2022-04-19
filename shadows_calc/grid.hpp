#pragma once

#include <iostream>
#include <cmath>
#include <vector>

struct iPnt {
    int x;
    int y;
};

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

class Grid {
private:

    int n_x = 100;
    int n_y = 100;

    std::vector<std::vector<int>> grid;

public:

    Grid();

    Grid(int size_x, int size_y);

    void plot(iPnt p, int value);

    std::vector<iPnt> plotLineLow(iPnt p1, iPnt p2);

    std::vector<iPnt> plotLineHigh(iPnt p1, iPnt p2);

    std::vector<iPnt> plotLine(iPnt p1, iPnt p2);

    void print_grid();
};

Grid::Grid() {
    for (int i = 0; i < n_y; i++) {
        std::vector<int> temp(n_x, 0);
        grid.push_back(temp);
    }
}

Grid::Grid(int size_x, int size_y) : n_x{size_x}, n_y{size_y} {
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

void Grid::print_grid() {
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
