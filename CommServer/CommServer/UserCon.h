#pragma once

#include <string>
#include "ServerTypes.h"

#include "Simple-WebSocket-Server/client_ws.hpp"
#include "Simple-WebSocket-Server/server_ws.hpp"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

class UserCon
{
public:
	std::string username;
	std::shared_ptr<WsServer::Connection> connection;
	SessionPtr session;

public:
	UserCon(std::string username, std::shared_ptr<WsServer::Connection> connection);
};