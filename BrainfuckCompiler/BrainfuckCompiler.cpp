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
#include "AstPrinter.h"
#include "CodeGenerator.h"
#include "Exceptions.h"
#include "FileHandler.h"
#include "LexicalAnalyzer.h"
#include "Node.h"
#include "Parser.h"

using node_ptr = std::shared_ptr<Node>;


std::string get_file_name(std::string file_name)
{
	std::regex r("[^.]*");
	std::smatch match;
	std::regex_search(file_name, match, r);
	return match[0];
}




class Optimizer
{
private:


	const std::map<std::tuple<OperationKind, bool, bool>, NodeKind> optimizer_operations_map_ = {
		{{OperationKind::MOVE, false, false}, NodeKind::MOVE_DOWN},
		{{OperationKind::MOVE, false, true}, NodeKind::MOVE_DEC},
		{{OperationKind::MOVE, true, false}, NodeKind::MOVE_UP},
		{{OperationKind::MOVE, true, true}, NodeKind::MOVE_INC},
		{{OperationKind::VALUE, false, false}, NodeKind::SUB},
		{{OperationKind::VALUE, false, true}, NodeKind::DEC},
		{{OperationKind::VALUE, true, false}, NodeKind::ADD},
		{{OperationKind::VALUE, true, true}, NodeKind::INC},
	};
	const std::unordered_set<OperationKind> optimizer_operations_ = {
		OperationKind::MOVE,
		OperationKind::VALUE,
	};
	const std::unordered_set<char> positive_operation_ = {
		'+',
		'>',
	};

	bool IsSpace(const char& symbol) const
	{
		return symbol == ' ';
	}

	bool IsOptimizerOperation(const OperationKind& kind) const
	{
		return optimizer_operations_.find(kind) != optimizer_operations_.end();
	}

	bool IsPositiveSymbol(const char& symbol) const
	{
		return positive_operation_.find(symbol) != positive_operation_.end();
	}

public:
	Optimizer()
	{
	}

	void Optimize(node_ptr root)
	{
		if (root->left)
			OptimizeNode(root->left, root);

		//bool is_start = false;
		//ReducedOperation prev_operation = ReducedOperation::VALUE;
		//int value = 0;
		//std::string line;
		//while (std::getline((*file_handler_), line))
		//{
		//	for (const char& symbol : line)
		//	{
		//		if (IsSpace(symbol))
		//			continue;
		//		
		//		/*ReducedOperation reduced_operation = symbol_to_reduced_operation_.at(symbol);

		//		if (IsOptimizerOperation(reduced_operation))
		//		{
		//			if (reduced_operation != prev_operation)
		//			{
		//				if (value)
		//				{
		//					bool is_positive = value > 0, is_one = abs(value) == 1;
		//					Operation operation = optimizer_operations_map_.at({ reduced_operation, is_positive, is_one });
		//					PrintToken(operation, value);
		//				}

		//				prev_operation = reduced_operation;
		//				value = 0;
		//			}

		//			if (IsPositiveSymbol(symbol))
		//				value++;
		//			else
		//				value--;
		//		}
		//		else
		//		{
		//			Operation operation = operations_map_.at(reduced_operation);
		//			PrintToken(operation, value);

		//			prev_operation = reduced_operation;
		//			value = 0;
		//		}*/
		//	}

		//	is_start = true;
		//}
	}

	void OptimizeNode(node_ptr node, node_ptr parent)
	{
		bool is_delete_node = false;
		if (node->right && node->operation_kind == node->right->operation_kind && IsOptimizerOperation(node->operation_kind))
		{
			int value = 0;
			CountValue(node, value);
			if (value == 0)
			{
				is_delete_node = true;

				if (parent->left == node)
					parent->left = node->left;
				else
					parent->right = node->left;
			}
			else
			{
				bool is_positive = value > 0, is_one = abs(value) == 1;
				NodeKind kind = optimizer_operations_map_.at({ node->operation_kind, is_positive, is_one });
				node->kind = kind;
				node->value = abs(value);
				node->right = nullptr;
			}
		}
		else if (node->right)
		{
			OptimizeNode(node->right, node);
		}

		if (node->left)
		{
			if (is_delete_node)
				OptimizeNode(node->left, parent);
			else
				OptimizeNode(node->left, node);
		}
	}

	void CountValue(node_ptr node, int& value)
	{
		if (node->kind == NodeKind::INC || node->kind == NodeKind::MOVE_INC || node->kind == NodeKind::ADD || node->kind == NodeKind::MOVE_UP)
			value += node->value;
		else
			value -= node->value;

		if (node->right)
			CountValue(node->right, value);
	}

	/*const std::unordered_map<Operation, std::string> operation_to_token_ = {
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
	};*/
	/*const std::unordered_set<Operation> operations_with_value_ = {
		Operation::ADD,
		Operation::MOVE_DOWN,
		Operation::MOVE_UP,
		Operation::SUB,
	};

	bool IsOperationWithValue(const Operation& operation) const
	{
		return operations_with_value_.find(operation) != operations_with_value_.end();
	}*/

	/*void PrintToken(const Operation& operation, const int& value = -1)
	{
		std::string token = operation_to_token_.at(operation);
		(*result_file_handler_) << token;

		if (IsOperationWithValue(operation))
			(*result_file_handler_) << value << ")";

		(*result_file_handler_) << std::endl;
	}*/
};







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
			Parser parser(file_name);
			node_ptr ast = parser.GetAst();
			AstPrinter::Print(ast);
			/*Optimizer optimizer(compiler_file_name);
			optimizer.Optimize();*/
			CodeGenerator generate("result");
			generate.Generate(ast);
		}
		{
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