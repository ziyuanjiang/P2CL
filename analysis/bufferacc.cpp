#include "bufferacc.hpp"

namespace p2cl{
	
	BufferAccumulator::BufferAccumulator()
	{
		extensionNum = 1;
	}

	void BufferAccumulator::append(int index, int size)
	{
		int max = 0;
		int vectorSize = sizeList.size();
		if(index < vectorSize)
		{
			max = sizeList[index];
			if(size > max)
			{
				sizeList[index] = size;
			}
		}else if(index == vectorSize){
			sizeList.push_back(size);
		}
	}

	void BufferAccumulator::setExt(int num)
	{
		extensionNum = num;
	}
}
