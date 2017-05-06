

#include "Dispatcher.h"
#include "Protocol.h"

pthread_mutex_t login_dispatcher_mutex = PTHREAD_MUTEX_INITIALIZER;

Dispatcher::Dispatcher(vector<string> names)
{
	listener = NULL;
	users_map_mutex = PTHREAD_MUTEX_INITIALIZER;
	users_map_size = 0;
	status = false;
	all_users_infile_vector = names;
}
void Dispatcher::add_user_to_vector(string name)
{
	all_users_infile_vector.push_back(name);
}
void Dispatcher::add_user(TCPSocket* peer,string name)
{
	if (pthread_mutex_lock(&login_dispatcher_mutex)!=0)
	{
		printf("Error locking mutex");
		exit(1);
	}
	User* user = new User;

	user->socket = peer;
	user->name = name;
	user->connectionStatus = false;
	user->connectedToUser = NULL;
	user->connectedToChatRoom = NULL;


	cout << user->name << " is now connected" << endl;

	users_map[name] = user;
	users_vector.push_back(peer);

	if (status == false)
	{
		status = true;
		this->start();
	}
	if (pthread_mutex_unlock(&login_dispatcher_mutex)!=0)
	{
		printf("Error unlocking mutex");
		exit(1);
	}
}

void Dispatcher::run()
{
	int command;

	while (status == true)
	{
		User* current_user = new User();
		listener = new MultipleTCPSocketsListener();
		listener->addSockets(users_vector);
		TCPSocket* temp_sock = listener->listenToSocket(2);
		delete(listener);

		if (temp_sock != NULL)
		{
			command = readCommand(temp_sock);
			current_user->socket = temp_sock; //Initialize the socket field
		}
		else
			continue;

		if (command == 0)
		{
			//cout<<"problem"<<endl;
			continue;
		}
		else if (command == OPEN_SESSION_WITH_USER) //get the user`s IP & port
		{
			open_session(current_user);
		}
		else if (command == CLOSE_SESSION_WITH_USER)
		{
			close_session(current_user);
		}
		else if (command == EXIT) //user is exiting, update all chat rooms and users list
		{
			user_exit(current_user);
		}
		else if (command == GET_ALL_USERS) //all names from file
		{
			list_all_users(current_user);
		}
		else if (command == OPEN_CHATROOM) //user is the owner of the new chatroom
		{
			join_chatroom(current_user);
		}
		else if (command == LEAVE_CHATROOM) //remove user from chatroom and update list and users
		{
			leave_chatroom(current_user);
		}
		else if (command == GET_ALL_CONNECTED_USERS) //send back to the user a list of all connected users
		{
			list_users(current_user);
		}
		else if(command == GET_ALL_CHATROOMS) //GET_ALL_CHATROOMS
		{
			list_chatrooms(current_user);
		}
		else if (command == GET_CHATROOM_USERS) //GET_CHATROOM_USERS
		{
			list_chatroom_users(current_user);
		}
	}
}


void Dispatcher::open_session(User* user)
{
	string userName,peerUsr,user_ip_and_port,target_ip_and_port;
	User* target;
	string with;
	User* _user;


	userName = readMsg(user->socket); // our name
	peerUsr = readMsg(user->socket);//The user name of the target user

	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		_user = users_iter->second;
		if (_user->socket == user->socket)
		{//if there is a match between the connected user to one of the users in the map->
		//initialize _user and break
			break;
		}
	}
	target = users_map[peerUsr];
	if (target->name!= peerUsr)
	{
		writeCommand(user->socket,OPEN_SESSION_ERROR);
		return ;
	}
	if ((_user->connectionStatus == false)&&(target->connectionStatus==false))
	{
		if ((users_map.find(peerUsr)!=users_map.end())&& (userName.compare(peerUsr)!=0)) //target user was found
		{

			target_ip_and_port = target->socket->destIpAndPort();
			//target_ip_and_port = target->socket->destIpAndPort();

			writeCommand(user->socket,TARGET_IP_AND_PORT);
			writeMsg(user->socket,target->name);
			writeMsg(user->socket,target_ip_and_port);
			user_ip_and_port = user->socket->destIpAndPort();

			writeCommand(target->socket,INCOMIMG_SESSION);
			writeMsg(target->socket,_user->name);
			writeMsg(target->socket,user_ip_and_port);
			target->connectionStatus = true;
			target->connectedToUser = _user;
			_user->connectionStatus = true;

			in_session_with[_user->name] = target->name; //for connection monitoring
		}
		else
		{
			writeCommand(user->socket,OPEN_SESSION_ERROR);
		}
	}
	else
	{
		writeCommand(user->socket,OPEN_CONNECTION_ERROR);
	}

}

