#pragma once
#include <QWidget>
#include <QTreeWidget>
#include "ui_UserSearch.h"
#include "UserInterface.h"

class UserSearch : public QWidget
{
	Q_OBJECT

public:
	enum class SearchOption : char
	{
		None = 0,
		SearchByTitle,
		SearchByAuthor,
		SearchByISBN
	};
	SearchOption ConvertToSearchOption(const QString&);

public:
	UserSearch(TcpSocket*, QWidget* parent = Q_NULLPTR);
	~UserSearch();

	void AddBlankItem(QTreeWidget*);
	QTreeWidgetItem* AddRootItem(QTreeWidget*, const std::string&);
	void AddChildItem(QTreeWidgetItem*, const std::string&);

	void AddBooksInList(QTreeWidget*, const std::string&);
	void PrintBooksList(QTreeWidget*);

	void BorrowBook(QTreeWidget* bookList, const std::string& bookID);
	void BorrowSelectedBook(QTreeWidget* bookList);

private:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void on_previous_clicked();
	void on_next_clicked();
	void on_borrowBookButton_clicked();
	void on_searchButton_clicked();

private:
	Ui::UserSearch* ui;
	TcpSocket* m_connectSocket;

	unsigned int m_bookListPageNumber;
};
