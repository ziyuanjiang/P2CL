#include <string>
#include <sstream>
#include <algorithm>
#include <regex>
#include "operation.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"

namespace p2cl{

	std::string Operation::commaSeparatedList(std::vector<std::string> list) const
	{
		std::stringstream ss;
		for(int i = 0; i < list.size(); ++i)
		{
			if(i != 0)
				ss<<", ";
			ss<<list[i];
		}
		return ss.str();
	}

	std::string Operation::commaSeparatedListOutput(std::vector<std::string> list) const
	{
		std::stringstream ss;
		for(int i = 0; i < list.size(); ++i)
		{
			if(i != 0)
				ss<<", ";
			ss<<'&'<<list[i];
		}
		return ss.str();
	}

	std::string Operation::inputShiftLengthExpr() const
	{
		return "g_size0";
	}

	std::string Operation::outputShiftLengthExpr() const
	{
		return "g_size0";
	}
	
	IOTupleType Operation::inputVarType(int i) const
	{
		return inputType(i);
	}

	std::string Operation::inputVarTypeName(int i) const
	{
		return inputVarType(i).getIOTupleTypeName();
	}

	std::string Operation::inputVarBaseTypeName(int i) const
	{
		return clTypeName(inputVarType(i).getBaseType());
	}

	IOTupleType Operation::outputVarType(int i) const
	{
		return outputType(i);
	}

	std::string Operation::outputVarTypeName(int i) const
	{
		return outputVarType(i).getIOTupleTypeName();
	}

	std::string Operation::outputVarBaseTypeName(int i) const
	{
		return clTypeName(outputVarType(i).getBaseType());
	}

	std::string Operation::completeStoreSrc(std::string index) const
	{
		std::stringstream ss;
		ss<<'{'<<std::endl;

		for(int i = 0; i < numOutput(); ++i)
		{
			ss<<'\t'<<outputBaseTypeName(i)<<" * "<<outputVarPointerName(i)
				<<" = ("<<outputBaseTypeName(i)<<" *)"
				<<"(&"<<outputVarName(i)<<')'
				<<';'<<std::endl;
		}

		if(opType == SKType::gather || opType == SKType::scatter)
			
			ss<<indent(partialSrcSingleStore(index, numOutput()))<<std::endl;
		else
			ss<<indent(partialSrcStore(index, numOutput()))<<std::endl;

		ss<<'}'<<std::endl;
		return ss.str();
	}

	std::string Operation::completeLoadSrc(std::string index) const
	{
		std::stringstream ss;
		ss<<'{'<<std::endl;

		for(int i = 0; i < numInput(); ++i)
		{
			ss<<'\t'<<inputVarBaseTypeName(i)<<" * "<<inputVarPointerName(i)
				<<" = ("<<inputVarBaseTypeName(i)<<" *)"
				<<"(&"<<inputVarName(i)<<')'
				<<';'<<std::endl;
		}

		if(opType == SKType::gather || opType == SKType::scatter)
			ss<<indent(partialSrcSingleLoad(index, numInput()))<<std::endl;
		else
			ss<<indent(partialSrcLoad(index, numInput()))<<std::endl;

		ss<<'}'<<std::endl;
		return ss.str();
	}

	std::string Operation::completeLoad2TempSrc(std::string index) const
	{
		std::stringstream ss;
		ss<<'{'<<std::endl;

		for(int i = 0; i < numInput(); ++i)
		{
			ss<<'\t'<<inputVarBaseTypeName(i)<<" * "<<inputVarPointerName(i)
				<<" = ("<<inputVarBaseTypeName(i)<<" *)"
				<<"(&"<<tempVarName(i)<<')'
				<<';'<<std::endl;
		}

		if(opType == SKType::gather || opType == SKType::scatter)
			ss<<indent(partialSrcSingleLoad(index, numInput()))<<std::endl;
		else
			ss<<indent(partialSrcLoad(index, numInput()))<<std::endl;

		ss<<'}'<<std::endl;
		return ss.str();
	}


