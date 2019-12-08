#pragma once

#include <regex>
#include <string>
#include <vector>
#include "Exceptions.h"


class Settings
{
public:
	Settings(const std::vector<std::string>& args);
	bool IsOnlyLexicalAnalyze() const;
	bool IsOptimize() const;
	bool IsPrintAstAfterOptimize() const;
	bool IsPrintAstBeforeOptimize() const;
	std::string CompilerFileName() const;
	std::string ResultFileName() const;

private:
	bool is_only_lexical_analyze_;
	bool is_optimize_;
	bool is_print_ast_after_optimize_;
	bool is_print_ast_before_optimize_;
	std::string file_name_;
	std::string result_file_name_;

	const std::string extension_ = ".c";

	std::string GetFileName(std::string file_name) const;
	bool IsFileNameWithoutExtension(std::string file_name) const;
	void CheckCorrectness(const std::vector<std::string>& args) const;
};