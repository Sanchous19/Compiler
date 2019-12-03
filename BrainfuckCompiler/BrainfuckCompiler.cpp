#include <iostream>
#include <fstream>
#include <unordered_set>
#include <memory>

#include <string>
#include <regex>
#include <unordered_map>



class FileHandler
{
public:
	enum class OpenMode
	{
		WRITE,
		READ,
	};
private:
	std::unique_ptr<std::fstream> file_ptr_;
	std::string file_name_;

	int ConstructOpenMode(const OpenMode& open_mode) const
	{
		if (open_mode == OpenMode::READ)
			return std::ios::in;
		else
			return std::ios::out;
	}
public:

	FileHandler(const std::string& file_name, const OpenMode open_mode) : file_ptr_(new std::fstream(file_name, ConstructOpenMode(open_mode))), file_name_(file_name)
	{
		CheckFileOpening();
	}

	std::fstream& operator* () const
	{
		return *file_ptr_;
	}

	/*std::fstream* operator-> () const
	{
		return file_ptr_;
	}*/
private:

	void CheckFileOpening()
	{
		if (!(file_ptr_->is_open()))
		{
			throw FileException(file_name_);
		}
	}

	class FileException : public std::exception
	{
	private:
		std::string error_text_;
	public:
		FileException(std::string file_name) : error_text_("Can't open file " + file_name) {}

		const char* what() const noexcept
		{
			return error_text_.c_str();
		}
	};
};


class LexicAnalizator
{
public:
	LexicAnalizator(const std::string& file_name) : file_handler_(file_name, FileHandler::OpenMode::READ) {}

	void Analize()
	{
		int open_brackets = 0;
		std::string line;
		while (std::getline((*file_handler_), line))
		{
			for (const char& symbol : line)
			{
				std::cout << symbol << " ";
				if (IsIncorrectSymbol(symbol))
				{
					throw LexicException("Not close all brackets");
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
						throw LexicException("Not close all brackets");
					}
				}
			}
		}
		if (open_brackets != 0)
			throw LexicException("Not close all brackets");
	}

private:
	const std::unordered_set<char> symbols_ = { '>', '<', '+', '-', '.', ',', '[', ']', ' ' };
	FileHandler file_handler_;

	bool IsIncorrectSymbol(const char& symbol) const
	{
		return symbols_.find(symbol) == symbols_.end();
	}

	class LexicException : public std::exception
	{
	private:
		std::string error_text_;
	public:
		LexicException(std::string file_name) : error_text_("Can't open file " + file_name) {}

		const char* what() const noexcept
		{
			return error_text_.c_str();
		}
	};
};


class Optimizer
{
private:
	const std::string optimize_prefix = "optimize_";
	FileHandler file_handler_;
	FileHandler optimizer_file_handler_;
public:
	Optimizer(const std::string& file_name) : file_handler_(file_name, FileHandler::OpenMode::READ), optimizer_file_handler_(optimize_prefix + file_name, FileHandler::OpenMode::WRITE)
	{
	}
};



enum class Operation
{
	ADD,
	CLOSE_BRACKET,
	DEC,
	INC,
	INPUT,
	MOVE_DEC,
	MOVE_DOWN,
	MOVE_INC,
	MOVE_UP,
	OPEN_BRACKET,
	OUTPUT,
	SUB,
};


class CodeGenerator
{
public:
	CodeGenerator(const std::string& file_name)
		: file_handler_(file_name, FileHandler::OpenMode::READ), template_file_handler_(template_file_name, FileHandler::OpenMode::READ), result_file_handler_("result_" + file_name, FileHandler::OpenMode::WRITE)
	{
		//(*result_file_ptr_).seekp(std::ios::end, 10);
	}

	void Generate()
	{
		int tab_number = 1;
		std::string line;
		while (std::getline((*file_handler_), line))
		{
			Operation operation = Operation::ADD;
			for (const auto& op : operations_)
			{
				if (std::regex_match(line, op.first))
				{
					operation = op.second;
					break;
				}
			}

			PrintOperation(operation, tab_number, line);
		}
	}
private:
	const std::string template_file_name = "template_file.c";