	std::string Operation::partialSrcSingleLoad(std::string index, int numOfInput) const
	{
		std::stringstream ss;
		ss<<'{'<<std::endl;

		for(int j = 0; j < numOfInput; ++j)
		{
			ss<<"\t"<<inputVarPointerName(j)
			<<"[0] = "
			<<inputName(j)<<"["<<index<<" * "
			<<inputType(j).getEleNum()<<" + l0];"<<std::endl;
		}

		ss<<'}'<<std::endl;
		return ss.str();
	}

	std::string Operation::partialSrcSingleStore(std::string index, int numOfOutput) const
	{
		std::stringstream ss;
		ss<<'{'<<std::endl;

		for(int j = 0; j < numOfOutput; ++j)
		{
			ss<<'\t'<<outputName(j)
			<<"[arrange_index * "<<outputType(j).getEleNum()
			<<" + l0] = "
			<<outputVarPointerName(j)
			<<"[0];"
			<<std::endl;
		}

		ss<<'}'<<std::endl;
		return ss.str();
	}

	std::string Operation::partialSrcReduce(std::string resultName, std::string inputName) const
	{
		return std::string("");
	}

	std::string Operation::partialSrcLoad(std::string index, int numOfInput) const
	{
		std::stringstream ss;
		ss<<'{'<<std::endl;
		for(int i = 0; i < numInput(); ++i)
		{
			ss<<'\t'<<tuplePointerLoad(inputVarPointerName(i), index, inputName(i), inputTupleSize(i))<<std::endl;
		}
		ss<<'}'<<std::endl;
		return ss.str();
	}

	std::string Operation::partialSrcStore(std::string index, int numOfOutput) const
	{
		std::stringstream ss;
		ss<<'{'<<std::endl;
		for(int i = 0; i < numOutput(); ++i)
		{
			ss<<'\t'<<tuplePointerStore(outputVarPointerName(i), index, outputName(i), outputTupleSize(i))<<std::endl;
		}
		ss<<'}'<<std::endl;
		return ss.str();
	}

	std::string Operation::partialSrcComputation(std::string indexName, std::vector<std::string> inputVarList, std::vector<std::string> outputVarList) const
	{
		return std::string("");
	}

	std::vector<std::string> Operation::getDependencyList() const
	{
		return dependencyList;
	}
			
	void Operation::resetDependencyList()
	{
		dependencyList.clear();
	}

	void Operation::appendParaDependencies(std::vector<std::string> list)
	{
		for(std::vector<std::string>::iterator it = list.begin();
			it != list.end();
			++it)
		{
			appendParaDependency(*it);
		}
	}

	void Operation::appendParaDependency(std::string para)
	{
		std::vector<std::string>::iterator it;
		for(it = dependencyList.begin();
			it != dependencyList.end(); ++it)
		{
			if((*it).compare(para) == 0)
			{
				break;
			}
		}

		if(it == dependencyList.end())
		{
			dependencyList.push_back(para);
		}
	}

	int Operation::inputTupleSize(int i) const
	{
		return inputType(i).getEleNum();
	}

	int Operation::outputTupleSize(int i) const
	{
		return outputType(i).getEleNum();
	}

	std::string Operation::localBarrier() const
	{
		return std::string("barrier(CLK_LOCAL_MEM_FENCE);");
	}

	std::string Operation::globalBarrier() const
	{
		return std::string("barrier(CLK_GLOBAL_MEM_FENCE);");
	}

	std::string Operation::tuplePointerLoad(std::string varName, std::string offset, std::string pName, int n) const
	{
		std::stringstream ss;
		if(n != 1)
		{
			ss<<"TUPLE_POINTER_LOAD"<<n<<'('<<varName<<", "<<offset<<", "
				<<pName<<')'<<';';
		}else{
			ss<<varName<<"[0] = "<<pName
				<<"["<<offset<<"];"
				<<std::endl;
		}
		return ss.str();
	}

	std::string Operation::tuplePointerLoadOutofRange(std::string varName, int n) const
	{
		std::stringstream ss;
		for(int i = 0; i < n; ++i)
		{
			ss<<varName<<"["<<i<<"] = 0"<<';'<<std::endl;
		}
		return ss.str();
	}



