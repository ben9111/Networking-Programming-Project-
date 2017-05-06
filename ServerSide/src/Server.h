

#ifndef SERVER_H_
#define SERVER_H_


#include "LoginAndSignUp.h"
#include "Common.h"


class Server : public MThread {

	TCPSocket* listen_sock; //listen socket
	LoginAndSignUp* loginAndSign; //all incoming connection are handled by this object
	bool status;

public:
	Server();
	void printAllUsers();
	void printUsers();
	void ptintSessions();
	void printChatRooms();
	void printUsersInChatRoom();
	void close();

	virtual ~Server();
	void run();

};


#endif /* SERVER_H_ */
