

#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include "TCPSocket.h"
using namespace std;

TCPSocket::TCPSocket(int connected_sock,struct sockaddr_in serverAddr,struct sockaddr_in peerAddr){
	this->serverAddr =  serverAddr;
	this->peerAddr =  peerAddr;
	socket_fd = connected_sock;
}


TCPSocket::TCPSocket(int port){
	/**
	 * int socket(int domain, int type, int protocol);
	 * creates a TCP socket
	 * AF_INET - IPv4 Internet protocols
	 * SOCK_STREAM - TCP
	 * 0 - default protocol type
	 */
	socket_fd = socket (AF_INET, SOCK_STREAM, 0);

	// clear the s_in struct
	bzero((char *) &serverAddr, sizeof(serverAddr));  /* They say you must do this    */

	//sets the sin address
	serverAddr.sin_family = (short)AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);    /* WILDCARD */
	serverAddr.sin_port = htons((u_short)port);

	//bind the socket on the specified address
	printf("TCP server binding...\n");
	if (bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror ("Error naming channel");
	}
}


TCPSocket::TCPSocket(string peerIp, int port){
	cout<<"Opening new client socket"<<endl;

	/**
	 * int socket(int domain, int type, int protocol);
	 * creates a TCP socket
	 * AF_INET - IPv4 Internet protocols
	 * SOCK_STREAM - TCP
	 * 0 - default protocol type
	 */
	socket_fd = socket (AF_INET, SOCK_STREAM, 0);

	// clear the s_in struct
	bzero((char *) &peerAddr, sizeof(peerAddr));

	//sets the sin address,sets the address of the peer we send to
	peerAddr.sin_family = (short)AF_INET;
	peerAddr.sin_addr.s_addr = inet_addr(peerIp.data());
	peerAddr.sin_port = htons((u_short)port);

	/**
	 * int connect(int socket, const struct sockaddr *address,socklen_t address_len);
	 * The connect() function shall attempt to make a connection on a socket.
	 * socket-Specifies the file descriptor associated with the socket.My socket file descriptor
	 * address-Points to a sockaddr structure containing the peer address.
	 * address_len-Specifies the length of the sockaddr structure pointed to by the address argument.
	 */
	if (connect(socket_fd, (struct sockaddr *)&peerAddr, sizeof(peerAddr)) < 0)
	{
		perror ("Error establishing communications");
		close(socket_fd);
	}
}


TCPSocket* TCPSocket::listenAndAccept(){
	int rc = listen(socket_fd, 1);
	if (rc<0){
		return NULL;
	}
	socklen_t len = sizeof(peerAddr);
	bzero((char *) &peerAddr, sizeof(peerAddr));

	int connect_sock = accept(socket_fd, (struct sockaddr *)&peerAddr, &len);
	if (connect_sock<0)return NULL;
	return new TCPSocket(connect_sock,serverAddr,peerAddr);
}


int TCPSocket::recv(char* buffer, int length){
	return read(socket_fd,buffer,length);
}


int TCPSocket::send(const char* msg, int len){
	return write(socket_fd,msg,len);
}


void TCPSocket::cclose(){
	cout<<"closing socket"<<endl;
	shutdown(socket_fd,SHUT_RDWR);
	close(socket_fd);
}


string TCPSocket::fromAddr(){
	return inet_ntoa(peerAddr.sin_addr);
}

string TCPSocket::destIpAndPort(){
	string str = inet_ntoa(peerAddr.sin_addr);
	str.append(":");
	char buff[10];
	sprintf(buff,"%d",ntohs(peerAddr.sin_port));
	str.append(buff);
	return str;
}

/**
 * returns the c socket fid
 */
int TCPSocket::getSocketFid(){
	return socket_fd;
}
