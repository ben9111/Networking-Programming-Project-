

#include "Client.h"

#include "Protocol.h"



const string MSG_FROM_SERVER =  "Got msg from server: ";
const string EXIT_SESSION = "to exit current session or chat room enter cs\n";

void print_instructions();

int main() {

	string command,username,password,roomName;
	string ip,peerUsr,msg;
	Client client;

	cout << "Welcome Dear User\nthis is your new messenger\nPlease enter a command by the following:" << endl;


	print_instructions();

	//waiting for commands from cli
	do
	{
		command.clear();
		cin >> command;

		if (command == "c") //connect to server
		{
			cin >> ip;
			if (client.connectToServer(ip,SERVER_PORT) == false)
			{
				cout << "Could not connect to sever" <<  endl;
			}
			else
			{
				client.isConnectedToServer = true;
			}
			continue;
		}
		else if (command == "login") //login to server
		{
			cin >> username;
			cin >> password;
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			else if(client.isLoggedIn==false)
			{
				client.login(username,password);
				sleep(5);
			}
			else
				cout<<"you are already logged in"<<endl;
		}
		else if (command == "register") //signup to server
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<endl;
				continue;
			}
			cin >> username;
			cin >> password;
			client.signup(username,password);
		}
		else if (command == "lu") //list all users from server
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			client.getUsers();
		}
		else if (command == "lcu") //list all connected users
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			client.listAllUsers();
		}
		else if (command == "lr") //list all chatrooms from server
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			client.listAllChatrooms();
		}
		else if (command == "lru") //print a chatroom`s user-list
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			cin>>roomName;
			client.listChatroomUsers(roomName);
		}
		else if (command == "o") //open a session with a user
		{
			cin>>peerUsr;
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			else if(client.isInSession==true )
			{
				cout << "ERROR - already in session with other user or room" <<EXIT_SESSION<<  endl;
				continue;
			}
			else
			{
				client.openSession(username,peerUsr);
			}
		}
		else if (command == "cs") //close the current session/chatroom
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			client.closeSession();
			//client.DeleteChatRoom(roomName);
			//client.ExitRoom();
		}

		else if (command == "or") //create (open)  a new chatroom
		{
			cin>>roomName;
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
		    else if(client.isInSession==true )
		    {
				cout << "ERROR - already in session with other user or room" <<EXIT_SESSION<<  endl;
			}
		    else
			{
				client.CreateChatRoom(roomName);
				client.chatName=roomName;
			}
		}
		else if (command == "s") //send a msg to user in session
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}

			else if(client.isInSession == true){
				getline(std::cin,msg);
				if(msg.size()>0 && msg[0]==' ')
					msg.erase(0,1);
				client.sendMsgToSession(msg);
			}
			else
				cout<<"error"<<endl;
		}
		else if (command == "l") //print your connection status
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			else{
				if (client.sessionStatus())
					cout << "You are connected to " <<client.getName()<< endl;
				else if (client.chartoomStatus())
					cout <<  "You are connected to a chatroom to"  <<client.getName()<< endl;
				else
					cout << "Your are connected only to the server" <<  endl;
			}

		}
		else if (command == "d") //disconnect from server
		{
			if (client.isConnectedToServer == false)
			{
				cout << "ERROR - You are not connected to a server" <<  endl;
				continue;
			}
			cout<<"Disconnecting Server..."<<endl;
			client.disconnectFromServer();
		}

		else if (command == "x")//close app
		{
			client.close();
		}
		else if (command == "p")
		{
			print_instructions();
		}
		else
		{
			cout << "Bad command!" << endl;
		}

	}
	while(command != "x");

	return 0;
}

