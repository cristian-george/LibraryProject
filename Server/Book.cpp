#include "Book.h"

Book::Book(const std::string& title, const std::string& author, const std::string& isbn)
{
	m_title = title;
	m_author = author;
	m_isbn = isbn;
}

void Book::SetTitle(const std::string& title)
{
	m_title = title;
}

void Book::SetAuthor(const std::string& author)
{
	m_author = author;
}

void Book::SetISBN(const std::string& isbn)
{
	m_isbn = isbn;
}

const std::string& Book::GetTitle() const
{
	return m_title;
}

const std::string& Book::GetAuthor() const
{
	return m_author;
}

const std::string& Book::GetISBN() const
{
	return m_isbn;
}

std::ostream& operator<<(std::ostream& out, const Book& book)
{
	out << book.GetTitle() << std::endl;
	out << "Author: " << book.GetAuthor() << std::endl;
	out << "ISBN: " << book.GetISBN();

	return out;
}
