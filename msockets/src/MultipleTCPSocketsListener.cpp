

#include "MultipleTCPSocketsListener.h"

using namespace std;



void MultipleTCPSocketsListener::addSocket(TCPSocket* socket){
	sockets.push_back(socket);
}

void MultipleTCPSocketsListener::addSockets(vector<TCPSocket*> socketVec){
	sockets = socketVec; //copy the initialized vector to the local one
}

TCPSocket* MultipleTCPSocketsListener::listenToSocket(int timeout){
	struct timeval tv = {timeout, 0};
	tSocketsContainer::iterator iter = sockets.begin(); // pointer to the beginning of the vector
	tSocketsContainer::iterator endIter = sockets.end(); // pointer to the end of the vector
	fd_set fdset; //Holds the file descriptor id's
	FD_ZERO(&fdset); //This function initializes the file descriptor set to contain no file descriptors.
	int highfd = 0; //Number of sockets
	int maxfd=0;

	for (;iter != endIter;iter++)
	{
		highfd++;
		if (maxfd < (*iter)->getSocketFid())
		{
			maxfd= (*iter)->getSocketFid();
		}
		FD_SET((*iter)->getSocketFid(), &fdset);
		//This function adds a file descriptor to a file descriptor set.
	}


	int returned;

	if (timeout>0)
	{
		returned = select(maxfd+1, &fdset, NULL, NULL, &tv);
		//When select returns, it has updated the sets to show which file descriptors have become ready
		//return the number of file descriptors that are ready
	}

	else
	{

		returned = select(maxfd+1, &fdset, NULL, NULL, NULL);
		//When select returns, it has updated the sets to show which file descriptors have become ready
	}

	if (returned) //Returns the number of ready descriptors, or -1 for errors.
	{
		for (int i = 0; i < highfd; i++)
		{
			TCPSocket* tmpSocket = sockets[i];
			if (FD_ISSET(tmpSocket->getSocketFid(), &fdset))
			{	//This function returns a value for the file descriptor in the file descriptor set.
				return tmpSocket;
			}
		}
	}

	return NULL;
}

