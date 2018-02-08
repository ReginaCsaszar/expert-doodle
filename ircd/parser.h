#pragma once
#pragma warning(push) 
#pragma warning(disable:4996)

#include "user.h"
#include <string>

class Parser {
    
public:

	static void parse(const std::string& message, User* user);
	Parser() = delete;
};

#pragma warning(pop) 