void print_instructions()
{
	cout << endl;
	cout << "c <ip> - connect to server" << endl;
	cout << "login <username> <password> - Login to server" << endl;
	cout << "register <username> <password> - Signup to server" <<  endl;
	cout << "lu - print all users from server" << endl;
	cout << "lcu - print all connected users from server" << endl;
	cout << "lr - print chatrooms list from server" << endl;
	cout << "lru <chatroom name> - print all chatroom users" << endl;
	cout << "o <username> - open a session with a user" << endl;
	cout << "cs - close the current session" << endl;
	cout << "or <roomname> - create a new chat room/join a chatroom if already exist" << endl;
	cout << "s <message> - send a message to session user" << endl;
	cout << "l - print connection status" << endl;
	cout << "d - disconnect from server" << endl;
	cout << "p - print instructions" << endl;
	cout << "x - close app" << endl;
	cout << endl;
}


Client::Client()
{

	server_sock = NULL;
	udp_sock = NULL;
	connectionStatus = false;
	isLoggedIn = false;
	isInSession = false;
	isInChatRoom = false;
	isRoomOwner = false;
	isConnectedToServer = false;
}

//get all users names from file
void Client::getUsers()
{
	writeCommand(server_sock,GET_ALL_USERS);
}
bool Client::connectToServer(string ip,int port)
{
	server_sock = new TCPSocket(ip,port);
	if (server_sock->getSocketFid() < 0)//socket construction failed
	{
		return false;
	}
	else
	{
		connectionStatus = true;
		this->start();
	}
	return true;
}
void Client::login(string name,string pass)
{
	writeCommand(server_sock,LOGIN);
	writeMsg(server_sock,name);
	writeMsg(server_sock,pass);

}
void Client::signup(string name,string pass)
{
	writeCommand(server_sock,SIGNUP);
	writeMsg(server_sock,name);
	writeMsg(server_sock,pass);
}


void Client::sendMsgToServer(string msg)
{
	writeMsg(server_sock,msg);
}
void Client::disconnectFromServer()
{
	connectionStatus = false;
	isInSession = false;
	isInChatRoom = false;
	isConnectedToServer = false;
	isLoggedIn=false;
	chatroom_partners.empty();
	session_partner.Clean();
	writeCommand(server_sock,EXIT);
	server_sock->cclose();

}
//get all connected users
void Client::listAllUsers()
{
	writeCommand(server_sock,GET_ALL_CONNECTED_USERS);
}
void Client::listAllChatrooms()
{
	writeCommand(server_sock,GET_ALL_CHATROOMS);

}
//list the users of the specified chat room
void Client::listChatroomUsers(string roomName)
{
	writeCommand(server_sock,GET_CHATROOM_USERS);
	writeMsg(server_sock,roomName);
}

void Client::openSession(string userName, string peerUsr)
{

	writeCommand(server_sock,OPEN_SESSION_WITH_USER);
	writeMsg(server_sock,userName);
	writeMsg(server_sock,peerUsr);
}
void Client::closeSession()
{
	if(isInSession)
	{
		if(isInChatRoom)
		{
				writeCommand(server_sock,LEAVE_CHATROOM);
		}
		else
		{
			writeCommand(server_sock,CLOSE_SESSION_WITH_USER);

			isInSession= false;
			cout<<"session has ended"<<endl;
		}
	}
	else
	{
		cout << "You don't have an open session" <<  endl;
	}
}

void Client::CreateChatRoom(string roomName)
{
	writeCommand(server_sock,OPEN_CHATROOM);
	writeMsg(server_sock,roomName);
}
//receiving the users names of chat room
void Client::gotChatroomUsersNames()
{
	int count;
	string name;

	count = readCommand(server_sock);

	for (int i=0 ; i <count ; i++)
	{
		name = readMsg(server_sock);
		cout << "Chatroom user #" << i+1 << ": " + name << endl;
	}
}
//receiving the name and ip and port users of chat room
void Client::gotChatroomUsersList()
{
	int count,int_port;
	string ip_and_port;
	char delim = ':';
	bool foundDelim = false;
	string port;



	count = readCommand(server_sock);

	for (int i=0 ; i<count ; i++)
	{

		Partner* partner = new Partner;
		partner->name = readMsg(server_sock);
		ip_and_port = readMsg(server_sock);
		foundDelim = false;
		port.clear();
		for (unsigned int i=0 ; i < ip_and_port.length() ; i++)
		{
			if (ip_and_port[i] != delim)
			{
				if (foundDelim == false)//still constructing the ip
					partner->ip += ip_and_port[i];
				else//already building the port
					port += ip_and_port[i];
			}
			else
			{
				foundDelim = true;
			}
		}

		//c_str:Returns a pointer to an array that contains a null-terminated sequence of characters representing the current value of the string object.
		int_port = atoi(port.c_str());//atoi:converts string to integer
		partner->port = int_port;
		isInSession = true;
		isInChatRoom = true;
		cout << "Chatroom partner: " << partner->name + "(" + partner->ip << ":" << partner->port << ")" << endl;
		chatroom_partners.push_back(partner); //add partner
	}

}

