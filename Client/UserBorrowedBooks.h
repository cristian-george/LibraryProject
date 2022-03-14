#pragma once

#ifndef USERBORROWEDBOOKS_H_
#define USERBORROWEDBOOKS_H_

#include <QWidget>
#include <QTreeWidget>
#include "ui_UserBorrowedBooks.h"
#include "UserInterface.h"

class UserBorrowedBooks : public QWidget
{
	Q_OBJECT

public:
	UserBorrowedBooks(TcpSocket*, QWidget* parent = Q_NULLPTR);
	~UserBorrowedBooks();

	void AddBlankItem(QTreeWidget*);
	QTreeWidgetItem* AddRootItem(QTreeWidget*, const std::string&);
	void AddChildItem(QTreeWidgetItem*, const std::string&);

	void AddBooksInList(QTreeWidget*, const std::string&);

	void PrintBorrowBooksList(QTreeWidget* bookList);
	void ExtendBorrowBookDuration(const std::string&);
	void ReturnBook(const std::string&);

private:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void on_readBookButton_clicked();
	void on_returnBookButton_clicked();
	void on_extendBorrowDurationButton_clicked();

private:
	Ui::UserBorrowedBooks* ui;
	TcpSocket* connectSocket;
};
#endif