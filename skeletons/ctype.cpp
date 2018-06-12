#include <string>
#include <sstream>
#include <regex>
#include "ctype.hpp"

namespace p2cl
{

	std::string clTypeName(CLType type)
	{
		switch(type)
		{
			case CHAR: return "char";
			case UNSIGNED_CHAR: return "unsigned char";
			case SHORT: return "short";
			case UNSIGNED_SHORT: return "unsigned short";
			case INT: return "int";
			case UNSIGNED_INT: return "unsigned int";
			case LONG: return "long";
			case UNSIGNED_LONG: return "unsigned long";
			case FLOAT: return "float";
			case VOID: return "void";
		}

	}

	int clTypeSize(CLType type)
	{
		switch (type)
		{
			case CHAR: return sizeof(char);
			case UNSIGNED_CHAR: return sizeof(unsigned char);
			case SHORT: return sizeof(short int);
			case UNSIGNED_SHORT: return sizeof(unsigned short);
			case INT: return sizeof(int);
			case UNSIGNED_INT: return sizeof(unsigned int);
			case LONG: return sizeof(long);
			case UNSIGNED_LONG: return sizeof(unsigned long);
			case FLOAT: return sizeof(float);
			case VOID: return 0;
		}
	}


	std::string IOTupleType::vstoreStr(int num)
	{
		std::stringstream ss;
		ss<<"vstore"<<num;
		return ss.str();
	}

	bool IOTupleType::operator ==(const IOTupleType &b) const
	{
		bool equal;
		equal = (baseType == b.baseType);
		equal &= (eleNum == b.eleNum);
		return equal;
	}

	bool IOTupleType::operator !=(const IOTupleType &b) const
	{
		return ! (this->operator == (b));
	}

	std::string IOTupleType::vloadStr(int num)
	{
		std::stringstream ss;
		ss<<"vload"<<num;
		return ss.str();
	}

	std::string IOTupleType::macroVLoad(int start, int num, int totalNum)
	{
		std::stringstream ss;

		if(num == totalNum)
		{
			ss<<"\t"<<vstoreStr(num)<<"("<<vloadStr(num)
				<<"(index, input)"
				<<", 0, p_input_var);\\"
				<<std::endl;
		}else{
			ss<<"\t"<<vstoreStr(num)<<"("<<vloadStr(num)
				<<"(0, input + index * "<<totalNum
				<<" + "<<start<<"), 0, p_input_var + "
				<<start<<");\\"<<std::endl;
		}

		return ss.str();
	}

	std::string IOTupleType::macroVStore(int start, int num, int totalNum)
	{
		std::stringstream ss;
		if(num == totalNum)
		{
			ss<<"\t"<<vstoreStr(num)<<"("<<vloadStr(num)
				<<"(0, p_output_var), index,"
				<<" output);\\"<<std::endl;
		}else{
			ss<<"\t"<<vstoreStr(num)<<"("<<vloadStr(num)
				<<"(0, p_output_var + "<< start<<"), 0,"
				<<" output + index * "<<totalNum
				<<" + "<<start<<");\\"<<std::endl;

		}
		return ss.str();
	}

	std::string IOTupleType::macroGeneralLoad(int start, int num, int totalNum)
	{
		std::stringstream ss;
		for(int i = 0; i < num; ++i)
		{
			ss<<'\t'<<"p_input_var["<<start + i<<"]"
				<<" = input[index * "<<totalNum
				<<" + "<<start + i<<"]\\"<<std::endl;
		}
		return ss.str();
	}

	std::string IOTupleType::macroGeneralStore(int start, int num, int totalNum)
	{
		std::stringstream ss;

		for(int i = 0; i < num; ++i)
		{
			ss<<'\t'<<"output[index * "<<totalNum
				<<" + "<<start + i<<"]"
				<<" = p_output_var["<<start + i<<"]\\"<<std::endl;

		}
		return ss.str();
	}

	std::string IOTupleType::getMacroDecl(int num)
	{

		int i;

		if(num < 1 || num > 32)
		{
			return std::string("");
		}

		std::stringstream ss, ssLoad, ssStore;
		ss<<"#ifndef _IOTUPLE_TYPE_"<<num<<"_"<<std::endl;
		ss<<"#define _IOTUPLE_TYPE_"<<num<<"_"<<std::endl;

		ssLoad<<"#define TUPLE_POINTER_LOAD"<<num
			<<"(p_input_var, index, input)\\"<<std::endl;
		ssLoad<<"do{\\"<<std::endl;

		ssStore<<"#define TUPLE_POINTER_STORE"<<num
			<<"(p_output_var, index, output)\\"
			<<std::endl;
		ssStore<<"do{\\"<<std::endl;


		for(i = 0; num - i >= 16 ; i+= 16)
		{
			ssLoad<<macroVLoad(i, 16, num);
			ssStore<<macroVStore(i, 16, num);
		}

		for(; num - i >= 8; i+= 8)
		{
			ssLoad<<macroVLoad(i, 8, num);
			ssStore<<macroVStore(i, 8, num);
		}

		for(; num - i >= 4; i+=4)
		{
			ssLoad<<macroVLoad(i, 4, num);
			ssStore<<macroVStore(i, 4, num);
		}

		for(; num - i >= 2; i+=2)
		{
			ssLoad<<macroVLoad(i, 2, num);
			ssStore<<macroVStore(i, 2, num);
		}

		for(; num - i >= 1; ++i)
		{
			ssLoad<<macroGeneralLoad(i, 1, num);
			ssStore<<macroGeneralStore(i, 1, num);
		}

		ssLoad<<"}while(0)\\"<<std::endl;
		ssStore<<"}while(0)\\"<<std::endl;

		ss<<ssLoad.str()<<std::endl;
		ss<<ssStore.str()<<std::endl;
		
		ss<<"#endif"<<std::endl;
		return ss.str();
	}

