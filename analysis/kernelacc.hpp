#ifndef _P2CL_KERNELACCUMULATOR_HPP_
#define _P2CL_KERNELACCUMULATOR_HPP_

#include <vector>
#include <sstream>
#include "ctype.hpp"

namespace p2cl{

	class KernelAccumulator{
		public:
			void append(std::string kenrelSrc);

			std::string getAllKernelSrc() const;
		private:
			std::vector<IOTupleType> typeList;
			std::vector<int> eleNumList;
			std::stringstream ss;
	};
}
#endif
