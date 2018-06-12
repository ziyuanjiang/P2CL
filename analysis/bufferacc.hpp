#ifndef _P2CL_BUFFERACCUMULATOR_HPP_
#define _P2CL_BUFFERACCUMULATOR_HPP_

#include <vector>
#include <sstream>
#include "ctype.hpp"

namespace p2cl{

	/*!
		@brief buffer accumulator
		The Executant class uses the same sizes of buffer for input and output for all operations
		This class accumulate the maximum buffer size for all indexes for input and output
	*/
	class BufferAccumulator{
		public:
			/*!
				@brief constructor
			*/
			BufferAccumulator();

			/*!
				@brief update size of at the index 
				@param index index
				@param size size
			*/
			void append(int index, int size);

			/*!
				@brief set current extension num
				@param num
			*/
			void setExt(int num);

			/*!
				@brief get current extension num
			*/
			int getCurrentExtNum() const
			{
				return extensionNum;
			}

			/*!
				@brief get list of sizes
			*/
			std::vector<size_t> getList() const
			{
				return sizeList;
			}

		private:
			std::vector<size_t> sizeList;
			int extensionNum;
	};
}
#endif
