#include <sstream>
#include <string>
#include <vector>
#include "ctype.hpp"
#include "typeacc.hpp"

namespace p2cl{

	void TypeAccumulator::append(IOTupleType type)
	{
		noRepeatAppend(typeList, type);
		noRepeatAppend(eleNumList, type.getEleNum());
	}

	std::string TypeAccumulator::getAllTypeDecl() const
	{
		std::stringstream ss;
		for(int i = 0; i < typeList.size(); ++i)
		{
			ss<<typeList[i].getStructDecl()<<std::endl;
		}
		for(int i = 0; i < eleNumList.size(); ++i)
		{
			ss<<IOTupleType::getMacroDecl(eleNumList[i])<<std::endl;
		}
		return ss.str();
	}

}
