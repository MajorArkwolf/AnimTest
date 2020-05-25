#pragma once

#include <vector>
#include "vert.hpp"
#include <fstream>
#include <iostream>
#include <time.h>

namespace Example1 {
    void loadHeightMap(std::vector<Verticies>& vertex) {
        std::ifstream myfile(".//images//height128.raw", std::ios::binary);
        if (myfile.is_open()) {
            size_t count = 0;
            while (!myfile.eof()) {
                float temp = {};
                temp = static_cast<float>(myfile.get());
                vertex.at(count).y = temp;
                ++count;
            }
            myfile.close();
        }
        else {
            std::cout << "Unable to load heightmap.\n";
        }
    }

    void upScale(std::vector<Verticies> &vertex, unsigned int scale) {
        for (auto& v : vertex) {
            v.x *= scale;
            v.z *= scale;
        }
    }
}

namespace Example2 {
    void filterPass(float* dataP, int increment, float weight, size_t size) {
        float yprev = *dataP;    // the starting point in the terrain array
        int j = increment; // +1, -1, +size, -size
        float k = weight;
        // loop through either
        // one row from left to right (increment = +1), or
        // one row from right to left (increment = -1), or
        // one column from top to bottom (increment = +size), or
        // one column from bottom to top (increment = -size)
        for (int i = 1; i < size; i++) {
            // yi           = k yi - 1 + (1 - k) xi;
            *(dataP + j) = k * yprev + (1 - k) * (*(dataP + j)); //
            yprev = *(dataP + j);
            j += increment;
        }
    }

    void addFilter(float* heights, float weight, size_t size) {
        int i;
        // erode left to right, starting at the beginning of each row
        for (i = 0; i < size; i++)
            filterPass(&heights[size * i], 1, weight, size);
        // erode right to left, starting at the end of each row
        for (i = 0; i < size; i++)
            filterPass(&heights[size * i + size - 1], -1, weight, size);
        // erode top to bottom, starting at the beginning of each column
        for (i = 0; i < size; i++)
            filterPass(&heights[i], size, weight, size);
        // erode from bottom to top, starting from the end of each column
        for (i = 0; i < size; i++)
            filterPass(&heights[size * (size - 1) + i], -size, weight, size);
    }

    void normaliseTerrain(float* heights, size_t size) {
        float fMin, fMax;
        float fHeight;
        int i;
        fMin = heights[0];
        fMax = heights[0];
        // find the min/max values of the height terrainData
        for (i = 1; i < size * size; i++) {
            if (heights[i] > fMax)
                fMax = heights[i];
            else if (heights[i] < fMin)
                fMin = heights[i];
        }
        // find the range of the altitude
        if (fMax <= fMin)
            return;
        fHeight = fMax - fMin;
        // scale the values to a range of 0-255
        for (i = 0; i < size * size; i++) {
            heights[i] = ((heights[i] - fMin) / fHeight) * 255.0f;
        }
    }

    bool genFaultFormation(std::vector<Verticies>& vertix, int iterations, int hSize, int minHeight,
        int maxHeight, float weight,
        int postSmoothingIterations, bool random) {
        int x1, x2, z1, z2;
        float* heights = nullptr;
        int displacement;
        if (hSize <= 0)
            return false;
        if (random) // create truly random map
            srand(time(NULL));
        // allocate memory for heightfield array
        size_t size = hSize;
        auto arraySize = size * size;
        heights = new float[arraySize]; // allocate memory

        if (heights == nullptr)
            return false;
        // initialise the heightfield array to all zeros
        for (int i = 0; i < arraySize; i++) {
            heights[i] = 0.0;
        }

        // generate heightfield
        for (int j = 0; j < iterations; j++) {
            // calculate reducing displacement value - how much to alter height
            displacement = maxHeight - ((maxHeight - minHeight) * j) / iterations;
            // pick the first point P1(x1, z1) at random from the height map
            x1 = (rand() % size);
            z1 = (rand() % size);
            // pick up the second random point P2(x2, z2) and make sure it is
            // different from the first point
            do {
                x2 = (rand() % size);
                z2 = (rand() % size);
            } while (x2 == x1 && z2 == z1);
            // for each point P(x, z) in the field, calculate the new height values
            for (int z = 0; z < size; z++) {
                for (int x = 0; x < size; x++) {
                    // determine which side of the line P1P2 the point P lies in
                    if (((x - x1) * (z2 - z1) - (x2 - x1) * (z - z1)) > 0) {
                        heights[(z * size) + x] += (float)displacement;
                    }
                }
            }
            addFilter(heights, weight, size);
        }
        for (auto i = 0; i < postSmoothingIterations; i++) {
            addFilter(heights, weight, size);
        }

        // normalise the heightfield
        normaliseTerrain(heights, size);
        // copy the float heightfield to terrainData (in unsign char)

        for (size_t x = 0; x < size; x++) {
            for (size_t z = 0; z < size; z++) {
                size_t place = (x * size) + z;
                vertix.at((x * size) + z).y = ((heights[place]));
            }
        }

        // dispose of the temporary array heights
        delete[] heights;
        return true;
    }
}