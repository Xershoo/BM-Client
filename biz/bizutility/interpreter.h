#pragma once

#include "str2type.h"
#include "bizutility/log/assert_log.h"
#include <map>
#include <stack>
#include <string>
#include <vector>

// 计算表达式解析器
// int ret = Interpret("3*(5+!7-(6==6))");
// -6 写成:0-6 !!7写成!(!7)


namespace biz_utility
{
	class Token
	{
	public:
		Token(std::string name, int priority) : _name(name), _priority(priority){}
		std::string Name() { return _name; }
		int Priority() { return _priority; }

		virtual void Handle(std::stack<int>& data, std::stack<Token*>& op) = 0;

	protected:
		std::string _name;
		int			_priority;
	};


	class Token_Data : public Token
	{
	public:
		Token_Data(std::string name) : Token(name, -1){}
		virtual void Handle(std::stack<int>& data, std::stack<Token*>& op)
		{
			data.push(a2int(_name));
			delete this;
		}
	};


	class Token_OP : public Token
	{
	public:
		Token_OP(std::string name, int priority) : Token(name, priority){}
		virtual void Handle(std::stack<int>& data, std::stack<Token*>& op)
		{
			// 先处理堆栈里面优先级高的符号.
			while (!op.empty() && op.top()->Priority() >= _priority)
			{
				Token_OP* pOp = dynamic_cast<Token_OP*>(op.top());
				if (NULL == pOp)
				{
					assert_(false);
					return;
				}
				pOp->HandleOP(data);
				op.pop();
			}

			op.push(this);
		}

		virtual void HandleOP(std::stack<int>& data) = 0;
	};


	class Token_LBrace : public Token
	{
	public:
		Token_LBrace() : Token("(", -1){}
		virtual void Handle(std::stack<int>& data, std::stack<Token*>& op)
		{
			op.push(this);
		}
	};


	class Token_RBrace : public Token
	{
	public:
		Token_RBrace() : Token(")", -1){}
		virtual void Handle(std::stack<int>& data, std::stack<Token*>& op)
		{
			// 处理堆栈里左括号之后的符号.
			while (!op.empty())
			{
				if (op.top()->Name() == "(")
				{
					op.pop();
					return;
				}

				Token_OP* pOP = dynamic_cast<Token_OP*>(op.top());
				if (NULL== pOP)
				{
					assert_(false);
					return;
				}

				pOP->HandleOP(data);
				op.pop();
			}

			assert_(false);
		}
	};


	class Token_OP2 : public Token_OP
	{
	public:
		Token_OP2(std::string name, int priority) : Token_OP(name, priority){}

		virtual void HandleOP(std::stack<int>& data)
		{
			if (data.empty())
			{
				assert_(false);
				return;
			}
			int right = data.top();
			data.pop();
			if (data.empty())
			{
				assert_(false);
				return;
			}
			int left = data.top();
			data.pop();
			data.push(DoOP2(left, right));
		}

		virtual int DoOP2(int left, int right) = 0;
	};


	class Token_OP1 : public Token_OP
	{
	public:
		Token_OP1(std::string name, int priority) : Token_OP(name, priority){}

		virtual void HandleOP(std::stack<int>& data)
		{
			if (data.empty())
			{
				assert_(false);
				return;
			}
			int right = data.top();
			data.pop();
			data.push(DoOP1(right));
		}

		virtual int DoOP1(int right) = 0;
	};


	class Token_Add : public Token_OP2
	{
	public:
		Token_Add() : Token_OP2("+", 3){}
		virtual int DoOP2(int left, int right)
		{
			return left + right;
		}
	};

	class Token_Sub : public Token_OP2
	{
	public:
		Token_Sub() : Token_OP2("-", 3){}
		virtual int DoOP2(int left, int right)
		{
			return left - right;
		}
	};

	class Token_Mul : public Token_OP2
	{
	public:
		Token_Mul() : Token_OP2("*", 4){}
		virtual int DoOP2(int left, int right)
		{
			return left * right;
		}
	};

	class Token_Div : public Token_OP2
	{
	public:
		Token_Div() : Token_OP2("/", 4){}
		virtual int DoOP2(int left, int right)
		{
			return left / right;
		}
	};

	class Token_Mod : public Token_OP2
	{
	public:
		Token_Mod() : Token_OP2("%", 4){}
		virtual int DoOP2(int left, int right)
		{
			return left % right;
		}
	};

	class Token_Not : public Token_OP1
	{
	public:
		Token_Not() : Token_OP1("!", 5){}
		virtual int DoOP1(int right)
		{
			return !right;
		}
	};

	class Token_Equ : public Token_OP2
	{
	public:
		Token_Equ() : Token_OP2("==", 2){}
		virtual int DoOP2(int left, int right)
		{
			return left == right;
		}
	};

	std::map<std::string, Token*> map_tokens;


	std::vector<Token*> Break2Tokens(std::map<std::string, Token*>& mapTokens, std::string expr)
	{
		std::vector<Token*> ret;
		// 支持最多两个字符的操作符，且最长匹配
		int begin = 0;
		while (begin < (int)expr.size())
		{
			int j = begin;
			for (; j<(int)expr.size(); ++j)
			{
				if (j<(int)expr.size()-1 &&
					mapTokens.find(expr.substr(j, 2)) != mapTokens.end())
				{
					if (j != begin)
						ret.push_back(new Token_Data(expr.substr(begin, j-begin)));
					ret.push_back(mapTokens[expr.substr(j,2)]);
					begin = j+2;
					break;
				}
				if (mapTokens.find(expr.substr(j, 1)) != mapTokens.end())
				{
					if (j != begin)
						ret.push_back(new Token_Data(expr.substr(begin, j-begin)));
					ret.push_back(mapTokens[expr.substr(j,1)]);
					begin = j+1;
					break;
				}
			}

			if (j == (int)expr.size())
			{
				ret.push_back(new Token_Data(expr.substr(begin)));
				break;
			}
		}

		return ret;
	}

	int Interpret(std::string expr)
	{
		// 1.构造操作符.
		Token* opTokens[] =
		{
			new Token_LBrace(),
			new Token_RBrace(),
			new Token_Add(),
			new Token_Sub(),
			new Token_Mul(),
			new Token_Div(),
			new Token_Mod(),
			new Token_Not(),
			new Token_Equ()
		};

		std::map<std::string, Token*> mapTokens;
		for (int i=0; i< sizeof(opTokens)/sizeof(*opTokens); ++i)
			mapTokens[opTokens[i]->Name()] = opTokens[i];

		// 2.分割token.
		std::vector<Token*> tokens = Break2Tokens(mapTokens, expr);

		// 3.依次处理每一个token.
		std::stack<int> data;
		std::stack<Token*> op;
		std::vector<Token*>::iterator it = tokens.begin();
		for (; it!=tokens.end(); ++it)
			(*it)->Handle(data, op);

		// 4.处理符号栈里剩余的操作符.
		while (!op.empty())
		{
			Token_OP* pOP = dynamic_cast<Token_OP*>(op.top());
			if (NULL== pOP)
			{
				assert_(false);
				break;
			}
			pOP->HandleOP(data);
			op.pop();
		}


		// 5.清除符号token，数据token在步骤3中已经析构.
		for (int i=0; i< sizeof(opTokens)/sizeof(*opTokens); ++i)
			delete opTokens[i];
		
		if (data.size() != 1)
		{
			assert_(false);
			return 0;
		}
		return data.top();
	}
}