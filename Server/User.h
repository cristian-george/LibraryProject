#pragma once
#include "Book.h"
#include <string>
#include <vector>
#include <tuple>

constexpr auto NUMBER_OF_BORROWED_BOOKS = 5;

class User
{
public:
	User();
	User(const std::string&);

	void SetUsername(const std::string&);

	const std::string& GetUsername() const;
	const std::vector<std::tuple<unsigned int, std::string, unsigned int>>& GetBorrowedBooks() const;

	void AddBorrowedBook(unsigned int, const std::string&, unsigned int);
	void ReturnBorrowedBook(unsigned int);

private:
	std::string m_username;
	std::vector<std::tuple<unsigned int, std::string, unsigned int>> m_borrowedBooks;
};