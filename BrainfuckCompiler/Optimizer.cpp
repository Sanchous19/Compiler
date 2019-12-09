#include "Optimizer.h"


void Optimizer::Optimize(node_ptr root)
{
	if (root->left)
		OptimizeNode(root->left);
}


void Optimizer::OptimizeNode(node_ptr node)
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


node_ptr Optimizer::OptimizeNodeSequence(node_ptr first_node)
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


void Optimizer::ChangeChildren(node_ptr old_child, node_ptr new_child)
{
	node_ptr parent = old_child->parent;

	if (parent->left == old_child)
		parent->left = new_child;
	else
		parent->right = new_child;

	if (new_child)
		new_child->parent = parent;
}


bool Optimizer::IsOptimizerOperation(const OperationKind& kind) const
{
	return optimizer_operations_.find(kind) != optimizer_operations_.end();
}


bool Optimizer::IsPositiveNodeKind(const NodeKind& kind) const
{
	return positive_node_kinds_.find(kind) != positive_node_kinds_.end();
}


const std::unordered_set<OperationKind> Optimizer::optimizer_operations_ = { OperationKind::MOVE, OperationKind::VALUE };


const std::unordered_set<NodeKind> Optimizer::positive_node_kinds_ = { NodeKind::ADD, NodeKind::INC, NodeKind::MOVE_INC, NodeKind::MOVE_UP };