void Client::sendMsgToSession(string msg)
{
	if (isInSession == true)
	{
		if (isInChatRoom == true)
			{
				for (unsigned int i = 0 ; i < chatroom_partners.size() ; i++ )
				{
					udp_sock->sendTo("[" + client_name + "]" + ":" +  msg,chatroom_partners[i]->ip,chatroom_partners[i]->port);
				}

			}
		else
		{
			udp_sock->sendTo("[" + client_name + "]" +  msg,session_partner.ip,session_partner.port);
		}
	}
	else
	{
		cout << "You don't have an open session" <<  endl;
	}
}
void Client::gotIpAndPort()
{
	string msg,name;
	string port;
	char delim = ':';
	bool foundDelim = false;
	name = readMsg(server_sock);
	msg = readMsg(server_sock); //msg is ip & port
	session_partner.Clean();//clean the partner data
	session_partner.name=name;
	for (unsigned int i=0 ; i < msg.length() ; i++)
	{
		if (msg[i] != delim)
		{
			if (foundDelim == false)
				session_partner.ip += msg[i];
			else
				port += msg[i];
		}
		else
		{
			foundDelim = true;
		}
	}
	session_partner.port = atoi(port.c_str());//atoi-string to int
	cout << "You are now in session with ->" <<session_partner.name<< "(" +  msg + ")" << endl;
	isInSession = true;

}
//when logged in,construct udp socket and listen to incoming messages
void Client::loggedIn()
{

	string ip_and_port;
	string ip,port;
	char delim = ':';
	bool foundDelim = false;
	int int_port;

	client_name = readMsg(server_sock);
	ip_and_port = readMsg(server_sock);

	isLoggedIn = true;

	for (unsigned int i=0 ; i < ip_and_port.length() ; i++)
	{
		if (ip_and_port[i] != delim)
		{
			if (foundDelim == false)
				ip += ip_and_port[i];
			else
				port += ip_and_port[i];
		}
		else
		{
			foundDelim = true;
		}
	}

	int_port = atoi(port.c_str());
	udp_sock = new Peer2Peer(int_port);

}
bool Client::ConnectionStatus()
{
	return connectionStatus;
}
bool Client::sessionStatus()
{
	return isInSession;
}
bool Client::chartoomStatus()
{
	return isInChatRoom;
}

