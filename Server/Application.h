#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <regex>
#include <array>
#include "Library.h"
#include "Search.h"
#include "StringTokenizer.h"
#include "../NetworkingLibrary/TcpSocket.h"
#include "../Logger/Logger.h"

constexpr auto BUFFER_SIZE = 1024;
constexpr auto BOOKS_PER_PAGE = 50;
constexpr auto BORROW_TIME = 14;
constexpr auto EXTEND_BORROW_DURATION = 0;

class Application
{
public:
	enum class Option : char
	{
		None = 0,
		Register,
		Login,
		Logout,
		ShowBooksList,
		BorrowBook,
		ShowBorrowedBooksList,
		ReadBook,
		ReturnBook,
		ExtendBorrowDuration,
		SearchByTitle,
		SearchByAuthor,
		SearchByISBN,
		DeleteAccount,
		Disconnect,
	};
	Option ConvertToOption(const std::string&);

public:
	Application();
	static void ClientRun(Application*, SOCKET);

	template<class ... Args>
	void WriteLogger(Logger::Level level, Args&& ... params)
	{
		m_consoleLogger.Log(level, std::forward<Args>(params)...);
		m_fileLogger.Log(level, std::forward<Args>(params)...);
	}

public:
	const std::vector<std::string>& GetTokens(const std::array<char, BUFFER_SIZE>& receiveBuffer, int received);
	Option ClientOption(TcpSocket* client, const std::array<char, BUFFER_SIZE>& receiveBuffer, int received);

	std::string Register(TcpSocket* client, const std::string& username, const std::string& password);
	std::string Login(TcpSocket* client, const std::string& username, const std::string& password);
	std::string Logout(TcpSocket* client);
	std::string ShowBooksList(TcpSocket* client, const std::string& page);
	std::string BorrowBook(TcpSocket* client, const std::string& bookID);
	std::string ShowBorrowedBooksList(TcpSocket* client);
	void ReturnBook(TcpSocket* client, const std::string& bookID);
	std::string ExtendBorrowDuration(TcpSocket* client, const std::string& bookID);
	std::string SearchBy(const std::string& option, TcpSocket* client, const std::vector<std::string>& query);
	std::string SearchByISBN(TcpSocket* client, const std::vector<std::string>& query);
	std::string DeleteAccount(TcpSocket* client);

public:
	void AddUser(const std::string& username, const std::string& password);
	const std::unordered_set<std::string>& GetUsernames();
	void AddBooks();
	void AddUsers();
	void PrintBook(std::ofstream&, unsigned int bookID);
	void EditBookQuantity(unsigned int bookID, unsigned int quantity);
	void AddBorrowedBook(const std::string& username, unsigned int bookID, const std::string&, unsigned int borrowNumber);
	void ReturnBorrowedBook(const std::string& username, unsigned int bookID);
	void ExtendBorrowDuration(const std::string& username, unsigned int bookID);
	void RemoveUser(const std::string& username);

public:
	std::string TimeToString(const std::time_t& time);
	std::time_t StringToTime(const std::string& date);
	unsigned int GetTimeDifference(const std::time_t& startDate, const std::time_t& endDate);
	void AddDaysToDate(std::time_t& date, unsigned int days);

private:
	static const uint64_t FNV_offset_basis = 14695981039346656037;
	static const uint64_t FNV_prime = 1099511628211;
	uint64_t Hash(const std::string&);

private:
	bool VerifyUsername(const std::string&);
	bool VerifyPassword(const std::string&);

	bool VerifyRegister(const std::string&, const std::string&);
	bool VerifyLogin(const std::string&, const std::string&);
	bool VerifyDeleteAccount(const std::string&);
	bool VerifyBorrow(const std::string&, int);
	bool VerifyExtendBorrowDuration(const std::string&, unsigned int);

private:
	bool VerifySearch(const std::vector<unsigned int>& bookIDs);
	bool VerifySearchByISBN(const std::vector<std::string>& tokens);

private:
	Library m_library;
	Search m_search;

	Logger m_consoleLogger;
	Logger m_fileLogger;
};