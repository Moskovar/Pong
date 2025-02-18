#include "Connection.h"
#include <chrono>
#include <thread>
#include "Addr.h"

Connection::Connection()//gérer les erreurs avec des exceptions
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //--- TCP SOCKET ---//
    // Initialisation du socket TCP
    tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcpSocket == INVALID_SOCKET) {
        std::cerr << "Erreur lors de la création du socket TCP." << std::endl;
        WSACleanup();
        exit(1);
    }

    //--- Définition de l'adresse du serveur TCP ---//
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(50000);
    inet_pton(AF_INET, ADDR, &tcpServerAddr.sin_addr);

    //--- Connexion au serveur TCP ---//
    if (connect(tcpSocket, (sockaddr*)&tcpServerAddr, sizeof(tcpServerAddr)) == SOCKET_ERROR) {
        std::cerr << "Erreur lors de la connexion au serveur TCP." << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        exit(1);
    }

    std::cout << "Connected to distant TCP server" << std::endl;

    //--- UDP SOCKET ---//
    // Initialisation du socket UDP
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Erreur lors de la création du socket UDP." << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        exit(1);
    }

    //--- Définition de l'adresse du serveur UDP ---//
    udpServerAddr.sin_family = AF_INET;
    udpServerAddr.sin_port = htons(50001);
    inet_pton(AF_INET, ADDR, &udpServerAddr.sin_addr);

    //--- Socket TCP et UDP mit en mode non bloquant ---//
    u_long modeTCP = 1;
    ioctlsocket(tcpSocket, FIONBIO, &modeTCP);

    u_long modeUDP = 1;
    ioctlsocket(udpSocket, FIONBIO, &modeUDP);
}

Connection::~Connection()
{
    closesocket(tcpSocket);
    closesocket(udpSocket);
    WSACleanup();
}

//void Connection::sendNEUDP(NetworkEntity& ne)
//{
//    ne.id = htons(ne.id);
//    ne.xMap = htonl(ne.xMap);
//    ne.yMap = htonl(ne.yMap);
//
//    // Envoi des données sérialisées
//    int bytesSent = sendto(udpSocket, (const char*)&ne, sizeof(ne), 0, (sockaddr*)&udpServerAddr, sizeof(udpServerAddr));
//    if (bytesSent == SOCKET_ERROR) {
//        std::cerr << "Erreur lors de l'envoi des donnees -> " << WSAGetLastError() << std::endl;
//    }
//    else if (bytesSent != sizeof(ne)) 
//    {
//        std::cerr << "Erreur : seuls " << bytesSent << " octets sur " << sizeof(ne) << " ont ete envoyes." << std::endl;
//    }
//}

