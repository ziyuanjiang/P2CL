#include <string>
#include <sstream>
#include "computationop.hpp"
#include "mapop.hpp"
#include "fusedop.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"
#include "exception.hpp"

namespace p2cl{

	FusedOp* FusedOp::clone() const
	{
		return new FusedOp(*this);
	}

	FusedOp::FusedOp(const FusedOp& obj):Operation(obj)
	{
		Operation * pOp;
		setSKType(obj.getSKType());
		for(int i = 0; i < obj.opsList.size(); ++i)
		{
			pOp = obj.opsList[i]->clone();
			opsList.push_back(pOp);
			
			if(pOp->getSKType() == SKType::map)
			{
				MapOp * pMap = dynamic_cast<MapOp*>(pOp);
				if(pMap != NULL)
				{
					mapOpsList.push_back(pMap);
				}else{
					//exception
				}
			}		
		}
	}

	IOTupleType FusedOp::outputVarType(int i) const
	{
		if(opsList.empty())
			return std::string("");
		else if(getSKType() == SKType::reduce)
		{
			return opsList.back()->outputVarType(i);
		}else if(mapOpsList.empty())
		{
			return opsList[0]->outputVarType(i);
		}else{
			return mapOpsList[0]->outputVarType(i);
		}
	}

	std::string FusedOp::inputShiftLengthExpr() const
	{
		if(opsList.empty())
			return "g_size0";
		else
			return opsList[0]->inputShiftLengthExpr();

	}

	std::string FusedOp::outputShiftLengthExpr() const
	{
		if(opsList.empty())
			return "g_size0";
		else
			return opsList.back()->outputShiftLengthExpr();

	}



	IOTupleType FusedOp::inputVarType(int i) const
	{
		if(opsList.empty())
			return std::string("");
		else if(getSKType() == SKType::reduce)
		{
			return opsList.back()->inputVarType(i);
		}else if(mapOpsList.empty())
		{
			return opsList[0]->inputVarType(i);
		}else{
			return mapOpsList[0]->inputVarType(i);
		}
	}

	FusedOp::~FusedOp()
	{
		for(std::vector<Operation*>::iterator it = opsList.begin();
			it != opsList.end();
			++it)
		{
			delete *it;
		}

		opsList.clear();
		mapOpsList.clear();
	}


	FusedOp::FusedOp(int id): Operation(SKType::map, id)
	{
	}


	IOTupleType FusedOp::inputType(int i) const
	{
		if(opsList.empty())
			return IOTupleType(CLType::FLOAT, 1);
		else
			return opsList[0]->inputType(i); 
	}

	IOTupleType FusedOp::outputType(int i) const
	{
		if(opsList.empty())
			return IOTupleType(CLType::FLOAT, 1);
		else
			return opsList.back()->outputType(i); 
	}

	int FusedOp::numInput() const
	{
		if(opsList.empty())
			return 0;
		else if(mapOpsList.empty())
			return opsList[0]->numInput(); 
		else
			return mapOpsList[0]->numInput();
	}

	int FusedOp::numOutput() const
	{
		if(opsList.empty())
			return 0;
		else if(mapOpsList.empty())
			return opsList[0]->numOutput(); 
		else
			return mapOpsList[0]->numOutput();
	}

	std::string FusedOp::partialSrcLoad(std::string index, int numOfInput) const
	{
		if(opsList.empty())
			return std::string("");

		return opsList[0]->partialSrcLoad(index, numOfInput);
	}

	std::string FusedOp::partialSrcStore(std::string index, int numOfOutput) const
	{
		if(opsList.empty())
			return std::string("");
	
		return opsList.back()->partialSrcStore(index, numOfOutput);
	}

	std::string FusedOp::partialSrcSingleLoad(std::string index, int numOfInput) const
	{
		if(opsList.empty())
			return std::string("");

		return opsList[0]->partialSrcSingleLoad(index, numOfInput);

	}

	std::string FusedOp::partialSrcSingleStore(std::string index, int numOfOutput) const
	{
		if(opsList.empty())
			return std::string("");

		return opsList[0]->partialSrcSingleStore(index, numOfOutput);
	}

