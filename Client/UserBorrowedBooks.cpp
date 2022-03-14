#include "UserBorrowedBooks.h"

constexpr auto COLUMN = 0;

UserBorrowedBooks::UserBorrowedBooks(TcpSocket* connectSocket, QWidget* parent)
	: connectSocket(connectSocket), QWidget(parent), ui(new Ui::UserBorrowedBooks)
{
	ui->setupUi(this);
	this->setAutoFillBackground(true);
	QPalette palette;
	QPixmap pixmap("background.jfif");
	palette.setBrush(QPalette::Window, QBrush(pixmap));
	this->setPalette(palette);

	PrintBorrowBooksList(ui->bookList);
}

UserBorrowedBooks::~UserBorrowedBooks()
{
	delete ui;
}

void UserBorrowedBooks::AddBlankItem(QTreeWidget* treeWidget)
{
	QString text("Book titles");

	constexpr auto scrollbarWidth = 850;
	for (int index = 0; index < scrollbarWidth; ++index)
		text.append(' ');

	QTreeWidgetItem* blank = AddRootItem(treeWidget, text.toStdString());
	blank->setDisabled(true);
	treeWidget->resizeColumnToContents(COLUMN);
}

QTreeWidgetItem* UserBorrowedBooks::AddRootItem(QTreeWidget* tree, const std::string& text)
{
	QFont font;
	font.setBold(true);

	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(COLUMN, QString(text.c_str()));
	item->setFont(COLUMN, font);
	tree->addTopLevelItem(item);
	return item;
}

void UserBorrowedBooks::AddChildItem(QTreeWidgetItem* otherItem, const std::string& text)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(COLUMN, QString(text.c_str()));
	item->setDisabled(true);
	otherItem->addChild(item);
}

void UserBorrowedBooks::AddBooksInList(QTreeWidget* treeWidget, const std::string& filename)
{
	AddBlankItem(treeWidget);

	std::ifstream fin(filename);
	if (!fin.is_open())
		return;

	std::string title, id, author, isbn, date, borrowNumber;
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

			std::getline(fin, id);
			AddChildItem(root, id);

			std::getline(fin, date);
			AddChildItem(root, date);

			std::getline(fin, borrowNumber);
			AddChildItem(root, borrowNumber);
		}
	}

	fin.close();
	remove(filename.c_str());
}

void UserBorrowedBooks::PrintBorrowBooksList(QTreeWidget* bookList)
{
	ui->bookList->clear();

	std::string message = "showBorrowedBooksList";
	connectSocket->Send(message.c_str(), message.size());

	std::array<char, BUFFER_SIZE> receiveBuffer;
	int received;
	connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::string filename(receiveBuffer.data(), received);

	AddBooksInList(bookList, filename);
}

void UserBorrowedBooks::ExtendBorrowBookDuration(const std::string& bookID)
{
	std::string message = "extendBorrowDuration " + bookID;
	connectSocket->Send(message.c_str(), message.size());

	std::array<char, BUFFER_SIZE> receiveBuffer;
	int received;
	connectSocket->Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::string receivedMessage(receiveBuffer.data(), received);

	if (receivedMessage == "OK")
	{
		QMessageBox::information(this, "Extend borrow duration", "You have successfully extended your borrow duration for this book !");

		PrintBorrowBooksList(ui->bookList);
		ui->bookList->selectionModel()->clear();
	}
	else
	{
		QMessageBox::critical(this, "Extend borrow duration", receivedMessage.c_str());
	}
}

void UserBorrowedBooks::ReturnBook(const std::string& bookID)
{
	std::string message = "returnBook " + bookID;
	connectSocket->Send(message.c_str(), message.size());

	QMessageBox::information(this, "Return book", "You have successfully returned this book !");

	PrintBorrowBooksList(ui->bookList);
	ui->bookList->selectionModel()->clear();
}

void UserBorrowedBooks::closeEvent(QCloseEvent* event)
{
	this->deleteLater();
	UserInterface* userInterface = new UserInterface(connectSocket);
	userInterface->showMaximized();
}

void UserBorrowedBooks::on_readBookButton_clicked()
{
	if (ui->bookList->selectionModel()->hasSelection())
	{
		QWidget* widget = new QWidget();
		widget->setMinimumSize(800, 800);
		widget->setMaximumSize(800, 800);
		widget->setWindowTitle(QString("Reading book..."));
		widget->setWindowIcon(QIcon("library.png"));

		QPalette palette;
		QPixmap pixmap("read.jpg");
		palette.setBrush(QPalette::Window, QBrush(pixmap));
		widget->setPalette(palette);
		widget->show();
	}
}

void UserBorrowedBooks::on_returnBookButton_clicked()
{
	if (ui->bookList->selectionModel()->hasSelection())
	{
		QTreeWidgetItem* selectedItem = ui->bookList->currentItem()->child(2);
		std::string bookID = selectedItem->text(COLUMN).toStdString().substr(4);

		ReturnBook(bookID);
	}
}

void UserBorrowedBooks::on_extendBorrowDurationButton_clicked()
{
	if (ui->bookList->selectionModel()->hasSelection())
	{
		QTreeWidgetItem* selectedItem = ui->bookList->currentItem()->child(2);
		std::string bookID = selectedItem->text(COLUMN).toStdString().substr(4);

		ExtendBorrowBookDuration(bookID);
	}
}