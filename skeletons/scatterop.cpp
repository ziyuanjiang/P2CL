#include <string>
#include <sstream>
#include <limits.h>
#include "scatterop.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"

namespace p2cl{
		ScatterOp::ScatterOp(int id):ArrangementOp(SKType::scatter, id)
		{
		}

		ScatterOp* ScatterOp::clone() const
		{
			return new ScatterOp(*this);
		}
		
		IOTupleType ScatterOp::inputVarType(int i) const
		{
			return IOTupleType(getBaseType(i), getOffsetNum());
		}

		IOTupleType ScatterOp::inputType(int i) const
		{
			return IOTupleType(getBaseType(i), 
				getTupleLength() * getOffsetNum());
		}

		IOTupleType ScatterOp::outputType(int i) const
		{
			return IOTupleType(getBaseType(i), getTupleLength());
		}

		std::string ScatterOp::outputShiftLengthExpr() const 
		{
			std::stringstream ss;
		
			if(rangeIsExpr)
				ss<<'('<<rangeExpr<<')';
			else
				ss<<range;

			return ss.str();
		}

		std::string ScatterOp::partialSrcSingleStore(std::string index, int numOfOutput) const
		{
			std::stringstream ss;
			ss<<'{'<<std::endl;
			ss<<'\t'<<"int arrange_index_base = "<<index<<";"<<std::endl;

			ss<<'\t'<<"int arrange_index;"<<std::endl;

			for(int i = 0; i < getOffsetNum(); ++i)
			{
				ss<<"\tarrange_index = arrange_index_base + (";
				if(offsetIsExpr[i])
				{
					ss<<offsetExprList[i]; 
				}else{
					ss<<offsetValList[i];
				}
				ss<<");"<<std::endl;

				ss<<"\tarrange_index = arrange_index * (";

				ss<<tupleLength;

				ss<<");"<<std::endl;

				ss<<std::endl;
				
				for(int j = 0; j < numOfOutput; ++j)
				{
					ss<<'\t'<<outputName(j)
					<<"[arrange_index * "<<getTupleLength()
					<<" + l0] = "
					<<outputVarPointerName(j)
					<<'['<<i<<"];"
					<<std::endl;
				}
			}
			ss<<'}'<<std::endl;
			return ss.str();
		}

		std::string ScatterOp::partialSrcSingleLoad(std::string index, int numOfInput) const
		{
			std::stringstream ss;
			ss<<'{'<<std::endl;

			ss<<'\t'<<"int arrange_index_base = "
			<<index<<" * ";
			ss<<tupleLength * getOffsetNum();

			ss<<';'<<std::endl;
			ss<<'\t'<<"arrange_index_base += "<<"l0;"<<std::endl;
			for(int j = 0; j < numOfInput; ++j)
			{
				for(int i = 0; i < getOffsetNum(); ++i)
				{
					ss<<"\t"<<inputVarPointerName(j)
					<<"["<<i<<"] = "
					<<inputName(j)<<"[arrange_index_base + "
					<<getTupleLength() * i<<"];"<<std::endl;
				}
			}

			ss<<'}'<<std::endl;
			return ss.str();
		}

		std::string ScatterOp::partialSrcStore(std::string index, int numOfOutput) const
		{
			std::stringstream ss;
			ss<<'{'<<std::endl;
			ss<<'\t'<<"int arrange_index_base = "<<index<<';'<<std::endl;

			ss<<'\t'<<"int arrange_index;"<<std::endl;

			for(int i = 0; i < getOffsetNum(); ++i)
			{
				ss<<"\tarrange_index = arrange_index_base + (";
				if(offsetIsExpr[i])
				{
					ss<<offsetExprList[i]; 
				}else{
					ss<<offsetValList[i];
				}
				ss<<");"<<std::endl;
				ss<<std::endl;
				
			
				for(int j = 0; j < numOfOutput; ++j)
				{
					if(i != 0)	
					{
						ss<<'\t'<<outputVarPointerName(j)<<" += "<<getTupleLength()<<';'<<std::endl;
					}
	
					ss<<'\t'<<tuplePointerStore(outputVarPointerName(j), "arrange_index", outputName(j), getTupleLength())<<std::endl;

					ss<<std::endl;
				}
			}
			ss<<'}'<<std::endl;
			return ss.str();
		}
}