	std::string FusedOp::reduceLoad(std::string varName, std::string offset) const
	{
		std::stringstream ss;
		std::vector<std::string> inputVarList;
		std::vector<std::string> outputVarList;

		ss<<'{'<<std::endl;
		for(int i = 0; i < numInput(); ++i)
		{
			ss<< '\t'<<inputVarTypeName(i)<<' '<<tempVarName(i)
				<<";"<<std::endl;

			inputVarList.push_back(tempVarName(i));
		}

		outputVarList.push_back(varName);

		ss<<indent(completeLoad2TempSrc(offset))<<std::endl;
		ss<<indent(partialSrcComputation(offset, inputVarList, outputVarList))<<std::endl;
		ss<<'}'<<std::endl;

		return ss.str();
	}

	std::string FusedOp::partialSrcReduce(std::string resultName, std::string inputName) const
	{
		if(pReduceObj == NULL)
			return std::string("");
		else{
			return pReduceObj->partialSrcReduce(resultName, inputName);
		}
	}


	std::string FusedOp::partialSrcComputation(std::string indexName, std::vector<std::string> inputVarList, std::vector<std::string> outputVarList) const
	{
		std::stringstream ssAll;
		std::stringstream ss;
		std::vector<std::string> tempVarList1;
		std::vector<std::string> tempVarList2;
		int tempCount = 0;
		int i;

		if(mapOpsList.empty())
		{
			for(int i = 0; i < inputVarList.size(); ++i)
			{
				ss<<inputVarList[i]<<" = "<<outputVarList[i]<<';'<<std::endl;
			}
			return ss.str();
		}else if(mapOpsList.size() == 1)
		{
			return mapOpsList[0]->partialSrcComputation(indexName, inputVarList, outputVarList);
		}

		//first function call
		ssAll<<'{'<<std::endl;
		for(int j = 0; j < mapOpsList[0]->numOutput(); ++j)
		{
			tempVarList2.push_back(tempVarName(tempCount));
			ss<<mapOpsList[0]->outputVarTypeName(j)<<' '<<tempVarName(tempCount++)<<';'<<std::endl;
		}

		ss<<mapOpsList[0]->partialSrcComputation(indexName, inputVarList, tempVarList2)<<std::endl;
		tempVarList1 = tempVarList2;

		for(i = 1; i < mapOpsList.size()- 1; ++i)
		{
			for(int j = 0; j < mapOpsList[i]->numOutput(); ++j)
			{
				tempVarList2.push_back(tempVarName(tempCount));
				ss<<mapOpsList[i]->outputTupleTypeName(j)<<' '<<tempVarName(tempCount++)<<';'<<std::endl;
			}


			ss<<mapOpsList[i]->partialSrcComputation(indexName, tempVarList1, tempVarList2)<<std::endl;
			tempVarList1 = tempVarList2;
		}
		ss<<mapOpsList.back()->partialSrcComputation(indexName, tempVarList1, outputVarList)<<std::endl;
		ssAll<<indent(ss.str())<<std::endl;
		ssAll<<'}'<<std::endl;
		return ssAll.str();
	}

	void FusedOp::clearOpsList()
	{
		for(std::vector<Operation*>::iterator it = opsList.begin();
			it != opsList.end();
			++it)
		{
			delete *it;
		}

		resetDependencyList();
		opsList.clear();
		mapOpsList.clear();
		pReduceObj = NULL;
	}

	void FusedOp::update()
	{
		SKType currentSKType;
		setSKType(SKType::scatter);
		for(int i = 0; i < opsList.size(); ++i)
		{
			currentSKType = opsList[i]->getSKType();
			appendParaDependencies(opsList[i]->getDependencyList());
			if(currentSKType == SKType::map)
			{
				MapOp * pMap = dynamic_cast<MapOp*>(opsList[i]);
				if(pMap != NULL)
				{
					setSKType(SKType::map);
					mapOpsList.push_back(pMap);
				}else{
					throw Exception("fusion error", "only the four basic operation object can be fused.");
					//exception
				}
			}else if(currentSKType == SKType::reduce){
				ReduceOp * pReduce = dynamic_cast<ReduceOp*>(opsList[i]);
				if(pReduce != NULL)
				{
					setSKType(SKType::reduce);
					pReduceObj = pReduce;
				}else{
					throw Exception("fusion error", "only the four basic operation object can be fused.");
					//exception
				}
			}else if(currentSKType == SKType::gather)
			{
				setSKType(SKType::gather);
			}else if(currentSKType == SKType::scatter && getSKType() == SKType::gather)
			{
				setSKType(SKType::map);
			}
		}
	}

}
