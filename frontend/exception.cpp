#include "exception.hpp"


namespace p2cl{

	Exception::Exception(const std::string& excepTypeName, const std::string &moreInfo)
	{
		type = excepTypeName;
		exceptInfo= moreInfo;
		errmsg = "Error:\t";
		errmsg += type;
		errmsg += "\t";
		errmsg += exceptInfo;
	}

	const char * Exception::what() const noexcept
	{
		return errmsg.c_str();
	}
}
