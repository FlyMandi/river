#pragma once

#include <vector>
#include <string>

namespace engine{

class EnginePipeline{
 public:
    EnginePipeline(const std::string& vertFilepath, const std::string& fragFilepath);

 private:
    static std::vector<char> readFile(const std::string &filepath);

    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
};

}  //namespace engine 
    

