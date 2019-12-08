#pragma once

#include <iostream>
#include <string>
#include "Node.h"

using node_ptr = std::shared_ptr<Node>;


class AstPrinter
{
public:
	static void Print(const node_ptr node, const std::string& start_line = "");

private:
	static const char edge_ = '|';
};