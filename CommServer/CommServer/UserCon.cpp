#include "PrecompiledHeader.h"

UserCon::UserCon(std::string username, std::shared_ptr<WsServer::Connection> connection)
	:	username(username),
		connection(connection)
{
}