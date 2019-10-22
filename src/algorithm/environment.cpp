#include "environment.h"

//将新的变量或函数注册到解释器环境里
void setEnvSymbol(std::string symbol, UserType value, Environment* env)
{
	//先不搞太复杂的
	//就更新在当前环境 不考虑父环境
	//更新
	env->EnvMap[symbol] = value;
}

//获得特定名字的函数的实体
std::optional<UserType> getEnvSymbol(std::string symbol, Environment* env)
{
	//查找
	auto iter = env->EnvMap.find(symbol);
	//如果存在
	if (iter != env->EnvMap.end())
	{
		return { iter->second };
	}
	//如果存在父环境 则继续查询父环境
	else if (env->parent)
	{
		return getEnvSymbol(symbol, env->parent);
	}
	else
	{
		//不存在时返回无效值
		return {};
	}
}