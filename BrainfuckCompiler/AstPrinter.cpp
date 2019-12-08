#include "AstPrinter.h"


void AstPrinter::Print(const node_ptr node, const std::string& start_line)
{
	std::cout << start_line << (*node) << std::endl;

	if (node->right)
	{
		std::string continue_line(4, ' ');
		if (node->left)
			continue_line[0] = edge_;

		Print(node->right, start_line + continue_line);
	}

	if (node->left)
		Print(node->left, start_line);
}