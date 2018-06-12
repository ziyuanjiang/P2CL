#include "kernelacc.hpp"

namespace p2cl{

	std::string KernelAccumulator::getAllKernelSrc() const
	{
		return ss.str();
	}

	void KernelAccumulator::append(std::string kenrelSrc)
	{
		ss<<kenrelSrc<<std::endl;
	}
}
