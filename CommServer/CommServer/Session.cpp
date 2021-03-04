#include "PrecompiledHeader.h"

Session::Session(SessionMgr* manager, std::string name)
	:	manager(manager),
		roomName(name)
{
}

void Session::ServerThreadFunction()
{
	std::cout << "Thread started for room " << this->roomName << std::endl;

	this->curRunning = true;
	while (this->keepRunning == true)
	{
		std::vector<UserConPtr> localNewUsers;
		this->usersNew_mut.lock();
		std::swap(this->usersNew, localNewUsers);
		this->usersNew_mut.unlock();

		std::vector<UserConPtr> localExitUsers;
		this->usersExit_mut.lock();
		std::swap(this->usersExit, localExitUsers);
		this->usersExit_mut.unlock();

		// Handle exited users first so we don't send anymore messages
		// to them when we broadcast everything else.
		if (localExitUsers.size() > 0)
		{
			auto send_stream = std::make_shared<WsServer::SendStream>();
			*send_stream << "{\"msg\":\"userleave\", \"data\":[";

			for (size_t i = 0; i < localExitUsers.size(); ++i)
			{
				if (i != 0)
					*send_stream << ", ";

				*send_stream << "\"" << localExitUsers[i]->username << "\"";

				for (size_t rm = 0; rm < this->usersCurrent.size(); ++rm)
				{
					// There are probably more robust ways of identifying who to remove
					// instead of using the username.
					if (this->usersCurrent[rm]->username == localExitUsers[i]->username)
					{
						this->usersCurrent.erase(this->usersCurrent.begin() + rm);
						break;
					}
					// TODO: What if we didn't find them in the list of existing users?
				}
			}
			*send_stream << "]}";

			for (auto it = this->usersCurrent.begin(); it != this->usersCurrent.end(); ++it)
				(*it)->connection->send(send_stream);
		}
		
		if (localNewUsers.size() > 0)
		{
			// Send information about these new user(s) to existing users
			auto msgUserEntered = std::make_shared<WsServer::SendStream>();
			*msgUserEntered << "{\"msg\":\"userentered\", \"data\":[";

			for (size_t i = 0; i < localNewUsers.size(); ++i)
			{
				if (i != 0)
					*msgUserEntered << ", ";

				*msgUserEntered << "\"" << localNewUsers[i]->username << "\"";
			}
			*msgUserEntered << "]}";

			for (auto it = this->usersCurrent.begin(); it != this->usersCurrent.end(); ++it)
				(*it)->connection->send(msgUserEntered);

			// Officially integrate them into the list of current users.

			auto msgConfirmJoined = std::make_shared<WsServer::SendStream>();
			// There may be more info we want to attach, but for now that's it.
			*msgConfirmJoined << "{\"msg\" : \"entered\"}"; 
			for (UserConPtr& ucp : localNewUsers)
			{
				this->usersCurrent.push_back(ucp);
				ucp->connection->send(msgConfirmJoined);
			}

			// Send the chat population to the new users - for simplicitly, we'll also.
			// include themselves in the listings.

			auto msgUserList = std::make_shared<WsServer::SendStream>();
			*msgUserList << "{\"msg\":\"userlist\", \"data\":[";

			for (size_t i = 0; i < this->usersCurrent.size(); ++i)
			{
				if (i != 0)
					*msgUserList << ", ";

				*msgUserList << "\"" << this->usersCurrent[i]->username << "\"";
			}
			*msgUserList << "]}";

			for (auto it = localNewUsers.begin(); it != localNewUsers.end(); ++it)
				(*it)->connection->send(msgUserList);
		}

		// Get all chat messages
		this->chatMessages_mut.lock();
		std::vector<MsgChat> localChat;
		std::swap(localChat, this->chatMessages);
		this->chatMessages_mut.unlock();

		if (localChat.empty() == false)
		{
			auto msgChat = std::make_shared<WsServer::SendStream>();
			*msgChat << "{\"msg\":\"chat\", \"data\":[";
			bool insed = false;
			for (const MsgChat& mc : localChat)
			{
				if (insed == false)
					insed = true;
				else
					*msgChat << ", ";

				// TODO: Escape the username and chat string
				*msgChat << "{\"user\":\"" << mc.user->username << "\", \"data\" : \"" << mc.message << "\"}";
			}
			*msgChat << "]}";

			for (UserConPtr& ucp : this->usersCurrent)
				ucp->connection->send(msgChat);
		}

		// Don't  run at full speed. We may want to find a way to sleep unless
		// network messages are queued. Or the timer should take into account
		// how long the previous frame took and subtract that from the polling delay.
		Sleep(100); 
	}
	this->curRunning = false;
	
}

void Session::StartSession()
{
	std::cout << "Starting session for room " << this->roomName << std::endl;

	this->curRunning = false;
	this->shutdown = false;
	this->keepRunning = true;

	this->selfThread = 
		std::thread(
			[this]()
			{
				this->ServerThreadFunction();
				this->shutdown = true;
			});
}

void Session::StageNewUser(UserConPtr newUser)
{
	this->usersNew_mut.lock();
	this->usersNew.push_back(newUser);
	this->usersNew_mut.unlock();
}

void Session::StageExitingUser(UserConPtr exitingUser)
{
	this->usersExit_mut.lock();
	this->usersExit.push_back(exitingUser);
	this->usersExit_mut.unlock();
}
