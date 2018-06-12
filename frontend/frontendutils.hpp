#ifndef _P2CL_FRONTENDUTILS_HPP_
#define _P2CL_FRONTENDUTILS_HPP_

#include <vector>

namespace p2cl{

	/*!
		@brief split parameters in a space-separated list 
			into a vector
	*/
	std::vector<std::string> splitParaList(std::string paraList);

	/*!
		@brief
			load the file and get contents
	*/
	std::string loadProgram(std::string input);

}
#endif