short Connection::recvTCP(NetworkBall& nball, NetworkBallSpeed& nbs, GLboolean& run)
{
    int bytesReceived = 0;
    int totalReceived = 0;
    char buffer[512];

    short header = -1;

    // Réception du header
    while (totalReceived < sizeof(header))
    {
        //std::cout << "Reception du header..." << std::endl;
        bytesReceived = recv(tcpSocket, ((char*)&header) + totalReceived, sizeof(header) - totalReceived, 0);
        if (bytesReceived <= 0) {
            int wsaError = WSAGetLastError();
            if (wsaError == 10035)
            { // socket en mode non bloquant n'a rien reçu
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                if (!run) break;
                continue;
            }
            std::cerr << "Error receiving header: " << wsaError << std::endl;
            return -1;
        }
        totalReceived += bytesReceived;
    }

    // Convertir le header
    header = ntohs(header);
    //std::cout << "Header: " << header << std::endl;

    // Copier le header dans le buffer
    std::memcpy(buffer, &header, sizeof(header));

    // Définir la taille des données en fonction du header
    unsigned long dataSize = 0;
    if (header == Header::BALL)
    {
        //std::cout << "Header " << header << " received" << std::endl;
        dataSize = sizeof(NetworkBall);
    }
    else if (header == Header::BALLSPEED)
    {
        //std::cout << "Header " << header << " received" << std::endl;
        dataSize = sizeof(NetworkBallSpeed);
    }
    else
    {
        std::cout << "Wrong TCP message, NBALL was expected: " << header << " has been received..." << std::endl;

        return -1;
    }

    // Réception des données restantes
    totalReceived = sizeof(header); // Réinitialiser totalReceived pour recevoir les données après le header
    while (totalReceived < dataSize)
    {
        //std::cout << "recv..." << std::endl;
        bytesReceived = recv(tcpSocket, buffer + totalReceived, dataSize - totalReceived, 0);
        if (bytesReceived <= 0)
        {
            int wsaError = WSAGetLastError();
            if (wsaError == 10035)
            { // socket en mode non bloquant n'a rien reçu
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                if (!run) break;
                continue;
            }
            std::cerr << "Error receiving message: " << wsaError << std::endl;
            return -1;
        }
        totalReceived += bytesReceived;
        //std::cout << totalReceived << std::endl;
    }

    if (header == Header::BALL)
    {
        // Copier les données reçues (y compris le header) dans la structure NetworkEntity
        std::memcpy(&nball, buffer, dataSize);

        // Convertir les champs en endian correct si nécessaire
        nball.x         = ntohl(nball.x);
        nball.z         = ntohl(nball.z);
        nball.velocityX = ntohl(nball.velocityX);
        nball.velocityZ = ntohl(nball.velocityZ);
        nball.timestamp = ntohl(nball.timestamp);

        std::cout << "TCP BALL: " << (float)(nball.x / 1000.0f) << " : " << (float)(nball.z / 1000.0f) << " : " << (float)(nball.velocityX / 1000.0f) << " : " << (float)(nball.velocityZ / 1000.0f) << std::endl;
    }
    else if (header == Header::BALLSPEED)
    {
        // Copier les données reçues (y compris le header) dans la structure NetworkEntity
        std::memcpy(&nbs, buffer, dataSize);

        // Convertir les champs en endian correct si nécessaire
        nbs.speed = ntohs(nbs.speed);

        std::cout << "TCP BALLSPEED: " << nbs.speed << std::endl;
    }

    return header;
}

