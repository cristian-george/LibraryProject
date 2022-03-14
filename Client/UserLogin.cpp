#include "UserLogin.h"
#include "UserInterface.h"

UserLogin::UserLogin(TcpSocket* connectSocket, QWidget* parent)
	: connectSocket(connectSocket), QWidget(parent), ui(new Ui::UserLogin)
{
	ui->setupUi(this);
	this->setAutoFillBackground(true);
	QPalette palette;
	QPixmap pixmap("background.jfif");
	palette.setBrush(QPalette::Window, QBrush(pixmap));
	this->setPalette(palette);

	QString lastUser = GetLastUserLoggedIn();
	if (!lastUser.isEmpty())
		ui->usernameBox->setText(lastUser);
}

UserLogin::~UserLogin()
{
	delete ui;
}

QString UserLogin::GetLastUserLoggedIn()
{
	std::ifstream fin("lastUser");
	std::string username;
	fin >> username;

	return QString(username.c_str());
}

void UserLogin::SetLastUserLoggedIn(const QString& username)
{
	std::ofstream fout("lastUser");
	fout << username.toStdString();
}

void UserLogin::SignIn(const QString& option, const QString& message)
{
	QString username = ui->usernameBox->text();
	QString password = ui->passwordBox->text();

	if (username.isEmpty() || password.isEmpty())
		return;

	std::string messageToSend = option.toStdString();
	messageToSend[0] = tolower(messageToSend[0]);

	messageToSend += " " + username.toStdString() + " " + password.toStdString();
	connectSocket->Send(messageToSend.c_str(), messageToSend.size());

	std::array<char, BUFFER_SIZE> receiveBuffer;
	int received;
	connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::string receivedMessage(receiveBuffer.data(), received);

	if (receivedMessage == "OK")
	{
		QMessageBox::information(this, option, message);
		SetLastUserLoggedIn(username);
		this->deleteLater();

		UserInterface* userInterface = new UserInterface(connectSocket);
		userInterface->showMaximized();
	}
	else
	{
		QMessageBox::critical(this, option, receivedMessage.c_str());
	}
}

void UserLogin::on_loginButton_clicked()
{
	SignIn("Login", "Your credentials are correct !");
}

void UserLogin::on_registerButton_clicked()
{
	SignIn("Register", "Your account has been created !");
}