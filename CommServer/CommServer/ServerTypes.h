#pragma once
#include <memory>

class SessionsMgr;

class Session;
typedef std::shared_ptr<Session> SessionPtr;

class UserCon;
typedef std::shared_ptr<UserCon> UserConPtr;