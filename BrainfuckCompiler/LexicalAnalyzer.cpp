#include "LexicalAnalyzer.h"


LexicalAnalyzer::LexicalAnalyzer(const std::string& file_name) : file_handler_(file_name, FileHandler::OpenMode::READ) {}


void LexicalAnalyzer::Analyze()
{
	int open_brackets = 0;
	std::string line;
	char symbol;
	while (file_handler_.GetSymbol(symbol))
	{
		if (IsIncorrectSymbol(symbol))
		{
			throw LexicException("Incorrect symbol: " + symbol);
		}

		if (symbol == '[')
		{
			open_brackets++;
		}
		else if (symbol == ']')
		{
			open_brackets--;
			if (open_brackets < 0)
			{
				throw LexicException("Discrepancy of brackets");
			}
		}
	}
	if (open_brackets != 0)
		throw LexicException("Not close all brackets");
}


bool LexicalAnalyzer::IsIncorrectSymbol(const char& symbol) const
{
	return symbols_.find(symbol) == symbols_.end();
}


const std::unordered_set<char> LexicalAnalyzer::symbols_{ '+', '-', '>', '<', '.', ',', '[', ']', ' ', '\t' };