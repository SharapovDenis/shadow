#pragma once

#include <iostream>
#include <cmath>
#include <vector>

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
   * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
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
