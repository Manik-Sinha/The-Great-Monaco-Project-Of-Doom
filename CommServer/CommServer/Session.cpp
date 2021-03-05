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
		lrUsersNew.Swap(localNewUsers);

		std::vector<UserConPtr> localExitUsers;
		lrUsersExit.Swap(localExitUsers);

		// Handle exited users first so we don't send anymore messages
		// to them when we broadcast everything else.
		if (localExitUsers.size() > 0)
		{
			auto send_stream = std::make_shared<WsServer::SendStream>();
			*send_stream << R"({"msg":"userleave", "data":[)";

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
			*msgUserEntered << R"({"msg":"userentered", "data":[)";

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
			*msgConfirmJoined << R"({"msg" : "entered"})"; 
			for (UserConPtr& ucp : localNewUsers)
			{
				this->usersCurrent.push_back(ucp);
				ucp->connection->send(msgConfirmJoined);
			}

			// Send the chat population to the new users - for simplicitly, we'll also.
			// include themselves in the listings.

			auto msgUserList = std::make_shared<WsServer::SendStream>();
			*msgUserList << R"({"msg":"userlist", "data":[)";

			for (size_t i = 0; i < this->usersCurrent.size(); ++i)
			{
				if (i != 0)
					*msgUserList << ", ";

				*msgUserList << R"(")" << this->usersCurrent[i]->username << R"(")";
			}
			*msgUserList << "]}";

			for (auto it = localNewUsers.begin(); it != localNewUsers.end(); ++it)
				(*it)->connection->send(msgUserList);
		}

		// Get all chat messages

		
		std::vector<MsgChat> localChat;
		lrChatMessages.Swap(localChat);

		if (localChat.empty() == false)
		{
			auto msgChat = std::make_shared<WsServer::SendStream>();
			*msgChat << R"({"msg":"chat", "data":[)";
			bool insed = false;
			for (const MsgChat& mc : localChat)
			{
				if (insed == false)
					insed = true;
				else
					*msgChat << ", ";

				// TODO: Escape the username and chat string
				*msgChat << R"({"user":")" << mc.user->username << R"(", "data" : ")" << mc.message << R"("})";
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
	{LOCK_SCOPE(lrUsersNew, usersNew)
		usersNew.push_back(newUser);
	}
}

void Session::StageExitingUser(UserConPtr exitingUser)
{
	{ LOCK_SCOPE(lrUsersExit, usersExit)
		usersExit.push_back(exitingUser);
	}
}
