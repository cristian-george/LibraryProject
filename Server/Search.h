#pragma once
#include "Library.h"
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>

constexpr auto DELIMITERS = " _.,-:;!?#&()\"/0123456789";

template<typename T>
typename T::size_type LevenshteinDistance(const T& source, const T& target)
{
	if (source.size() > target.size())
		return LevenshteinDistance(target, source);

	using Size = typename T::size_type;
	const Size minSize = source.size(), maxSize = target.size();
	std::vector<Size> levDist(minSize + 1);

	for (Size index = 0; index <= minSize; ++index)
		levDist[index] = index;

	for (Size j = 1; j <= maxSize; ++j)
	{
		Size previousDiagonal = levDist[0], previousDiagonalSave;
		++levDist[0];

		for (Size i = 1; i <= minSize; ++i)
		{
			previousDiagonalSave = levDist[i];

			if (source[i - 1] == target[j - 1])
				levDist[i] = previousDiagonal;
			else
				levDist[i] = std::min(std::min(levDist[i - 1], levDist[i]), previousDiagonal) + 1;

			previousDiagonal = previousDiagonalSave;
		}
	}

	return levDist[minSize];
}

class Search
{
public:
	using Index = std::unordered_map<std::string, std::unordered_set<unsigned int>>;
	using IndexISBN = std::unordered_map<std::string, unsigned int>;

public:
	Search() = default;

	void ToLowerCase(std::string&);

	void SetIndexes(const Library&, Index&, const std::string&);
	void SetTitleIndexes(const Library&);
	void SetAuthorIndexes(const Library&);
	void SetISBNIndexes(const Library&);

	std::unordered_set<unsigned int> SetIntersection(const std::vector<std::set<unsigned int>>& sets);
	std::vector<unsigned int> Searching(const std::vector<std::string>& words, const Index& indexes, unsigned int numberOfBooksOnPage);

	std::vector<unsigned int> SearchByTitle(const std::vector<std::string>& words, unsigned int numberOfBooksOnPage);
	std::vector<unsigned int> SearchByAuthor(const std::vector<std::string>& words, unsigned int numberOfBooksOnPage);
	unsigned int SearchByISBN(const std::string& word);

private:
	Index m_titleIndexes;
	Index m_authorIndexes;
	IndexISBN m_isbnIndexes;
};