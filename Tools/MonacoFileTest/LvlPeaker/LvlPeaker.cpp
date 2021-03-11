// LvlPeaker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

constexpr short CODE(char a, char b)
{
    return (b << 8) | a;
}

std::string GetPascalString(int& idx, char* pData)
{
    int strSz = pData[idx];
    ++idx;

    std::string ret = std::string(&pData[idx], strSz);
    idx += strSz;

    return ret;
}

void SkipUnknownSection(std::ostream & output, int& idx, char* pData)
{
    int size = pData[idx];
    ++idx;

    output << "\tUnknown section of size " << size << std::endl;
    output << "\t\t DATA :";

    for (int i = 0; i < size; ++i)
    {
        output << ' ' << (int)pData[idx];
        ++idx;
    }

    output << std::endl;
}

void SkipUnknownSection(std::ostream& output, short code, int& idx, char* pData)
{
    output << "Entered unknown code [" << std::hex << (int)((char*)&code)[0] << "," << (int)((char*)&code)[1] << std::dec << "] : Skipping" << std::endl;
    SkipUnknownSection(output, idx, pData);
}

struct Floor
{
    std::string name;
    std::vector<std::string> blueprintNames;
    std::vector<char> levelData;
};

int main(int argc, char ** argv)
{
    std::string lvlFilePath =
        "C:\\Program Files (x86)\\Steam\\userdata\\10170710\\113020\\remote\\Worlds\\My Stage.lvl";
        //"C:\\Users\\Reavenk\\Desktop\\MonacoFiles\\The_Prison_Break.lvl";
        //"C:\\Program Files (x86)\\Steam\\userdata\\10170710\\113020\\remote\\Worlds\\Empty4.lvl";

    if (argc > 1)
        lvlFilePath = argv[1];

    std::ifstream infile(lvlFilePath, std::ios_base::binary);

    std::vector<char> levelData =
        std::vector<char>(
            std::istreambuf_iterator<char>(infile),
            std::istreambuf_iterator<char>());

    infile.close();

    char* pData = &levelData[0];

    if (pData[0] != 0 || pData[1] != 2 || pData[2] != 2)
        std::cout << "File did not start with the predicted 0x00 0x02 0x03" << std::endl;
    else
        std::cout << "File started with predicted 0x00 0x02 0x03" << std::endl;

    int idx = 3;

    int version = pData[idx];
    std::cout << "Predicting version is " << version << std::endl;
    ++idx;

    int gameMode = pData[idx];
    std::cout << "Predicting game mode is " << gameMode << std::endl;
    ++idx;

    // Goal type is found lower
    int goalType = pData[idx];
    //std::cout << "Predicting goal type is " << goalType << std::endl;
    ++idx;

    std::cout << "Skipping unknown byte at " << idx << " with value " << (int)pData[idx] << std::endl;
    ++idx;

    std::string strAddress = GetPascalString(idx, pData);
    std::cout << "Predicting original Address as " << strAddress << std::endl;

    std::string strCurAddress;
    std::string strWeatherName;
    std::string strUnknown_6_2;
    std::string strGoalText;
    std::vector<std::string> charNames;
    std::vector<std::string> dlgText;
    std::vector<Floor> floors;

    int floorCtr = 0;
    bool continueLooping = true;
    while (continueLooping == true)
    {
        short sectionCode = *(short*)&pData[idx];
        idx += 2;

        switch (sectionCode)
        {
            case CODE(0x02, 0x03):
                std::cout << "Entering Current address: " << std::endl;
                strCurAddress = GetPascalString(idx, pData);
                std::cout << "\t" << strCurAddress << std::endl;
                break;

            case CODE(0x05, 0x03):
                std::cout << "Entering Weather Name: " << std::endl;
                strWeatherName = GetPascalString(idx, pData);
                std::cout << "\t" << strWeatherName << std::endl;
                break;

            case CODE(0x07,0x04):
                {
                    std::cout << "Entering Characters Listing: " << std::endl;
                    int charCt = pData[idx];
                    ++idx;
                    std::cout << "\t" << charCt << " Characters" << std::endl;
                    for (int i = 0; i < charCt; ++i)
                    {
                        std::string charName = GetPascalString(idx, pData);
                        charNames.push_back(charName);
                        std::cout << "\t\t" << charName << std::endl;
                    }
                }
                break;

            case CODE(0x06, 0x02):
            case CODE(0x13, 0x01):
            case CODE(0x15, 0x02):
            case CODE(0x14, 0x02):
            case CODE(0x0F, 0x03): // Commonly 8 bytes.
            case CODE(0x10, 0x03):
            case CODE(0x11, 0x01):
                {
                    SkipUnknownSection(std::cout, sectionCode, idx, pData);
                }
                break;

            case CODE(0x08, 0x01):
                {
                    std::cout << "Entered goal type" << std::endl;
                    goalType = pData[idx];
                    ++idx;
                    std::cout << "\t" << goalType << std::endl;
                }
                break;

            case CODE(0x0E, 0x03):
                std::cout << "Entered goal data" << std::endl;
                strGoalText = GetPascalString(idx, pData);
                break;

            case CODE(0x12, 0x04):
                {
                    std::cout << "Entered Dialog Text:";
                    int dlgTextCt = pData[idx];
                    ++idx;
                    std::cout << "\t" << dlgTextCt << " entries" << std::endl;
                    for (int i = 0; i < dlgTextCt; ++i)
                    {
                        std::string dlg = GetPascalString(idx, pData);
                        dlgText.push_back(dlg);
                        std::cout << "\t\t" << dlg << std::endl;
                    }
                }
                break;

            case CODE(0x09, 0x00):
                {
                    std::cout << "Entered floor section" << std::endl;
                    
                    Floor f;
                    bool innerContinue = true;
                    while (innerContinue)
                    {
                        short floorCode = *(short*)(&pData[idx]);
                        idx += 2;

                        switch (floorCode)
                        {
                        case CODE(0x0A, 0x03):
                            std::cout << "FLOOR[" << floorCtr << "]: " << "Encountered floor name" << std::endl;
                            f.name = GetPascalString(idx, pData);
                            std::cout << "\t" << f.name << std::endl;
                            //break;

                        //case CODE(0x0B, 0x04)

                            if (pData[idx + 0] != 0x0B || pData[idx + 1] != 0x04 || pData[idx + 2] != 0x16)
                            {
                                std::cout << "Did not encounter myster 0x0B 0x04 0x16 where expected" << std::endl;
                                return 0;
                            }
                            idx += 3;

                            // There doesn't seem to be any count of blueprint names AFAICT, it's just
                            // multiple pascals until a zero length.
                            while (pData[idx] != 0)
                            {
                                std::string bluesprintName = GetPascalString(idx, pData);
                                std::cout << "\tBLUEPRINT_NAME : " << bluesprintName <<std::endl;
                                f.blueprintNames.push_back(bluesprintName);
                            }
                            // And then a seemingly random number of zeros until a 0x0C 0x02 is reached.
                            while (pData[idx] == 0)
                                ++idx;
                            break;

                        case CODE(0x0C, 0x02):
                            SkipUnknownSection(std::cout, floorCode, idx, pData);
                            break;

                        case CODE(0x0D, 0x05):
                            std::cout << "Entered level tileset data." << std::endl;
                            break;

                        default:
                            std::cout << "Encountered unknown code, aborting." << std::endl;
                            return 0;
                        }
                    }
                }
                break;

            default:
                continueLooping = false;
        }
    }
}
