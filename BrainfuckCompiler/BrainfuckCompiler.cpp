#include <iostream>
#include "AstPrinter.h"
#include "CodeGenerator.h"
#include "Exceptions.h"
#include "FileHandler.h"
#include "LexicalAnalyzer.h"
#include "Node.h"
#include "Optimizer.h"
#include "Parser.h"
#include "Settings.h"

using node_ptr = std::shared_ptr<Node>;


int main(int argc, char* argv[])
{
	std::vector<std::string> args;
	for (size_t i = 1; i < static_cast<size_t>(argc); i++)
	{
		args.push_back(argv[i]);
	}

	try
	{
		Settings settings(args);

		if (settings.IsHelp())
		{
			std::cout << settings.Help();
			return 0;
		}

		std::string file_name = settings.CompilerFileName();

		LexicalAnalyzer lexical_analyzer(file_name);
		lexical_analyzer.Analyze();

		if (settings.IsOnlyLexicalAnalyze())
			return 0;

		Parser parser(settings.CompilerFileName());
		node_ptr ast = parser.GetAst();

		if (settings.IsPrintAstBeforeOptimize())
		{
			AstPrinter::Print(ast);
			std::cout << std::endl;
		}

		if (settings.IsOptimize())
		{
			Optimizer optimizer;
			optimizer.Optimize(ast);

			if (settings.IsPrintAstAfterOptimize())
			{
				AstPrinter::Print(ast);
				std::cout << std::endl;
			}
		}

		std::string result_file_name = settings.ResultFileName();
		CodeGenerator generate(result_file_name);
		generate.Generate(ast);
	}
	catch (std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}