#include "UserInterface.h"

UserInterface::UserInterface(TcpSocket* connectSocket, QWidget* parent)
	: connectSocket(connectSocket), QWidget(parent), ui(new Ui::UserInterface)
{
	ui->setupUi(this);
	this->setAutoFillBackground(true);
	QPalette palette;
	QPixmap pixmap("background.jfif");
	palette.setBrush(QPalette::Window, QBrush(pixmap));
	this->setPalette(palette);
}

UserInterface::~UserInterface()
{
	delete ui;
}

void UserInterface::Logout()
{
	std::string message = "logout";
	connectSocket->Send(message.c_str(), message.size());

	std::array<char, BUFFER_SIZE> receiveBuffer;
	int received;
	connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::string receivedMessage(receiveBuffer.data(), received);

	if (receivedMessage == "OK")
	{
		QMessageBox::information(this, "Logout", "You have successfully logged out !");
		this->deleteLater();

		UserLogin* userLogin = new UserLogin(connectSocket);
		userLogin->showMaximized();
	}
	else
	{
		QMessageBox::critical(this, "Logout", receivedMessage.c_str());
	}
}

void UserInterface::closeEvent(QCloseEvent* event)
{
	Logout();
}

void UserInterface::on_searchBook_clicked()
{
	this->deleteLater();
	UserSearch* userSearch = new UserSearch(connectSocket);
	userSearch->showMaximized();
}

void UserInterface::on_borrowedBooks_clicked()
{
	this->deleteLater();
	UserBorrowedBooks* userBorrowedBooks = new UserBorrowedBooks(connectSocket);
	userBorrowedBooks->showMaximized();
}

void UserInterface::on_deleteAccount_clicked()
{
	std::string message = "deleteAccount";
	connectSocket->Send(message.c_str(), message.size());

	std::array<char, BUFFER_SIZE> receiveBuffer;
	int received;
	connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::string receivedMessage(receiveBuffer.data(), received);

	if (receivedMessage == "OK")
	{
		QMessageBox::information(this, "Delete account", "You have successfully deleted your account !");
		this->deleteLater();

		UserLogin* userLogin = new UserLogin(connectSocket);
		userLogin->showMaximized();
	}
	else
	{
		QMessageBox::critical(this, "Delete account", receivedMessage.c_str());
	}
}

void UserInterface::on_logout_clicked()
{
	Logout();
}
