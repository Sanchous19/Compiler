#include "Node.h"


Node::Node(NodeKind node_kind, OperationKind operation_kind, const int& value) : kind(node_kind), operation_kind(operation_kind), value(value), left(nullptr), right(nullptr)
{
	if (value_one_kinds_.find(node_kind) != value_one_kinds_.end())
	{
		this->value = 1;
	}
}


std::ostream& operator<<(std::ostream& out, const Node& node)
{
	std::string node_text = Node::node_kind_to_text_.at(node.kind);
	std::string value = "";
	if (node.value > 1)
		value = " " + std::to_string(node.value);

	out << node_text << value;
	return out;
}


const std::unordered_set<NodeKind> Node::value_one_kinds_ = {
		NodeKind::DEC,
		NodeKind::INC,
		NodeKind::MOVE_DEC,
		NodeKind::MOVE_INC,
};


const std::unordered_map<NodeKind, std::string> Node::node_kind_to_text_ = {
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