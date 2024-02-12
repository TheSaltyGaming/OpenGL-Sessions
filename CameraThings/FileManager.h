#pragma once
#include <string>
#include <vector>

#include "Vertex.h"


class FileManager
{
public:
    std::string readFile(const std::string& filename) ;
    std::vector<Vertex> readPointsFromFile(const std::string& filename);
    std::vector<float> convertPointsToFloats(const std::vector<Vertex>& points, float scale);
    
};
