// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <memory>

#include <string>
#include <regex>
#include <unordered_map>
#include <tuple>
#include <set>
#include <map>
#include <stack>


std::string get_file_name(std::string file_name)
{
	std::regex r("[^.]*");
	std::smatch match;
	std::regex_search(file_name, match, r);
	return match[0];
}



enum class Operation
{
	ADD,
	CLOSE_BRACKET,
	DEC,
	ELSE,
	IF,
	INC,
	INPUT,
	MOVE_DEC,
	MOVE_DOWN,
	MOVE_INC,
	MOVE_UP,
	OPEN_BRACKET,
	OUTPUT,
	SUB,
	WHILE,
};


enum class NodeKind
{
	ADD,
	CLOSE_BRACKET,
	DEC,
	ELSE,
	IF,
	INC,
	INPUT,
	MOVE_DEC,
	MOVE_DOWN,
	MOVE_INC,
	MOVE_UP,
	OPEN_BRACKET,
	OUTPUT,
	ROOT,
	SUB,
	WHILE,
};





class FileException : public std::exception
{
public:
	FileException(std::string message) : std::exception(message.c_str()) {}
};

class LexicException : public std::exception
{
public:
	LexicException(std::string message) : std::exception(message.c_str()) {}
};





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







class LexicalAnalyzer
{
public:
	LexicalAnalyzer(const std::string& file_name);

	void Analyze();

private:
	const std::unordered_set<char> symbols_ = { '>', '<', '+', '-', '.', ',', '[', ']', ' ', '\t' };
	FileHandler file_handler_;

	bool IsIncorrectSymbol(const char& symbol) const;
};


LexicalAnalyzer::LexicalAnalyzer(const std::string& file_name) : file_handler_(file_name, FileHandler::OpenMode::READ) {}

