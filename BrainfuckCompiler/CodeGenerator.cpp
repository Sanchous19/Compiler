#include "CodeGenerator.h"


CodeGenerator::CodeGenerator(const std::string& file_name)
	: file_name_(file_name + ".c"), file_handler_(file_name_, FileHandler::OpenMode::WRITE), template_file_handler_(template_file_name_, FileHandler::OpenMode::READ) {}


void CodeGenerator::Generate(node_ptr root)
{
	CopyBeginTemplate();

	if (root->left)
		PrintNode(root->left);

	CopyEndTemplate();
}


void CodeGenerator::CopyBeginTemplate()
{
	for (int i = 0; i < first_lines_count_; i++)
	{
		std::string line;
		std::getline((*template_file_handler_), line);
		(*file_handler_) << line << std::endl;
	}
}


void CodeGenerator::CopyEndTemplate()
{
	std::string line;
	while (std::getline((*template_file_handler_), line))
	{
		(*file_handler_) << line << std::endl;
	}
}


bool CodeGenerator::IsPrintValueNode(const node_ptr node) const
{
	return print_value_nodes_.find(node->kind) != print_value_nodes_.end();
}


void CodeGenerator::PrintNode(node_ptr node, const int& tab_number)
{
	std::string tabs(tab_number, tab_);
	std::string repr = node_representation_.at(node->kind);
	(*file_handler_) << tabs << repr;

	if (IsPrintValueNode(node))
	{
		(*file_handler_) << node->value << ";";
	}
	(*file_handler_) << std::endl;

	if (node->right)
	{
		PrintNode(node->right, tab_number + 1);
	}
	if (node->left)
	{
		PrintNode(node->left, tab_number);
	}
}


const std::unordered_map<NodeKind, std::string> CodeGenerator::node_representation_ = {
	{NodeKind::ADD, "arr[i] += "},
	{NodeKind::CLOSE_BRACKET, "}"},
	{NodeKind::DEC, "arr[i]--;"},
	{NodeKind::INC, "arr[i]++;"},
	{NodeKind::INPUT, "arr[i] = getchar();"},
	{NodeKind::MOVE_DEC, "i--;"},
	{NodeKind::MOVE_DOWN, "i -= "},
	{NodeKind::MOVE_INC, "i++;"},
	{NodeKind::MOVE_UP, "i += "},
	{NodeKind::OPEN_BRACKET, "while (arr[i]) {"},
	{NodeKind::OUTPUT, "putchar(arr[i]);"},
	{NodeKind::SUB, "arr[i] -= "},
};


const std::unordered_set<NodeKind> CodeGenerator::print_value_nodes_ = {
	NodeKind::ADD,
	NodeKind::MOVE_DOWN,
	NodeKind::MOVE_UP,
	NodeKind::SUB,
};