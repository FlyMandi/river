#include "Cnake.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main(){
    engine::Cnake cnake{};

    try {
        cnake.run();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n'; 
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
