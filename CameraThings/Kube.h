#pragma once
#include <string>
#include <vector>
#include <glm/fwd.hpp>

struct vertex {
    float x, y, z, r, g, b;
};

class Kube
{
public:

    Kube(float size)
    {
        a = size;
        vertex v0 = { -a, -a, a, 1, 0, 0 };
        vertex v1 = { a, -a, a, 1, 0, 0 };
        vertex v2 = { a, a, a, 1, 0, 0 };
        vertex v3 = { -a, a, a, 1, 0, 0 };
        mVertices.push_back(v0);
        mVertices.push_back(v1);
        mVertices.push_back(v3);
        mVertices.push_back(v3);
        mVertices.push_back(v1);
        mVertices.push_back(v2);
    }
    std::vector<vertex> mVertices;
    // glm::mat4<float> matrix;
    // glm::vec3<float> position;
    // std::string Name;

private:
    float a{1.0f};

    float radius = a * sqrt(3);

    // void test(obj mia)
    // {
    //     for (statiske objekter)
    //     {
    //         auto obj = 1;//objext container
    //         float d = ; //calculate distance
    //         if (mia.radius + obj.radius > d)
    //         {
    //             //collision happened
    //         }
    //     }
    // }
};
