
#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */



using namespace std;

#define SERVER_PORT 3346

#define CLOSE_SESSION_WITH_USER 	1
#define OPEN_SESSION_WITH_USER 		2
#define EXIT						3
#define SEND_MSG_TO_PEER			4
#define SESSION_REFUSED				5
#define SESSION_ESTABLISHED			6

#define LOGIN						7
#define SIGNUP 						8
#define OPEN_CHATROOM				9
#define CLOSE_CHATROOM 				10
#define JOIN_CHATROOM 				11
#define LEAVE_CHATROOM 				12
#define GET_ALL_USERS				13
#define ENTER_USERNAME				14
#define ENTER_PASSWORD				15
#define CONFIRM_USER 				16 //login was accomplished
#define LOGIN_DENIED 				17
#define USERNAME_TAKEN 				18
#define BAD_COMMAND 				19
#define ENTER_USER_NAME 			21	 //"Enter user name: "
#define INCOMIMG_SESSION 			22 	//"Incoming session from: "
#define OPEN_SESSION_ERROR 			23 	//"Username not found "
#define CLOSE_SESSION_ERROR 		24 	//"You are not in session "
#define ENTER_CHATROOM_NAME 		25 	//"Enter a name for new chatroom "
#define CREATE_CHATROOM_NAME_ERROR 	26	// "A chatroom with that name already exists "
#define CHATROOM_NAME_ERROR 		27 	//"A chatroom with that name doesn't exists "
#define CLOSE_CHATROOM_OWNER_ERROR 	28 	//"You are not the owner of this chatroom"
#define OPEN_CONNECTION_ERROR 		29 	//"You have an open connection"
#define SESSION_ENDED 				30
#define NEW_USER_TO_CHATROOM 		31
#define USER_HAS_LEFT_CHATROOM 		32
#define OPEN_SESSION_ERROR2			33 //"You have an open session"
#define CONNECTION_ERROR 			34 //"You don't have any open connections"
#define CHATROOM_IS_CLOSING 		35
#define BYE_BYE_MSG 				36
#define GET_ALL_CHATROOMS 			37
#define GET_CHATROOM_USERS 			38
#define LEAVE_CHATROOM_ERROR 		39
#define CHATROOM_LIST 				40
#define TARGET_IP_AND_PORT 			41
#define LOGIN_OR_SIGNUP 			42 //now the client can sign up or login
#define USERS_LIST 					43
#define CHATROOMS_LIST 				44
#define CHATROOM_CREATED 			45
#define CHATROOM_WAS_CLOSED 		46
#define CHATROOM_CLOSE_ERROR 		47
#define CHATROOM_USERS_LIST 		48
#define ALL_USERS_LIST 				49
#define GET_ALL_CONNECTED_USERS		50
#define CLOSE_CONNECTION			51

#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"


#endif /* TCPMESSENGERPROTOCOL_H_ */
