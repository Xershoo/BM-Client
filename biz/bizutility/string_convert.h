#pragma once

#include <string>
#include <sstream>


namespace biz_utility
{
	// 字符串转整数
	unsigned short a2unsignshort(std::string str);
	unsigned short w2unsignshort(std::wstring str);
	int a2int(std::string str);
	int w2int(std::wstring str);
	double a2double(std::string str);
	double w2double(std::string str);


	// 多字节宽字节转换
	std::wstring m2w(std::string str);
	std::string w2m(std::wstring str);
	std::wstring m2w(std::string str, unsigned int cp);
	std::string w2m(std::wstring str, unsigned int cp);


	// 字符和其他类型转换模板函数
	template<typename T>
	std::string tostring(T t)
	{
		std::ostringstream ostm;
		ostm << t;
		return ostm.str();
	}

	inline std::string tostring(char c)
	{
		std::ostringstream ostm;
		ostm << (int)c;
		return ostm.str();
	}

	template<typename T>
	void stringtoT(std::string str, T& t)
	{
		std::istringstream istm(str);
		istm >> t;
	}

	// 宽字符版本
	template<typename T>
	std::wstring towstring(T t)
	{
		std::wostringstream ostm;
		ostm << t;
		return ostm.str();
	}

	inline std::wstring towstring(char c)
	{
		std::wostringstream ostm;
		ostm << (int)c;
		return ostm.str();
	}

	template<typename T>
	void wstringtoT(std::wstring str, T& t)
	{
		std::wistringstream istm(str);
		istm >> t;
	}
}