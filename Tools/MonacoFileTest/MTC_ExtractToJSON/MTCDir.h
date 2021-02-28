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

        /// <summary>
        /// It's not 100% known what this int bool does, but it seems to be related to
        /// signaling if PNG files in Worlds mdf are corrupted.
        /// </summary>
        unsigned int confabed;

        /// <summary>
        /// It's not 100% known what this int book does. It has always been observed to
        /// be 0 for both the World and Content MTC. The current beleif is that it is 
        /// a flag for if the file is encrypted.
        /// 
        /// Another hypothesis is that this is a flag for if things should be remain
        /// hidden (From what exactly? Who knows.)
        /// </summary>
        unsigned int encrypted;

        /// <summary>
        /// This is only a guess, but it seems to be the size the data payload takes up
        /// in the MDF, including any empty padding.
        /// 
        /// This is different from mdfSize in that the mdfSize does not include padding.
        /// </summary>
        unsigned int alignmentSize;

        /// <summary>
        /// This is only a guess, but it is beleive to be the final size of the size. This
        /// is different from mdfSize, because that size can be different if there is 
        /// padding corruption, or if the file in the MDF is compressed.
        /// </summary>
        unsigned int fileSize;

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