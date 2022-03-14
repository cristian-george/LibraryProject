#pragma once

#ifndef USERLOGIN_H_
#define USERLOGIN_H_

#include <QWidget>
#include <QMessageBox>
#include <fstream>
#include "ui_UserLogin.h"

#include "../NetworkingLibrary/TcpSocket.h"
#include <array>
constexpr auto BUFFER_SIZE = 512;

class UserLogin : public QWidget
{
	Q_OBJECT

public:
	UserLogin(TcpSocket* connectSocket, QWidget* parent = Q_NULLPTR);
	~UserLogin();

	QString GetLastUserLoggedIn();
	void SetLastUserLoggedIn(const QString& username);
	void SignIn(const QString& option, const QString& message);

private slots:
	void on_loginButton_clicked();
	void on_registerButton_clicked();

private:
	Ui::UserLogin* ui;
	TcpSocket* connectSocket;
};
#endif