void Client::run()
{
	int command;
	string username,chatroom_name;
	string pass;
	string ip_and_port;
	string msg;

	while (connectionStatus)
	{
		command = readCommand(server_sock);

		if (!command)
			continue;

		switch(command)
		{
		case LOGIN_OR_SIGNUP://sign up or login complited succesfully
			cout << "Connection established\nLogin or register to enter a chat room or establish a session" <<  endl;
			break;
		case LOGIN_DENIED:
			cout << MSG_FROM_SERVER + "Bad username or password" << endl;
			break;
		case USERNAME_TAKEN:
			cout <<  MSG_FROM_SERVER + "The username you entered is already taken" << endl;
			break;
		case ALL_USERS_LIST://the users list is ready(from file)
			gotAllUsers();
			break;
		case CONFIRM_USER:
			cout << MSG_FROM_SERVER + "You are now logged in" << endl;
			loggedIn();
			break;
		case BAD_COMMAND:
			cout << MSG_FROM_SERVER +  "You need to login/register first" <<  endl;
			break;
		case INCOMIMG_SESSION:
			gotIncomingSession();
			break;
		case OPEN_SESSION_ERROR:
			cout << MSG_FROM_SERVER + "Error opening session " << endl;
			isInSession=false;
			break;
		case CLOSE_SESSION_ERROR:
			cout << MSG_FROM_SERVER + "You are not in session" << endl;
			isInSession=false;
			break;
		case ENTER_CHATROOM_NAME:
			cout << MSG_FROM_SERVER + "Enter chatroom name" << endl;
			isInSession=true;
			//	gotEnterChatroomName();
			break;
		case CHATROOM_CREATED:
			cout << MSG_FROM_SERVER + "Chatroom was created" << endl;
			isInChatRoom = true;
			isRoomOwner = true;
			isInSession=true;
			break;
		case CHATROOM_WAS_CLOSED:
			chatroomWasClosed();
			isInSession=false;
			//cout << MSG_FROM_SERVER + "Chatroom was closed" << endl;
			break;
		case CHATROOM_CLOSE_ERROR:
			isInSession=false;
			cout << MSG_FROM_SERVER + "Can`t close chatroom, you are not the owner of any room" << endl;
			break;
		case CREATE_CHATROOM_NAME_ERROR:
			isInSession=false;
			cout << MSG_FROM_SERVER + "Chatroom name already exists" << endl;
			chatName.clear();
			break;
		case CHATROOM_NAME_ERROR:
			isInSession=false;
			cout << MSG_FROM_SERVER + "A chatroom with that name doesn't exists" << endl;
			break;
		case CLOSE_CHATROOM_OWNER_ERROR:
			isInSession=false;
			cout << MSG_FROM_SERVER + "You are not the owner of this chatroom" << endl;
			break;
		case OPEN_CONNECTION_ERROR:
			cout << MSG_FROM_SERVER + "The user is already in session or chatroom\nTry again Later..." << endl;
			break;
		case SESSION_ENDED:
			cout << MSG_FROM_SERVER + "Session has ended" << endl;
			isInSession = false;
			break;
		case NEW_USER_TO_CHATROOM:
			userHasJoinedChatroom();
			break;
		case USER_HAS_LEFT_CHATROOM:
			userHasLeftChatroom();
			//cout << MSG_FROM_SERVER + "User has left the chatroom" << endl;
			break;
		case LEAVE_CHATROOM_ERROR:
			cout << MSG_FROM_SERVER + "You are the owner of the chatroom, you must close it" <<  endl;
			break;
		case OPEN_SESSION_ERROR2:
			cout << MSG_FROM_SERVER + "You have an open session" << endl;
			break;
		case CONNECTION_ERROR:
			cout << MSG_FROM_SERVER + "You don't have any open connections" << endl;
			break;
		case CHATROOM_IS_CLOSING:
			closeConnection();
			cout << MSG_FROM_SERVER + "Server is Shutdown" << endl;
			break;
		case USERS_LIST://all the users in file
			gotUsersList();
			break;
		case CHATROOMS_LIST://the names of the chat rooms
			gotChatroomsList();
			break;
		case CHATROOM_LIST://the names and ip and port of people in chat room
			gotChatroomUsersList();
			break;
		case TARGET_IP_AND_PORT:
			gotIpAndPort();
			break;
		case CHATROOM_USERS_LIST://the names of people in chat room
			gotChatroomUsersNames();
			break;
		case BYE_BYE_MSG:
			cout << MSG_FROM_SERVER + "Bye Bye" << endl;
			break;
		//default:
			//cout << " Got UNKNOWN command from server" << endl;
		}
	}
}
//now connected to some session
void Client::gotIncomingSession()
{
	string ip_and_port;
	string ip,port,name;
	int int_port;
	char delim = ':';
	bool found = false;

	name = readMsg(server_sock);
	ip_and_port = readMsg(server_sock);
	cout << MSG_FROM_SERVER + "You have an incoming session from: " + name + "(" + ip_and_port + "), use 's <message>' to send a msg" <<endl;
	isInSession = true;

	//add details to session partner
	for (unsigned int i = 0 ; i < ip_and_port.length() ; i++)
	{
		if (ip_and_port[i] != delim)
		{
			if (found == false)
				ip += ip_and_port[i];
			else
				port += ip_and_port[i];
		}
		else
		{
			found = true;
		}
	}
	int_port = atoi(port.c_str());
	session_partner.name = name;
	session_partner.ip = ip;
	session_partner.port = int_port;
}

