#include "Application.h"

std::ofstream out("log.txt", std::ios::app);

Application::Option Application::ConvertToOption(const std::string& type)
{
	if (type == "register")
		return Option::Register;
	else if (type == "login")
		return Option::Login;
	else if (type == "logout")
		return Option::Logout;
	else if (type == "showBooksList")
		return Option::ShowBooksList;
	else if (type == "borrowBook")
		return Option::BorrowBook;
	else if (type == "showBorrowedBooksList")
		return Option::ShowBorrowedBooksList;
	else if (type == "readBook")
		return Option::ReadBook;
	else if (type == "returnBook")
		return Option::ReturnBook;
	else if (type == "extendBorrowDuration")
		return Option::ExtendBorrowDuration;
	else if (type == "searchByTitle")
		return Option::SearchByTitle;
	else if (type == "searchByAuthor")
		return Option::SearchByAuthor;
	else if (type == "searchByISBN")
		return Option::SearchByISBN;
	else if (type == "deleteAccount")
		return Option::DeleteAccount;
	else if (type == "disconnect")
		return Option::Disconnect;

	return Option::None;
}

Application::Application() :
	m_consoleLogger(std::cout), m_fileLogger(out)
{
	AddBooks();
	AddUsers();

	m_search.SetTitleIndexes(m_library);
	m_search.SetAuthorIndexes(m_library);
	m_search.SetISBNIndexes(m_library);
}

void Application::ClientRun(Application* application, SOCKET clientSocket)
{
	TcpSocket* client = new TcpSocket(clientSocket);
	client->SetUsername("SOCKET #" + std::to_string(clientSocket));
	application->WriteLogger(Logger::Level::Info, client->GetUsername(), "has connected");

	while (true)
	{
		std::array<char, BUFFER_SIZE> receiveBuffer;
		int received;
		client->Receive(receiveBuffer.data(), receiveBuffer.size(), received);

		if (application->ClientOption(client, receiveBuffer, received) == Option::Disconnect)
			break;
	}
}

const std::vector<std::string>& Application::GetTokens(const std::array<char, BUFFER_SIZE>& receiveBuffer, int received)
{
	static std::vector<std::string> receivedMessageTokenized;
	receivedMessageTokenized.clear();

	std::string receivedMessage(receiveBuffer.data(), received);
	Tokenizer token(receivedMessage, " ");

	while (token.NextToken())
		receivedMessageTokenized.push_back(token.GetToken());

	return receivedMessageTokenized;
}