bool Connection::recvVersionTCP(NetworkVersion& nv)
{
    int bytesReceived = 0;
    int totalReceived = 0;
    char buffer[512];

    short header = 0;

    // Réception du header
    while (totalReceived < sizeof(header))
    {
        //std::cout << "Reception du header..." << std::endl;
        bytesReceived = recv(tcpSocket, ((char*)&header) + totalReceived, sizeof(header) - totalReceived, 0);
        if (bytesReceived <= 0) {
            int wsaError = WSAGetLastError();
            if (wsaError == 10035)
            { // socket en mode non bloquant n'a rien reçu
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            std::cerr << "Error receiving header: " << wsaError << std::endl;
            return false;
        }
        totalReceived += bytesReceived;
    }

    // Convertir le header
    header = ntohs(header);
    //std::cout << "Header: " << header << std::endl;

    // Copier le header dans le buffer
    std::memcpy(buffer, &header, sizeof(header));

    // Définir la taille des données en fonction du header
    unsigned long dataSize = 0;
    if (header == Header::VERSION)
    {
        //std::cout << "Header " << header << " received" << std::endl;
        dataSize = sizeof(NetworkVersion);
    }
    else
    {
        std::cout << "Wrong TCP message, NPS was expected: " << header << " has been received..." << std::endl;

        return false;
    }

    // Réception des données restantes
    totalReceived = sizeof(header); // Réinitialiser totalReceived pour recevoir les données après le header
    while (totalReceived < dataSize)
    {
        //std::cout << "recv..." << std::endl;
        bytesReceived = recv(tcpSocket, buffer + totalReceived, dataSize - totalReceived, 0);
        if (bytesReceived <= 0)
        {
            int wsaError = WSAGetLastError();
            if (wsaError == 10035)
            { // socket en mode non bloquant n'a rien reçu
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            std::cerr << "Error receiving message: " << wsaError << std::endl;
            return false;
        }
        totalReceived += bytesReceived;
        //std::cout << totalReceived << std::endl;
    }

    if (header == Header::VERSION)
    {
        // Copier les données reçues (y compris le header) dans la structure NetworkEntity
        std::memcpy(&nv, buffer, dataSize);

        // Convertir les champs en endian correct si nécessaire
        nv.version = ntohl(nv.version);

        std::cout << "Version received: " << nv.version << std::endl;
    }

    return true;
}

bool Connection::recvNPSTCP(NetworkPaddleStart& nps, bool& run)
{
    int bytesReceived = 0;
    int totalReceived = 0;
    char buffer[512];

    short header = 0;

    // Réception du header
    while (totalReceived < sizeof(header)) 
    {
        //std::cout << "Reception du header..." << std::endl;
        bytesReceived = recv(tcpSocket, ((char*)&header) + totalReceived, sizeof(header) - totalReceived, 0);
        if (bytesReceived <= 0) {
            int wsaError = WSAGetLastError();
            if (wsaError == 10035) 
            { // socket en mode non bloquant n'a rien reçu
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                if (!run) break;
                continue;
            }
            std::cerr << "Error receiving header: " << wsaError << std::endl;
            return false;
        }
        totalReceived += bytesReceived;
    }

    // Convertir le header
    header = ntohs(header);
    //std::cout << "Header: " << header << std::endl;

    // Copier le header dans le buffer
    std::memcpy(buffer, &header, sizeof(header));

    // Définir la taille des données en fonction du header
    unsigned long dataSize = 0;
    if (header == Header::NPS)
    {
        //std::cout << "Header " << header << " received" << std::endl;
        dataSize = sizeof(NetworkPaddleStart);
    }
    else
    {
        std::cout << "Wrong TCP message, NPS was expected: " << header << " has been received..." << std::endl;

        return false;
    }

    // Réception des données restantes
    totalReceived = sizeof(header); // Réinitialiser totalReceived pour recevoir les données après le header
    while (totalReceived < dataSize) 
    {
        //std::cout << "recv..." << std::endl;
        bytesReceived = recv(tcpSocket, buffer + totalReceived, dataSize - totalReceived, 0);
        if (bytesReceived <= 0) 
        {
            int wsaError = WSAGetLastError();
            if (wsaError == 10035) 
            { // socket en mode non bloquant n'a rien reçu
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                if (!run) break;
                continue;
            }
            std::cerr << "Error receiving message: " << wsaError << std::endl;
            return false;
        }
        totalReceived += bytesReceived;
        //std::cout << totalReceived << std::endl;
    }

    if (header == Header::NPS)
    {
        // Copier les données reçues (y compris le header) dans la structure NetworkEntity
        std::memcpy(&nps, buffer, dataSize);

        // Convertir les champs en endian correct si nécessaire
        nps.gameID  = ntohs(nps.gameID);
        nps.id      = ntohs(nps.id);
        nps.side    = ntohs(nps.side);

        std::cout << "PLAYER ID: " << nps.gameID << " : " << nps.id << " : " << nps.side << std::endl;
    }

    return true;
}

short Connection::recvUDP(NetworkPaddle& np, NetworkBall& nb)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(udpSocket, &readfds);
    socklen_t udpServerAddrLen = sizeof(udpServerAddr);

    // Timeout pour éviter un blocage infini
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 50000; // 50 ms

    // Vérifier si des données sont disponibles
    int result = select(udpSocket + 1, &readfds, NULL, NULL, &timeout);
    if (result <= 0) return -1; // Pas de données ou erreur

    // Buffer temporaire pour recevoir les données
    char buffer[512];
    int bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer), 0,
        (sockaddr*)&udpServerAddr, &udpServerAddrLen);

    if (bytesReceived == SOCKET_ERROR) 
    {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) return -1; // Pas de données
        std::cerr << "Erreur reception UDP: " << err << std::endl;
        return -1;
    }

    

    // Lire le header (les 2 premiers octets)
    short header;
    std::memcpy(&header, buffer, sizeof(header));
    header = ntohs(header);

    //std::cout << "HEADER: " << header << std::endl;

    // Traiter selon le header
    if (header == Header::NP) 
    {
        if (bytesReceived >= sizeof(NetworkPaddle)) {
            std::memcpy(&np, buffer, sizeof(NetworkPaddle));
            np.id           = ntohs(np.id);
            np.gameID       = ntohs(np.gameID);
            np.z            = ntohl(np.z);
            np.timestamp    = ntohl(np.timestamp);
            //std::cout << "[UDP] Paddle reçu: id=" << np.id << ", z=" << np.z << " :: " << np.timestamp << std::endl;
            return Header::NP;
        }
    }
    else if (header == Header::BALL) 
    {
        if (bytesReceived >= sizeof(NetworkBall)) {
            std::memcpy(&nb, buffer, sizeof(NetworkBall));
            nb.x            = ntohl(nb.x);
            nb.z            = ntohl(nb.z);
            nb.velocityX    = ntohl(nb.velocityX);
            nb.velocityZ    = ntohl(nb.velocityZ);
            nb.timestamp    = ntohl(nb.timestamp);
            //std::cout << "[UDP] Ball reçue: x=" << (float)(nb.x / 1000.0f) << ", z=" << (float)(nb.z / 1000.0f) << " velocityX: " << (float)(nb.velocityX / 1000.0f) << " velocityZ: " << (float)(nb.velocityZ / 1000.0f) << " :: " << nb.timestamp << std::endl;
            return Header::BALL;
        }
    }

    return -1; // Aucun message valide reçu
}

