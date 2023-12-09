//	UDPSocket.h - Class to handle UDP network communication.
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

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include <string>
#include <stdint.h>

//------------------------------------------------------------------------------
///	Class to handle UDP network communication.
class UDPSocket
{
  public:
	///	Default constructor.
	/*!
		This version is intended for keeping a socket open for a fairly long
		time.
	 */
	UDPSocket();
	///	Constructor.
	/*!
		\param address The address to send data to.
		\param port The port to send data from/to.

		This version is intended for times when you just want to quickly open a
		socket and send something (i.e. in a method that doesn't get called
		often).
	 */
	UDPSocket(const std::string& address, const int16_t port);
	///	Destructor.
	~UDPSocket();

	///	Sets the address to send data to.
	void setAddress(const std::string& address);
	///	Sets the muticast group address to listen on.
	void setMulticastGroup(const std::string& address);
	///	Sets the port to send data from/to.
	void setPort(const int16_t port);
	///	Binds sock to the currently-set port.
	/*!
		\param loopback If true, only accept data from apps running on this
		computer.

		You must call this before you call getData().
	 */
	void bindSocket(bool loopback = false);

	///	Returns the currently-set port.
	int16_t getPort() const {return port;};
	///	Returns the currently-set multicast group to listen on.
	const std::string& getMulticastGroup() const {return multicastGroup;};
	///	Returns this computer's ip address.
	const std::string getIpAddress() const;

	///	Sends data to previously-set address/port.
	/*!
		\param data The data to send (a contiguous block of bytes).
		\param size The size of the data to send.
	 */
	void sendData(char *data, const int32_t size);

	///	Returns a data packet sent to us.
	/*!
		On return, size will be filled with the size of the packet, or -1 if it
		failed.

		This will block until it receives data, or until 1/4 of a second has
		passed, whichever comes first.
	 */
	char *getData(int32_t& size);
  private:
	///	The address to send data to.
	std::string address;
	///	The multicast group address to listen on.
	std::string multicastGroup;
	///	The port to send data from/to.
	int16_t port;
	///	Our socket.
	int32_t sock;

	enum
	{
		MaxBufferSize = 16384 //16kB should be enough?
	};

	///	Buffer to receive any data sent to us.
	char receiveBuffer[MaxBufferSize];

	///	Used when we want to change the bound port.
	bool firstRun;
};

//------------------------------------------------------------------------------
///	Singleton class used to set up socket stuff on Windows.
/*!
	We need this to make sure we're set up properly before we start sending
	data, and also so that we don't end up calling it more than once.
 */
class SocketSetup
{
  public:
	///	Returns the single active instance of the class.
	/*!
		If the class is not yet instantiated, constructs it.  Since the actual
		setup is done in the constructor, the returned pointer probably isn't
		going to be any use...
	 */
	static SocketSetup *getInstance();

	///	Destructor.
	/*!
		Note: always remember to delete the singleton when you're done with it.
	 */
	~SocketSetup();
  private:
	///	Constructor.
	SocketSetup();

	///	Pointer to the single instance of this class.
	static SocketSetup *instance;
};

#endif