void Dispatcher::list_all_users(User* user)
{
	writeCommand(user->socket,ALL_USERS_LIST);
	writeCommand(user->socket,all_users_infile_vector.size());

	for (unsigned int i=0 ; i < all_users_infile_vector.size() ; i++)
	{
		writeMsg(user->socket,all_users_infile_vector[i]);
	}
}
void Dispatcher::close_session(User* user )
{
	string partner;
	User* _user;
	bool first = false;
	bool second = false;


	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		_user = users_iter->second;
		if (_user->socket == user->socket)
		{//if there is a match between the connected user to one of the users in the map->
		//initialize _user and break
			break;
		}
	}

	//check if user is in session

	for (in_session_with_iter = in_session_with.begin() ; in_session_with_iter != in_session_with.end() ; in_session_with_iter++)
	{
		if (in_session_with_iter->first == _user->name) //user has an open session that he started
		{
			partner = in_session_with_iter->second;
			first = true;
		}
		else if (in_session_with_iter->second == _user->name) //user has an open session that he DIDNT started
		{
			partner = in_session_with_iter->first;
			second = true;
		}
	}

	if (first == true || second == true)
	{
		writeCommand((users_map[partner])->socket,SESSION_ENDED);
		users_map[partner]->connectionStatus = false;
		users_map[partner]->connectedToUser = NULL;
		users_map[_user->name]->connectionStatus = false;
		users_map[_user->name]->connectedToUser = NULL;
		if (first == true)
		{
			in_session_with.erase(_user->name);
		}
		else if (second == true)
		{
			in_session_with.erase(partner);
		}
	}
	else
	{
		writeCommand(_user->socket,CLOSE_SESSION_ERROR);
	}

}
void Dispatcher::create_chatroom(User* user ,string name)
{
	ChatRoom* chatroom = new ChatRoom;
	User* _user=user ;
	//string name;
	string owner;

	//name = readMsg(user->socket); //Chat Room name

	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		_user = users_iter->second;
		if (_user->socket == user->socket)
		{
			owner = _user->name;
			break;
		}
	}
	chatroom->name = name;
	chatrooms_map[name] = chatroom;
	writeCommand(user->socket,CHATROOM_CREATED);
	_user->connectionStatus = true;
	_user->connectedToChatRoom = chatroom;
	chatroom->owner = owner;
	(chatroom->users).push_back(owner);
}
void Dispatcher::close_chatroom(User* user)//delete
{
	string  name;
	ChatRoom* chatroom = NULL;
	User* _user;
	bool found = false;
	string username;


	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		_user = users_iter->second;
		if (_user->socket == user->socket)
		{
			break;
		}
	}


	for (chatrooms_iter = chatrooms_map.begin() ; chatrooms_iter != chatrooms_map.end() ; chatrooms_iter++)
	{
		chatroom = chatrooms_iter->second;
		if (_user->name == chatroom->owner)
		{// if the user that asks to close the chatroom is the owner of the room -> send command
			//writeCommand(user->socket,CHATROOM_WAS_CLOSED);
			for (unsigned int i=0 ; i < (chatroom->users).size() ; i++) //broadcast msg
				{

					username = (chatroom->users)[i];
					TCPSocket* temp_sock = (users_map[username])->socket;

					if (temp_sock != NULL)
					{
						writeCommand( temp_sock,CHATROOM_WAS_CLOSED );
					}
					users_map[username]->connectionStatus=false;

				}
			found = true;
		}
	}

	if (found == false)
	{ // The user that is not the owner cannot close the room
		writeCommand(user->socket,CHATROOM_CLOSE_ERROR);
	}
	else
	{// If you are the owner delete the chatroom from the map
		chatrooms_map.erase(chatroom->name);
	}


}
void Dispatcher::join_chatroom(User* user)
{
	string  name;
	string username;
	string ip_and_port;
	ChatRoom* chatroom = NULL;
	User* _user;

	name = readMsg(user->socket);// chat room name

	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{//check if the socket is in the users map
		_user = users_iter->second;
		if (_user->socket == user->socket)
		{
			break;
		}
	}

	cout << _user->name << " wants to join " << name << endl;


	if ( chatrooms_map.find(name) != chatrooms_map.end() )
	{ //chatroom name found
		chatroom = chatrooms_map[name];
		for (unsigned int i=0 ; i < (chatroom->users).size() ; i++) //broadcast msg
		{
			username = (chatroom->users)[i];//take the name of the user
			TCPSocket* tempsock = (users_map[username])->socket;//take the user socket
			if (tempsock != NULL)
			{
				//sending each user of the room a message that there is a new user in chat room
				writeCommand(tempsock,NEW_USER_TO_CHATROOM);//print that there is new user in room to all users
				writeMsg(tempsock,_user->name);
				writeMsg(tempsock,_user->socket->destIpAndPort());
			}
		}
		//sending the users of chat room,to user in order to add them to his list
		writeCommand(user->socket,CHATROOM_LIST);
		writeCommand(user->socket,chatroom->users.size());


		for (unsigned int i=0 ; i < (chatroom->users).size() ; i++) //broadcast msg
		{
			writeMsg(user->socket,(chatroom->users)[i]);
			writeMsg(user->socket , (users_map[(chatroom->users)[i]])->socket->destIpAndPort());
		}

		chatroom->add_user(_user->name);
		_user->connectedToChatRoom = chatroom;
		_user->connectionStatus = true;
		users_map[_user->name] = _user;
	}
	else
	{
		this->create_chatroom(user,name);
		cout << _user->name << " opening a new chatroom " << name << endl;
	}



}
void Dispatcher::leave_chatroom(User* user )
{
	string  name;
	string username;
	ChatRoom* chatroom = NULL;
	User* _user;

	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		_user = users_iter->second;
		if (_user->socket == user->socket)
		{
			break;
		}
	}


	if (_user->connectionStatus == true)
	{
		if (_user->connectedToChatRoom != NULL)
		{

			chatroom = _user->connectedToChatRoom;
			if (chatroom->owner == _user->name)//if the user who want to disconnect is the owner of the room
			{
				//writeCommand(user->socket,CHATROOM_WAS_CLOSED);
				//writeCommand(user->socket,LEAVE_CHATROOM_ERROR);
				for (unsigned int i=0 ; i < (chatroom->users).size() ; i++) //broadcast msg
				{
					username = (chatroom->users)[i];
					TCPSocket* temp_sock = (users_map[username])->socket;

					if (temp_sock != NULL)
					{
						writeCommand( temp_sock,CHATROOM_WAS_CLOSED );
					}
					//Disconnect each user from the chat room
					users_map[username]->connectionStatus=false;
					users_map[username]->connectedToChatRoom=NULL;
				}

				chatrooms_map.erase(chatroom->name);
			}
			else
			{
				for (unsigned int i=0 ; i < (chatroom->users).size() ; i++) //broadcast msg
				{
					username = (chatroom->users)[i];
					TCPSocket* temp_sock = (users_map[username])->socket;

					if (temp_sock != NULL)
					{
						writeCommand( temp_sock,USER_HAS_LEFT_CHATROOM );
						writeMsg(temp_sock,_user->name);
					}

				}
				_user->connectedToChatRoom = NULL;
				_user->connectionStatus = false;
			}

			chatroom->remove_user(_user->name);
			cout << "removed: " << _user->name << " from:" << chatroom->name << endl;
			}
		else
		{
			writeCommand(user->socket,OPEN_SESSION_ERROR2);
		}
	}
	else
	{
		writeCommand(user->socket,CONNECTION_ERROR);
	}
}
void Dispatcher::list_users(User* user)
{
	string user_name;
	writeCommand(user->socket,USERS_LIST);
	writeCommand(user->socket,users_map.size());
	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		user_name = users_iter->first;
		writeMsg(user->socket,user_name);
	}
}
void Dispatcher::list_chatrooms(User* user)
{
	string chatroom_name;
	writeCommand(user->socket,CHATROOMS_LIST);
	writeCommand(user->socket,chatrooms_map.size());
	for (chatrooms_iter = chatrooms_map.begin() ; chatrooms_iter != chatrooms_map.end() ; chatrooms_iter++)
	{
		chatroom_name = chatrooms_iter->first;
		writeMsg(user->socket,chatroom_name);
	}
}

