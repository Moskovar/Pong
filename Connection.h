#pragma once
#define NOMINMAX  // D�sactive la d�finition des macros min et max dans Windows.h
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

		void sendNETCP(NetworkEntity ne);
		void sendNESTCP(NetworkEntitySpell nes);
		void sendNESETCP(NetworkEntitySpellEffect nes);
		void sendNETTCP(NetworkEntityTarget net);
		bool recvTCP(NetworkBall& nball, GLboolean& run);

		bool recvNPSTCP(NetworkPaddleStart& nps, bool& run);
		short recvUDP(NetworkPaddle& np, NetworkBall& nb);//retourne le header de la modif, -1 si pas de modif

		void sendNPUDP(NetworkPaddle& np);
		void recvNEUDP(NetworkEntity& ne);

		void setWaitingModeTCP(bool wait);

	private:
		WSADATA wsaData;

		SOCKET tcpSocket = INVALID_SOCKET;
		SOCKET udpSocket = INVALID_SOCKET;
		sockaddr_in tcpServerAddr, udpServerAddr;
};

