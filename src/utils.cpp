#include "river.h"
#include "engine.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>


//TODO: rewrite with recursion, base case is when the current path is the drive root, throw runtime error there
void river::getProjectRoot(const char *rootName){
    using namespace engine;

    std::filesystem::path current = std::filesystem::current_path();

    for(int i = 0; i < 3; ++i){
        if(strcmp(current.filename().string().c_str(), rootName) == 0) {
            appRoot = current;
            debugLog = current / "debug.log";
            printDebugLog("project root:", 0, 0);
            printDebugLog(current.string(), 1, 1);
            return;
        }else{
            current = current.parent_path();
        }
    }
}

bool firstOpen = true;

//TODO: look into the cost of this function, maybe there's no need to open/close it on every function call
void engine::printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines){
    if(build_DEBUG){
        if(firstOpen){
            remove(debugLog);
            firstOpen = false;
        }

        std::ofstream log(debugLog, std::ios::app);
        if(!log.is_open()){
            printDebugLog("\nERROR: failed to open file", 2, 1);
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
