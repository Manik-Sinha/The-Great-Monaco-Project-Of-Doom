#pragma once

#include <map>
#include <string>
#include <mutex>

#include "Simple-WebSocket-Server/server_ws.hpp"
#include "Session.h"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class SessionMgr
{
public:
	
	std::mutex sessions_mut;
	std::map<std::string, SessionPtr> sessions;

	std::mutex awaitVerifs_mut;
	std::map<std::shared_ptr<WsServer::Connection>, std::string> awaitVerifs;

	std::mutex userLookup_mut;
	std::map< std::shared_ptr<WsServer::Connection>, UserConPtr> userLookup;

	WsServer server;
	bool doneShutdown = false;
	std::thread serverThread;

public:
	SessionMgr();
	~SessionMgr();

	void StartServer();

	void ServerThreadFunction();
};