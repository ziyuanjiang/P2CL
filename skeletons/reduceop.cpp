#include <string>
#include <sstream>
#include <limits.h>
#include "reduceop.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"
#include "exception.hpp"

namespace p2cl{
	
		ReduceOp* ReduceOp::clone() const
		{
			return new ReduceOp(*this);
		}

		ReduceOp::ReduceOp(const CFunction & rfunc, int id):ComputationOp(SKType::reduce, rfunc, id)
		{
		}
		
		IOTupleType ReduceOp::inputType(int i) const
		{
			return function.getInputType(0);
		}

		IOTupleType ReduceOp::outputType(int i) const
		{
			return function.getOutputType(0);
		}

		std::string ReduceOp::inputShiftLengthExpr() const
		{
			return "reduce_length";
		}

		std::string ReduceOp::outputShiftLengthExpr() const
		{
			return "get_local_size(0)";
		}

		int ReduceOp::numInput() const
		{
			return 1;
		}

		int ReduceOp::numOutput() const
		{
			return 1;
		}

		std::string ReduceOp::partialSrcReduce(std::string resultName, std::string inputName) const
		{
			std::stringstream ss;
			std::stringstream funcParaList;
			for(int i = 0; i < actualNumFuncPara; ++i)
			{
				if(i == 0)
				{
					funcParaList<<", ";
				}

				if(funcParaIsAssigned[i])
				{
					if(funcParaIsExpr[i])
					{
						funcParaList<<funcParaValList[i];
					}else{
						funcParaList<<'('
							<<funcParaIsExpr[i]
							<<')';
					}
				}else{
					std::stringstream errmsg;
					errmsg<<"parameter "<<i<<" of ";
					errmsg<<function.getName();
					errmsg<<" is not set";
					//throw exception
					throw Exception("skeletonError", errmsg.str());

				}
			}

			ss<<'{'<<std::endl;

			ss<<'\t'<<function.getFunctionCall(resultName, inputName, std::string("&") + resultName, funcParaList.str())<<std::endl;

			ss<<'}'<<std::endl;
			return ss.str();
		}

}
