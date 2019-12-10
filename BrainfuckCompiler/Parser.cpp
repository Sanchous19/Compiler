#include "Parser.h"


Parser::Parser(const std::string& file_name) : file_handler_(file_name, FileHandler::OpenMode::READ) {}


node_ptr Parser::GetAst()
{
	node_ptr root(new Node(NodeKind::ROOT, nullptr));
	node_ptr prev = root;

	char symbol;
	std::stack<node_ptr> open_bracket_nodes;
	while (file_handler_.GetSymbol(symbol))
	{
		if (IsIgnoreSymbol(symbol))
			continue;

		NodeKind node_kind = node_kinds_.at(symbol);
		node_ptr current(new Node(node_kind, prev));

		if (current->operation_kind == OperationKind::CLOSE_BRACKET)
		{
			prev = open_bracket_nodes.top();
			open_bracket_nodes.pop();
			current->parent = prev;
			prev->left = current;
			prev = current;
		}
		else
		{
			if (prev->operation_kind == OperationKind::WHILE)
				prev->right = current;
			else
				prev->left = current;
			prev = current;
		}

		if (current->operation_kind == OperationKind::WHILE)
		{
			open_bracket_nodes.push(current);
		}
	}

	return root;
}


bool Parser::IsIgnoreSymbol(const char& c) const
{
	return ignore_symbols_.find(c) != ignore_symbols_.end();
}


const std::unordered_map<char, NodeKind> Parser::node_kinds_ = {
	{']', NodeKind::CLOSE_BRACKET},
	{',', NodeKind::INPUT},
	{'>', NodeKind::MOVE_INC},
	{'<', NodeKind::MOVE_DEC},
	{'[', NodeKind::WHILE},
	{'.', NodeKind::OUTPUT},
	{'+', NodeKind::INC},
	{'-', NodeKind::DEC},
};


const std::unordered_set<char> Parser::ignore_symbols_ = { ' ', '\t' };