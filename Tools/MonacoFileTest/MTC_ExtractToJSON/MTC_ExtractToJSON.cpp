// MTC_ExtractToJSON.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "crc32_simple.h"
#include "MTCDir.h"

// https://forum.xentax.com/viewtopic.php?t=12480

int main()
{
    MTCDir mtcDir;
    mtcDir.LoadMTC("E:\\SteamLibrary\\steamapps\\common\\Monaco\\Mag\\Worlds_win32.mtc");


    std::stringstream ss;

    ss << "{\n\t\"Entries\":[";

    for (size_t i = 0; i < mtcDir.entries.size(); ++i)
    {
        ss << "\t{\n";

        ss << "\t\t" << "\"idx\" : "        << mtcDir.entries[i].index          << "," << "\n";
        ss << "\t\t" << "\"dirpos\" : "     << mtcDir.entries[i].dirPos         << "," << "\n";
        ss << "\t\t" << "\"dirid\" : "      << mtcDir.entries[i].dirID          << "," << "\n";
        ss << "\t\t" << "\"mdfpos\" : "     << mtcDir.entries[i].mdfPos         << "," << "\n";
        ss << "\t\t" << "\"mdfsize\" : "    << mtcDir.entries[i].mdfSize        << "," << "\n";
        ss << "\t\t" << "\"unkn0\" : "      << mtcDir.entries[i].hidden_unkn0   << "," << "\n";
        ss << "\t\t" << "\"unkn1\" : "      << mtcDir.entries[i].hidden_unkn1   << "," << "\n";
        ss << "\t\t" << "\"unkn2\" : "      << mtcDir.entries[i].hidden_unkn2   << "," << "\n";
        ss << "\t\t" << "\"unkn3\" : "      << mtcDir.entries[i].hidden_unkn3   << "\n";

        ss << "\t}";

        if (i != mtcDir.entries.size() - 1)
            ss << ",";

        ss << "\n";

    }
    ss << "\t]\n\}";

    std::ofstream outFile = std::ofstream("C:\\Users\\Reavenk\\Desktop\\MTCExtract.json");
    outFile << ss.rdbuf();
    outFile.close();

    // The CRC code may not be relevant, but we'll keep it in for now until this project is more formalized.
    //uint32_t table[256];
    //crc32::generate_table(table);

    //std::cout << "Hash of Content/Credits.xml " << crc32::MonacoUpdate(table, "Content/Credits.xml") << std::endl;
    //
    //std::cout << "Hello World!\n";
}
