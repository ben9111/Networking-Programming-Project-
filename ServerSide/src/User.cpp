

#include "User.h"

User::User() {
	socket = NULL;
	connectionStatus = false;
	connectedToUser = NULL;
	connectedToChatRoom = NULL;
}

User::~User() {
	// TODO Auto-generated destructor stub
}
