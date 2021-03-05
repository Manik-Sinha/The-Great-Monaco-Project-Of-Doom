#pragma once

#include <iostream>
#include <memory>

#include "Simple-WebSocket-Server/client_ws.hpp"
#include "Simple-WebSocket-Server/server_ws.hpp"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

#include "SessionsMgr.h"
#include "Session.h"
#include "UserCon.h"
#include "Utils.h"

#include "Simpleson/json.h"