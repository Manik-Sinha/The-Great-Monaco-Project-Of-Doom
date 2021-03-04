#include "PrecompiledHeader.h"
#include <combaseapi.h> // GUID stuff

SessionMgr::SessionMgr()
{
}

SessionMgr::~SessionMgr()
{
}

void SessionMgr::StartServer()
{
    this->server.config.port = 7011;
    auto& echo = this->server.endpoint["^/MonacoServer/?$"];

    echo.on_message = 
        [this](std::shared_ptr<WsServer::Connection> connection, std::shared_ptr<WsServer::Message> message) 
        {
            std::string msg = message->string();

            json::jobject result = json::jobject::parse(msg);
            std::string msgTy = result["msg"].as_string();

            std::cout << "Server: Message received: " << msg << std::endl;
            if (msgTy == "respauth")
            {
                std::string authKey     = result["data"].as_object()["key"] .as_string();
                std::string userName    = result["data"].as_object()["name"].as_string();
                std::string room        = result["data"].as_object()["room"].as_string();

                // TODO: Validate room name, including empty room name

                bool wasInAuthState = false;
                this->awaitVerifs_mut.lock();
                auto findIt = this->awaitVerifs.find(connection);
                if (findIt != this->awaitVerifs.end())
                {
                    this->awaitVerifs.erase(findIt);
                    wasInAuthState = true;
                }
                this->awaitVerifs_mut.unlock();

                if (wasInAuthState == true)
                {
                    
                }
                else
                {
                    auto send_stream = std::make_shared<WsServer::SendStream>();
                    *send_stream << std::string("{\"msg\":\"dc\", \"data\":\"Attempting to log in without proper handshake.\"}");
                    connection->send(send_stream);
                    connection->send_close(0);
                }

                UserConPtr newUserPtr = UserConPtr(new UserCon(userName, connection));

                this->sessions_mut.lock();
                this->userLookup_mut.lock();
                auto itSess = this->sessions.find(room);
                if (itSess == this->sessions.end())
                {
                    std::cout << "Creating new room " << room << std::endl;
                    SessionPtr newSession= SessionPtr(new Session(this, room));
                    this->sessions[room] = newSession;

                    std::cout << "Added user " << userName << " to new room " << room << std::endl;
                    newUserPtr->session = newSession;
                    newSession->StageNewUser(newUserPtr);
                    newSession->StartSession();
                    this->userLookup[connection] = newUserPtr;
                }
                else
                {
                    std::cout << "Added user " << userName << " to existing room " << room << std::endl;
                    newUserPtr->session = itSess->second;
                    itSess->second->StageNewUser(newUserPtr);
                    this->userLookup[connection] = newUserPtr;
                }
                this->userLookup_mut.unlock();
                this->sessions_mut.unlock();

            }
            else if (msgTy == "chat")
            {
                // The session manager is (currently) in charge of intercepting all web server
                // messages, it we can't handle chat messages directly. Instead we'll intercept
                // them and delegate them to the proper session for them to do any error checking
                // and broadcasting.
                this->userLookup_mut.lock();
                auto itFind = this->userLookup.find(connection);
                if (itFind != this->userLookup.end())
                {
                    SessionPtr session = itFind->second->session;
                    session->chatMessages_mut.lock();

                    MsgChat msgChat;
                    msgChat.user = itFind->second;
                    msgChat.message = result["data"].as_string();
                    session->chatMessages.push_back(msgChat);

                    session->chatMessages_mut.unlock();
                }
                this->userLookup_mut.unlock();
            }
            else if (msgTy == "addmarker")
            {} // Unknown if this will be implemented, just brainstorming.
            else if (msgTy == "rmmarker")
            {} // Unknown if this will be implemented, just brainstorming.
            else if (msgTy == "addgeom")
            {} // Unknown if this will be implemented, just brainstorming.
            else if (msgTy == "rmgeom")
            {} // Unknown if this will be implemented, just brainstorming.

            //auto send_stream = std::make_shared<WsServer::SendStream>();
            //*send_stream << message_str;
            //// connection->send is an asynchronous function
            //connection->send(
            //    send_stream, 
            //    [](const SimpleWeb::error_code& ec) 
            //    {
            //        if (ec) {
            //            std::cout << "Server: Error sending message. " <<
            //            // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
            //            "Error: " << ec << ", error message: " << ec.message() << std::endl;
            //    }
            //});
        };

    echo.on_open = 
        [this](std::shared_ptr<WsServer::Connection> connection) 
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

            std::string guidString(guidStr);

            this->awaitVerifs_mut.lock();
            this->awaitVerifs[connection] = guidStr;
            this->awaitVerifs_mut.unlock();

            std::stringstream ss;
            ss << "User opened connection : Sending authorization query with unique key " << guidStr << "." << std::endl;

            std::cout << ss.str();

            auto send_stream = std::make_shared<WsServer::SendStream>();
            *send_stream << std::string("{\"msg\":\"auth\", \"data\":\"") + guidString + "\"}";
            connection->send(send_stream);

        };

    // See RFC 6455 7.4.1. for status codes
    echo.on_close = 
        [this](std::shared_ptr<WsServer::Connection> connection, int status, const std::string& /*reason*/) 
        {
            std::cout << "Server: Closed connection " << connection.get() << " with status code " << status << std::endl;

            this->awaitVerifs_mut.lock();
            auto findIt = this->awaitVerifs.find(connection);
            if (findIt != this->awaitVerifs.end())
            {
                this->awaitVerifs.erase(findIt);
            }
            this->awaitVerifs_mut.unlock();

            userLookup_mut.lock();
            auto itFindUCP = this->userLookup.find(connection);
            if (itFindUCP != this->userLookup.end())
            {
                UserConPtr user = itFindUCP->second;
                user->session->usersExit_mut.lock();
                user->session->usersExit.push_back(user);
                user->session->usersExit_mut.unlock();
                std::cout << "Closed user was queued for removal from session.";

                this->userLookup.erase(itFindUCP);
                std::cout << "Closed user was removed from global server directory.";

            }
            userLookup_mut.unlock();
        };

    // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    echo.on_error = 
        [this](std::shared_ptr<WsServer::Connection> connection, const SimpleWeb::error_code& ec) 
        {
            std::cout << "Server: Error in connection " << connection.get() << ". " << "Error: " << ec << ", error message: " << ec.message() << std::endl;

            this->awaitVerifs_mut.lock();
            auto findRem = this->awaitVerifs.find(connection);
            if (findRem != this->awaitVerifs.end())
                this->awaitVerifs.erase(findRem);
            this->awaitVerifs_mut.unlock();
        };

    std::cout << "Setting up server\n";
    this->serverThread = std::thread(
        [this]() 
        {
            std::cout << "Setting up server\n";
            // Start WS-server
            this->server.start();
        });

    //server_thread.join();
}

void SessionMgr::ServerThreadFunction()
{
}