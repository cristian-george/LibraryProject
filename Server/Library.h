#pragma once
#include "Book.h"
#include "User.h"
#include <unordered_map>
#include <unordered_set>

constexpr auto NUMBER_OF_BOOKS = 10000;

class Library
{
public:
    enum class Quantity
    {
        zero = 0,
        one,
        two,
        three
    };

    Quantity ConvertToQuantity(unsigned int);

public:
    Library();

    const std::vector<Book>& GetBooks() const;
    const std::vector<Quantity>& GetBooksQuantity() const;
    User& GetUser(const std::string& username);
    const std::unordered_map<std::string, User>& GetUsers() const;

    void AddBook(const std::string& title, const std::string& author, const std::string& isbn);
    void AddBookQuantity(unsigned int quantity);
    void AddUser(const std::string& username);
    void AddBorrowedBook(const std::string& username, unsigned int bookID, const std::string& date, unsigned int borrowNumber);
    void RemoveUser(const std::string& username);

    void SetBookQuantity(unsigned int bookID, unsigned int quantity);
    unsigned int GetBookQuantity(unsigned int bookID) const;

private:
    std::vector<Book> m_books;
    std::vector<Quantity> m_booksQuantity;
    std::unordered_map<std::string, User> m_users;
};