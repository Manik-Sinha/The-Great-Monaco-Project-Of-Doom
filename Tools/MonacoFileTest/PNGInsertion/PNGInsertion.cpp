// PNGInsertion.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include "lodepng.h"

int main()
{
    const std::string lvlFile = "C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\Lyons_Den_Identity.lvl";
    const std::string samplePNG = "C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\EmbedTarget.png";
    const std::string outputPng = "C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\InsertionTest.png";

    //////////////////////////////////////////////////
    //
    //      Load PNG
    //
    //////////////////////////////////////////////////

    unsigned int width, height;
    std::vector<unsigned char> targImg;

    unsigned error =
        lodepng::decode(
            targImg,
            width,
            height,
            samplePNG);

    const int usableBits = width * height * 3;

    //////////////////////////////////////////////////
    //
    //      Load level data
    //
    //////////////////////////////////////////////////

    std::ifstream infile(lvlFile, std::ios_base::binary);

    std::vector<char> levelData =
        std::vector<char>(
            std::istreambuf_iterator<char>(infile),
            std::istreambuf_iterator<char>());

    infile.close();

    const int neededBits = levelData.size() * 12;

    //////////////////////////////////////////////////

    if (usableBits < neededBits)
    {
        std::cerr << "Target image isn't big enough to embed data payload.";
        return 0;
    }

    // To make things easier, we're going to extract the bits and put
    // them in a container.
    std::vector<bool> payloadBits;

    // It is currently unknown what the extra 4 bits per byte representations
    // are fore.

    payloadBits.reserve(neededBits);
    for (size_t i = 0; i < levelData.size(); ++i)
    {
        char curByte = levelData[i];

        // Do the inverse of the encoding from PNGExtraction
        /*00*/ payloadBits.push_back(0);
        /*01*/ payloadBits.push_back((curByte & (1<< 7)) != 0);
        /*02*/ payloadBits.push_back((curByte & (1<< 6)) != 0);
        /*03*/ payloadBits.push_back(0);
        /*04*/ payloadBits.push_back((curByte & (1 << 5)) != 0);
        /*05*/ payloadBits.push_back((curByte & (1 << 4)) != 0);
        /*06*/ payloadBits.push_back(0);
        /*07*/ payloadBits.push_back((curByte & (1 << 3)) != 0);
        /*08*/ payloadBits.push_back((curByte & (1 << 2)) != 0);
        /*09*/ payloadBits.push_back(0);
        /*10*/ payloadBits.push_back((curByte & (1 << 1)) != 0);
        /*11*/ payloadBits.push_back((curByte & (1 << 0)) != 0);
    }

    int pngIdx = 0;
    int dataIdx = 0;
    for (; dataIdx < neededBits; ++pngIdx)
    {
        // Don't mess with the alpha channel
        if (pngIdx % 4 == 3)
            continue;

        if (payloadBits[dataIdx] == true)
            targImg[pngIdx] |= 1;
        else
            targImg[pngIdx] &= ~1;

        ++dataIdx;

    }

    for (; pngIdx < targImg.size(); ++pngIdx)
    {
        // Don't mess with the alpha channel
        if (pngIdx % 4 == 3)
            continue;

        // Zero out the rest of the pixel LSBs.
        // This has been observed in proper Monaco PNGs. It may be required
        // as a form of corruption detection.
        targImg[pngIdx] = targImg[pngIdx] & ~1;
    }

    //////////////////////////////////////////////////
    //
    //      Save Out
    //
    //////////////////////////////////////////////////

    error = lodepng::encode(outputPng, targImg, width, height);

}
