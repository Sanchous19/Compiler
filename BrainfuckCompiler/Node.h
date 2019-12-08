#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>


enum class NodeKind;
enum class OperationKind;


struct Node
{
	std::shared_ptr<Node> left;
	std::shared_ptr<Node> right;
	std::shared_ptr<Node> parent;
	NodeKind kind;
	OperationKind operation_kind;
	int value;

	Node(NodeKind node_kind, OperationKind operation_kind, std::shared_ptr<Node> parent, const int& value = 0);

	void ChangeType(const int& new_value);

	friend std::ostream& operator<<(std::ostream& out, const Node& node);

private:
	static const std::unordered_set<NodeKind> value_one_kinds_;
	static const std::unordered_map<NodeKind, std::string> node_kind_to_text_;
	static const std::map<std::tuple<OperationKind, bool, bool>, NodeKind> change_type_map_;
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