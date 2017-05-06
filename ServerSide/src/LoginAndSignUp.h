

#ifndef LOGINANDSIGNUP_H_
#define LOGINANDSIGNUP_H_

#define LOGIN_OR_SIGNUP 			42

#include "Dispatcher.h"
#include "Common.h"

class LoginAndSignUp : public MThread {

	vector <TCPSocket*> peers;
	vector <TCPSocket*>::iterator iter;

	MultipleTCPSocketsListener* sockets; //handle multiple clients
	Dispatcher* dispatcher; //once the user is logged in he 'moves on' to the dispatcher
	bool status;

public:
	LoginAndSignUp();

	void addPeer(TCPSocket* peer);
	bool login(TCPSocket* peer,string name,string pass);//return true-if name was found but not logged in,false-name already logged in
	bool signup(TCPSocket* peer,string name,string pass);

	void printUsers();
	vector<string> getAllUsers();
	void printSessions();
	void printChatRooms();
	void printUsersInChatRoom();


	void close();
	void run();


	virtual ~LoginAndSignUp();
};

#endif /* LOGINANDSIGNUP_H_ */
