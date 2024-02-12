#include "FileManager.h"

#include <fstream>
#include <iostream>
#include <sstream>




/// \brief reads the content of a file and returns a stringstream.
/// \param filename the exact filename
/// \return the content of the file as stringstream
std::string FileManager::readFile(const std::string& filename)
{
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/// \brief Reads points from a file and returns a vector of vertices
/// \param filename name of the file
/// \return vector of vertices
std::vector<Vertex> FileManager::readPointsFromFile(const std::string& filename)
{
    std::vector<Vertex> points;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cout << "Unable to open file: " << filename << std::endl;
        return points;
    }

    // Skip the first line
    std::getline(file, line);

    while (std::getline(file, line)) {
        Vertex point;
        // Assuming the Point struct has members x, y, z, r, g, b
        int ret = sscanf_s(line.c_str(), "X: %f, Y: %f, Z: %f, r: %f, g: %f, b: %f", &point.x, &point.y, &point.z, &point.r, &point.g, &point.b);
        if (ret == 6) { // if all six values are successfully read
            points.push_back(point);
        } else {
            std::cout << "Failed to read line: " << line << std::endl;
        }
    }

    file.close();
    return points;
}

/// \brief Converts a vector of points to a vector of floats
/// \param points list of verftices
/// \param scale 
/// \return vector of floats
std::vector<float> FileManager::convertPointsToFloats(const std::vector<Vertex>& points, float scale)
{
    std::vector<float> floats;
    for (const auto& point : points) {
        floats.push_back(point.x*scale);
        floats.push_back(point.y*scale);
        floats.push_back(point.z*scale);
        floats.push_back(point.r);
        floats.push_back(point.g);
        floats.push_back(point.b);
    }
    return floats;
}
