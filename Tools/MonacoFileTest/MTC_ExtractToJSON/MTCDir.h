#pragma once
#include <vector>
#include <string>

struct MTCDir
{

    struct Entry
    {
        /// <summary>
        /// The index that the entry was found in the MTC file.
        /// </summary>
        unsigned int index;

        /// <summary>
        /// The position in the MTC file the entry was listed.
        /// </summary>
        unsigned int dirPos;

        /// <summary>
        /// An unknown hash unique to each file. It's pausible it's a CRC32 
        /// of something (or Monaco's unique variant of a CRC32) but they appeared
        /// mostly ordered in the MTC.
        /// </summary>
        unsigned int dirID;

        unsigned int hidden_unkn0;
        unsigned int hidden_unkn1; // Current guess : (BOOL) isEncrypted
        unsigned int hidden_unkn2; // Current guess : (UINT) size of file in MDF (including zero padding)
        unsigned int hidden_unkn3; // Current guess : (UINT) uncompressed file size

        /// <summary>
        /// The position the file contents are located in the MDF file.
        /// </summary>
        unsigned int mdfPos;

        /// <summary>
        /// The size of the file contents in the MDF file.
        /// 
        /// If the file is uncompressed, this will be similar to the uncompressed 
        /// file size. It's not currently known why it's slightly off for some PNGs
        /// in the Worlds MDF but may be related to the spurrious padding noted for 
        /// some corrupt PNGs - and possibly related to the hidden_unkn0 which is set
        /// to 1 for all XMLs, but 0 for some level PNGs.
        /// </summary>
        unsigned int mdfSize;

    };

    std::vector<Entry> entries;

    int LoadMTC(std::string filename);
};