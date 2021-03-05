#pragma once
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include "ServerTypes.h"
#include "MsgChat.h"
#include "LockedResource.h"

class SessionMgr;

class Session
{
public:
	std::string roomName;

	// Starts true, should only ever be set false after construction.
	// TODO: Encapsulate this constraint.
	bool keepRunning = true;
	bool curRunning = false;

	// Starts false, should only ever be set to true after destruction.
	// TODO: Encapsulate this constraint.
	bool shutdown = false;

	std::vector<std::string> lvl_base64;
	std::vector<char> lvl_raw;

	LockedResource< std::vector<UserConPtr>>	lrUsersNew;
	LockedResource< std::vector<UserConPtr>>	lrUsersExit;
	LockedResource< std::vector<MsgChat>>		lrChatMessages;

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