#include "river.h"

#include <cstring>
#include <iostream>

//TODO: rewrite with recursion, base case is when the current path is the drive root, throw runtime error there
std::filesystem::path getProjectRoot(const char *rootName){
    std::filesystem::path current = std::filesystem::current_path();

    for(int i = 0; i < 3; ++i){
        if(strcmp(current.filename().string().c_str(), rootName) == 0) {
            if(build_DEBUG){ 
                std::cout << "project root: " << current << '\n'; 
            }
            return current;
        }else{
            current = current.parent_path();
        }
    }

    throw std::runtime_error("failed to find root folder!");
}
