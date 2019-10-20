#pragma once

#include "token.h"

#include <string>
#include <tuple>

//解析表达式
std::tuple<double, std::string> parseExpression(std::string input);