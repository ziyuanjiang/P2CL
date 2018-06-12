#ifndef _P2CL_EXCEPTION_HPP_
#define _P2CL_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace p2cl{
	class Exception :public std::exception{
		public:
			Exception(const std::string & excepTypeName, const std::string& moreInfo);
			virtual ~Exception(){}
			virtual const char* what() const noexcept;
		private:
			std::string type;
			std::string exceptInfo;
			std::string errmsg;
	};
}

#endif
