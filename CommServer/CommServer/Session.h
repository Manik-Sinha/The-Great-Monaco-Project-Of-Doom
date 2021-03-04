#pragma once
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include "ServerTypes.h"
#include "MsgChat.h"

class SessionMgr;

class Session
{
public:
	std::string roomName;
	bool keepRunning = true;
	bool curRunning = false;
	bool shutdown = false;

	std::vector<std::string> lvl_base64;
	std::vector<char> lvl_raw;

	std::mutex usersNew_mut;
	std::vector<UserConPtr> usersNew;

	std::mutex usersExit_mut;
	std::vector<UserConPtr> usersExit;

	std::mutex chatMessages_mut;
	std::vector<MsgChat> chatMessages;

private:
	// Should never be manipulated outside the class
	std::vector<UserConPtr> usersCurrent;

public:
	std::thread selfThread;
	SessionMgr * manager;

public:
	Session(SessionMgr* manager, std::string name);

	void ServerThreadFunction();
	void StartSession();

	void StageNewUser(UserConPtr newUser);
	void StageExitingUser(UserConPtr exitingUser);
};