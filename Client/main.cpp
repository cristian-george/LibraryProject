#include <QtWidgets/QApplication>

#include "UserLogin.h"
#include "../NetworkingLibrary/TcpSocket.h"

constexpr auto PORT = 27015;
constexpr auto REMOTE_ADDRESS = "localhost";

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	TcpSocket connectSocket;
	connectSocket.Connect(REMOTE_ADDRESS, PORT);

	UserLogin* userLogin = new UserLogin(&connectSocket);
	userLogin->showMaximized();

	return a.exec();
}