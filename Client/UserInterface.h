#pragma once

#ifndef USERINTERFACE_H_
#define USERINTERFACE_H_

#include <QWidget>
#include "ui_UserInterface.h"
#include "UserLogin.h"
#include "UserSearch.h"
#include "UserBorrowedBooks.h"
#include <QCloseEvent>

class UserInterface : public QWidget
{
	Q_OBJECT

public:
	UserInterface(TcpSocket*, QWidget* parent = Q_NULLPTR);
	~UserInterface();

	void Logout();

private:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void on_searchBook_clicked();
	void on_borrowedBooks_clicked();
	void on_deleteAccount_clicked();
	void on_logout_clicked();

private:
	Ui::UserInterface* ui;
	TcpSocket* connectSocket;
};
#endif
