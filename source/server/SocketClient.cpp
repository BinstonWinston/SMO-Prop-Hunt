#include "server/SocketClient.hpp"
#include <cstdlib>
#include <cstring>

#include "SocketBase.hpp"
#include "logger.hpp"
#include "nn/result.h"
#include "nn/socket.h"
#include "packets/Packet.h"
#include "packets/UdpPacket.h"
#include "types.h"

nn::Result SocketClient::init(const char* ip, u16 port) {

    this->sock_ip = ip;
    this->port    = port;
    
    in_addr  hostAddress   = { 0 };
    sockaddr serverAddress = { 0 };
    sockaddr udpAddress = { 0 };

    Logger::log("SocketClient::init: %s:%d sock %s\n", ip, port, getStateChar());

    nn::nifm::Initialize();
    nn::nifm::SubmitNetworkRequest();

    while (nn::nifm::IsNetworkRequestOnHold()) { }

    // emulators (ryujinx) make this return false always, so skip it during init
    #ifndef EMU
    if (!nn::nifm::IsNetworkAvailable()) {
        Logger::log("Network Unavailable.\n");
        this->socket_log_state = SOCKET_LOG_UNAVAILABLE;
        this->socket_errno = nn::socket::GetLastErrno();
        return -1;
    }
    #endif

    if ((this->socket_log_socket = nn::socket::Socket(2, 1, 6)) < 0) {
        Logger::log("Socket Unavailable.\n");
        this->socket_errno = nn::socket::GetLastErrno();
        this->socket_log_state = SOCKET_LOG_UNAVAILABLE;
        return -1;
    }

    if (! this->stringToIPAddress(this->sock_ip, &hostAddress)) {
        Logger::log("IP address is invalid or hostname not resolveable.\n");
        this->socket_errno = nn::socket::GetLastErrno();
        this->socket_log_state = SOCKET_LOG_UNAVAILABLE;
        return -1;
    }

    serverAddress.address = hostAddress;
    serverAddress.port = nn::socket::InetHtons(this->port);
    serverAddress.family = 2;

    int sockOptValue = true;

    nn::socket::SetSockOpt(this->socket_log_socket, 6, TCP_NODELAY, &sockOptValue, sizeof(sockOptValue));

    nn::Result result;
    
    if((result = nn::socket::Connect(this->socket_log_socket, &serverAddress, sizeof(serverAddress))).isFailure()) {
        Logger::log("Socket Connection Failed!\n");
        this->socket_errno = nn::socket::GetLastErrno();
        this->socket_log_state = SOCKET_LOG_UNAVAILABLE;
        return result;
    }

	if ((this->udp_socket = nn::socket::Socket(2, 2, 17)) < 0) {
        Logger::log("Udp Socket failed to create");
        this->socket_errno = nn::socket::GetLastErrno();
        this->socket_log_state = SOCKET_LOG_UNAVAILABLE;
        return -1;
	}

    udpAddress.address = hostAddress;
    udpAddress.port = nn::socket::InetHtons(41553);
    udpAddress.family = 2;
    this->udp_addr = udpAddress;

    // udpAddress.address = hostAddress;
    // udpAddress.port = nn::socket::InetHtons(57734);
    // udpAddress.family = 2;

    if((result = nn::socket::Connect(this->udp_socket, &udpAddress, sizeof(udpAddress))).isFailure()) {
        Logger::log("Udp Socket Connection Failed!\n");
        this->socket_errno = nn::socket::GetLastErrno();
        this->socket_log_state = SOCKET_LOG_UNAVAILABLE;
        return result;
    }

    this->socket_log_state = SOCKET_LOG_CONNECTED;

    Logger::log("Socket fd: %d\n", socket_log_socket);

    return result;

}

u16 SocketClient::getUdpPort() {
    sockaddr udpAddress = { 0 };
    u32 size = sizeof(udpAddress);

    nn::Result result;
    if((result = nn::socket::GetSockName(this->udp_socket, &udpAddress, &size)).isFailure()) {
        this->socket_errno = nn::socket::GetLastErrno();
        return 0;
    }

    return nn::socket::InetNtohs(udpAddress.port);
}


