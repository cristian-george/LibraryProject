#include "TcpSocket.h"

#pragma comment(lib, "Ws2_32.lib")	//  links to Ws2_32.lib

const SOCKET TcpSocket::InvalidSocketHandle = INVALID_SOCKET;

TcpSocket::TcpSocket() : m_socket(InvalidSocketHandle)
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == InvalidSocketHandle)
	{
		throw std::string("Error at socket(): ") + std::to_string(WSAGetLastError());
	}
}

TcpSocket::~TcpSocket()
{
	Send("disconnect", 10);

	if (m_socket != InvalidSocketHandle)
	{
		closesocket(m_socket);
	}
}

void TcpSocket::Listen(uint16_t port)
{
	struct addrinfo* result = nullptr, hints;
	ZeroMemory(&hints, sizeof(hints));	// memset to 0
	hints.ai_family = AF_INET;			// IPv4

	int iResult = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0)
	{
		throw std::string("getaddrinfo failed: %d\n", iResult);
	}

	iResult = bind(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		freeaddrinfo(result);
		closesocket(m_socket);
		throw std::string("bind failed with error: %d\n", WSAGetLastError());
	}

	freeaddrinfo(result);

	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		throw std::string("Listen failed with error: %ld\n", WSAGetLastError());
	}
}

SOCKET TcpSocket::Accept()
{
	SOCKET clientSocket = accept(m_socket, nullptr, nullptr);
	if (clientSocket == InvalidSocketHandle)
	{
		throw "accept failed: " + std::to_string(WSAGetLastError());
	}

	return clientSocket;
}

void TcpSocket::Connect(const std::string& remoteAddress, uint16_t port)
{
	addrinfo* result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));	// memset to 0
	hints.ai_family = AF_INET;			// IPv4

	int iResult = getaddrinfo(remoteAddress.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0)
	{
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		freeaddrinfo(result);
	}

	// Attempt to connect to the first address returned by the call to getaddrinfo
	iResult = connect(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(m_socket);
		m_socket = InvalidSocketHandle;
	}

	freeaddrinfo(result);

	if (m_socket == InvalidSocketHandle)
	{
		std::cerr << "Unable to connect to server: " << WSAGetLastError() << std::endl;
	}
}

void TcpSocket::Send(const void* data, size_t size)
{
	int result = 0;
	for (size_t sent = 0; sent < size; sent += result)
	{
		result = send(m_socket, static_cast<const char*>(data), (int)size, 0);
		if (result < 0)
		{
			std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		}
	}
}

void TcpSocket::Receive(void* data, size_t size, int& received)
{
	received = recv(m_socket, static_cast<char*>(data), (int)size, 0);
	if (received < 0)
	{
		std::cerr << "receive failed: " << WSAGetLastError() << std::endl;
	}
}

SOCKET TcpSocket::GetSocket() const
{
	return m_socket;
}

void TcpSocket::SetUsername(const std::string& username)
{
	m_username = username;
}

const std::string& TcpSocket::GetUsername() const
{
	return m_username;
}

TcpSocket::TcpSocket(SOCKET socket) : m_socket(socket) {}

struct WinsocketInitializer
{
	WinsocketInitializer()
	{
		// *** initialize Winsock ***
		// initialize Winsock before making other Winsock functions calls (only once per application/dll)
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);        // initiate use of WS2_32.dll, with version 2.2
		if (iResult != 0)
		{
			throw std::string("WSAStartup failed: ") + std::to_string(iResult);
		}
	}

	~WinsocketInitializer()
	{
		WSACleanup();
	}
};

WinsocketInitializer globalInitializer;
