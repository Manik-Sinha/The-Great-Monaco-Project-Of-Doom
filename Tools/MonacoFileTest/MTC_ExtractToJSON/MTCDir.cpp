#include "MTCDir.h"
#include <fstream>
#include <string>

int MTCDir::LoadMTC(std::string filename)
{
    // https://forum.xentax.com/viewtopic.php?t=12480
    // 
    //--------------------------------------
    //--- 010 Editor v6.0 Binary Template
    //
    // File: .mtc in monaco
    // Author: MiRiKan
    // Revision: 1.0
    // Purpose: unpacking monaco files
    //--------------------------------------
    //    local uint i, pos0, pos1;
    //    uint header;
    //    uint count;
    //
    //    struct pointers {
    //        for (i = 0; i < count; i++) {
    //            struct _POS {
    //                uint pointer;
    //                pos0 = FTell();
    //                FSeek(pointer);
    //                uint _pos;
    //                uint _ID;
    //                pos1 = FTell();
    //                FSeek(_pos);
    //                uint unk;
    //                uint unk <hidden = true>;
    //                uint realpointer; //pointer for .mdf
    //                uint unk <hidden = true>;
    //                uint unk;
    //                uint64 realFileSize; // files in .mdf
    //                FSeek(pos0);
    //            }Data;
    //        }
    //    }data;

    int loaded = 0;

    std::ifstream infile(filename, std::ios_base::binary);

    std::vector<char> buffer = std::vector<char>(std::istreambuf_iterator<char>(infile),
        std::istreambuf_iterator<char>());

    int pos = 0;

    int header = *(int*)&buffer[pos];
    pos += 4;
    //std::cout << "Header of " << header << std::endl;

    int count = *(int*)&buffer[pos];
    pos += 4;

    int ptrs = pos; // struct pointers

    for (int i = 0; i < count; ++i) // for (i = 0; i < count; i++){
    {
        Entry filePtr;
        filePtr.index = loaded;
        // struct _POS{

        int ptr = *(int*)&buffer[pos]; // uint pointer
        pos += 4;

        // pos0 = FTell();
        int pos0 = pos;

        // FSeek(pointer);
        pos = ptr;

        // FSeek(pointer);
        filePtr.dirPos = *(unsigned int*)&buffer[pos]; // uint _pos;
        pos += 4;

        filePtr.dirID = *(unsigned int*)&buffer[pos]; // uint _ID;
        pos += 4;

        // pos1 = FTell();
        int pos1 = pos;

        // FSeek(_pos);
        pos = filePtr.dirPos;

        // uint unk;
        int unk_0 = *(int*)&buffer[pos];
        filePtr.hidden_unkn0 = unk_0;
        pos += 4;

        // uint unk <hidden = true>;
        int unk_1 = *(int*)&buffer[pos];
        filePtr.hidden_unkn1 = unk_1;
        pos += 4;

        // uint realpointer; //pointer for .mdf
        filePtr.mdfPos = *(unsigned int*)&buffer[pos];
        pos += 4;

        // uint unk <hidden = true>;
        int unk_2 = *(int*)&buffer[pos];
        filePtr.hidden_unkn2 = unk_2;
        pos += 4;

        // uint unk;
        int unk_3 = *(int*)&buffer[pos];
        filePtr.hidden_unkn3 = unk_3;
        pos += 4;

        // uint64 realFileSize; // files in .mdf
        filePtr.mdfSize = *(unsigned int*)&buffer[pos];
        pos += 4;

        // FSeek(pos0);
        pos = pos0;

        this->entries.push_back(filePtr);
        ++loaded;

    }

    return loaded;
}