s32 SocketClient::setPeerUdpPort(u16 port) {
    u16 net_port = nn::socket::InetHtons(port);
    this->udp_addr.port = net_port;

    nn::Result result;
    if((result = nn::socket::Connect(this->udp_socket, &this->udp_addr, sizeof(this->udp_addr))).isFailure()) {
        Logger::log("Udp Socket Connection Failed!\n");
        this->socket_errno = nn::socket::GetLastErrno();
        this->socket_log_state = SOCKET_LOG_UNAVAILABLE;
        return -1;
    }

    return 0;

}

bool SocketClient::SEND(Packet *packet) {

    if (this->socket_log_state != SOCKET_LOG_CONNECTED)
        return false;

    char* buffer = reinterpret_cast<char*>(packet);

    int valread = 0;

	int fd = -1;
    if (packet->mType != PLAYERINF && packet->mType != HACKCAPINF && packet->mType != HOLEPUNCH) {
		Logger::log("Sending packet: %s\n", packetNames[packet->mType]);
		fd = this->socket_log_socket;
	} else {

		fd = this->udp_socket;
	}


	if ((valread = nn::socket::Send(fd, buffer, packet->mPacketSize + sizeof(Packet), 0) > 0)) {
        return true;
    } else {
        Logger::log("Failed to Fully Send Packet! Result: %d Type: %s Packet Size: %d\n", valread, packetNames[packet->mType], packet->mPacketSize);
        this->socket_errno = nn::socket::GetLastErrno();
        this->closeSocket();
        return false;
    }
    return true;
}

bool SocketClient::RECV() {

    if (this->socket_log_state != SOCKET_LOG_CONNECTED) {
        Logger::log("Unable To Receive! Socket Not Connected.\n");
        this->socket_errno = nn::socket::GetLastErrno();
        return false;
    }
    
    int headerSize = sizeof(Packet);
    char headerBuf[MAXPACKSIZE * 2] = {};
    int valread = 0;

	const int fd_count = 2;
	struct pollfd pfds[fd_count] = {{0}, {0}};
	pfds[0].fd = this->socket_log_socket;
	pfds[0].events = 1;
	pfds[0].revents = 0;
	pfds[1].fd = this->udp_socket;
	pfds[1].events = 1;
	pfds[1].revents = 0;


	if (nn::socket::Poll(pfds, fd_count, 0) <= 0) {
		return true;
	}

	s32 fd = -1;
	s32 index = -1;
	for (int i = 0; i < fd_count; i++){
		if (pfds[i].revents & 1) {
			fd = pfds[i].fd;
			index = i;
		}
	}

	if (fd == -1) {
		return true;
	}
if (index == 1) {
        int result = nn::socket::Recv(fd, headerBuf, sizeof(headerBuf), this->sock_flags);
        if (result < headerSize){
            return true;
        }

        Packet* header = reinterpret_cast<Packet*>(headerBuf);
        int fullSize = header->mPacketSize + sizeof(Packet);

        if (result < fullSize || result > MAXPACKSIZE || fullSize > MAXPACKSIZE){
            return true;
        }

        char* packetBuf = (char*)malloc(fullSize);

        if (!(header->mType > PacketType::UNKNOWN && header->mType < PacketType::End)) {
            Logger::log("Failed to acquire valid packet type! Packet Type: %d Full Packet Size %d valread size: %d", header->mType, fullSize, result);
            free(packetBuf);
            return true;
        }

        memcpy(packetBuf, headerBuf, fullSize);


        Packet *packet = reinterpret_cast<Packet*>(packetBuf);

        if(mPacketQueue.size() < maxBufSize - 1) {
            mPacketQueue.pushBack(packet);
        } else {
            free(packetBuf);
        }
        return true;
    }

    // read only the size of a header
    while(valread < headerSize) {
        int result = 0;
		result = nn::socket::Recv(fd, headerBuf + valread,
										headerSize - valread, this->sock_flags);

        this->socket_errno = nn::socket::GetLastErrno();
        
        if(result > 0) {
            valread += result;
        } else {
            if(this->socket_errno==11){
                return true;
            } else {
                Logger::log("Header Read Failed! Value: %d Total Read: %d\n", result, valread);
                this->closeSocket();
                return false;
            }
        }
    }

    if(valread > 0) {
        Packet* header = reinterpret_cast<Packet*>(headerBuf);

        int fullSize = header->mPacketSize + sizeof(Packet);

        if (fullSize <= MAXPACKSIZE && fullSize > 0 && valread == sizeof(Packet)) {

            if (header->mType != PLAYERINF && header->mType != HACKCAPINF) {
                Logger::log("Received packet (from %02X%02X):", header->mUserID.data[0],
                            header->mUserID.data[1]);
                Logger::disableName();
                Logger::log(" Size: %d", header->mPacketSize);
                Logger::log(" Type: %d", header->mType);
                if(packetNames[header->mType])
                    Logger::log(" Type String: %s\n", packetNames[header->mType]);
                Logger::enableName();
            }
                

            char* packetBuf = (char*)malloc(fullSize);

            if (packetBuf) {
                
                memcpy(packetBuf, headerBuf, sizeof(Packet));

                while (valread < fullSize) {

                    int result = nn::socket::Recv(fd, packetBuf + valread,
                                                  fullSize - valread, this->sock_flags);

                    this->socket_errno = nn::socket::GetLastErrno();

                    if (result > 0) {
                        valread += result;
                    } else {
                        free(packetBuf);
                        Logger::log("Packet Read Failed! Value: %d\nPacket Size: %d\nPacket Type: %s\n", result, header->mPacketSize, packetNames[header->mType]);
                        this->closeSocket();
                        return false;
                    }
                }

                if (!(header->mType > PacketType::UNKNOWN && header->mType < PacketType::End)) {
                    Logger::log("Failed to acquire valid packet type! Packet Type: %d Full Packet Size %d valread size: %d", header->mType, fullSize, valread);
                    free(packetBuf);
                    return true;
                }

                Packet *packet = reinterpret_cast<Packet*>(packetBuf);

                if(mPacketQueue.size() < maxBufSize - 1) {
                    mPacketQueue.pushBack(packet);
                } else {
                    free(packetBuf);
                }
            }
        } else {
            Logger::log("Failed to acquire valid data! Packet Type: %d Full Packet Size %d valread size: %d", header->mType, fullSize, valread);
        }
        
        return true;
    } else {  // if we error'd, close the socket
        Logger::log("valread was zero! Disconnecting.\n");
        this->socket_errno = nn::socket::GetLastErrno();
        this->closeSocket();
        return false;
    }
}

