#pragma once
#define NOMINMAX  // Désactive la définition des macros min et max dans Windows.h
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include "uti.hpp"

#pragma comment(lib, "Ws2_32.lib")

class Connection
{
	public:
		Connection();
		~Connection();

		short recvTCP(NetworkBall& nball, NetworkPaddleStart& nps, GLboolean& run);
		bool recvVersionTCP(NetworkVersion& nv);
		bool recvNPSTCP(NetworkPaddleStart& nps, bool& run);

		short recvUDP(NetworkPaddle& np, NetworkBall& nb);//retourne le header de la modif, -1 si pas de modif

		void sendMatchmakingTCP();
		void sendNPUDP(NetworkPaddle& np);
		void sendNSTCP(NetworkSpell ns);
		void sendLeaveGameTCP();

		void setWaitingModeTCP(bool wait);

	private:
		WSADATA wsaData;

		SOCKET tcpSocket = INVALID_SOCKET;
		SOCKET udpSocket = INVALID_SOCKET;
		sockaddr_in tcpServerAddr, udpServerAddr;
};