	std::string IOTupleType::getStructDecl() const
	{
		if(eleNum <= 1)
		{
			return std::string("");
		}
		std::stringstream ss;
		ss<<"#ifndef _IOTUPLE_TYPE_"<<getIOTupleTypeName()<<"_"<<std::endl;
		ss<<"#define _IOTUPLE_TYPE_"<<getIOTupleTypeName()<<"_"<<std::endl;

		ss<<"typedef struct{"<<std::endl;
		for(int i = 0; i < eleNum; ++i)
		{
			ss<<'\t'<<clTypeName(getBaseType())<<"\te"<<i<<';'
			<<std::endl;
		}
		ss<<'}'<<getIOTupleTypeName()<<';'<<std::endl;

		ss<<"#endif"<<std::endl;
		return ss.str();
	}

	std::string IOTupleType::clTypeNameCap(CLType type) const
	{
		switch(type)
		{
			case CHAR: return "CHAR";
			case UNSIGNED_CHAR: return "UCHAR";
			case SHORT: return "SHORT";
			case UNSIGNED_SHORT: return "USHORT";
			case INT: return "INT";
			case UNSIGNED_INT: return "UINT";
			case LONG: return "LONG";
			case UNSIGNED_LONG: return "ULONG";
			case FLOAT: return "FLOAT";
			case VOID: return "";
		}
	}

	IOTupleType::IOTupleType(CLType baseType, int num)
	{
		this->baseType = baseType;
		eleNum = num;
	}

	int IOTupleType::size() const
	{
		return eleNum * clTypeSize(baseType);
	}

	IOTupleType::IOTupleType(std::string type)
	{
		int num;
		this->baseType = typeFromString(type, num);
		eleNum = num;
	}

	std::string IOTupleType::getIOTupleTypeName() const
	{
		std::stringstream ss;
		if(eleNum != 1)
		{
			ss<<clTypeNameCap(baseType)<<eleNum;
		}else{
			ss<<clTypeName(baseType);
		}
		return ss.str();
	}

	

	CLType IOTupleType::typeFromString(std::string typeName, int & num) const
	{
		std::istringstream iss;
		CLType retVal;
		std::smatch sm;
		std::regex charRegex("^[ \t\r\n]*((char)|(CHAR([1-9][0-9]*)))[ \t\r\n]*$");
		std::regex unsignedCharRegex("^[ \t\r\n]*(uchar|(unsigned[ \t]+char)|(UCHAR([1-9][0-9]*)))[ \t\r\n]*$");
		std::regex shortRegex("^[ \t\r\n]*((short)|(SHORT([1-9][0-9]*)))[ \t\r\n]*$");
		std::regex unsignedShortRegex("^[ \t\r\n]*(ushort|(unsigned[ \t]+short)|(USHORT([1-9][0-9]*)))[ \t\r\n]*$");

		std::regex intRegex("^[ \t\r\n]*((int)|(INT([1-9][0-9]*)))[ \t\r\n]*$");
		std::regex unsignedIntRegex("^[ \t\r\n]*(uint|(unsigned[ \t]+int)|(UINT([1-9][0-9]*)))[ \t\r\n]*$");

		std::regex longRegex("^[ \t\r\n]*((long)|(LONG([1-9][0-9]*)))[ \t\r\n]*$");
		std::regex unsignedLongRegex("^[ \t\r\n]*(ulong|(unsigned[ \t]+long)|(ULONG([1-9][0-9]*)))[ \t\r\n]*$");
		std::regex floatRegex("^[ \t\r\n]*((float)|(FLOAT([1-9][0-9]*)))[ \t\r\n]*$");
		std::regex numRegex("[1-9][0-9]*");

		num = 0;

		if(regex_search(typeName, sm, numRegex))
		{
			iss.str(sm.str());
			iss>>num;
		}else{
			num = 1;
		}

		if(regex_match(typeName, charRegex))
		{
			retVal = CHAR;
		}else if(regex_match(typeName, unsignedCharRegex))
		{
			retVal = UNSIGNED_CHAR;
		}else if(regex_match(typeName, shortRegex))
		{
			retVal = SHORT;
		}else if(regex_match(typeName, shortRegex))
		{
			retVal = UNSIGNED_SHORT;
		}else if(regex_match(typeName, intRegex))
		{
			retVal = INT;
		}else if(regex_match(typeName, unsignedIntRegex))
		{
			retVal = UNSIGNED_INT;
		}else if(regex_match(typeName, longRegex))
		{
			retVal = LONG;
		}else if(regex_match(typeName, unsignedLongRegex))
		{
			retVal = UNSIGNED_LONG;
		}else if(regex_match(typeName, floatRegex))
		{
			retVal = FLOAT;
		}else{
			retVal = VOID;
			num = 0;
		}

		return retVal;
	}

	
	CLType IOTupleType::getBaseType() const
	{
		return baseType;
	}
	
	int IOTupleType::getEleNum() const
	{
		return eleNum;
	}

}