	std::string Operation::tuplePointerStore(std::string varName, std::string offset, std::string pName, int n) const
	{
		std::stringstream ss;
		if(n != 1)
		{
			ss<<"TUPLE_POINTER_STORE"<<n<<'('<<varName<<", "<<offset<<", "
				<<pName<<')'<<';';
		}else{
			ss<<pName<<"["<<offset<<"] = "<<varName<<"[0];"<<std::endl;
		}

		return ss.str();
	}

	std::string Operation::inputBaseTypeName(int i) const
	{
		return clTypeName(inputType(i).getBaseType());
	}

	std::string Operation::outputBaseTypeName(int i) const
	{
		return clTypeName(outputType(i).getBaseType());
	}

	std::string Operation::inputTupleTypeName(int i) const
	{
		return inputType(i).getIOTupleTypeName();
	}

	std::string Operation::outputTupleTypeName(int i) const
	{
		return outputType(i).getIOTupleTypeName();
	}

	Operation::Operation(SKType type, int id)
	{
		opType = type;
		extensionEnable = false;
		setId(id);
	}

	void Operation::setId(int id)
	{
		operationID = id;
		std::stringstream ss;
		ss << "operation" << id;
		kernelIdStr = ss.str();
	}

	std::string Operation::kernelDecl(bool tuning) const
	{
		std::stringstream ss;
		ss<<"__kernel void ";
		ss<< kernelIdStr << '(' << std::endl;
		for(int i = 0; i < numInput(); ++i)
		{
			if(i != 0)
			{
				ss<<"\t\t,"<<std::endl;
			}
			ss<< "\t\t" << "__global "
				<< inputBaseTypeName(i) << " *\t"
				<< "input" << i <<std::endl;
		}

		for(int i = 0; i < numOutput(); ++i)
		{
			if(i != 0 || numInput() != 0)
				ss<<"\t\t,"<<std::endl;

			ss<< "\t\t" << "__global "
				<< outputBaseTypeName(i) << " *\t"
				<< "output" <<i <<std::endl;
		}

		for(int i = 0; i < dependencyList.size(); ++i)
		{
			ss<<"\t\t,"<<std::endl;
			ss<< "\t\tint " << dependencyList[i] <<std::endl;
		}

		if(opType == SKType::reduce)
		{
			ss<<"\t\t,"<<std::endl;
			ss<< "\t\tint reduce_length"<<std::endl;

			ss<<"\t\t,"<<std::endl;
			ss<<"\t\t__local "<<outputBaseTypeName(0)<< "*\t"<<localSpaceName(0)<<std::endl;
		}
		if(opType == SKType::transpose)
		{
			for(int i = 0;i < numInput(); ++i)
			{
				ss<<"\t\t,"<<std::endl;
				ss<<"\t\t__local "
				<<inputBaseTypeName(i)<<" *\t"
				<<localSpaceName(i)<<std::endl;
			}
			ss<<"\t\t,"<<std::endl;
			ss<<"\t\tint width"<<std::endl;
			ss<<"\t\t,"<<std::endl;
			ss<<"\t\tint height"<<std::endl;
			ss<<"\t\t,"<<std::endl;
			ss<<"\t\tint group_dimx"<<std::endl;
			ss<<"\t\t,"<<std::endl;
			ss<<"\t\tint group_dimy"<<std::endl;
		}

		if(tuning)
		{
			for(int i = 0; i < numPlatPara(); ++i)
			{
				ss<<"\t\t,"<<std::endl;
				ss<< "\t\tint " << kernelIdStr<<"PLATPARA" <<i <<std::endl;
			}
		} 

		ss<<")"<<std::endl;
		return ss.str();
	}

	std::string Operation::localSpaceName(int i) const
	{
		std::stringstream ss;
		ss<<kernelIdStr<<"local_space" <<i;
		return ss.str();
	}

	std::string Operation::platParaName(int i) const
	{
		std::stringstream ss;
		ss<<kernelIdStr<<"PLATPARA" <<i;
		return ss.str();
	}

	std::string Operation::tempVarName(int i) const
	{
		std::stringstream ss;
		ss<<"temp_variable"<<i;
		return ss.str();
	}

