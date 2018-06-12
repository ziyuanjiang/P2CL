#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include "ctype.hpp"
#include "cfunction.hpp"

namespace p2cl
{
	CFunction::CFunction(const std::string & func, int nInput, int nOutput, int nFuncPara)
	{
		setFunction(func, nInput, nOutput, nFuncPara);
	}


	CFunction::CFunction()
	{
		numInput = 0;
		numOutput = 0;
		numFuncPara = 0;
		totalNumPara = 0;
	}

	std::string CFunction::getName() const
	{
		return functionName;
	}

	int CFunction::paraNameIndex(std::string paraName) const
	{
		int i;
		for(i = 0; i < paraNameList.size(); ++i)
		{
			if((paraNameList[i]).compare(paraName) == 0)
			{
				break;
			}
		}
		
		if(i != paraNameList.size())
		{
			return i;
		}else
		{
			//paraname not found
			return -1;
			//throw ParameterNotFoundException
		}
	}

	int CFunction::getNumInput() const
	{
		return numInput;
	}

	int CFunction::getNumOutput() const
	{
		return numOutput;
	}

	int CFunction::getNumFuncPara() const
	{
		return numFuncPara;
	}

	int CFunction::getNumInputOutput() const
	{
		return totalNumPara - numFuncPara;
	}

	void CFunction::setFunction(const std::string & func, int nInput, int nOutput, int nFuncPara)
	{
		std::vector<std::string> paraTypes;
		std::string fName;

		parameterExtract(func, fName, paraTypes, paraNameList);
		totalNumPara = paraTypes.size();
		numInput = nInput;
		numOutput = nOutput;
		numFuncPara = nFuncPara;
		typeList.clear();
		for(std::vector<std::string>::iterator it = paraTypes.begin(); it != paraTypes.end(); ++it)
		{
			typeList.push_back(IOTupleType(*it));
		}

		functionName = fName;
		functionDecl = func;
	}
	
	IOTupleType CFunction::getInputType(int index) const
	{
		return typeList[index];
	}

	IOTupleType CFunction::getOutputType(int index) const
	{
		return typeList[index + numInput];
	}

	bool CFunction::parameterExtract(const std::string & function, std::string & functionName, std::vector<std::string>& typeName,std::vector<std::string> & paraName) const
	{
		int inx0;
		int inx1;
		int start, end, nextStart, finalEnd;
		int paraStart, paraEnd;
		std::vector<std::string> retVal, paraNameRetVal;
		//return type
		start = function.find_first_not_of(" \t\r\n");
		end = function.find_first_of(" \t(),", start);
		//retVal.push_back(function.substr(start , end-start+1));
		//function name
		start = function.find_first_not_of(" \t\r\n", end);
		end = function.find_first_of(" \t\r\n(", start);
		functionName = function.substr(start, end - start);
		//parameter list
		start = function.find_first_of("(", end);
		++start;
		//find the closing bracket
		finalEnd = function.find_first_of(")", start);

		while(start < finalEnd)
		{
			//start of typeName
			start = function.find_first_not_of(" \t\r\n", start);
			//end of typeName
			end = function.find_first_of(",)", start);
			nextStart = end + 1;
			//parameter name
			end = function.find_last_not_of(" *\t\r\n", end -1);
			paraEnd = end;
			//end of typename
			end = function.find_last_of(" *\t\r\n", end);
			paraStart = end + 1;
			end = function.find_last_not_of(" *\t\r\n", end);
			std::string str = function.substr(start, end-start+1);
			//remove pointer symbol
			str.erase(std::remove(str.begin(), str.end(), '*'), str.end());
			//parameter type name
			retVal.push_back(str);
			str = function.substr(paraStart, paraEnd-paraStart+1);
			paraNameRetVal.push_back(str);
			start = nextStart;
		}

		typeName = retVal;
		paraName = paraNameRetVal;
		return true;
	}

}
