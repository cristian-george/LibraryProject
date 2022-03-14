#include "Library.h"

Library::Quantity Library::ConvertToQuantity(unsigned int quantity)
{
	switch (quantity)
	{
	case 0:
		return Library::Quantity::zero;
	case 1:
		return Library::Quantity::one;
	case 2:
		return Library::Quantity::two;
	case 3:
		return Library::Quantity::three;
	}
	throw "Invalid quantity";
}

Library::Library()
{
	m_books.reserve(NUMBER_OF_BOOKS);
	m_booksQuantity.reserve(NUMBER_OF_BOOKS);
}

const std::vector<Book>& Library::GetBooks() const
{
	return m_books;
}

const std::vector<Library::Quantity>& Library::GetBooksQuantity() const
{
	return m_booksQuantity;
}

User& Library::GetUser(const std::string& username)
{
	return m_users.at(username);
}

const std::unordered_map<std::string, User>& Library::GetUsers() const
{
	return m_users;
}

void Library::AddBook(const std::string& title, const std::string& author, const std::string& isbn)
{
	m_books.emplace_back(title, author, isbn);
}

void Library::AddBookQuantity(unsigned int quantity)
{
	m_booksQuantity.push_back(ConvertToQuantity(quantity));
}

void Library::AddUser(const std::string& username)
{
	m_users[username] = User(username);
}

void Library::AddBorrowedBook(const std::string& username, unsigned int bookID, const std::string& date, unsigned int borrowNumber)
{
	m_users[username].AddBorrowedBook(bookID, date, borrowNumber);
}

void Library::SetBookQuantity(unsigned int bookID, unsigned int quantity)
{
	m_booksQuantity[bookID] = ConvertToQuantity(quantity);
}

unsigned int Library::GetBookQuantity(unsigned int bookID) const
{
	Quantity quantity = m_booksQuantity[bookID];

	if (quantity == Quantity::zero)
		return 0;
	if (quantity == Quantity::one)
		return 1;
	if (quantity == Quantity::two)
		return 2;
	if (quantity == Quantity::three)
		return 3;

	throw "Invalid quantity";
}

void Library::RemoveUser(const std::string& username)
{
	m_users.erase(username);
}