#pragma once

#include <fstream>
#include <memory>
#include <string>
#include "Exceptions.h"


class FileHandler
{
public:
	enum class OpenMode
	{
		WRITE,
		READ,
	};

	FileHandler(const std::string& file_name, const OpenMode open_mode);

	bool GetSymbol(char& c);

	std::fstream& operator* () const;

private:
	const std::string file_name_;
	const OpenMode open_mode_;
	std::unique_ptr<std::fstream> file_ptr_;
	std::string line_;
	int current_symbol_;

	int ConvertOpenMode(const OpenMode& open_mode) const;
	void CheckFileOpening();
};