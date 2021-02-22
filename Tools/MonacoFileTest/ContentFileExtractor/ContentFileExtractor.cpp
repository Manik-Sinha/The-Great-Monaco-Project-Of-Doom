#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <zlib.h>
#include <assert.h>

#define CHUNK 2048

/// <summary>
/// Application to extract the XML contents of the Content_win32.mdf archive file without use of the Magazine TOC.
/// </summary>
/// <returns></returns>
int main()
{
    std::string ContentMDFPath = 
        "E:\\SteamLibrary\\steamapps\\common\\Monaco\\Mag\\Content_win32.mdf";

    const std::string outDir = "C:\\Users\\Reavenk\\Desktop\\OutDir\\";
    const std::string baseName = "Content_";

    std::ifstream infile(ContentMDFPath, std::ios_base::binary);

    std::vector<char> buffer = 
        std::vector<char>(
            std::istreambuf_iterator<char>(infile),
            std::istreambuf_iterator<char>());

    const unsigned int FileBoundary = 0x1000;

    int idx = 0;
    size_t baseIdx = 0;
    while (baseIdx < buffer.size())
    {
        // Get DEFLATE encoded data

        ////////////////////////////////////////////////////////////////////////////////
        //
        //  GET IMPLIED END OF FILE
        //
        ////////////////////////////////////////////////////////////////////////////////
        // The bounds is determined by finding the first two consecutive zero bytes
        //
        // The current theory is that The location and ends are actually defined in a 
        // table of contents file that we currently don't understand, so we're using
        // implied boundaries ATM. Eventually we'd like to use the formal defined
        // directory.
        //
        // NOTE: When this happens, the Content and World extractors will probably be 
        // merged into a single utility that makes in a MTC and MDF and correctly dumps
        // out its data.
        //

        int end = baseIdx;
        while (true)
        {
            if (buffer[end + 1] == 0 && buffer[end + 2] == 0)
                break;

            ++end;
        }

        // This is broken out instead of chained together or else it mysteriously crashes.
        std::cout << "Implying file starting at ";
        std::cout << baseIdx;
        std::cout << " of size ";
        std::cout << (end - baseIdx) << std::endl;

        std::cout << "File index " << idx << std::endl;

        if (end == baseIdx)
        {
            std::cout << "Empty implied file. Skipping." << idx << std::endl;
            baseIdx += FileBoundary;
            continue;
        }

        ////////////////////////////////////////////////////////////////////////////////
        //
        //  DECOMPRESS THE FILE DATA
        //
        ////////////////////////////////////////////////////////////////////////////////

        std::cout << "Decompressing file with DEFLATE." << std::endl;

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
            continue;

        std::vector<char> fileContents;

        strm.next_in = (Bytef*)&buffer[baseIdx];
        strm.avail_in = end - baseIdx;

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
                //return ret;
                break;
            }

            have = CHUNK - strm.avail_out;
            size_t oldSz = fileContents.size();
            fileContents.resize(oldSz + have);

            if (have == 0)
                break;

            memcpy(&fileContents[oldSz], out, have);

        } 
        while (strm.avail_out == 0);

        /* clean up and return */
        (void)inflateEnd(&strm);

        std::cout << "Finished decompression." << std::endl;

        if (fileContents.size() == 0)
        {
            std::cout << "Empty file detected, skipping " << baseIdx << std::endl;
            baseIdx += FileBoundary;
            continue;
        }

        ////////////////////////////////////////////////////////////////////////////////
        //
        //  SAVE OUT THE SERIALIZED CONTENT
        //
        ////////////////////////////////////////////////////////////////////////////////

        std::string filepath = outDir + baseName + std::to_string(baseIdx) + ".xml";
        std::cout << "Saving File to " << filepath << std::endl;

        std::ofstream fout(filepath, std::ios::out | std::ios::binary);
        fout.write((char*)&fileContents[0], fileContents.size());
        fout.close();

        std::cout << "Saved." << std::endl;

        // Increment
        ++idx;
        baseIdx += FileBoundary;
    }
}
