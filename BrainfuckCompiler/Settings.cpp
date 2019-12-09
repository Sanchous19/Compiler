#include "Settings.h"


Settings::Settings(const std::vector<std::string>& args)
	: is_help_(false), is_only_lexical_analyze_(false), is_optimize_(true), is_print_ast_after_optimize_(false), is_print_ast_before_optimize_(false), file_name_(""), result_file_name_("")
{
	for (size_t i = 0; i < args.size(); i++)
	{
		const std::string& arg = args[i];

		if (arg.size() > 2 && arg.substr(0, 2) == "--")
		{
			if (arg == "--help")
				is_help_ = true;
			else if (arg == "--la")
				is_only_lexical_analyze_ = true;
			else if (arg == "--no-opt")
				is_optimize_ = false;
			else if (arg == "--pr-ast")
				is_print_ast_before_optimize_ = true;
			else if (arg == "--pr-opt-ast")
				is_print_ast_after_optimize_ = true;
			else
			{
				throw ArgsException("Argument " + arg + " is not supported");
			}
		}
		else if (arg.size() > 1 && arg[0] == '-')
		{
			if (arg == "-o")
			{
				if (!result_file_name_.empty())
				{
					throw ArgsException("Two or more output files");
				}

				i++;
				if (i == args.size())
				{
					throw ArgsException("No output file name");
				}

				if (!IsFileNameWithoutExtension(args[i]))
				{
					throw ArgsException("Output file should be without extension");
				}

				result_file_name_ = args[i];
			}
			else
			{
				throw ArgsException("Argument " + arg + " is not supported");
			}
		}
		else
		{
			if (file_name_.empty())
			{
				file_name_ = arg;
			}
			else
			{
				throw ArgsException("Two or more input files");
			}
		}
	}

	CheckCorrectness(args);

	if (!is_help_ && !is_only_lexical_analyze_ && result_file_name_.empty())
	{
		result_file_name_ = GetFileName(file_name_) + extension_;
	}
}


bool Settings::IsHelp() const
{
	return is_help_;
}


bool Settings::IsOnlyLexicalAnalyze() const
{
	return is_only_lexical_analyze_;
}


bool Settings::IsOptimize() const
{
	return is_optimize_;
}


bool Settings::IsPrintAstAfterOptimize() const
{
	return is_print_ast_after_optimize_;
}


bool Settings::IsPrintAstBeforeOptimize() const
{
	return is_print_ast_before_optimize_;
}


std::string Settings::CompilerFileName() const
{
	return file_name_;
}

std::string Settings::ResultFileName() const
{
	return result_file_name_;
}


std::string Settings::Help() const
{
	std::string help_text = std::string("") +
		"Usage: BrainfuckCompiler.exe [options] file\n" +
		"Options:\n" +
		"--help\t\t\t Display this information.\n" +
		"--la\t\t\t Only lexical analyze of file.\n" +
		"--no-opt\t\t Compile file without optimization.\n" +
		"--pr-ast\t\t Display AST before optimization.\n" +
		"--pr-opt-ast\t\t Display AST after optimization.\n" +
		"-o <result_file>\t Place the output into <result_file>.\n";
	return help_text;
}


std::string Settings::GetFileName(std::string file_name) const
{
	std::regex r("[^.]*");
	std::smatch match;
	std::regex_search(file_name, match, r);
	return match[0];
}

bool Settings::IsFileNameWithoutExtension(std::string file_name) const
{
	return std::count(file_name.begin(), file_name.end(), '.') == 0;
}


void Settings::CheckCorrectness(const std::vector<std::string>& args) const
{
	if (is_help_)
	{
		if (args.size() > 1)
			throw ArgsException("Arguments exception when only help");
		else
			return;
	}

	if (file_name_.empty())
	{
		throw ArgsException("No input file");
	}

	if (is_only_lexical_analyze_ && args.size() > 2)
	{
		throw ArgsException("Arguments exception when only lexical analize");
	}

	if (!is_optimize_ && is_print_ast_after_optimize_)
	{
		throw ArgsException("Can't print AST because was chosen not to optimize");
	}
}