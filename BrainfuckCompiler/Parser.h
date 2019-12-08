#pragma once

#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "FileHandler.h"
#include "Node.h"

using node_ptr = std::shared_ptr<Node>;


class Parser
{
public:
	Parser(const std::string& file_name);

	node_ptr GetAst();

private:
	FileHandler file_handler_;

	static const std::unordered_map<char, NodeKind> node_kinds_;
	static const std::unordered_map<char, OperationKind> operation_kinds_;
	static const std::unordered_set<char> ignore_symbols_;

	bool IsIgnoreSymbol(const char& c) const;
};