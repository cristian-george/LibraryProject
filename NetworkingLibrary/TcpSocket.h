#pragma once

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class TcpSocket
{
private:
	static const SOCKET InvalidSocketHandle;

public:
	TcpSocket();
	TcpSocket(SOCKET socket);
	~TcpSocket();

	void Listen(uint16_t port);
	SOCKET Accept();
	void Connect(const std::string& remoteAddress, uint16_t port);
	void Send(const void* data, size_t size);
	void Receive(void* data, size_t size, int& received);

	SOCKET GetSocket() const;

	void SetUsername(const std::string&);
	const std::string& GetUsername() const;

private:
	SOCKET m_socket;
	std::string m_username;
};