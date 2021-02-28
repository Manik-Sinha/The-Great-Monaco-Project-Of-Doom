// MTC_ExtractToJSON.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "crc32_simple.h"
#include "MTCDir.h"

// https://forum.xentax.com/viewtopic.php?t=12480

bool ProcessMTC(std::string mtcPath, std::string outPath);

int main()
{
    ProcessMTC(
        "E:\\SteamLibrary\\steamapps\\common\\Monaco\\Mag\\Worlds_win32.mtc",
        "C:\\Users\\Reavenk\\Desktop\\MTCExtract.json");

    // The CRC code may not be relevant, but we'll keep it in for now until this project is more formalized.
    //uint32_t table[256];
    //crc32::generate_table(table);

    //std::cout << "Hash of Content/Credits.xml " << crc32::MonacoUpdate(table, "Content/Credits.xml") << std::endl;
    //
    //std::cout << "Hello World!\n";
}

bool ProcessMTC(std::string mtcPath, std::string outPath)
{
    MTCDir mtcDir;
    int items = mtcDir.LoadMTC(mtcPath);

    if (items == 0)
        return false;

    std::stringstream ss;

    ss << "{\n\t\"Entries\":[";

    for (size_t i = 0; i < mtcDir.entries.size(); ++i)
    {
        ss << "\t{\n";

        ss << "\t\t" << "\"idx\" : " << mtcDir.entries[i].index << "," << "\n";
        ss << "\t\t" << "\"dirpos\" : " << mtcDir.entries[i].dirPos << "," << "\n";
        ss << "\t\t" << "\"dirid\" : " << mtcDir.entries[i].dirID << "," << "\n";
        ss << "\t\t" << "\"mdfpos\" : " << mtcDir.entries[i].mdfPos << "," << "\n";
        ss << "\t\t" << "\"mdfsize\" : " << mtcDir.entries[i].mdfSize << "," << "\n";
        ss << "\t\t" << "\"confabed\" : " << mtcDir.entries[i].confabed << "," << "\n";
        ss << "\t\t" << "\"encryped\" : " << mtcDir.entries[i].encrypted << "," << "\n";
        ss << "\t\t" << "\"alignment\" : " << mtcDir.entries[i].alignmentSize << "," << "\n";
        ss << "\t\t" << "\"fileSize\" : " << mtcDir.entries[i].fileSize << "\n";

        ss << "\t}";

        if (i != mtcDir.entries.size() - 1)
            ss << ",";

        ss << "\n";

    }
    ss << "\t]\n}";

    std::ofstream outFile = std::ofstream(outPath);
    outFile << ss.rdbuf();
    outFile.close();

    return true;
}