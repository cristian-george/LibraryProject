#pragma once
#include <string>

class Tokenizer
{
public:
	Tokenizer() = default;
	Tokenizer(const std::string&);
	Tokenizer(const std::string&, const std::string&);
	bool NextToken();
	bool NextToken(const std::string&);
	const std::string GetToken() const;
	void SetToken(const std::string&, const std::string&);
private:
	static const std::string DELIMITERS;
	size_t m_offset;
	std::string m_string;
	std::string m_token;
	std::string m_delimiters;
};