void LexicalAnalyzer::Analyze()
{
	int open_brackets = 0;
	std::string line;
	char symbol;
	while (file_handler_.GetSymbol(symbol))
	{
		std::cout << symbol << " ";
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

enum class OperationKind
{
	CLOSE_BRACKET,
	DEFINE_PROGRAM,
	INPUT,
	MOVE,
	OPEN_BRACKET,
	OUTPUT,
	VALUE,
};

struct Node
{
	std::shared_ptr<Node> left;
	std::shared_ptr<Node> right;
	NodeKind kind;
	OperationKind operation_kind;
	int value;

	Node(NodeKind node_kind, OperationKind operation_kind) : kind(node_kind), operation_kind(operation_kind), value(0), left(nullptr), right(nullptr) {}
};


using node_ptr = std::shared_ptr<Node>;


class Parser
{
public:

	const std::unordered_map<char, OperationKind> operation_kinds_ = {
		{']', OperationKind::CLOSE_BRACKET},
		{',', OperationKind::INPUT},
		{'>', OperationKind::MOVE},
		{'<', OperationKind::MOVE},
		{'[', OperationKind::OPEN_BRACKET},
		{'.', OperationKind::OUTPUT},
		{'+', OperationKind::VALUE},
		{'-', OperationKind::VALUE},
	};
	const std::unordered_map<char, NodeKind> node_kinds_ = {
		{']', NodeKind::CLOSE_BRACKET},
		{',', NodeKind::INPUT},
		{'>', NodeKind::MOVE_INC},
		{'<', NodeKind::MOVE_DEC},
		{'[', NodeKind::OPEN_BRACKET},
		{'.', NodeKind::OUTPUT},
		{'+', NodeKind::INC},
		{'-', NodeKind::DEC},
	};

	Parser(const std::string& file_name) : file_handler_(file_name, FileHandler::OpenMode::READ) {}

	node_ptr GetAst()
	{
		std::stack<node_ptr> open_bracket_nodes;
		node_ptr root(new Node(NodeKind::ROOT, OperationKind::DEFINE_PROGRAM));
		node_ptr prev_node = root;
		node_ptr common_node = root;
		char symbol;
		while (file_handler_.GetSymbol(symbol))
		{
			if (symbol == ' ' || symbol == '\t')
				continue;

			NodeKind node_kind = node_kinds_.at(symbol);
			OperationKind operation_kind = operation_kinds_.at(symbol);
			node_ptr current(new Node(node_kind, operation_kind));
			if (operation_kind == prev_node->operation_kind && (operation_kind == OperationKind::MOVE || operation_kind == OperationKind::VALUE))
			{
				prev_node->right = current;
				prev_node = current;
			}
			else if (operation_kind == OperationKind::CLOSE_BRACKET)
			{
				common_node = open_bracket_nodes.top();
				open_bracket_nodes.pop();
				prev_node = current;
			}
			else
			{
				if (prev_node->operation_kind == OperationKind::OPEN_BRACKET)
					common_node->right = current;
				else
					common_node->left = current;
				common_node = current;
				prev_node = current;
			}

			if (operation_kind == OperationKind::OPEN_BRACKET)
			{
				open_bracket_nodes.push(current);
			}
		}

		return root;
	}
private:
	FileHandler file_handler_;
};


const std::unordered_map<NodeKind, std::string> node_kind = {
	{NodeKind::ADD, "ADD"},
	{NodeKind::CLOSE_BRACKET, "CLOSE_BRACKET"},
	{NodeKind::DEC, "DEC"},
	{NodeKind::ELSE, "ELSE"},
	{NodeKind::IF, "IF"},
	{NodeKind::INC, "INC"},
	{NodeKind::INPUT, "INPUT"},
	{NodeKind::MOVE_DEC, "MOVE_DEC"},
	{NodeKind::MOVE_DOWN, "MOVE_DOWN"},
	{NodeKind::MOVE_INC, "MOVE_INC"},
	{NodeKind::MOVE_UP, "MOVE_UP"},
	{NodeKind::OPEN_BRACKET, "OPEN_BRACKET"},
	{NodeKind::OUTPUT, "OUTPUT"},
	{NodeKind::ROOT, "ROOT"},
	{NodeKind::SUB, "SUB"},
	{NodeKind::WHILE, "WHILE"},
};

void print_ast(node_ptr node, int tab_num = 0)
{
	std::string tabs(tab_num, '\t');
	std::cout << tabs;
	std::string node_kind_text = node_kind.at(node->kind);
	std::cout << node_kind_text << std::endl;
	if (node->right)
		print_ast(node->right, tab_num + 1);
	if (node->left)
		print_ast(node->left, tab_num);
}



class Optimizer
{
private:
	const std::string result_file_name_;
	FileHandler file_handler_;
	FileHandler result_file_handler_;

	enum class ReducedOperation
	{
		CLOSE_BRACKET,
		INPUT,
		MOVE,
		OPEN_BRACKET,
		OUTPUT,
		VALUE,
	};


	const std::map<std::tuple<ReducedOperation, bool, bool>, Operation> optimizer_operations_map_ = {
		{{ReducedOperation::MOVE, false, false}, Operation::MOVE_DOWN},
		{{ReducedOperation::MOVE, false, true}, Operation::MOVE_DEC},
		{{ReducedOperation::MOVE, true, false}, Operation::MOVE_UP},
		{{ReducedOperation::MOVE, true, true}, Operation::MOVE_INC},
		{{ReducedOperation::VALUE, false, false}, Operation::SUB},
		{{ReducedOperation::VALUE, false, true}, Operation::DEC},
		{{ReducedOperation::VALUE, true, false}, Operation::ADD},
		{{ReducedOperation::VALUE, true, true}, Operation::INC},
	};
	const std::unordered_map<ReducedOperation, Operation> operations_map_ = {
		{ReducedOperation::CLOSE_BRACKET, Operation::CLOSE_BRACKET},
		{ReducedOperation::INPUT, Operation::INPUT},
		{ReducedOperation::OPEN_BRACKET, Operation::OPEN_BRACKET},
		{ReducedOperation::OUTPUT, Operation::OUTPUT},
	};
	const std::unordered_set<ReducedOperation> optimizer_operations_ = {
		ReducedOperation::MOVE,
		ReducedOperation::VALUE,
	};
	const std::unordered_set<char> positive_operation_ = {
		'+',
		'>',
	};

	bool IsSpace(const char& symbol) const
	{
		return symbol == ' ';
	}

	bool IsOptimizerOperation(const ReducedOperation& operation) const
	{
		return optimizer_operations_.find(operation) != optimizer_operations_.end();
	}

	bool IsPositiveSymbol(const char& symbol) const
	{
		return positive_operation_.find(symbol) != positive_operation_.end();
	}

public:
	Optimizer(const std::string& file_name) : result_file_name_("opt" + get_file_name(file_name) + ".txt"), file_handler_(file_name, FileHandler::OpenMode::READ), result_file_handler_(result_file_name_, FileHandler::OpenMode::WRITE)
	{
	}

	std::string Optimize()
	{
		bool is_start = false;
		ReducedOperation prev_operation = ReducedOperation::VALUE;
		int value = 0;
		std::string line;
		while (std::getline((*file_handler_), line))
		{
			for (const char& symbol : line)
			{
				if (IsSpace(symbol))
					continue;

				/*ReducedOperation reduced_operation = symbol_to_reduced_operation_.at(symbol);

				if (IsOptimizerOperation(reduced_operation))
				{
					if (reduced_operation != prev_operation)
					{
						if (value)
						{
							bool is_positive = value > 0, is_one = abs(value) == 1;
							Operation operation = optimizer_operations_map_.at({ reduced_operation, is_positive, is_one });
							PrintToken(operation, value);
						}

						prev_operation = reduced_operation;
						value = 0;
					}

					if (IsPositiveSymbol(symbol))
						value++;
					else
						value--;
				}
				else
				{
					Operation operation = operations_map_.at(reduced_operation);
					PrintToken(operation, value);

					prev_operation = reduced_operation;
					value = 0;
				}*/
			}

			is_start = true;
		}
		return result_file_name_;
	}


	const std::unordered_map<Operation, std::string> operation_to_token_ = {
		{Operation::ADD, "ADD("},
		{Operation::CLOSE_BRACKET, "CLOSE_BRACKET"},
		{Operation::DEC, "DEC"},
		{Operation::INC, "INC"},
		{Operation::INPUT, "INPUT"},
		{Operation::MOVE_DEC, "MOVE_DEC"},
		{Operation::MOVE_DOWN, "MOVE_DOWN("},
		{Operation::MOVE_INC, "MOVE_INC"},
		{Operation::MOVE_UP, "MOVE_UP("},
		{Operation::OPEN_BRACKET, "OPEN_BRACKET"},
		{Operation::OUTPUT, "OUTPUT"},
		{Operation::SUB, "SUB("},
	};
	const std::unordered_set<Operation> operations_with_value_ = {
		Operation::ADD,
		Operation::MOVE_DOWN,
		Operation::MOVE_UP,
		Operation::SUB,
	};

	bool IsOperationWithValue(const Operation& operation) const
	{
		return operations_with_value_.find(operation) != operations_with_value_.end();
	}

	void PrintToken(const Operation& operation, const int& value = -1)
	{
		std::string token = operation_to_token_.at(operation);
		(*result_file_handler_) << token;

		if (IsOperationWithValue(operation))
			(*result_file_handler_) << value << ")";

		(*result_file_handler_) << std::endl;
	}
};





class CodeGenerator
{
public:
	CodeGenerator(const std::string& file_name);

	std::string Generate();

private:
	const std::string template_file_name_ = "template.c";
	const int first_lines_count_ = 9;
	const char tab_ = '\t';
	std::string result_file_name_;

	FileHandler file_handler_;
	FileHandler template_file_handler_;
	FileHandler result_file_handler_;

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
		{Operation::INPUT, "arr[i] = getchar();"},
		{Operation::MOVE_DEC, "i--;"},
		{Operation::MOVE_DOWN, "i -= "},
		{Operation::MOVE_INC, "i++;"},
		{Operation::MOVE_UP, "i += "},
		{Operation::OPEN_BRACKET, "while (arr[i]) {"},
		{Operation::OUTPUT, "putchar(arr[i]);"},
		{Operation::SUB, "arr[i] -= "},
	};

	void CopyBeginTemplate();
	void CopyEndTemplate();
	void PrintTabs(const int& tab_number);
	void PrintOperation(const Operation& operation, int& tab_number, const std::string line);
	std::string GetNumberFromLine(std::string line);
};


CodeGenerator::CodeGenerator(const std::string& file_name)
	: result_file_name_(get_file_name(file_name) + ".c"), file_handler_(file_name, FileHandler::OpenMode::READ),
	template_file_handler_(template_file_name_, FileHandler::OpenMode::READ), result_file_handler_(result_file_name_, FileHandler::OpenMode::WRITE) {}


std::string CodeGenerator::Generate()
{
	CopyBeginTemplate();

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

	CopyEndTemplate();
	return result_file_name_;
}



void CodeGenerator::CopyBeginTemplate()
{
	for (int i = 0; i < first_lines_count_; i++)
	{
		std::string line;
		std::getline((*template_file_handler_), line);
		(*result_file_handler_) << line << std::endl;
	}
}

void CodeGenerator::CopyEndTemplate()
{
	std::string line;
	while (std::getline((*template_file_handler_), line))
	{
		(*result_file_handler_) << line << std::endl;
	}
}

void CodeGenerator::PrintTabs(const int& tab_number)
{
	std::string tabs(tab_number, tab_);
	(*result_file_handler_) << tabs;
}

void CodeGenerator::PrintOperation(const Operation& operation, int& tab_number, const std::string line)
{
	if (operation == Operation::CLOSE_BRACKET)
		tab_number--;

	PrintTabs(tab_number);

	(*result_file_handler_) << symbols_.at(operation);

	switch (operation)
	{
	case Operation::ADD:
	case Operation::MOVE_DOWN:
	case Operation::MOVE_UP:
	case Operation::SUB:

		(*result_file_handler_) << GetNumberFromLine(line) << ";";
		break;
	case Operation::OPEN_BRACKET:
		tab_number++;
		break;
	}

	(*result_file_handler_) << std::endl;
}

std::string CodeGenerator::GetNumberFromLine(std::string line)
{
	std::regex number_regex("\\d+");
	std::smatch match;
	std::regex_search(line, match, number_regex);
	return match[0];
}





int main(int argc, char* argv[])
{
	std::string s = "sadfdfg" + 'c';
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
		const std::string file_name = argv[1];
		{
			LexicalAnalyzer lexical_analyzer(file_name);
			lexical_analyzer.Analyze();
		}
		{
			std::cout << std::endl;
			Parser parser(file_name);
			node_ptr ast = parser.GetAst();
			print_ast(ast);
			/*Optimizer optimizer(compiler_file_name);
			optimizer.Optimize();*/
		}
		{
			/*CodeGenerator generate("optimize_file_name.txt");
			std::string result_file_name = generate.Generate();
			std::cout << std::endl;
			std::cout << result_file_name << std::endl;*/
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