Application::Option Application::ClientOption(TcpSocket* client, const std::array<char, BUFFER_SIZE>& receiveBuffer, int received)
{
	std::vector<std::string> tokens = GetTokens(receiveBuffer, received);
	Option option = ConvertToOption(tokens[0]);
	tokens.erase(tokens.begin());

	std::string messageToSend;

	switch (option)
	{
	case Option::Register:
	{
		messageToSend = Register(client, tokens[0], tokens[1]);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::Login:
	{
		messageToSend = Login(client, tokens[0], tokens[1]);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::Logout:
	{
		messageToSend = Logout(client);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::ShowBooksList:
	{
		messageToSend = ShowBooksList(client, tokens[0]);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::BorrowBook:
	{
		messageToSend = BorrowBook(client, tokens[0]);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::ShowBorrowedBooksList:
	{
		messageToSend = ShowBorrowedBooksList(client);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::ReadBook:
	{
		WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has successfully read a book");
		break;
	}
	case Option::ReturnBook:
	{
		ReturnBook(client, tokens[0]);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::ExtendBorrowDuration:
	{
		messageToSend = ExtendBorrowDuration(client, tokens[0]);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::SearchByTitle:
	{
		messageToSend = SearchBy("Title", client, tokens);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::SearchByAuthor:
	{
		messageToSend = SearchBy("Author", client, tokens);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::SearchByISBN:
	{
		messageToSend = SearchByISBN(client, tokens);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::DeleteAccount:
	{
		messageToSend = DeleteAccount(client);
		client->Send(messageToSend.c_str(), messageToSend.size());
		break;
	}
	case Option::Disconnect:
	{
		WriteLogger(Logger::Level::Info, client->GetUsername(), "has disconnected");
		break;
	}

	default:
		break;
	};

	return option;
}

std::string Application::Register(TcpSocket* client, const std::string& username, const std::string& password)
{
	try
	{
		if (VerifyRegister(username, password))
		{
			WriteLogger(Logger::Level::Info, client->GetUsername(), ": has created an account with username: ", username);

			client->SetUsername(username);
			AddUser(username, std::to_string(Hash(password)));
		}
	}
	catch (const char* errorMessage)
	{
		WriteLogger(Logger::Level::Warning, client->GetUsername(), ": attempted to register");
		return errorMessage;
	}

	return "OK";
}

std::string Application::Login(TcpSocket* client, const std::string& username, const std::string& password)
{
	try
	{
		if (VerifyLogin(username, password))
		{
			client->SetUsername(username);
			WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has successfully logged in");
		}
	}
	catch (const char* errorMessage)
	{
		WriteLogger(Logger::Level::Warning, client->GetUsername(), ": attempted to login");
		return errorMessage;
	}

	return "OK";
}

std::string Application::Logout(TcpSocket* client)
{
	WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has successfully logged out");
	client->SetUsername("SOCKET #" + std::to_string(client->GetSocket()));
	return "OK";
}

std::string Application::ShowBooksList(TcpSocket* client, const std::string& page)
{
	unsigned int pageNumber = std::atoi(page.c_str());

	std::string filename = "../Client/booksList.txt";
	std::ofstream fout(filename, std::ios::app);

	for (unsigned int bookID = BOOKS_PER_PAGE * (pageNumber - 1); bookID < BOOKS_PER_PAGE * pageNumber; ++bookID)
	{
		PrintBook(fout, bookID);
		fout << "Quantity: " << m_library.GetBookQuantity(bookID) << std::endl;
	}

	return filename;
}

std::string Application::BorrowBook(TcpSocket* client, const std::string& bookID)
{
	unsigned int id = std::atoi(bookID.c_str());

	try
	{
		if (VerifyBorrow(client->GetUsername(), id))
		{
			WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has successfully borrowed the book with ID:", bookID);

			unsigned int quantity = m_library.GetBookQuantity(id) - 1;
			EditBookQuantity(id, quantity);

			std::time_t currentTime = time(0);
			AddDaysToDate(currentTime, BORROW_TIME);

			AddBorrowedBook(client->GetUsername(), id, TimeToString(currentTime), EXTEND_BORROW_DURATION);
		}
	}
	catch (const char* errorMessage)
	{
		WriteLogger(Logger::Level::Warning, "User:", client->GetUsername(), "attempted to borrow the book with ID:", bookID);
		return errorMessage;
	}

	return "OK";
}

std::string Application::ShowBorrowedBooksList(TcpSocket* client)
{
	std::string filename = "../Client/borrowedBooksList.txt";
	std::ofstream fout(filename, std::ios::app);

	const auto& borrowedBook = m_library.GetUser(client->GetUsername()).GetBorrowedBooks();
	for (const auto& borrowBook : borrowedBook)
	{
		PrintBook(fout, std::get<0>(borrowBook));
		fout << "Return date: " << std::get<1>(borrowBook) << std::endl;
		fout << "Borrow number: " << std::get<2>(borrowBook) << std::endl;
	}

	return filename;
}

void Application::ReturnBook(TcpSocket* client, const std::string& bookID)
{
	unsigned int id = std::atoi(bookID.c_str());

	int quantity = m_library.GetBookQuantity(id) + 1;
	EditBookQuantity(id, quantity);
	ReturnBorrowedBook(client->GetUsername(), id);

	WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has successfully returned the book with ID:", bookID);
}

std::string Application::ExtendBorrowDuration(TcpSocket* client, const std::string& bookID)
{
	try
	{
		unsigned int id = std::atoi(bookID.c_str());

		if (VerifyExtendBorrowDuration(client->GetUsername(), id))
		{
			WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has successfully extended borrow duration for the book with ID:", bookID);
			ExtendBorrowDuration(client->GetUsername(), id);
		}
	}
	catch (const char* errorMessage)
	{
		WriteLogger(Logger::Level::Warning, "User:", client->GetUsername(), "attempted to extend their borrow duration for a book");
		return errorMessage;
	}

	return "OK";
}

std::string Application::SearchBy(const std::string& option, TcpSocket* client, const std::vector<std::string>& query)
{
	std::string filename = "../ClientbooksFoundBy";
	std::vector<unsigned int> bookIDs;

	if (option == "Title")
	{
		filename += "Title.txt";
		bookIDs = m_search.SearchByTitle(query, BOOKS_PER_PAGE);
	}
	else
	{
		filename += "Author.txt";
		bookIDs = m_search.SearchByAuthor(query, BOOKS_PER_PAGE);
	}
	try
	{
		if (VerifySearch(bookIDs))
		{
			WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has found", std::to_string(bookIDs.size()), "books");

			std::ofstream fout(filename);

			for (const auto& bookID : bookIDs)
			{
				PrintBook(fout, bookID);
				fout << "Quantity: " << m_library.GetBookQuantity(bookID) << std::endl;
			}
		}
	}
	catch (const char* errorMessage)
	{
		WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "hasn't found any books");
		return errorMessage;
	}

	return "OK " + filename;
}

std::string Application::SearchByISBN(TcpSocket* client, const std::vector<std::string>& query)
{
	std::string filename = "../Client/bookFoundByISBN.txt";

	try
	{
		if (VerifySearchByISBN(query))
		{
			WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "has found the book with ISBN:", query[0]);

			std::ofstream fout(filename);

			unsigned int bookID = m_search.SearchByISBN(query[0]);
			PrintBook(fout, bookID);
			fout << "Quantity: " << m_library.GetBookQuantity(bookID) << std::endl;
		}
	}
	catch (const char* errorMessage)
	{
		WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "hasn't found the book with ISBN:", query[0]);
		return errorMessage;
	}

	return "OK " + filename;
}

std::string Application::DeleteAccount(TcpSocket* client)
{
	try
	{
		std::string username = client->GetUsername();

		if (VerifyDeleteAccount(username))
		{
			WriteLogger(Logger::Level::Info, "User:", username, "has removed their library account");

			RemoveUser(username);
			client->SetUsername("SOCKET #" + std::to_string(client->GetSocket()));
		}
	}
	catch (const char* errorMessage)
	{
		WriteLogger(Logger::Level::Info, "User:", client->GetUsername(), "attempted to delete their account");
		return errorMessage;
	}

	return "OK";
}

void Application::AddUser(const std::string& username, const std::string& password)
{
	m_library.AddUser(username);

	std::ofstream fout("users.txt", std::ios::app);
	fout << username << " " << password << std::endl;
	fout.close();
}

const std::unordered_set<std::string>& Application::GetUsernames()
{
	static std::unordered_set<std::string> usernames;
	usernames.clear();

	auto& users = m_library.GetUsers();

	for (const auto& user : users)
	{
		std::string username = user.first;
		usernames.insert(username);
	}

	return usernames;
}

void Application::AddBooks()
{
	std::ifstream finTitle("titles.txt");
	std::ifstream finAuthor("authors.txt");
	std::ifstream finISBN("isbns.txt");
	std::ifstream finAmount("amounts.txt");

	std::string title, author, isbn;
	unsigned int quantity;

	for (int index = 0; index < NUMBER_OF_BOOKS; ++index)
	{
		std::getline(finTitle, title);
		std::getline(finAuthor, author);
		std::getline(finISBN, isbn);
		finAmount >> quantity;
		finAmount.get();

		m_library.AddBook(title, author, isbn);
		m_library.AddBookQuantity(quantity);
	}
}

void Application::AddUsers()
{
	std::ifstream finUser("users.txt");
	if (finUser.peek() == std::ifstream::traits_type::eof())
		return;

	std::string line;

	while (!finUser.eof())
	{
		std::getline(finUser, line);
		Tokenizer token(line, " ");
		if (token.NextToken())
		{
			std::string username = token.GetToken();
			m_library.AddUser(username);

			std::ifstream finBorrowedBooks(username);
			if (finBorrowedBooks.is_open())
			{
				std::string borrowedBookID, date, borrowNumber;

				while (finBorrowedBooks >> borrowedBookID >> date >> borrowNumber)
					if (!borrowedBookID.empty())
					{
						m_library.AddBorrowedBook(username, std::atoi(borrowedBookID.c_str()), date, std::atoi(borrowNumber.c_str()));
					}

				finBorrowedBooks.close();
			}
		}
	}

	finUser.close();
}

void Application::PrintBook(std::ofstream& fout, unsigned int bookID)
{
	const auto& book = m_library.GetBooks()[bookID];
	fout << book << std::endl;
	fout << "ID: " << bookID << std::endl;
}

void Application::EditBookQuantity(unsigned int bookID, unsigned int quantity)
{
	std::fstream file("amounts.txt");
	if (!file.is_open())
		throw "Failed to open file!";

	long long int pos = file.tellp();
	for (unsigned int index = 1; index <= bookID; ++index)
	{
		file.ignore(LONG_MAX, '\n');
	}

	pos = file.tellp();
	file.seekp(pos);
	file << quantity;

	m_library.SetBookQuantity(bookID, quantity);
}

void Application::AddBorrowedBook(const std::string& username, unsigned int bookID, const std::string& date, unsigned int borrowNumber)
{
	m_library.AddBorrowedBook(username, bookID, date, borrowNumber);

	std::ofstream fout(username, std::ios::app);
	fout << bookID << " " << date << " " << borrowNumber << std::endl;
}

void Application::ReturnBorrowedBook(const std::string& username, unsigned int bookID)
{
	m_library.GetUser(username).ReturnBorrowedBook(bookID);

	std::ofstream fout(username);

	const auto& borrowedBooks = m_library.GetUser(username).GetBorrowedBooks();

	for (const auto& borrowedBook : borrowedBooks)
		fout << std::get<0>(borrowedBook) << " " << std::get<1>(borrowedBook) << " " << std::get<2>(borrowedBook) << std::endl;
}

void Application::ExtendBorrowDuration(const std::string& username, unsigned int bookID)
{
	const auto& user = m_library.GetUser(username);
	const auto& borrowedBooks = user.GetBorrowedBooks();

	std::time_t date;
	unsigned int borrowNumber = 0;

	for (const auto& borrowedBook : borrowedBooks)
		if (std::get<0>(borrowedBook) == bookID)
		{
			date = StringToTime(std::get<1>(borrowedBook));
			borrowNumber = std::get<2>(borrowedBook);
			break;
		}

	const unsigned int numberOfDays = 14;
	AddDaysToDate(date, numberOfDays);

	ReturnBorrowedBook(username, bookID);
	AddBorrowedBook(username, bookID, TimeToString(date), borrowNumber + 1);
}

void Application::RemoveUser(const std::string& username)
{
	std::string line;
	std::ifstream fin("users.txt");

	if (!fin.is_open())
		throw "Failed to open file!";

	m_library.RemoveUser(username);
	remove(username.c_str());

	std::vector<std::string> lines;
	lines.reserve(m_library.GetUsers().size());

	while (std::getline(fin, line))
	{
		if (line.find(username) == std::string::npos)
			lines.emplace_back(line);
	}
	fin.close();

	std::ofstream fout("users.txt");
	for (const auto& line : lines)
		fout << line << std::endl;

	fout.close();
}

std::string Application::TimeToString(const std::time_t& time)
{
	std::tm tmDate;
	localtime_s(&tmDate, &time);
	std::string timeString = std::to_string(tmDate.tm_mday) + '.' + std::to_string(tmDate.tm_mon + 1) + '.' + std::to_string(tmDate.tm_year + 1900);
	return timeString;
}

std::time_t Application::StringToTime(const std::string& date)
{
	// dd.mm.yyyy
	std::tm tmTime = { 0, 0, 0, 0, 0, 0 };
	std::time_t time;

	if (!std::regex_match(date, std::regex("^(0?[1-9]|[12][0-9]|3[01])[\\.\\-\\/](0?[1-9]|1[012])[\\.\\-\\/]\\d{4}$")))
	{
		std::cerr << "Invalid date format\n";
		time = std::mktime(&tmTime);
		return time;
	}

	int day, month, year;
	day = !isdigit(date[1]) ? date[0] - '0' : ((date[0] - '0') * 10 + date[1] - '0');
	month = !isdigit(date[3]) ? (date[2] - '0' - 1) : !isdigit(date[4]) ? (!isdigit(date[2]) ? date[3] - '0' - 1 : ((date[2] - '0') * 10 + date[3] - '0' - 1)) : (date[3] - '0') * 10 + date[4] - '0' - 1;
	year = !isdigit(date[5]) ? std::stoi(date.substr(6, 4)) - 1900 : !isdigit(date[4]) ? std::stoi(date.substr(5, 4)) - 1900 : std::stoi(date.substr(4, 4)) - 1900;

	tmTime = { 0, 0, 0, day, month, year };
	time = std::mktime(&tmTime);

	return time;
}

unsigned int Application::GetTimeDifference(const std::time_t& startDate, const std::time_t& endDate)
{
	int difference = 0;

	if (startDate != (std::time_t)(-1) && endDate != (std::time_t)(-1))
		difference = std::difftime(endDate, startDate) / (60.0 * 60.0 * 24.0);
	return abs(difference);
}

void Application::AddDaysToDate(std::time_t& date, unsigned int days)
{
	std::tm tmDate;
	localtime_s(&tmDate, &date);
	tmDate.tm_mday += days;
	date = mktime(&tmDate);
}

bool Application::VerifyUsername(const std::string& username)
{
	return regex_match(username, std::regex("(?=.*[A-Za-z])(?=.*[0-9])[A-Za-z0-9]{8,20}"));
}

bool Application::VerifyPassword(const std::string& password)
{
	return regex_match(password, std::regex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[@$!%*?&])[A-Za-z0-9@$!%*?&]{8,20}"));
}

uint64_t Application::Hash(const std::string& word)
{
	uint64_t hash = FNV_offset_basis;
	for (size_t index = 0; index < word.length(); index++)
	{
		hash = hash ^ (word[index]);
		hash = hash * FNV_prime;
	}
	return hash;
}

bool Application::VerifyRegister(const std::string& username, const std::string& password)
{
	auto& usernames = GetUsernames();
	if (usernames.find(username) != usernames.end())
		throw "An user with this username already exists.";

	if (!VerifyUsername(username))
		throw "Your username should contain minimum 8 characters and maximum 20 characters, at least one uppercase letter, one lowercase letter and one number.";

	if (!VerifyPassword(password))
		throw "Your password should contain minimum 8 characters and maximum 20 characters, at least one uppercase letter, one lowercase letter, one number and one special character.";

	return true;
}

bool Application::VerifyLogin(const std::string& username, const std::string& password)
{
	auto& usernames = GetUsernames();
	if (usernames.find(username) == usernames.end())
		throw "There are no users with this username.";

	std::ifstream fin("Users.txt");
	if (fin.peek() == std::ifstream::traits_type::eof())
		throw "There are no users with this username.";

	std::string line;
	while (!fin.eof())
	{
		std::getline(fin, line);
		if (line.find(username) != std::string::npos)
		{
			std::string hashedPassword = std::to_string(Hash(password));
			if (line.find(hashedPassword) == std::string::npos)
				throw "Your password is incorrect.";

			break;
		}
	}

	return true;
}

bool Application::VerifyDeleteAccount(const std::string& username)
{
	auto& user = m_library.GetUser(username);
	if (!user.GetBorrowedBooks().empty())
		throw "You have to return all your borrowed books before removing your account !";

	return true;
}

bool Application::VerifyBorrow(const std::string& username, int bookID)
{
	int quantity = m_library.GetBookQuantity(bookID);

	const auto& user = m_library.GetUser(username);
	const auto& borrowedBooks = user.GetBorrowedBooks();

	const unsigned int maxBorrowedBooks = 5;
	if (user.GetBorrowedBooks().size() == maxBorrowedBooks)
		throw "You have too many borrowed books !";

	for (const auto& borrowedBook : borrowedBooks)
		if (std::get<1>(borrowedBook) == TimeToString(time(0)))
			throw "You have borrowed one or more books that need to be returned !";

	if (quantity == 0)
		throw "The book is unavailable !";

	for (const auto& borrowedBook : borrowedBooks)
		if (std::get<0>(borrowedBook) == bookID)
			throw "You have already borrowed this book !";

	return true;
}

bool Application::VerifyExtendBorrowDuration(const std::string& username, unsigned int bookID)
{
	const auto& user = m_library.GetUser(username);
	const auto& borrowedBooks = user.GetBorrowedBooks();

	const unsigned int maxExtendBorrowDuration = 2;

	for (const auto& borrowedBook : borrowedBooks)
		if (std::get<0>(borrowedBook) == bookID && std::get<2>(borrowedBook) == maxExtendBorrowDuration)
			throw "You can't extend your borrow duration for this book !";

	return true;
}

bool Application::VerifySearch(const std::vector<unsigned int>& bookIDs)
{
	if (!bookIDs.size())
		throw "No results were found";

	return true;
}

bool Application::VerifySearchByISBN(const std::vector<std::string>& query)
{
	if (query.size() > 1)
		throw "Your query must contain one word !";

	if (m_search.SearchByISBN(query[0]) == -1)
		throw "No book with that ISBN was found !";

	return true;
}