#pragma once
#include <iostream>
#include <string>

class Book
{
public:
	Book() = default;
	Book(const std::string&, const std::string&, const std::string&);

	void SetTitle(const std::string&);
	void SetAuthor(const std::string&);
	void SetISBN(const std::string&);

	const std::string& GetTitle() const;
	const std::string& GetAuthor() const;
	const std::string& GetISBN() const;

	friend std::ostream& operator<<(std::ostream&, const Book&);

private:
	std::string m_title, m_author, m_isbn;
};

