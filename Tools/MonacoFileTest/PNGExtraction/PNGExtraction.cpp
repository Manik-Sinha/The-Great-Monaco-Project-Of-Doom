// Extract the Monaco level information from PNGs

#include <iostream>
#include <fstream>
#include <vector>
#include "lodepng.h"

int main()
{
    std::cout << "Loading File ...\n";

    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;


    //std::string extractTarget = "C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\InsertionTest.png";
    std::string extractTarget = "C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\Prison_Break.png";

    unsigned error =
        lodepng::decode(
            image,
            width,
            height,
            extractTarget
            //"C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\Prison_Break.png"
        );

    std::cout << "Loaded File!\n";
    std::cout << "Gathering Steno Bits ...\n";

    // While not as efficient as constructing the bits in-place, it's definitly more sane
    std::vector<bool> bits;
    //
    for (size_t i = 0; i + 3 < image.size(); i += 4)
    {
        bits.push_back((image[i + 0] & 1) != 0);
        bits.push_back((image[i + 1] & 1) != 0);
        bits.push_back((image[i + 2] & 1) != 0);
    }

    std::cout << "Gathered!\n";
    std::cout << "Reconstructing ...\n";

    const bool RAW_DATA = false;
    std::vector<char> reconstr;
    std::vector<char> rawExtracted;

    if (RAW_DATA == true)
    {
        for (size_t i = 0; i + 7 < bits.size(); i += 8)
        {
            char c = 0;
            for (int j = 0; j < 8; ++j)
            {
                if (bits[i + j] == true)
                    c |= 1 << j;
            }
            reconstr.push_back(c);
        }
    }
    else
    {
        // Pure data, every 2 bytes is represented in the encoded
        // version with 3 bytes.
        for (size_t i = 0; i + 23 < bits.size(); i += 24)
        {

            char ra = 0;
            char rb = 0;
            char rc = 0;
            if (bits[i +  0] == true) { ra |= 1 << 0; }
            if (bits[i +  1] == true) { ra |= 1 << 1; }
            if (bits[i +  2] == true) { ra |= 1 << 2; }
            if (bits[i +  3] == true) { ra |= 1 << 3; }
            if (bits[i +  4] == true) { ra |= 1 << 4; }
            if (bits[i +  5] == true) { ra |= 1 << 5; }
            if (bits[i +  6] == true) { ra |= 1 << 6; }
            if (bits[i +  7] == true) { ra |= 1 << 7; }

            if (bits[i +  8] == true) { rb |= 1 << 0; }
            if (bits[i +  9] == true) { rb |= 1 << 1; }
            if (bits[i + 10] == true) { rb |= 1 << 2; }
            if (bits[i + 11] == true) { rb |= 1 << 3; }
            if (bits[i + 12] == true) { rb |= 1 << 4; }
            if (bits[i + 13] == true) { rb |= 1 << 5; }
            if (bits[i + 14] == true) { rb |= 1 << 6; }
            if (bits[i + 15] == true) { rb |= 1 << 7; }

            if (bits[i + 16] == true) { rc |= 1 << 0; }
            if (bits[i + 17] == true) { rc |= 1 << 1; }
            if (bits[i + 18] == true) { rc |= 1 << 2; }
            if (bits[i + 19] == true) { rc |= 1 << 3; }
            if (bits[i + 20] == true) { rc |= 1 << 4; }
            if (bits[i + 21] == true) { rc |= 1 << 5; }
            if (bits[i + 22] == true) { rc |= 1 << 6; }
            if (bits[i + 23] == true) { rc |= 1 << 7; }

            rawExtracted.push_back(ra);
            rawExtracted.push_back(rb);
            rawExtracted.push_back(rc);


            char a = 0;
            char b = 0;

            if (bits[i +  1] == true) { a |= 1 << 7; }
            if (bits[i +  2] == true) { a |= 1 << 6; }
            if (bits[i +  4] == true) { a |= 1 << 5; }
            if (bits[i +  5] == true) { a |= 1 << 4; }
            if (bits[i +  7] == true) { a |= 1 << 3; }
            if (bits[i +  8] == true) { a |= 1 << 2;}
            if (bits[i + 10] == true) { a |= 1 << 1;}
            if (bits[i + 11] == true) { a |= 1 << 0;}

            if (bits[i + 13] == true) { b |= 1 << 7;}
            if (bits[i + 14] == true) { b |= 1 << 6;}
            if (bits[i + 16] == true) { b |= 1 << 5;}
            if (bits[i + 17] == true) { b |= 1 << 4;}
            if (bits[i + 19] == true) { b |= 1 << 3;}
            if (bits[i + 20] == true) { b |= 1 << 2;}
            if (bits[i + 22] == true) { b |= 1 << 1;}
            if (bits[i + 23] == true) { b |= 1 << 0;}

            std::cout << "Bit block " << i << std::endl;

            std::cout << "From Data [0] : " << bits[i + 11] << bits[i + 10] << bits[i + 8] << bits[i + 7] << bits[i + 5] << bits[i + 4] << bits[i + 2] << bits[i + 1] << std::endl;
            std::cout << "From Data [1] : " << bits[i + 23] << bits[i + 22] << bits[i + 20] << bits[i + 19] << bits[i + 17] << bits[i + 16] << bits[i + 14] << bits[i + 13] << std::endl;

            if(
                bits[i + 0 ] || bits[i +  3] || bits[i +  6] || bits[i +  9] ||
                bits[i + 12] || bits[i + 15] || bits[i + 18] || bits[i + 21])
            {
                // The extra padding bits seem to always be zero, so this 
                // is never expected to happen - so far it has never been 
                // observed on valid files.

                std::cout << "Suspicious bit placement at bit block " << i << std::endl;

                std::cout << "Filler : " << bits[i + 0 ] << bits[i +  3] << bits[i +  6] << bits[i +  9] << bits[i + 12] << bits[i + 15] << bits[i + 18] << bits[i + 21] << std::endl;
            }

            // Write out the 2 bytes we extracted from the 3 bytes
            reconstr.push_back(a);
            reconstr.push_back(b);
        }
    }

    std::cout << "Reconstructed!\n";
    std::cout << "Saving ... \n";

    std::ofstream fout("C:\\Users\\Reavenk\\Desktop\\Out_InsertionTest.dat", std::ios::out | std::ios::binary);
    fout.write((char*)&reconstr[0], reconstr.size());
    fout.close();
    std::cout << "Saved!\n";

    std::ofstream foutRE("C:\\Users\\Reavenk\\Desktop\\Out_InsertionTestRE.dat", std::ios::out | std::ios::binary);
    foutRE.write((char*)&rawExtracted[0], rawExtracted.size());
    foutRE.close();

}