	std::string Operation::inputVarPointerName(int i) const
	{
		std::stringstream ss;
		ss<<"p_input_var"<<i;
		return ss.str();
	}

	std::string Operation::inputVarName(int i) const
	{
		std::stringstream ss;
		if(opType != SKType::scatter)
		{
			ss<<"input_variable";
		}else{
			ss<<"output_variable";
		}
		ss<<i;
		return ss.str();
	}
			
	std::string Operation::outputVarPointerName(int i) const
	{
		std::stringstream ss;
		ss<<"p_output_var"<<i;
		return ss.str();
	}

	std::string Operation::outputVarName(int i) const
	{
		std::stringstream ss;
		if(opType != SKType::gather)
		{
			ss<<"output_variable";
		}else{
			ss<<"input_variable";
		}
		ss<<i;
		return ss.str();
	}

	std::string Operation::inputName(int i) const
	{
		std::stringstream ss;
		ss<<"input"<<i;
		return ss.str();
	}

	std::string Operation::outputName(int i) const
	{
		std::stringstream ss;
		ss<<"output"<<i;
		return ss.str();
	}

	std::string Operation::kernelWrap(const std::string &kernelDecl, const std::string & kernelBody) const
	{
		std::stringstream ss;

		ss<< kernelDecl;
		ss<<'{'<<std::endl;

		ss<<indent(kernelBody);
    		
		ss<<'}'<<std::endl;
		return ss.str();
	}

	std::string Operation::loopWrap(const std:: string &functionCall, int initial, int length) const
	{
		std::string strLength;
		std::string strInit;
		std::stringstream ss;
		ss<<length;
		strLength = ss.str();
		ss.clear();
		ss.str("");
		ss<<initial;
		strInit = ss.str();
		return loopWrap(functionCall, strInit, strLength);
	}

	std::string Operation::loopWrapStride(const std:: string &functionCall, const std::string& initial,const std::string& endIndex, const std::string& stride) const
	{
		std::stringstream ss;
	
		ss<<"int it_end = "<<endIndex<<';'<<std::endl;
	
		ss<<"for(int it = "<<initial<<"; it < "
			<<"it_end"<< "; it += "<<stride<<")"<<std::endl;
		ss<<'{'<<std::endl;

		ss<<indent(functionCall)<<std::endl;

		ss<<'}';
		return ss.str();

	}


	std::string Operation::loopWrap(const std:: string &functionCall, const std::string &initial, const std::string& length) const
	{
		std::stringstream ss;
	
		ss<<"int it_end = ("<<initial<<" + "<<length<<");"<<std::endl;
	
		ss<<"for(int it = "<<initial<<"; it < "
			<<"it_end"<< "; ++it)"<<std::endl;
		ss<<'{'<<std::endl;

		ss<<indent(functionCall)<<std::endl;

		ss<<'}';
	
		return ss.str();
	}
	
	std::string Operation::whileWrap(std::string expr, std::string content) const
	{
		std::stringstream ss;
		ss<<"while("<<expr<<')'<<std::endl;
		ss<<'{'<<std::endl;
		ss<<indent(content)<<std::endl;
		ss<<'}';
		return ss.str();
	}

	std::string Operation::ifWrap(std::string expr, std::string content) const
	{
		std::stringstream ss;
		ss<<"if("<<expr<<')'<<std::endl;
		ss<<'{'<<std::endl;
		ss<<indent(content)<<std::endl;
		ss<<'}';
		return ss.str();
	}

	std::string Operation::ifElseWrap(std::string expr, std::string trueContent, std::string falseContent) const
	{
		std::stringstream ss;
		ss<<"if("<<expr<<')'<<std::endl;
		ss<<'{'<<std::endl;
		ss<<indent(trueContent)<<std::endl;
		ss<<"}else{"<<std::endl;
		ss<<indent(falseContent)<<std::endl;
		ss<<'}';
		return ss.str();
	}


	std::string Operation::getTuningKernelSrc() const
	{
	 	return kernelWrap(kernelDecl(true), kernelBodySrc());
	}

	std::string Operation::getKernelSrc() const
	{
	 	return kernelWrap(kernelDecl(false), kernelBodySrc());
	}