// prints packet to debug logger
void SocketClient::printPacket(Packet *packet) {
    packet->mUserID.print();
    Logger::log("Type: %s\n", packetNames[packet->mType]);

    switch (packet->mType)
    {
    case PacketType::PLAYERINF:
        Logger::log("Pos X: %f Pos Y: %f Pos Z: %f\n", ((PlayerInf*)packet)->playerPos.x, ((PlayerInf*)packet)->playerPos.y, ((PlayerInf*)packet)->playerPos.z);
        Logger::log("Rot X: %f Rot Y: %f Rot Z: %f\nRot W: %f\n", ((PlayerInf*)packet)->playerRot.x, ((PlayerInf*)packet)->playerRot.y, ((PlayerInf*)packet)->playerRot.z, ((PlayerInf*)packet)->playerRot.w);
        break;
    default:
        break;
    }
}

bool SocketClient::closeSocket() {

    Logger::log("Closing Socket.\n");

    bool result = false;

    if (!(result = SocketBase::closeSocket())) {
        Logger::log("Failed to close socket!\n");
    }

    return result;
}

bool SocketClient::stringToIPAddress(const char* str, in_addr* out) {
    // string to IPv4
    if (nn::socket::InetAton(str, out)) {
        return true;
    }

    // get IPs via DNS
    struct hostent *he = nn::socket::GetHostByName(str);
    if (! he) { return false; }

    // might give us multiple IP addresses, so pick the first one
    struct in_addr **addr_list = (struct in_addr **) he->h_addr_list;
    for (int i = 0 ; addr_list[i] != NULL ; i++) {
        *out = *addr_list[i];
        return true;
    }

    return false;
}
