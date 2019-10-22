#pragma once

#include <string>
#include <tuple>

struct Environment;

//解析表达式
std::tuple<double, std::string> parseExpression(std::string input, Environment* env);

//解析语句
void parseStatement(std::string input, Environment* env);