#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "frontendutils.hpp"

namespace p2cl{

	std::vector<std::string> splitParaList(std::string paraList)
	{
		int start = 0;
		int end = 0;
		int i = 0;
		std::vector<std::string> retVal;
		std::string tempStr;

		while(i < paraList.size())
		{
			while(i < paraList.size())
			{
				if(paraList[i] == ' '
					||
					paraList[i] == '\n'
					||
					paraList[i] == '\r'
				)
				{
					++i;
				}else{
					break;
				}
			}

			start = i;

			while(i < paraList.size())
			{
				if(paraList[i] == ' '
					||
					paraList[i] == '\n'
					||
					paraList[i] == '\r'
				)
				{
					break;
				}else{
					++i;
				}
			}

			end = i;

			if(end > start)
			{
				tempStr = paraList.substr(start, end - start);
				end = start;
				retVal.push_back(tempStr);
			}
		}
		return retVal;
	}

	std::string loadProgram(std::string input)
	{
		std::ifstream stream(input.c_str());
		if (!stream.is_open()) 
		{
			std::cout << "Cannot open file: " << input << std::endl;
			exit(1);
		}

		return std::string(
		std::istreambuf_iterator<char>(stream),
		(std::istreambuf_iterator<char>()));
	}

}
