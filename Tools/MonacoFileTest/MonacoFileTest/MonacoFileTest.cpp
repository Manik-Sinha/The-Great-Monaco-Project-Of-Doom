// MonacoFileTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <zlib.h>
#include <vector>
#include <assert.h>
#include "lodepng.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 2048

int main()
{
#if 0
    std::cout << "Loading File ...\n";

    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;

    unsigned error = 
        lodepng::decode(
            image, 
            width, 
            height, 
            "C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\Prison_Break.png");

    std::cout << "Loaded File!\n";
    std::cout << "Gathering Steno Bits ...\n";

    // While not as efficient as constructing the bits in-place, it's definitly more sane
    std::vector<bool> bits; 
    //
    for (size_t i = 0; i + 3< image.size(); i +=4)
    {
        bits.push_back((image[i + 0] & 1) != 0);
        bits.push_back((image[i + 1] & 1) != 0);
        bits.push_back((image[i + 2] & 1) != 0);
    }

    std::cout << "Gathered!\n";
    std::cout << "Reconstructing ...\n";

    std::vector<char> reconstr;
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

    std::cout << "Reconstructed!\n";
    std::cout << "Saving ... \n";

    std::ofstream fout("C:\\Users\\Reavenk\\Desktop\\Out.dat", std::ios::out | std::ios::binary);
    fout.write((char*)&reconstr[0], reconstr.size());
    fout.close();

    std::cout << "Saved!\n";
#elif 0
    //const std::string inputFile = "C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\The_Prison_Break.lvl";
    const std::string inputFile = "E:\\SteamLibrary\\steamapps\\common\\Monaco\\Mag\\Content_win32.mdf";

    std::ifstream infile(inputFile, std::ios_base::binary);

    std::vector<char> buffer = std::vector<char>(std::istreambuf_iterator<char>(infile),
        std::istreambuf_iterator<char>());

    
    std::cout << buffer.size();

    //SET_BINARY_MODE(stdin);
    //SET_BINARY_MODE(stdout);

    // https://zlib.net/zlib_how.html
    int ret, flush;
    unsigned have;

    /* allocate deflate state */
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        return ret;

    int writePos = 0;
    /* compress until end of file */
    strm.avail_in = 413;// buffer.size();
    strm.next_in = (Bytef*)&buffer[0];

    Bytef out[CHUNK];
    std::vector<char> fileContents;

    /* run deflate() on input until output buffer not full, finish
    compression if all of source has been read in */
    do 
    {
        strm.avail_out = CHUNK;
        strm.next_out = out;
        ret = deflate(&strm, Z_FINISH);

        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

        have = CHUNK - strm.avail_out;
        size_t oldSz = fileContents.size();
        fileContents.resize(oldSz + have);

        memcpy(&fileContents[oldSz], out, have);

    } 
    while (strm.avail_out == 0);

    assert(strm.avail_in == 0);     // All input will be used
    assert(ret == Z_STREAM_END);    // Stream will be complete

    // Clean up and return.
    (void)deflateEnd(&strm);
    //return Z_OK;

    std::ofstream fout("C:\\Users\\Reavenk\\Desktop\\Out2.dat", std::ios::out | std::ios::binary);
    fout.write((char*)&fileContents[0], fileContents.size());
    fout.close();
#else 

    const std::string inputFile = "E:\\SteamLibrary\\steamapps\\common\\Monaco\\Mag\\Content_win32.mdf";

    std::ifstream infile(inputFile, std::ios_base::binary);

    std::vector<char> buffer = std::vector<char>(std::istreambuf_iterator<char>(infile),
        std::istreambuf_iterator<char>());

    int ret;
    unsigned have;
    z_stream strm;
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    std::vector<char> fileContents;

    /* decompress until deflate stream ends or end of file */
    strm.next_in = (Bytef*)&buffer[0];
    strm.avail_in = 413;
    /* run inflate() on input until output buffer not full */
    do 
    {
        strm.avail_out = CHUNK;
        strm.next_out = out;

        ret = inflate(&strm, Z_FINISH);
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     /* and fall through */
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            return ret;
        }

        have = CHUNK - strm.avail_out;
        size_t oldSz = fileContents.size();
        fileContents.resize(oldSz + have);

        memcpy(&fileContents[oldSz], out, have);

    } while (strm.avail_out == 0);

    /* clean up and return */
    (void)inflateEnd(&strm);
    
    std::ofstream fout("C:\\Users\\Reavenk\\Desktop\\Out2.dat", std::ios::out | std::ios::binary);
    fout.write((char*)&fileContents[0], fileContents.size());
    fout.close();

#endif
}