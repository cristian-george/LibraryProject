#include "User.h"

User::User()
{
	m_borrowedBooks.reserve(NUMBER_OF_BORROWED_BOOKS);
}

User::User(const std::string& username) : m_username(username)
{
	m_borrowedBooks.reserve(NUMBER_OF_BORROWED_BOOKS);
}

const std::string& User::GetUsername() const
{
	return m_username;
}

void User::SetUsername(const std::string& username)
{
	m_username = username;
}

void User::AddBorrowedBook(unsigned int bookID, const std::string& date, unsigned int borrowNumber)
{
	m_borrowedBooks.emplace_back(bookID, date, borrowNumber);
}

void User::ReturnBorrowedBook(unsigned int bookID)
{
	for (auto it = m_borrowedBooks.begin(); it != m_borrowedBooks.end(); ++it)
		if (std::get<0>(*it) == bookID)
		{
			m_borrowedBooks.erase(it);
			break;
		}
}

const std::vector<std::tuple<unsigned int, std::string, unsigned int>>& User::GetBorrowedBooks() const
{
	return m_borrowedBooks;
}