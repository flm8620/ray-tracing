#include <iostream>

void example_sphere_fog();
void example_fish_tank();
void example_sphere_rgb();
int main(int argc, char **argv) {
    // usage: ./ray <example_name>
    if (argc < 2) {
        std::cerr << "Usage: ./ray <example_name>" << std::endl;
        return 1;
    }

    std::string example_name = argv[1];
    if (example_name == "sphere_fog") {
        example_sphere_fog();
    } else if (example_name == "sphere_rgb") {
        example_sphere_rgb();
    } else if (example_name == "fish_tank") {
        example_fish_tank();
    } else {
        std::cerr << "Unknown example name: " << example_name << std::endl;
        return 1;
    }

    return 0;
}
