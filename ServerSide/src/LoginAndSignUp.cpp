

#include "LoginAndSignUp.h"
#include "Protocol.h"
#include <fstream>

LoginAndSignUp::LoginAndSignUp()
{

	dispatcher = new Dispatcher(getAllUsers());
	sockets = NULL;
	status = false;
	//ofstream writeToFile;
	//writeToFile.open("users.txt");
	//writeToFile.close();

}

LoginAndSignUp::~LoginAndSignUp()
{

}
void LoginAndSignUp::close()
{
	dispatcher->close();
	dispatcher->waitForThread();
	status = false;
	if(dispatcher!=NULL)
	delete(dispatcher);
	if(sockets!=NULL)
		delete (sockets);
}

void LoginAndSignUp::run()
{
	int command;
	string username,password;

	status = true;

	while (status == true)
	{
		//mutex lock
		TCPSocket* ready_peer;
		sockets = new MultipleTCPSocketsListener();
		sockets->addSockets(peers);

		ready_peer = sockets->listenToSocket(2);

		delete(sockets);
		/*if (ready_peer != NULL)
		{
			cout << "peer is ready" <<  endl;
		}
		else
		{
			cout << "error in ready peer" << endl;
			continue;
		}
		 */

		if (ready_peer != NULL)
		{
			command = readCommand(ready_peer);
		}
		else
			continue;

		if (command == 0)
		{
			cout << "error in ready peer" << endl;
			continue;
		}
		else if (command == LOGIN)
		{
			username = readMsg(ready_peer);
			password = readMsg(ready_peer);
			if  (login(ready_peer,username,password) == true)
			{
				writeCommand(ready_peer,CONFIRM_USER);
				writeMsg(ready_peer,username);
				writeMsg(ready_peer,ready_peer->destIpAndPort());
				dispatcher->add_user(ready_peer,username);
				//peers.clear();
				iter = find(peers.begin(), peers.end(), ready_peer);
				if (iter != peers.end()) //peer was found
				{
					peers.erase(iter);
				}

			}
			else
			{
				writeCommand(ready_peer,LOGIN_DENIED);
			}
		}
		else if (command == SIGNUP)
		{
			username = readMsg(ready_peer);
			password = readMsg(ready_peer);
			if (signup(ready_peer,username,password) == true)
			{
				writeCommand(ready_peer,CONFIRM_USER);
				writeMsg(ready_peer,username);
				writeMsg(ready_peer,ready_peer->destIpAndPort());

				dispatcher->add_user(ready_peer,username);
				dispatcher->add_user_to_vector(username);
				iter = find(peers.begin(), peers.end(), ready_peer);
				if (iter != peers.end()) //peer was found
				{
					peers.erase(iter);
				}
			}
			else
			{
				writeCommand(ready_peer,USERNAME_TAKEN);
			}
		}
		else
		{
			writeCommand(ready_peer,BAD_COMMAND);
		}

	}
}

void LoginAndSignUp::addPeer(TCPSocket* peer)
{
	peers.push_back(peer);
	writeCommand(peer,LOGIN_OR_SIGNUP);//now the client can sign up or login
	if (status == false)
	{
		status = true;
		this->start();
	}

}

void LoginAndSignUp::printUsers()
{
	dispatcher->printUsers();
}
void LoginAndSignUp::printSessions()
{
	dispatcher->ptintSessions();
}
void LoginAndSignUp::printChatRooms()
{
	dispatcher->printChatRooms();
}
void LoginAndSignUp::printUsersInChatRoom()
{
	dispatcher->printUsersInChatRoom();
}

vector<string> LoginAndSignUp::getAllUsers()
{
	vector<string> names;
	string line;
	string name;
	bool found = false;
	char delim = ' ';
	ifstream readFromFile;

	readFromFile.open("users.txt");
	if (!readFromFile)
	{
		cout << "Unable to open file " << readFromFile <<  endl;
		return names;
	}

	while (getline(readFromFile,line))
	{
		name.clear();
		for (unsigned int i=0 ; i < line.length() ; i++)
		{
			if (line[i] != delim)
			{
				name += line[i];
			}
			else
			{
				break;
			}
		}
		names.push_back(name);
	}

	return names;
}

bool  LoginAndSignUp::login(TCPSocket* peer,string name,string pass)
{
	string file_line;
	string user_and_pass;
	ifstream readFromFile;
	bool flag = false;//if found password and name
	bool nameExist = false;

	readFromFile.open("users.txt");
	if (!readFromFile)
	{
		cout << "Unable to open file " << readFromFile <<  endl;
		return false;
	}

	user_and_pass = name + " " + pass;

	while (flag == false && getline(readFromFile,file_line))
	{
		if (file_line == user_and_pass)
		{
			flag = true;
			break;
		}

	}
	nameExist = dispatcher->checkUser(name);

	if (flag == true && nameExist == false) //name was found and isn't logged in yet
	{
		return true;
	}
	else
		return false;

}
bool LoginAndSignUp::signup(TCPSocket* peer,string name,string pass)
{
	ofstream writeToFile;
	ifstream readFromFile;
	string line;
	string line_name;
	bool flag = false;

	readFromFile.open("users.txt" );

	/*if (!readFromFile)
	{
		cout << "Unable to open file " << readFromFile <<  endl;
		return false;
	}*/

	while (flag == false && getline(readFromFile,line))
	{
		for (unsigned int i = 0 ; i < line.length() ; i++)
		{
			if (line[i] != ' ')
			{
				line_name += line[i];
			}
			else
			{
				if (line_name.compare(name)==0)
				{
					flag = true;//a user with the same name already exists
				}
				line_name.clear();
				break;
			}

		}
	}

	if (flag == true) //user is taken
	{
		return false;
	}
	readFromFile.close();
	line = name + " " + pass;

	writeToFile.open("users.txt", ios::out | ios::app);
	if (!writeToFile)
	{
		cout << "Unable to open file: " << writeToFile << endl;
	}

	writeToFile << line << "\n";
	writeToFile.close();
	return true;
}