void Dispatcher::list_chatroom_users(User* user)
{
	string  name;
	ChatRoom* chatroom = NULL;

	name = readMsg(user->socket);

	if (user->connectionStatus == false)
	{

		if ( chatrooms_map.find(name) != chatrooms_map.end() ) //chatroom name found
		{
			chatroom = chatrooms_map[name];
			writeCommand(user->socket,CHATROOM_USERS_LIST);
			writeCommand(user->socket,chatroom->users.size());

			for (unsigned int i=0 ; i < chatroom->users.size() ; i++)
			{
				writeMsg(user->socket,(chatroom->users)[i]);

			}

		}
		else
		{
			writeCommand(user->socket,CHATROOM_NAME_ERROR);
		}
	}
	else
	{
		writeCommand(user->socket,OPEN_CONNECTION_ERROR);
	}

}
void Dispatcher::user_exit(User* user)
{
	ChatRoom* chatroom = NULL;
	string name;

	//check if user is in chatroom or is owner

	User* _user;

	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		_user = users_iter->second;
		if (_user->socket == user->socket)
		{
			break;
		}
	}

	if (_user->connectionStatus == true)
	{
		if (_user->connectedToChatRoom != NULL) //user is in a chatroom
		{
			//check if user is owner
			chatroom = _user->connectedToChatRoom;
			if (chatroom->owner == _user->name)
			{
				//
				for (unsigned int i=0 ; i <chatroom->users.size() ; i++) //broadcast msg
				{
					name = (chatroom->users)[i];
					TCPSocket* tempsock = (users_map[name])->socket;
					writeCommand(tempsock,CHATROOM_WAS_CLOSED);
				}
				chatroom->close();
				chatrooms_map.erase(chatroom->name);
			}
			else
			{
				chatroom->remove_user(_user->name);
				for (unsigned int i=0 ; i <chatroom->users.size() ; i++) //broadcast msg
				{
					name = (chatroom->users)[i];
					TCPSocket* tempsock = (users_map[name])->socket;
					writeCommand(tempsock,USER_HAS_LEFT_CHATROOM);
					writeMsg(tempsock,_user->name);
				}
			}
		}
		else //user is in a session
		{
			close_session(_user);
		}
	}

	writeCommand(_user->socket,BYE_BYE_MSG);


	//remove user from map
	cout << _user->name << " has disconnected" << endl;
	users_map.erase(_user->name);


}