	FileHandler file_handler_;
	FileHandler template_file_handler_;
	FileHandler result_file_handler_;
	const char tab_ = '\t';
	const std::vector<std::pair<std::regex, Operation>> operations_ = {
		{std::regex("^ADD\\(\\d+\\)$"), Operation::ADD},
		{std::regex("^CLOSE_BRACKET$"), Operation::CLOSE_BRACKET},
		{std::regex("^DEC$"), Operation::DEC},
		{std::regex("^INC$"), Operation::INC},
		{std::regex("^INPUT$"), Operation::INPUT},
		{std::regex("^MOVE_DEC$"), Operation::MOVE_DEC},
		{std::regex("^MOVE_DOWN\\(\\d+\\)$"), Operation::MOVE_DOWN},
		{std::regex("^MOVE_INC$"), Operation::MOVE_INC},
		{std::regex("^MOVE_UP\\(\\d+\\)$"), Operation::MOVE_UP},
		{std::regex("^OPEN_BRACKET$"), Operation::OPEN_BRACKET},
		{std::regex("^OUTPUT$"), Operation::OUTPUT},
		{std::regex("^SUB\\(\\d+\\)$"), Operation::SUB},
	};
	const std::unordered_map<Operation, std::string> symbols_ = {
		{Operation::ADD, "arr[i] += "},
		{Operation::CLOSE_BRACKET, "}"},
		{Operation::DEC, "arr[i]--;"},
		{Operation::INC, "arr[i]++;"},
		{Operation::INPUT, "arr[i] = getchar;"},
		{Operation::MOVE_DEC, "i--;"},
		{Operation::MOVE_DOWN, "i -= "},
		{Operation::MOVE_INC, "i++;"},
		{Operation::MOVE_UP, "i += "},
		{Operation::OPEN_BRACKET, "while (arr[i]) {"},
		{Operation::OUTPUT, "putchar(arr[i]);"},
		{Operation::SUB, "arr[i] -= "},
	};

	void CopyTemplate()
	{
		std::string line;
		while (std::getline((*template_file_handler_), line))
		{
			(*template_file_handler_) << line;
		}
	}

	void PrintTabs(const int& tab_number)
	{
		std::string tabs(tab_number, tab_);
		(*result_file_handler_) << tabs;
	}

	void PrintOperation(const Operation& operation, int& tab_number, const std::string line)
	{
		if (operation == Operation::CLOSE_BRACKET)
			tab_number--;

		PrintTabs(tab_number);

		(*result_file_handler_) << symbols_.at(operation);

		std::regex number_regex("\\d+");
		std::smatch match;
		std::string number;

		switch (operation)
		{
		case Operation::ADD:
		case Operation::MOVE_DOWN:
		case Operation::MOVE_UP:
		case Operation::SUB:
			std::regex_search(line, match, number_regex);
			(*result_file_handler_) << match[0] << ";";
			break;
		case Operation::OPEN_BRACKET:
			tab_number++;
			break;
		}

		(*result_file_handler_) << std::endl;
	}

};




int main(int argc, char* argv[])
{
	/*std::string s = "ADD(5vvbh3453425)";
	std::regex r("\\d+");
	std::smatch match;
	std::regex_search(s, match, r);
	for (auto it : match)
		std::cout << it << " ";

	std::fstream f("f", std::ios::out);*/
	if (argc < 2)
		exit(1);
	try
	{
		const std::string compiler_file_name = argv[1];
		{
			LexicAnalizator lexix_analizator(compiler_file_name);
			lexix_analizator.Analize();
		}
		{
			//Optimizer optimizer(compiler_file_name);
		}
		{
			CodeGenerator generate("optimize_file_name.txt");
			generate.Generate();
		}
		/*const std::string s = "cat " + compiler_file_name;
		system(s.c_str());*/

	}
	catch (std::exception & ex)
	{
		std::cerr << ex.what();
	}



	std::cout << "Hello World!\n";
	return 0;
}