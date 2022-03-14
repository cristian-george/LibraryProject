#include "Search.h"
#include "StringTokenizer.h"

void Search::ToLowerCase(std::string& word)
{
	std::for_each
	(
		word.begin(), word.end(),
		[](char& character)
		{
			character = tolower(character);
		}
	);
}

void Search::SetIndexes(const Library& library, Index& indexes, const std::string& option)
{
	const auto& books = library.GetBooks();

	for (size_t index = 0; index < books.size(); ++index)
	{
		std::string words;
		if (option == "title")
			words = books[index].GetTitle();
		else
			words = books[index].GetAuthor();

		Tokenizer token(words, DELIMITERS);
		while (token.NextToken())
		{
			std::string word = token.GetToken();
			ToLowerCase(word);
			indexes[word].insert(index);
		}
	}
}

std::unordered_set<unsigned int> Search::SetIntersection(const std::vector<std::set<unsigned int>>& sets)
{
	std::vector<unsigned int> result;

	if (sets.size() == 1)
	{
		result.assign(sets[0].begin(), sets[0].end());
	}
	else
		if (sets.size() >= 2)
		{
			std::set_intersection(sets[0].begin(), sets[0].end(), sets[1].begin(), sets[1].end(), std::back_inserter(result));

			std::vector<unsigned int> buffer;
			for (size_t index = 2; index < sets.size(); ++index)
			{
				buffer.clear();

				std::set_intersection(result.begin(), result.end(), sets[index].begin(), sets[index].end(), std::back_inserter(buffer));
				std::swap(result, buffer);
			}
		}

	return std::unordered_set<unsigned int>(result.begin(), result.end());
}

void Search::SetTitleIndexes(const Library& library)
{
	SetIndexes(library, m_titleIndexes, "title");
}

void Search::SetAuthorIndexes(const Library& library)
{
	SetIndexes(library, m_authorIndexes, "author");
}

void Search::SetISBNIndexes(const Library& library)
{
	const auto& books = library.GetBooks();

	for (size_t index = 0; index < books.size(); ++index)
	{
		const auto& isbn = books[index].GetISBN();
		if (!isbn.empty())
			m_isbnIndexes[isbn] = index;
	}
}

std::vector<unsigned int> Search::Searching(const std::vector<std::string>& words, const Index& indexes, unsigned int numberOfBooksOnPage)
{
	std::vector<std::set<unsigned int>> sets;

	std::set<unsigned int> found;

	using Pair = std::pair<size_t, std::string>;
	std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> aproximatelyFound;

	for (size_t i = 0; i < words.size(); ++i)
	{
		std::string currentWord = words[i];
		ToLowerCase(currentWord);

		if (indexes.find(currentWord) != indexes.end())
		{
			const auto& bookIDs = indexes.at(currentWord);
			for (const auto& bookID : bookIDs)
				found.insert(bookID);

			if (!found.empty())
				sets.push_back(found);

			found.clear();
		}
		else
		{
			for (const auto& element : indexes)
			{
				const auto& indexWord = element.first;

				size_t distance = LevenshteinDistance<std::string>(currentWord, indexWord);
				aproximatelyFound.push(std::make_pair(distance, indexWord));
			}
		}
	}

	std::unordered_set<unsigned int> exactFound = SetIntersection(sets);
	std::vector<unsigned int> searchResults;



	if (exactFound.size() > numberOfBooksOnPage)
		std::copy_n(exactFound.begin(), numberOfBooksOnPage, std::back_inserter(searchResults));
	else
		std::copy_n(exactFound.begin(), exactFound.size(), std::back_inserter(searchResults));



	while (!aproximatelyFound.empty() && searchResults.size() < numberOfBooksOnPage)
	{
		const auto& indexWord = aproximatelyFound.top().second;
		const auto& bookIDs = indexes.at(indexWord);

		for (const auto& bookID : bookIDs)
			if (searchResults.size() < numberOfBooksOnPage && exactFound.find(bookID) == exactFound.end())
				searchResults.push_back(bookID);
			else
				break;

		aproximatelyFound.pop();
	}

	return searchResults;
}

std::vector<unsigned int> Search::SearchByTitle(const std::vector<std::string>& words, unsigned int numberOfBooksOnPage)
{
	return Searching(words, m_titleIndexes, numberOfBooksOnPage);
}

std::vector<unsigned int> Search::SearchByAuthor(const std::vector<std::string>& words, unsigned int numberOfBooksOnPage)
{
	return Searching(words, m_authorIndexes, numberOfBooksOnPage);
}

unsigned int Search::SearchByISBN(const std::string& word)
{
	if (m_isbnIndexes.find(word) != m_isbnIndexes.end())
		return m_isbnIndexes[word];

	return -1;
}