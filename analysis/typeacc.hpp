#ifndef _P2CL_TYPEACCUMULATOR_HPP_
#define _P2CL_TYPEACCUMULATOR_HPP_

#include <vector>
#include "ctype.hpp"

namespace p2cl{

	class TypeAccumulator{
		public:
			void append(IOTupleType type);

			std::string getAllTypeDecl() const;
		private:
			std::vector<IOTupleType> typeList;
			std::vector<int> eleNumList;

			template<typename T>
			void noRepeatAppend(std::vector<T>& list, T value)const
			{
				int i;
				for(i = 0; i < list.size(); ++i)
				{
					if(list[i] == value)
						break;
				}
				if(i == list.size())
				{
					list.push_back(value);
				}
			}

	};
}
#endif
