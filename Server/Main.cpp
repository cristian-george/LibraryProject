#include "Application.h"
#include <thread>

constexpr auto PORT = 27015;

int main()
{
	TcpSocket serverSocket;
	serverSocket.Listen(PORT);
	serverSocket.SetUsername("Server");

	Application* application = new Application();
	application->WriteLogger(Logger::Level::Info, "Starting server");

	while (true)
	{
		SOCKET clientSocket = serverSocket.Accept();

		std::thread clientThread(Application::ClientRun, application, clientSocket);
		clientThread.detach();
	}

	return 0;
}