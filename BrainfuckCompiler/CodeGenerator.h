#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "FileHandler.h"
#include "Node.h"

using node_ptr = std::shared_ptr<Node>;


class CodeGenerator
{
public:
	CodeGenerator(const std::string& file_name);

	void Generate(node_ptr root);

private:
	const std::string template_file_name_ = "template.c";
	const int first_lines_count_ = 9;
	const char tab_ = '\t';

	std::string file_name_;
	FileHandler file_handler_;
	FileHandler template_file_handler_;

	static const std::unordered_map<NodeKind, std::string> node_representation_;
	static const std::unordered_set<NodeKind> print_value_nodes_;

	void CopyBeginTemplate();
	void CopyEndTemplate();
	bool IsPrintValueNode(const node_ptr node) const;
	void PrintNode(node_ptr node, const int& tab_number = 1);
};