void Dispatcher::printUsers()
{
	if (pthread_mutex_lock(&users_map_mutex)!=0)
	{
		printf("Error locking mutex");
		exit(1);
	}

	if (users_map.empty() == true)
	{
		cout <<"There are no connected users" << endl;
	}
	else
	{
		for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
		{
			cout << "Connected user: " << users_iter->first << endl;
		}
	}
	if (pthread_mutex_unlock(&users_map_mutex)!=0)
	{
		printf("Error unlocking mutex");
		exit(1);
	}

	/*
	if (users_map.size() > 0)
	{
		cout << "Users:" <<  endl;
		for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
		{
			cout << users_iter->first << endl;
		}
	}
	else
	{
		cout << "There are no logged in users" << endl;
	}
	 */
}
void Dispatcher::ptintSessions()
{
	if (in_session_with.size() != 0)
	{
		cout << "Sessions:" << endl;
		for (in_session_with_iter = in_session_with.begin() ; in_session_with_iter != in_session_with.end() ; in_session_with_iter++)
		{
			cout << in_session_with_iter->first;
			cout << " is in session with ";
			cout << in_session_with_iter->second << endl;
		}
	}
	else
	{
		cout << "There are no active sessions at the moment" << endl;
	}
}
void Dispatcher::printChatRooms()
{
	if (chatrooms_map.size() != 0)
	{
		cout << "Chatrooms:"  << endl;
		for (chatrooms_iter = chatrooms_map.begin() ; chatrooms_iter != chatrooms_map.end() ; chatrooms_iter++)
		{
			cout << chatrooms_iter->first << endl;
		}
	}
	else
	{
		cout << "There are no open chatrooms at this moment" << endl;
	}
}
void Dispatcher::printUsersInChatRoom()
{
	string name;
	cin >> name;
	if (chatrooms_map.size() != 0)
	{
		if ( chatrooms_map.find(name) != chatrooms_map.end() ) //target name was found
		{
			cout << "Users in " << chatrooms_map[name]->name + ":" << endl;
			for (unsigned int i=0 ; i< (chatrooms_map[name]->users).size() ; i++ )
			{
				cout << (chatrooms_map[name]->users)[i] << endl;
			}
		}
		else
		{
			cout << "A chatroom with that name doesn't exist" << endl;
		}

	}
	else
	{
		cout << "There are no open chatrooms at this moment" << endl;
	}
}

bool Dispatcher::checkUser(string name)
{
	return users_map.find(name) != users_map.end();//if there are no such name return the iterator::end
}

void Dispatcher::close()
{
	User* _user;
	status = false;
	for (users_iter = users_map.begin() ; users_iter != users_map.end() ; users_iter++)
	{
		_user = users_iter->second;
		writeCommand(_user->socket,CHATROOM_IS_CLOSING);
		cout << users_iter->first << endl;
	}
	if (listener!=NULL)
	{
		delete(listener);
	}


}

Dispatcher::~Dispatcher() {

}