void Client::userHasLeftChatroom()
{
	string name;
	unsigned int i;

	name = readMsg(server_sock);

	if (name == client_name)
	{
		cout<<"you left the chat room"<<endl;
		isInSession=false;
		isInChatRoom=false;
		isRoomOwner=false;
		chatroom_partners.clear();
		chatName.clear();
		return;
	}

	for  (i = 0 ; i < chatroom_partners.size() ; i++)
	{
		if ((chatroom_partners[i])->name == name)
		{
			break;
		}
	}
	cout << name << " Has left the chatroom" << endl;
	chatroom_partners.erase(chatroom_partners.begin() + i);

}
//all the users in file
void Client::gotUsersList()
{
	int usersCount;
	string userName;

	usersCount = readCommand(server_sock); //size of list
	for (int i=0 ; i<usersCount ; i++)
	{
		userName = readMsg(server_sock);
		cout << "User #" << i+1 << ": " << userName << endl;
	}
}

void Client::chatroomWasClosed()//delete
{
	if (isRoomOwner == true)
	{
		cout << "Your room was closed" << endl;
		isRoomOwner = false;
		isInChatRoom = false;
		chatroom_partners.clear();
		chatName.clear();
	}
	else
	{
	cout << "The chatroom was closed by the owner" << endl;
	isInChatRoom = false;
	chatroom_partners.clear();
	chatName.clear();
	}
}
void Client::userHasJoinedChatroom()
{
	string name;
	string ip_and_port;
	string port;
	char delim = ':';
	bool foundDelim = false;

	Partner* partner = new Partner;


	name = readMsg(server_sock);
	ip_and_port = readMsg(server_sock);

	port.clear();
	for (unsigned int i = 0 ; i < ip_and_port.length() ; i++)
	{
		if (ip_and_port[i] != delim)
		{
			if (foundDelim != true)
				partner->ip += ip_and_port[i];
			else
				port += ip_and_port[i];
		}
		else
		{
			foundDelim = true;
		}
	}
	partner->name = name;
	partner->port = atoi(port.c_str());
	cout << "New user: " << partner->name + "(" + partner->ip << ":" << partner->port << ")" << endl;
	chatroom_partners.push_back(partner);

}
//receiving the names of users in file
void Client::gotAllUsers()
{
	int size;
	string name;
	size = readCommand(server_sock);

	cout << "*** ALL USERS ***" << endl;

	for (int i=0 ; i < size ; i++)
	{
		name = readMsg(server_sock);
		cout << "User #" << i+1 << ": " << name << endl;
	}
}
//the names of chat rooms
void Client::gotChatroomsList()
{
	int roomsCount;
	string roomName;

	roomsCount = readCommand(server_sock); //size of list

	if (roomsCount == 0)
	{
		cout << "There are no open rooms at the moment" << endl;
	}
	else
		for (int i=0 ; i<roomsCount ; i++)
		{
			roomName = readMsg(server_sock);
			cout << "Chatroom #" << i+1 << ": " << roomName << endl;
		}

}
void Client::closeConnection()
{

		connectionStatus = false;
		isInSession = false;
		isInChatRoom = false;
		isConnectedToServer = false;
		isLoggedIn=false;
		chatroom_partners.empty();
		session_partner.Clean();
		server_sock->cclose();


}
void Client::close()
{
	if(isConnectedToServer==true)
		disconnectFromServer();
}
//return chat room name if the user is in chat room,return the partner name if in session
string Client::getName()
{
	if(isInChatRoom)
		return chatName;
	else
		return session_partner.name;
}
Client::~Client()
{
	if(server_sock!=NULL)
     	close();
}
