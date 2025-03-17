#include "river.h"

#include <cstring>
#include <fstream>

//TODO: rewrite with recursion, base case is when the current path is the drive root, throw runtime error there
std::filesystem::path getProjectRoot(const char *rootName){
    std::filesystem::path current = std::filesystem::current_path();

    for(int i = 0; i < 3; ++i){
        if(strcmp(current.filename().string().c_str(), rootName) == 0) {
            // FIXME: can't use debugLog here because it's still not defined... what do?
            // printDebugLog("project root: ", debugLog);
            // printDebugLog(current, debugLog);
            return current;
        }else{
            current = current.parent_path();
        }
    }

    throw std::runtime_error("failed to find root folder!");
}

//TODO: flush every time the application is started, prob use a boolean
void printDebugLog(const std::string &text, std::filesystem::path &logFile){
    if(build_DEBUG){
        std::ofstream file(logFile, std::ios::app);

        if(!file.is_open()){
            throw std::runtime_error("failed to open file");
        }else{
            file << text << '\n';
        }

        file.close();
    }
}
