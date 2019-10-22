#pragma once;

#include "ASTNode.h"

//打印语法树
void printAST(std::shared_ptr<ASTNode> ast, std::shared_ptr<ASTEnvironment> env);

//执行语法树
double executeAST(std::shared_ptr<ASTNode> ast, std::shared_ptr<ASTEnvironment> env);