
#include <iostream>
#include <cassert>

#include "loadlevel.cpp"

void case_1() {
    const std::string filename = "no_level.txt";
    const bool result = read_levels_structure(filename);
    assert(result == false);
}

int main() {
    case_1();
    return EXIT_SUCCESS;
}
