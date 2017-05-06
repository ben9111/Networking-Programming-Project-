

#include "UDPSocket.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

UDPSocket::UDPSocket(int port){
	/**
	 * int socket(int domain, int type, int protocol);
	 * creates a UDP socket
	 * AF_INET - IPv4 Internet protocols
	 * SOCK_DGRAM - UDP
	 * 0 - default protocol type fo UDP
	 */
	socket_fd = socket (AF_INET, SOCK_DGRAM, 0);

	// clear the s_in struct
	bzero((char *) &s_in, sizeof(s_in));  /* They say you must do this    */

	//sets the sin address
	s_in.sin_family = (short)AF_INET;
	s_in.sin_addr.s_addr = htonl(INADDR_ANY);    /* WILDCARD */
	s_in.sin_port = htons((u_short)port);

	fsize = sizeof(from);

	//bind the socket on the specified address
	if(bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in))<0){
		perror ("Error naming channel");
	}
}

int UDPSocket::recv(char* buffer, int length){
	//printf("UDP server receive loop has started\n");
	//ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
	//					struct sockaddr *src_addr, socklen_t *addrlen);
	return recvfrom(socket_fd,buffer,length,0,(struct sockaddr *)&from,&fsize);
}

int UDPSocket::sendTo(string msg, string ip, int port){
	struct sockaddr_in toAddr;
	bzero((char *) &toAddr, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	toAddr.sin_addr.s_addr = inet_addr(ip.data());
	toAddr.sin_port = htons(port);
	return sendto(socket_fd,msg.data(),msg.length(),0,(struct sockaddr *)&toAddr,sizeof(toAddr));
}

int UDPSocket::reply(string msg){
	return sendto(socket_fd,msg.data(),msg.length(),0,(struct sockaddr *)&from,sizeof(from));
}

void UDPSocket::cclose(){
	cout<<"closing udp socket .."<<endl;
	shutdown(socket_fd,SHUT_RDWR);
	close(socket_fd);
}

string UDPSocket::fromAddr(){
	return inet_ntoa(from.sin_addr);
}



