

#include "Server.h"
#include "Protocol.h"

void print_instructions();

int main() {

	Server server;
	string command;

	cout << "*** WELCOME TO THE SERVER APPLICATION ***" << endl;

	print_instructions();

	do
	{
		cin >> command;

		if (command == "lcu") //list all connected users
		{
		server.printUsers();
		}
		else if (command == "lu") // list all users
		{
			server.printAllUsers();
		}
		else if (command == "ls") //list all sessions
		{
			server.ptintSessions();
		}
		else if (command == "lr") //list all chatrooms
		{
			server.printChatRooms();
		}
		else if (command == "lru") //list all users in a chatroom
		{
			server.printUsersInChatRoom();
		}
		else if (command == "p")
		{
			print_instructions();
		}
		else if (command == "x") //close the app
		{
			server.close();
		}
		else
		{
			cout << "** BAD COMMAND **" << endl;
		}
	}
	while (command != "x");


	return 0;
}

void print_instructions()
{

	cout << endl;
	cout << "Your Commands:" << endl;
	cout << "lu - list all users" << endl;
	cout << "lcu - list all connected users" <<  endl;
	cout << "ls - list all sessions" << endl;
	cout << "lr - list all chatrooms" << endl;
	cout << "lru <room name> - list all users in a chatroom" << endl;
	cout << "p - print commands" << endl;
	cout << "x - close the app" << endl;
	cout << endl;
}

Server::Server() :loginAndSign(new LoginAndSignUp){

	listen_sock = NULL;

	status = false;
	this->start();

}

void Server::printAllUsers()
{

	vector<string> names = loginAndSign->getAllUsers();

	cout << "*** All users list ***" << endl;
	for (unsigned int i=0 ; i< names.size() ; i++)
	{
		cout << "User #" << i+1 << ": " <<  names[i] << endl;
	}
}

void Server::printUsers()
{
	loginAndSign->printUsers();
}
void Server::ptintSessions()
{
	loginAndSign->printSessions();
}
void Server::printChatRooms()
{
loginAndSign->printChatRooms();
}
void Server::printUsersInChatRoom()
{
	loginAndSign->printUsersInChatRoom();
}

void Server::run()
{

	status = true;
	listen_sock = new TCPSocket(SERVER_PORT);
	sleep(2);
	cout << "Server is listening on port " << SERVER_PORT << endl;

	while (status == true)
	{

		TCPSocket* temp_sock = listen_sock->listenAndAccept();

		if (!temp_sock)
			continue;
		else
		{
			loginAndSign->addPeer(temp_sock);
			//delete (temp_sock);
		}
	}
	cout << "Server stopped running" << endl;
}

void Server::close()
{

	cout << "Server application in closing ..." << endl;
	status = false;
	loginAndSign->close();
	loginAndSign->waitForThread();
	listen_sock->cclose();
	//listen_sock->waitForThread();
	delete(loginAndSign);
	if (listen_sock!=NULL)
		delete(listen_sock);



	}

Server::~Server() {

}
