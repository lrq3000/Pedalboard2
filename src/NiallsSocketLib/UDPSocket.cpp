//	UDPSocket.cpp - Class to handle UDP network communication.
//	----------------------------------------------------------------------------
//	This file is part of OSC Input Test - a simple test program to learn OSC.
//	Copyright (C) 2005-2009  Niall Moody
//	
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//	----------------------------------------------------------------------------

#include "UDPSocket.h"

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>

#ifndef inet_addr
#include <arpa/inet.h>
#endif

#ifdef __APPLE__
#include <ifaddrs.h>
#endif

#endif

#include <string.h>  //For memset.
#include <fstream>
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
UDPSocket::UDPSocket():
port(0),
firstRun(true)
{
	SocketSetup::getInstance();

	sock = socket(AF_INET, SOCK_DGRAM, 0);
}

//------------------------------------------------------------------------------
UDPSocket::UDPSocket(const std::string& address, const int16_t port):
firstRun(true)
{
	this->address = address;
	this->port = port;

	SocketSetup::getInstance();

	sock = socket(AF_INET, SOCK_DGRAM, 0);
}

//------------------------------------------------------------------------------
UDPSocket::~UDPSocket()
{
#ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif
}

//------------------------------------------------------------------------------
void UDPSocket::setAddress(const std::string& address)
{
	this->address = address;
}

//------------------------------------------------------------------------------
void UDPSocket::setMulticastGroup(const string& address)
{
	multicastGroup = address;
}

//------------------------------------------------------------------------------
void UDPSocket::setPort(const int16_t port)
{
	this->port = port;
}

//------------------------------------------------------------------------------
void UDPSocket::bindSocket(bool loopback)
{
	sockaddr_in localAddress;

	if(firstRun)
		firstRun = false;
	else
	{
#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
		sock = socket(AF_INET, SOCK_DGRAM, 0);
	}

	localAddress.sin_family = AF_INET;
	localAddress.sin_port = htons(port);
	if(loopback)
		localAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else
		localAddress.sin_addr.s_addr = INADDR_ANY;
	memset(&(localAddress.sin_zero), 0, sizeof(localAddress.sin_zero));

	if(bind(sock, (sockaddr *)(&localAddress), sizeof(sockaddr_in)) != 0)
	{
#ifdef WIN32
		int err = WSAGetLastError();

		cout << "UDPSocket bind error: " << err << endl;
#endif
	}

	if(multicastGroup != "")
	{
		ip_mreq mreq;

		mreq.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
		mreq.imr_interface.s_addr = INADDR_ANY;

		setsockopt(sock,
				   IPPROTO_IP,
				   IP_ADD_MEMBERSHIP,
				   (const char *)&mreq,
				   sizeof(ip_mreq));
	}
}

//------------------------------------------------------------------------------
const string UDPSocket::getIpAddress() const
{
	string retval;
	char *localIp;
	char hostName[255];
	struct hostent *hostEntry;

	gethostname(hostName, 255);
	hostEntry=gethostbyname(hostName);
	localIp = inet_ntoa(*(struct in_addr *)*hostEntry->h_addr_list);

	retval = localIp;

	return retval;
}

//------------------------------------------------------------------------------
void UDPSocket::sendData(char *data, const int32_t size)
{
	sockaddr_in destAddress;
	hostent *he = gethostbyname(address.c_str());

	destAddress.sin_family = AF_INET;
	destAddress.sin_port = htons(port);
	/*destAddress.sin_addr = *((struct in_addr *)gethostbyaddr(inet_addr(address.c_str()),
															 (address.length()+1),
															 0)->h_addr_list);*/
	/*tempint = inet_addr(address.c_str());
	destAddress.sin_addr = *(in_addr *)(&tempint);*/
	destAddress.sin_addr = *(in_addr *)he->h_addr;
	memset(&(destAddress.sin_zero), '\0', 8);

	sendto(sock,
		   data,
		   size,
		   0,
		   reinterpret_cast<sockaddr *>(&destAddress),
		   sizeof(sockaddr));
}

//------------------------------------------------------------------------------
char *UDPSocket::getData(int32_t& size)
{
	int bytesReceived = 0;
	timeval timeToWait;
	fd_set setToCheck;
	sockaddr_in receivedAddress;

	timeToWait.tv_sec = 0;
	timeToWait.tv_usec = 25; // 1/4 of a second.

	FD_ZERO(&setToCheck);
	FD_SET(sock, &setToCheck);

	//Use select so that we can timeout to check if we're supposed to stop the
	//thread yet.
#ifdef WIN32
	if(select(1, &setToCheck, NULL, NULL, &timeToWait) == SOCKET_ERROR)
	{
		switch(WSAGetLastError())
		{
			case WSANOTINITIALISED:
				strcpy_s(receiveBuffer, "Not Initialised");
				break;
			case WSAEFAULT:
				strcpy_s(receiveBuffer, "Fault");
				break;
			case WSAENETDOWN:
				strcpy_s(receiveBuffer, "Network subsystem has failed.");
				break;
			case WSAEINVAL:
				strcpy_s(receiveBuffer, "Invalid parameter.");
				break;
			case WSAEINTR:
				strcpy_s(receiveBuffer, "Blocking socket was cancelled.");
				break;
			case WSAEINPROGRESS:
				strcpy_s(receiveBuffer, "Blocking socket in progress.");
				break;
			case WSAENOTSOCK:
				strcpy_s(receiveBuffer, "Not a socket.");
				break;
		}
		size = -1;
		return receiveBuffer;
	}
#else
	if(select((sock+1), &setToCheck, NULL, NULL, &timeToWait) == -1)
	{
		/*switch(errno)
		{
			case EBADF:
				strcpy(receiveBuffer, "Invalid file descriptor.");
				break;
			case EINTR:
				strcpy(receiveBuffer, "Non blocked signal was caught.");
				break;
			case EINVAL:
				strcpy(receiveBuffer, "Invalid time value.");
				break;
			case ENOMEM:
				strcpy(receiveBuffer, "Not enough memory.");
				break;
		}*/
		size = -1;
		return receiveBuffer;
	}
#endif

	if(FD_ISSET(sock, &setToCheck))
	{
		size = sizeof(sockaddr_in);
#ifndef WIN32
		bytesReceived = recvfrom(sock,
								 receiveBuffer,
								 (MaxBufferSize-1),
								 0,
								 (sockaddr *)(&receivedAddress),
								 (unsigned int *)(&size));
#else
		bytesReceived = recvfrom(sock,
								 receiveBuffer,
								 (MaxBufferSize-1),
								 0,
								 (sockaddr *)(&receivedAddress),
								 (int *)(&size));
#endif

		if(bytesReceived > 0)
		{
			size = bytesReceived;
			return receiveBuffer;
		}
		else
		{
			size = -1;
#ifdef WIN32
			strcpy_s(receiveBuffer, "Received an empty packet?");
#else
			strcpy(receiveBuffer, "Received an empty packet?");
#endif
			return receiveBuffer;
		}
	}
	else
	{
		size = -1;
		return 0;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SocketSetup *SocketSetup::getInstance()
{
	static SocketSetup instance;

	return &instance;
}

//------------------------------------------------------------------------------
SocketSetup::SocketSetup()
{
#ifdef WIN32
	WSADATA wsaData;   // if this doesn't work

	WSAStartup(MAKEWORD(1, 1), &wsaData);
#endif
}

//------------------------------------------------------------------------------
SocketSetup::~SocketSetup()
{
#ifdef WIN32
	WSACleanup();
#endif
}
