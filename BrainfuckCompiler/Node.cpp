#include "Node.h"


Node::Node(NodeKind node_kind, std::shared_ptr<Node> parent, const int& value) 
	: kind(node_kind), value(value), left(nullptr), right(nullptr)
{
	operation_kind = node_kind_to_operation_kind_.at(node_kind);

	this->parent = parent;

	if (value_one_kinds_.find(node_kind) != value_one_kinds_.end())
	{
		this->value = 1;
	}
}


void Node::ChangeType(const int& new_value, const bool& is_assign)
{
	if (is_assign)
	{
		kind = NodeKind::ASSIGN;
		value = new_value;
	}
	else
	{
		bool is_positive = new_value > 0, is_one = abs(new_value) == 1;
		NodeKind new_kind = change_type_map_.at({ operation_kind, is_positive, is_one });
		kind = new_kind;
		value = abs(new_value);
	}
}


std::ostream& operator<<(std::ostream& out, const Node& node)
{
	std::string node_text = Node::node_kind_to_text_.at(node.kind);
	std::string value = "";
	if (node.value > 1 || node.kind == NodeKind::ASSIGN)
		value = " " + std::to_string(node.value);

	out << node_text << value;
	return out;
}


const std::unordered_map<NodeKind, OperationKind> Node::node_kind_to_operation_kind_ = {
	{NodeKind::ADD, OperationKind::VALUE },
	{NodeKind::ASSIGN, OperationKind::VALUE },
	{NodeKind::CLOSE_BRACKET, OperationKind::CLOSE_BRACKET},
	{NodeKind::DEC, OperationKind::VALUE },
	{NodeKind::INC, OperationKind::VALUE },
	{NodeKind::INPUT, OperationKind::INPUT },
	{NodeKind::MOVE_DEC, OperationKind::MOVE },
	{NodeKind::MOVE_DOWN, OperationKind::MOVE },
	{NodeKind::MOVE_INC, OperationKind::MOVE },
	{NodeKind::MOVE_UP, OperationKind::MOVE },
	{NodeKind::OUTPUT, OperationKind::OUTPUT },
	{NodeKind::ROOT, OperationKind::DEFINE_PROGRAM},
	{NodeKind::SUB, OperationKind::VALUE },
	{NodeKind::WHILE, OperationKind::WHILE},
};


const std::unordered_set<NodeKind> Node::value_one_kinds_ = {
		NodeKind::DEC,
		NodeKind::INC,
		NodeKind::MOVE_DEC,
		NodeKind::MOVE_INC,
};


const std::unordered_map<NodeKind, std::string> Node::node_kind_to_text_ = {
	{NodeKind::ADD, "ADD"},
	{NodeKind::ASSIGN, "ASSIGN"},
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
	{NodeKind::OUTPUT, "OUTPUT"},
	{NodeKind::ROOT, "ROOT"},
	{NodeKind::SUB, "SUB"},
	{NodeKind::WHILE, "WHILE"},
};


const std::map<std::tuple<OperationKind, bool, bool>, NodeKind> Node::change_type_map_ = {
	{{OperationKind::MOVE, false, false}, NodeKind::MOVE_DOWN},
	{{OperationKind::MOVE, false, true}, NodeKind::MOVE_DEC},
	{{OperationKind::MOVE, true, false}, NodeKind::MOVE_UP},
	{{OperationKind::MOVE, true, true}, NodeKind::MOVE_INC},
	{{OperationKind::VALUE, false, false}, NodeKind::SUB},
	{{OperationKind::VALUE, false, true}, NodeKind::DEC},
	{{OperationKind::VALUE, true, false}, NodeKind::ADD},
	{{OperationKind::VALUE, true, true}, NodeKind::INC},
};