	std::string Operation::kernelBodySrc() const
	{
		switch(opType)
		{
			default:
			case SKType::map:
				return mapSrc();
			case SKType::reduce:
				return reduceSrc();
			case SKType::gather:
			case SKType::scatter:
				return arrangeSrc();
			case SKType::transpose:
				return transposeSrc();
		}
	}

	std::string Operation::transposeSrc() const
	{
	}

	std::string Operation::arrangeSrc() const
	{
		std::stringstream ss;
		ss<<groupIdSt("gp0", 0)<<std::endl;
		ss<<localIdSt("l0", 0)<<std::endl;
		ss<<shiftInputOutput()<<std::endl;
		ss<<std::endl;
		for(int i = 0; i < numInput(); ++i)
		{
			ss<<inputVarTypeName(i)<<" "<<inputVarName(i)<<';'<<std::endl;
		}
		ss<<std::endl;

		ss<<completeLoadSrc("gp0")<<std::endl;
		ss<<std::endl;
		ss<<completeStoreSrc("gp0")<<std::endl;
		return ss.str();
	}

	std::string Operation::shiftInputOutput() const
	{
		std::stringstream ss;
		std::string body;
		if(!extensionEnable)
		{
			return "";
		}
		ss<<globalIdSt("g1",1)<<std::endl;
		ss<<globalSize("g_size0",0)<<std::endl;
		for(int i = 0; i < numInput(); ++i)
		{
			ss<<inputName(i)<<" += g1  * "
			<<inputShiftLengthExpr();
			ss<<" * "<<inputTupleSize(i)<<";"<<std::endl;
		}
		for(int i = 0; i < numOutput(); ++i)
		{
			ss<<outputName(i)<<" += g1 * "
			<<outputShiftLengthExpr()<<" * "
			<<outputTupleSize(i)<<";"<<std::endl;
		}
		body = ss.str();
		ss.str("");
		ss.clear();
		ss<<"{"<<std::endl;
		ss<<indent(body);
		ss<<"}"<<std::endl;
		return ss.str();
	}

	std::string Operation::mapSrc() const
	{
		std::stringstream ss;
		std::vector<std::string> inputVarList;
		std::vector<std::string> outputVarList;
		ss<<globalIdSt("g0",0)<<std::endl;
		ss<<localIdSt("l0", 0)<<std::endl;
		ss<<shiftInputOutput()<<std::endl;
		//ss<<workGroupSize("wg_size0", 0)<<std::endl;
		//ss<<index("index0", "g0", "l0", "wg_size0")<<std::endl;

		for(int i = 0; i < numInput(); ++i)
		{
			ss<< inputVarTypeName(i)<<' '<<inputVarName(i)
				<<";"<<std::endl;


			inputVarList.push_back(inputVarName(i));
		}

		for(int i = 0; i < numOutput(); ++i)
		{
			ss<< outputVarTypeName(i)<<' '<<outputVarName(i)
				<<';'<<std::endl;

			outputVarList.push_back(outputVarName(i));
		}

		ss<<completeLoadSrc("g0")<<std::endl;
		ss<<partialSrcComputation("g0",inputVarList, outputVarList)<<std::endl;
		ss<<completeStoreSrc("g0")<<std::endl;

		//ss<<loopWrap(ssTmp.str(), "index0", platParaName(0))<<std::endl;

		return ss.str();
	}

	std::string Operation::getParaDecl(std::vector<int> para) const
	{
		std::stringstream ss;
		for(int i = 0; i < numPlatPara(); ++i)
  		{
			ss << "#define\t" << platParaName(i) << "\t"
				<< para[i]<<std::endl; 
		}
	}

	std::string Operation::reduceLoad(std::string varName, std::string offset) const
	{
		return tuplePointerLoad(varName, offset, inputName(0), inputTupleSize(0));
	}

