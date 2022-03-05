#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "json.hpp"
#include <stdlib.h>

using json = nlohmann::json;
using namespace std;

int main() {
    fstream file("output_data.json");
    json j;
    file >> j;
    // 4 --- amount of indents
    string a = j.dump(4);
    cout << a << endl;
    file.close();
}
