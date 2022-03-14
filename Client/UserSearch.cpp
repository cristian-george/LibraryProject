#include "UserSearch.h"

constexpr auto COLUMN = 0;
constexpr auto MINIMUM_PAGE_NUMBER = 1;
constexpr auto MAXIMUM_PAGE_NUMBER = 100;

UserSearch::SearchOption UserSearch::ConvertToSearchOption(const QString& type)
{
	if (type == "Search by title")
		return SearchOption::SearchByTitle;
	else if (type == "Search by author")
		return SearchOption::SearchByAuthor;
	else if (type == "Search by ISBN")
		return SearchOption::SearchByISBN;

	return SearchOption::None;
}

UserSearch::UserSearch(TcpSocket* connectSocket, QWidget* parent)
	: m_connectSocket(connectSocket), m_bookListPageNumber(1), QWidget(parent), ui(new Ui::UserSearch)
{
	ui->setupUi(this);
	this->setAutoFillBackground(true);
	QPalette palette;
	QPixmap pixmap("background.jfif");
	palette.setBrush(QPalette::Window, QBrush(pixmap));
	this->setPalette(palette);

	PrintBooksList(ui->bookList);
}

UserSearch::~UserSearch()
{
	delete ui;
}

void UserSearch::AddBlankItem(QTreeWidget* treeWidget)
{
	QString text("Book titles");

	constexpr auto scrollbarWidth = 850;
	for (int index = 0; index < scrollbarWidth; ++index)
		text.append(' ');

	QTreeWidgetItem* blank = AddRootItem(treeWidget, text.toStdString());
	blank->setDisabled(true);
	treeWidget->resizeColumnToContents(COLUMN);
}

QTreeWidgetItem* UserSearch::AddRootItem(QTreeWidget* tree, const std::string& text)
{
	QFont font;
	font.setBold(true);

	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(COLUMN, QString(text.c_str()));
	item->setFont(COLUMN, font);
	tree->addTopLevelItem(item);
	return item;
}

void UserSearch::AddChildItem(QTreeWidgetItem* otherItem, const std::string& text)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(COLUMN, QString(text.c_str()));
	item->setDisabled(true);
	otherItem->addChild(item);
}

void UserSearch::AddBooksInList(QTreeWidget* treeWidget, const std::string& filename)
{
	AddBlankItem(treeWidget);

	std::ifstream fin(filename);

	std::string title, ID, author, isbn, quantity;
	while (!fin.eof())
	{
		std::getline(fin, title);
		if (!title.empty())
		{
			QTreeWidgetItem* root = AddRootItem(treeWidget, title);

			std::getline(fin, author);
			AddChildItem(root, author);

			std::getline(fin, isbn);
			AddChildItem(root, isbn);

			std::getline(fin, ID);
			AddChildItem(root, ID);

			std::getline(fin, quantity);
			AddChildItem(root, quantity);
		}
	}

	fin.close();
	remove(filename.c_str());
}

void UserSearch::PrintBooksList(QTreeWidget* bookList)
{
	bookList->clear();

	std::string pageNumber = "page " + std::to_string(m_bookListPageNumber);
	ui->pageNumber->setText(QString(pageNumber.c_str()));

	std::string message = "showBooksList " + std::to_string(m_bookListPageNumber);
	m_connectSocket->Send(message.c_str(), message.size());

	std::array<char, BUFFER_SIZE> receiveBuffer;
	int received;
	m_connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::string filename(receiveBuffer.data(), received);

	AddBooksInList(bookList, filename);
}

void UserSearch::BorrowBook(QTreeWidget* bookList, const std::string& bookID)
{
	std::string message = "borrowBook " + bookID;
	m_connectSocket->Send(message.c_str(), message.size());

	std::array<char, BUFFER_SIZE> receiveBuffer;
	int received;
	m_connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::string receivedMessage(receiveBuffer.data(), received);

	if (receivedMessage == "OK")
	{
		QMessageBox::information(this, "Borrow", "You have successfully borrowed this book !");
		PrintBooksList(bookList);
	}
	else
	{
		QMessageBox::critical(this, "Borrow", receivedMessage.c_str());
	}
}

void UserSearch::BorrowSelectedBook(QTreeWidget* bookList)
{
	QTreeWidgetItem* selectedItem = bookList->currentItem()->child(2);
	std::string bookID = selectedItem->text(COLUMN).toStdString().substr(4);

	BorrowBook(bookList, bookID);
	bookList->selectionModel()->clear();
}

void UserSearch::closeEvent(QCloseEvent* event)
{
	this->deleteLater();
	UserInterface* userInterface = new UserInterface(m_connectSocket);
	userInterface->showMaximized();
}

void UserSearch::on_previous_clicked()
{
	if (m_bookListPageNumber - 1 >= MINIMUM_PAGE_NUMBER)
	{
		ui->bookList->clear();
		--m_bookListPageNumber;

		PrintBooksList(ui->bookList);
	}
}

void UserSearch::on_next_clicked()
{
	if (m_bookListPageNumber + 1 <= MAXIMUM_PAGE_NUMBER)
	{
		ui->bookList->clear();
		++m_bookListPageNumber;

		PrintBooksList(ui->bookList);
	}
}

void UserSearch::on_borrowBookButton_clicked()
{
	if (ui->bookList->selectionModel()->hasSelection() && ui->foundBookList->selectionModel()->hasSelection())
	{
		ui->bookList->selectionModel()->clear();
		ui->foundBookList->selectionModel()->clear();

		return;
	}

	if (ui->bookList->selectionModel()->hasSelection())
	{
		BorrowSelectedBook(ui->bookList);
	}
	else
		if (ui->foundBookList->selectionModel()->hasSelection())
		{
			BorrowSelectedBook(ui->foundBookList);
			PrintBooksList(ui->bookList);
			on_searchButton_clicked();
		}
}

void UserSearch::on_searchButton_clicked()
{
	QString query = ui->searchBox->text();

	if (!query.isEmpty())
	{
		SearchOption option = ConvertToSearchOption(ui->comboBox->currentText());
		std::string message;

		switch (option)
		{
		case SearchOption::SearchByTitle:
		{
			message = "searchByTitle " + query.toStdString();
			break;
		}
		case SearchOption::SearchByAuthor:
		{
			message = "searchByAuthor " + query.toStdString();
			break;
		}
		case SearchOption::SearchByISBN:
		{
			message = "searchByISBN " + query.toStdString();
			break;
		}

		default:
			break;
		};

		m_connectSocket->Send(message.c_str(), message.size());

		std::array<char, BUFFER_SIZE> receiveBuffer;
		int received;
		m_connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
		std::string receivedMessage(receiveBuffer.data(), received);

		if (receivedMessage.substr(0, 2) == "OK")
		{
			ui->foundBookList->clear();
			AddBooksInList(ui->foundBookList, receivedMessage.substr(3));
		}
		else
		{
			QMessageBox::critical(this, "Searching book...", receivedMessage.c_str());
		}
	}
	else
	{
		ui->foundBookList->clear();
	}
}