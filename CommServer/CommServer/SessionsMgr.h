#pragma once

#include <map>
#include <string>
#include <mutex>

#include "Simple-WebSocket-Server/server_ws.hpp"
#include "Session.h"

#include "LockedResource.h"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class SessionMgr
{
public:
	// TODO: Remove sessions are they are closed
	// And close sessions if they are found to have 0 users.
	LockedResource<std::map<std::string, SessionPtr>> lrSessions;
	LockedResource<std::map<std::shared_ptr<WsServer::Connection>, std::string>> lrAwaitVerifs;
	LockedResource<std::map< std::shared_ptr<WsServer::Connection>, UserConPtr>> lrUserLookup;

	WsServer server;
	bool doneShutdown = false;
	std::thread serverThread;

public:
	SessionMgr();
	~SessionMgr();

	void StartServer();

	void OnServer_Message(std::shared_ptr<WsServer::Connection> connection, std::shared_ptr<WsServer::Message> message);
	void OnServer_Open(std::shared_ptr<WsServer::Connection> connection);
	void OnServer_Close(std::shared_ptr<WsServer::Connection> connection, int status, const std::string& reason);
	void OnServer_Error(std::shared_ptr<WsServer::Connection> connection, const SimpleWeb::error_code& ec);
};