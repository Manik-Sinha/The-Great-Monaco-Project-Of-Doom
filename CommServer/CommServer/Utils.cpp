#include "PrecompiledHeader.h"

#include <combaseapi.h> // GUID stuff

std::string GenerateGUID()
{
    GUID guid;
    CoCreateGuid(&guid);

    // https://stackoverflow.com/a/27621890/2680066
    char guidStr[37];
    sprintf_s(
        guidStr,
        "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    return std::string(guidStr);
}