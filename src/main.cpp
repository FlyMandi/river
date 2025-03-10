#include "Cnake.h"

#include <cstdlib>
#include <iostream>

int main(){
    try{
        engine::Cnake cnake{};
        cnake.run();
    }catch (const std::exception &e){
        std::cerr << e.what() << '\n'; 
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
