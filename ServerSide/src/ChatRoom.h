

#ifndef CHATROOM_H_
#define CHATROOM_H_

#include "User.h"
#include "Common.h"

 class User;

class ChatRoom {

public:
	string name;
	string owner;
	vector<string> users;

public:
	ChatRoom();
	void add_user(string user);
	void remove_user(string user);
	vector<string> get_all_users();
	void close();

	virtual ~ChatRoom();
};

#endif /* CHATROOM_H_ */
