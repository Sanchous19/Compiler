#pragma once

#include <memory>
#include <unordered_set>
#include "Node.h"

using node_ptr = std::shared_ptr<Node>;


class Optimizer
{
public:
	void Optimize(node_ptr root);

private:
	static const std::unordered_set<OperationKind> optimizer_operations_;
	static const std::unordered_set<NodeKind> positive_node_kinds_;

	void OptimizeNode(node_ptr node);
	node_ptr OptimizeNodeSequence(node_ptr first_node);
	void ChangeChildren(node_ptr old_child, node_ptr new_child);
	bool IsOptimizerOperation(const OperationKind& kind) const;
	bool IsPositiveNodeKind(const NodeKind& kind) const;
};
