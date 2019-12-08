#include "FileHandler.h"


FileHandler::FileHandler(const std::string& file_name, const OpenMode open_mode) :
	file_name_(file_name), open_mode_(open_mode), file_ptr_(new std::fstream(file_name, ConvertOpenMode(open_mode))), line_(""), current_symbol_(0)
{
	CheckFileOpening();
}


std::fstream& FileHandler::operator* () const
{
	return *file_ptr_;
}


int FileHandler::ConvertOpenMode(const OpenMode& open_mode) const
{
	if (open_mode == OpenMode::READ)
		return std::ios::in;
	else
		return std::ios::out;
}


void FileHandler::CheckFileOpening()
{
	if (!(file_ptr_->is_open()))
	{
		throw FileException("Can't open file " + file_name_);
	}
}


bool FileHandler::GetSymbol(char& c)
{
	if (open_mode_ != OpenMode::READ)
	{
		throw FileException("The file is not open for reading");
	}

	bool result = true;

	if (current_symbol_ >= line_.size())
	{
		current_symbol_ = 0;
		result = static_cast<bool>(std::getline((*file_ptr_), line_));
	}

	if (result)
		c = line_[current_symbol_++];
	return result;
}