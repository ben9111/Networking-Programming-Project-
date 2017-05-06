

#include "ChatRoom.h"

ChatRoom::ChatRoom() {

}

void ChatRoom::add_user(string user)
{
	if (find(users.begin(),users.end(),name) == users.end()) //user is not in users list
	{
		users.push_back(user);
	}

}

void ChatRoom::remove_user(string user)
{
		for (unsigned int i=0 ; i< users.size() ; i++ )
		{
			if (users[i] == user)
			{
				cout << "chatroom user was found and removed" << endl;
				users.erase(users.begin()+i);
			}
		}
}

vector<string> ChatRoom::get_all_users()
{
	return users;
}
void ChatRoom::close() //broadcast closing msg
{

}

ChatRoom::~ChatRoom() {

}
