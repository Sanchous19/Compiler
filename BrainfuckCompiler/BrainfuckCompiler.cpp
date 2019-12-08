#include <iostream>
#include <fstream>
#include <unordered_set>
#include <memory>
#include <string>
#include <regex>
#include <unordered_map>
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
#include "Settings.h"

using node_ptr = std::shared_ptr<Node>;





class Optimizer
{
private:
	const std::unordered_set<OperationKind> optimizer_operations_ = {
		OperationKind::MOVE,
		OperationKind::VALUE,
	};
	const std::unordered_set<NodeKind> positive_node_kinds_ = { NodeKind::ADD, NodeKind::INC, NodeKind::MOVE_INC, NodeKind::MOVE_UP };

	bool IsOptimizerOperation(const OperationKind& kind) const
	{
		return optimizer_operations_.find(kind) != optimizer_operations_.end();
	}

	bool IsPositiveNodeKind(const NodeKind& kind) const
	{
		return positive_node_kinds_.find(kind) != positive_node_kinds_.end();
	}

public:
	Optimizer()
	{
	}

	void Optimize(node_ptr root)
	{
		if (root->left)
			OptimizeNode(root->left);

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

	void OptimizeNode(node_ptr node)
	{
		bool is_optimize = false;
		NodeKind k = node->kind;
		if (node->left && node->left->operation_kind == node->operation_kind && IsOptimizerOperation(node->operation_kind))
		{
			node = OptimizeNodeSequence(node);
			is_optimize = true;
		}
		else if (node->kind == NodeKind::CLOSE_BRACKET && node->parent->right == nullptr)
		{
			ChangeChildren(node->parent, node->left);
			node = node->left;
			is_optimize = true;
		}

		if (is_optimize)
		{
			if (node && IsOptimizerOperation(node->operation_kind) && node->operation_kind == node->parent->operation_kind)
			{
				OptimizeNode(node->parent);
			}
			else if (node)
			{
				OptimizeNode(node);
			}
			return;

		}

		if (node->right)
		{
			OptimizeNode(node->right);
		}

		if (node->left)
		{
			OptimizeNode(node->left);
		}
	}

	node_ptr OptimizeNodeSequence(node_ptr first_node)
	{
		int value = 0;
		node_ptr another_node = first_node;

		while (another_node && another_node->operation_kind == first_node->operation_kind)
		{
			if (IsPositiveNodeKind(another_node->kind))
				value += another_node->value;
			else
				value -= another_node->value;

			another_node = another_node->left;
		}

		if (value)
		{
			first_node->ChangeType(value);
			first_node->left = another_node;
			another_node->parent = first_node;
		}
		else
		{
			ChangeChildren(first_node, another_node);
		}

		return another_node;
	}

	void ChangeChildren(node_ptr old_child, node_ptr new_child)
	{
		node_ptr parent = old_child->parent;

		if (parent->left == old_child)
			parent->left = new_child;
		else
			parent->right = new_child;

		if (new_child)
			new_child->parent = parent;
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


	std::vector<std::string> args;
	for (size_t i = 1; i < static_cast<size_t>(argc); i++)
	{
		args.push_back(argv[i]);
	}

	try
	{
		Settings settings(args);
		std::string file_name = settings.CompilerFileName();

		LexicalAnalyzer lexical_analyzer(file_name);
		lexical_analyzer.Analyze();

		if (settings.IsOnlyLexicalAnalyze())
			return 0;

		Parser parser(settings.CompilerFileName());
		node_ptr ast = parser.GetAst();

		if (settings.IsPrintAstBeforeOptimize())
		{
			AstPrinter::Print(ast);
			std::cout << std::endl;
		}

		if (settings.IsOptimize())
		{
			Optimizer optimizer;
			optimizer.Optimize(ast);

			if (settings.IsPrintAstBeforeOptimize())
			{
				AstPrinter::Print(ast);
				std::cout << std::endl;
			}
		}

		std::string result_file_name = settings.ResultFileName();
		CodeGenerator generate("result");
		generate.Generate(ast);

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