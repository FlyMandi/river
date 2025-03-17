#include "river.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>

bool firstOpen = true;

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

//TODO: look into the cost of this function, maybe there's no need to open/close it on every function call
//just open the file on initGame, close it on gameShutdown
void printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines){
    if(build_DEBUG){
        if(firstOpen){
            remove(debugLog);
            firstOpen = false;
        }

        std::ofstream log(debugLog, std::ios::app);
        if(!log.is_open()){
            throw std::runtime_error("failed to open file");
        }else{
            for(;tabs > 0; --tabs){
                log << '\t'; 
            }

                log << text;

            for(;newlines > 0; --newlines){
                log << '\n';
            }
        }
        log.close();
    }
}
