#pragma once

#include <string>
#include <unordered_set>
#include "FileHandler.h"
#include "Exceptions.h"


class LexicalAnalyzer
{
public:
	LexicalAnalyzer(const std::string& file_name);

	void Analyze();

private:
	FileHandler file_handler_;

	static const std::unordered_set<char> symbols_;

	bool IsIncorrectSymbol(const char& symbol) const;
};