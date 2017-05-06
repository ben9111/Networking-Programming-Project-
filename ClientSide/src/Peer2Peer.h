
#ifndef Peer2Peer_H_
#define Peer2Peer_H_

#include "Common.h"

class Peer2Peer: public MThread {
public:
	UDPSocket* socket;
	int port;
	char* buff;
	bool running;

	Peer2Peer(int port);

	void run();
	void sendTo(string msg,string ip,int port);
	void reply(string msg);
	void close();
	virtual ~Peer2Peer();
};

#endif /* Peer2Peer_H_ */