void Connection::sendNPUDP(NetworkPaddle& np)
{
    //std::cout << "SENT: " << np.gameID << " : " << np.id << " : " << np.z << std::endl;

    // Convertir en Big-Endian
    np.gameID   = htons(np.gameID);
    np.id       = htons(np.id);
    np.z        = htonl(static_cast<int32_t>(np.z)); // Convertir en uint32_t pour éviter les erreurs de signe

    //std::cout << "SENT: " << ntohs(np.gameID) << " : " << ntohs(np.id) << " : " << static_cast<int32_t>(ntohl(np.z)) << std::endl;

    // Envoyer la structure complète
    int bytesSent = sendto(udpSocket, (const char*)&np, sizeof(np), 0, (sockaddr*)&udpServerAddr, sizeof(udpServerAddr));
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Erreur lors de l'envoi des données -> " << WSAGetLastError() << std::endl;
    }
    else if (bytesSent != sizeof(np))
    {
        std::cerr << "Erreur : seuls " << bytesSent << " octets sur " << sizeof(np) << " ont été envoyés." << std::endl;
    }
}

void Connection::sendNSTCP(NetworkSpell ns)
{
    if (tcpSocket == INVALID_SOCKET) 
    {
        std::cerr << "Invalid TCP socket." << std::endl;
        return;
    }

    ns.header   = htons(ns.header);
    ns.spellID  = htons(ns.spellID);

    int iResult = ::send(tcpSocket, (const char*)&ns, sizeof(ns), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
    }
    //std::cout << "Bytes Sent: " << iResult << std::endl;
}

void Connection::setWaitingModeTCP(bool wait)
{
    wait = !wait;//pour que le bool corresponde au mode -> 0 bloquant, 1 non bloquant

    u_long mode = wait; // 0 = mode bloquant
    ioctlsocket(tcpSocket, FIONBIO, &mode);
}
