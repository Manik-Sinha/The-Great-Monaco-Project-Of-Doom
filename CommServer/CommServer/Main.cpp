#include "PrecompiledHeader.h"
#include <iostream>
#include "SessionsMgr.h"
#include <Windows.h>
SessionMgr gMgr;

#include "Simple-WebSocket-Server/client_ws.hpp"
#include "Simple-WebSocket-Server/server_ws.hpp"

using namespace std;
using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

int main() 
{
	std::cout << "The Great Monaco Project Of Doom" << std::endl;
	std::cout << "\tCollaboration Server" << std::endl;
	std::cout << "========================================" << std::endl;
	std::cout << "Starting WebSockets server" << std::endl;

	gMgr.StartServer();
	std::cout << "Server initialized." << std::endl;
	std::cout << "Running server. Press Pause/Break key to close." << std::endl;

	while (true)
	{
		if (GetAsyncKeyState(VK_PAUSE) == true)
		{
			// TODO: limit to if the console is in focus
			break;
		}

		Sleep(1000);
	}
	return 0;

}