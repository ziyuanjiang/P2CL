#include <string>
#include <sstream>
#include <limits.h>
#include "gatherop.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"

namespace p2cl{
	
		GatherOp* GatherOp::clone() const
		{
			return new GatherOp(*this);
		}

		GatherOp::GatherOp(int id):ArrangementOp(SKType::gather, id)
		{
		}
		
		IOTupleType GatherOp::inputVarType(int i) const
		{
			return IOTupleType(getBaseType(i), getOffsetNum());
		}

		IOTupleType GatherOp::inputType(int i) const
		{
			return IOTupleType(getBaseType(i), getTupleLength());
		}

		IOTupleType GatherOp::outputType(int i) const
		{
			return IOTupleType(getBaseType(i), getTupleLength() * getOffsetNum());
		}

		std::string GatherOp::inputShiftLengthExpr() const
		{
			std::stringstream ss;
		
			if(rangeIsExpr)
				ss<<'('<<rangeExpr<<')';
			else
				ss<<range;

			return ss.str();
		}


		std::string GatherOp::partialSrcSingleStore(std::string index, int numOfOutput) const
		{
			std::stringstream ss;
			ss<<'{'<<std::endl;

			ss<<'\t'<<"int arrange_index_base = "
			<<index<<" * ";
			ss<<tupleLength * getOffsetNum();

			ss<<';'<<std::endl;

			ss<<'\t'<<"arrange_index_base += "<<"l0;"<<std::endl;

			for(int j = 0; j < numOfOutput; ++j)
			{
				for(int i = 0; i < getOffsetNum(); ++i)
				{
					ss<<"\t"<<outputName(j)<<"[arrange_index_base + "
					<<getTupleLength() * i<<"] = "
					<<outputVarPointerName(j)
					<<"["<<i<<"] ;"<<std::endl;

				}
			}

			ss<<'}'<<std::endl;
			return ss.str();
		}


		std::string GatherOp::partialSrcSingleLoad(std::string index, int numOfInput) const
		{
			std::stringstream ss;
			std::stringstream ssWithInRange;
			std::stringstream ssOutOfRange;
			ss<<'{'<<std::endl;

			ss<<'\t'<<"int arrange_index_base = "<<index<<';'<<std::endl;

			ss<<'\t'<<"int arrange_index;"<<std::endl;

			ss<<"\tint range = ";
			if(rangeIsExpr)
			{
				ss<<rangeExpr<<";"<<std::endl;
			}else{
				ss<<range<<';'<<std::endl;
			}

			for(int i = 0; i < getOffsetNum(); ++i)
			{
				ssWithInRange.str("");
				ssOutOfRange.str("");
				ssWithInRange.clear();
				ssOutOfRange.clear();

				ss<<"\tarrange_index = arrange_index_base + (";
				if(offsetIsExpr[i])
				{
					ss<<offsetExprList[i]; 
				}else{
					ss<<offsetValList[i];
				}

				ss<<");"<<std::endl;


				
				ssWithInRange<<"arrange_index = arrange_index * ("
				<<tupleLength<<");"<<std::endl;


				for(int j = 0; j < numOfInput; ++j)
				{
					ssWithInRange<<inputVarPointerName(j)<<'['<<i
					<<"] = "
					<<inputName(j)<<"[arrange_index"
					<<" + l0];"<<std::endl;

					ssOutOfRange<<inputVarPointerName(j)<<'['<<i
					<<"] = "
					<<"0;"<<std::endl;
				}

				ss<<indent(
					ifElseWrap("(arrange_index < range && arrange_index >= 0)", 
					ssWithInRange.str(),
					ssOutOfRange.str()
					)
				)<<std::endl;
			}


			ss<<'}'<<std::endl;
			return ss.str();
		}
	

		std::string GatherOp::partialSrcLoad(std::string index, int numOfInput) const
		{
			std::stringstream ss;
			std::stringstream ssWithInRange;
			std::stringstream ssOutOfRange;
			int eleNum = getTupleLength();
			ss<<'{'<<std::endl;

			ss<<'\t'<<"int arrange_index_base = "<<index;

			ss<<';'<<std::endl;

			ss<<"\tint range = ";
			if(rangeIsExpr)
			{
				ss<<rangeExpr<<";"<<std::endl;
			}else{
				ss<<range<<';'<<std::endl;
			}

			ss<<'\t'<<"int arrange_index;"<<std::endl;
			for(int i = 0; i < getOffsetNum(); ++i)
			{
				ssWithInRange.str("");
				ssOutOfRange.str("");
				ssWithInRange.clear();
				ssOutOfRange.clear();
				ss<<"\tarrange_index = arrange_index_base + (";
				if(offsetIsExpr[i])
				{
					ss<<offsetExprList[i]; 
				}else{
					ss<<offsetValList[i];
				}
				ss<<");"<<std::endl;
				
				for(int j = 0; j < numOfInput; ++j)
				{
					if(i != 0)	
					{
						ss<<'\t'<<inputVarPointerName(j)
						<<" += "<<eleNum<<';'<<std::endl;
					}
					ssWithInRange<<
						tuplePointerLoad(inputVarPointerName(j), "arrange_index", inputName(j), getTupleLength())
						<<std::endl;

					ssOutOfRange<<
						tuplePointerLoadOutofRange(inputVarPointerName(j), getTupleLength())
						<<std::endl;

				}

				ss<<indent(
					ifElseWrap("(arrange_index < range && arrange_index >= 0)", 
					ssWithInRange.str(),
					ssOutOfRange.str()
					)
				)<<std::endl;
			}

			ss<<'}'<<std::endl;
			return ss.str();
		}
}
