#pragma once
#include <string>
#include "ServerTypes.h"

struct MsgChat
{
	UserConPtr user;
	std::string message;
};