#pragma once

#include <exception>
#include <string>


class ArgsException : public std::exception
{
public:
	ArgsException(std::string message) : std::exception(message.c_str()) {}
};


class FileException : public std::exception
{
public:
	FileException(std::string message) : std::exception(message.c_str()) {}
};


class LexicException : public std::exception
{
public:
	LexicException(std::string message) : std::exception(message.c_str()) {}
};