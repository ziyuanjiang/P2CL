#include <string>
#include <sstream>
#include <limits.h>
#include "mapop.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"
#include "exception.hpp"

namespace p2cl{
	
		MapOp* MapOp::clone() const
		{
			return new MapOp(*this);
		}

		MapOp::MapOp(const CFunction & rfunc, int id):ComputationOp(SKType::map, rfunc, id)
		{
		}
		
		IOTupleType MapOp::inputType(int i) const
		{
			return function.getInputType(i);
		}

		IOTupleType MapOp::outputType(int i) const
		{
			return function.getOutputType(i);
		}

		int MapOp::numInput() const
		{
			int num = function.getNumInput();
			if(indexListAvailable)
			{
				--num;
			}
			return num;
		}

		int MapOp::numOutput() const
		{
			return function.getNumOutput();
		}

		std::string MapOp::partialSrcComputation(std::string indexName,std::vector<std::string> inputVarList, std::vector<std::string> outputVarList) const
		{
			std::stringstream ss;
			std::stringstream funcParaList;
			for(int i = 0; i < actualNumFuncPara; ++i)
			{
				if(i != 0)
				{
					funcParaList<<", ";
				}

				if(funcParaIsAssigned[i])
				{
					if(funcParaIsExpr[i])
					{
						funcParaList<<funcParaExprList[i];
					}else{
						funcParaList<<'('
							<<funcParaValList[i]
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
			if(indexListAvailable)
			{
				std::stringstream inputss;
				std::vector<std::string>::iterator it = inputVarList.begin();
				int i = 0;
				for(i = 0; i < function.getNumInput(); ++i)
				{   
					if(i != 0)
						inputss<<", ";

					if(i == indexListInput)
					{
						inputss<<indexName;
					}else{
						//might throw exception
						inputss<<(*it);
						++it;
					}

				}

				if(i == 0)
				{
					inputss<<"g0";
				}

				ss<<'\t'<<function.getFunctionCall(inputss.str(), commaSeparatedListOutput(outputVarList), funcParaList.str())<<std::endl;
			}else{
				ss<<'\t'<<function.getFunctionCall(commaSeparatedList(inputVarList), commaSeparatedListOutput(outputVarList), funcParaList.str())<<std::endl;
			}
			ss<<'}'<<std::endl;
			return ss.str();
		}

}