	std::string Operation::reduceSrc() const
	{
		std::stringstream ss;
		std::stringstream ssParaList;
		std::stringstream ssTmp;
		std::string itEnd;
		std::string ifStat;
		std::string reduceSrc;

		ss<<groupIdSt("gp0",0)<<std::endl;
		ss<<shiftInputOutput()<<std::endl;
		ss<<localIdSt("l0", 0)<<std::endl;
		ss<<workGroupSize("l_size0", 0)<<std::endl;
		ss<<globalSize("g_size0", 0)<<std::endl;
		ss<<globalIdSt("index0", 0)<<std::endl;

		ss<<outputVarTypeName(0)<<' '<<inputVarName(0)<<';'<<std::endl;
		ss<<outputVarTypeName(0)<<" partial_result"<<';'<<std::endl;
		ss<<outputVarBaseTypeName(0)<<"* p_partial_result = ("<< outputVarBaseTypeName(0)<<"*)&partial_result;"<<std::endl;
		ss<<outputVarBaseTypeName(0)<<"* "<<inputVarPointerName(0)<<"= ("<< outputVarBaseTypeName(0)<<"*)&"<<inputVarName(0)<<';'<<std::endl;

		ss<< "int partial_length = l_size0;"<<std::endl;

		//load first tuple
		ss<<reduceLoad("partial_result", "index0")<<std::endl;

		//
		reduceSrc = partialSrcReduce("partial_result", inputVarName(0));

		ssTmp.str("");

		ssTmp<<reduceLoad(inputVarName(0), "it")<<std::endl;
		ssTmp<<reduceSrc<<std::endl;

		ss<<loopWrapStride(ssTmp.str(), "index0 + g_size0", "reduce_length", "g_size0")<<std::endl;
		ss<<std::endl;
		//store result to local mem
		ss<<tuplePointerStore("(p_partial_result)", "l0", localSpaceName(0),outputTupleSize(0))<<std::endl;
		ss<<localBarrier()<<std::endl;

		//reduce first half of local array to the seconde half
		ssTmp.str("");

		ssTmp<<tuplePointerLoad(inputVarPointerName(0), "l0 + partial_length", localSpaceName(0), outputTupleSize(0))<<std::endl;
		ssTmp<<reduceSrc<<std::endl;

		ssTmp<<tuplePointerStore("p_partial_result", "l0", localSpaceName(0),outputTupleSize(0))<<std::endl;

		ifStat = ifWrap("l0 < partial_length", ssTmp.str());

		//while loop content
		ssTmp.str("");
		ssTmp<<"partial_length >>= 1;"<<std::endl;
		ssTmp<<ifStat<<std::endl;
		ssTmp<<localBarrier();

		ss<<whileWrap("partial_length > 1", ssTmp.str())<<std::endl;

		//write to gloabl mem
		ss<<ifWrap("l0 == 0",tuplePointerStore("p_partial_result", "gp0", outputName(0), outputTupleSize(0)))<<std::endl;

		return ss.str();
	}

	std::string Operation::workGroupSize(const std::string &varName, int dim) const
	{
		std::stringstream ss;
		ss<<"size_t "<< varName<< " = get_local_size("
		<<dim<<");"<<std::endl;
		return ss.str();
	}

	std::string Operation::globalSize(const std::string &varName, int dim) const
	{
		std::stringstream ss;
		ss<<"size_t "<< varName<< " = get_global_size("
		<<dim<<");"<<std::endl;
		return ss.str();
	}


	std::string Operation::groupIdSt(const std::string & varName, int dim) const
	{
		std::stringstream ss;
		ss<<"size_t "<<varName<<" = get_group_id("<<dim<<");"<<std::endl;
		return ss.str();
	}

	std::string Operation::globalIdSt(const std::string & varName, int dim) const
	{
		std::stringstream ss;
		ss<<"size_t "<<varName<<" = get_global_id("<<dim<<");"<<std::endl;
		return ss.str();
	}

	std::string Operation::localIdSt(const std::string &varName, int dim) const
	{
		std::stringstream ss;
		ss<<"size_t "<<varName<<" = get_local_id("<<dim<<");"<<std::endl;
		return ss.str();
	}

	std::string Operation::indent(const std::string & srcBody) const
	{
		std::stringstream ss;
		std::string line;
		std::istringstream isBody(srcBody);
		while (std::getline(isBody, line)) 
		{
			ss<<"\t"<<line<<std::endl;
		}

		return ss.str();
	}
}

