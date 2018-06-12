#include <string>
#include <sstream>
#include "computationop.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"
#include "frontendutils.hpp"

namespace p2cl{

	ComputationOp::ComputationOp(SKType type, const CFunction & rfunc,int id): function(rfunc), Operation(type, id)
	{
		int num;
		num = function.getNumFuncPara();
		funcParaValList.resize(num, 0);
		funcParaExprList.resize(num , std::string(""));
		funcParaIsExpr.resize(num , false);
		funcParaIsAssigned.resize(num, false);

		numAssignedPara = 0;
		actualNumFuncPara = num;

		indexListInput = -1;
		indexListAvailable = false;
	}


	bool ComputationOp::checkFuncPara() const
	{
		bool result = true;
		for(int i = 0; i < funcParaIsAssigned.size(); ++i)
		{
			result &= funcParaIsAssigned[i];
		}
		return result;
	}

	void ComputationOp::assignIndexList(int i)
	{
		if(i < 0 || i >= numInput())
		{
			//throw invalid function parameter exception
		}

		indexListInput = i;
		indexListAvailable = true;
	}

	void ComputationOp::assignIndexList(std::string paraName)
	{
		int i = function.paraNameIndex(paraName);
		if(i < 0 || i >= numInput())
		{
			//throw invalid function parameter exception
			return;
		}

		indexListInput = i;
		indexListAvailable = true;
	}

	void ComputationOp::setFuncPara(std::string paraName, int value)
	{
		int i = function.paraNameIndex(paraName) - function.getNumInputOutput();
		if(i < 0 || i >= actualNumFuncPara)
		{
			//throw invalid function parameter exception
		}

		if(!funcParaIsAssigned[i])
		{
			++numAssignedPara;
			funcParaIsAssigned[i] = true;
		}

		funcParaIsExpr[i] = false;
		funcParaValList[i] = value;
	}

	void ComputationOp::setFuncPara(std::string paraName, std::string expr, std::string dependencies)
	{
		setFuncPara(paraName, expr, splitParaList(dependencies));
	}

	void ComputationOp::setFuncPara(int i, std::string expr, std::string dependencies)
	{
		setFuncPara(i, expr, splitParaList(dependencies));
	}


	void ComputationOp::setFuncPara(std::string paraName, 
		std::string expr,
		std::vector<std::string> dependencies)
	{
		
		int i = function.paraNameIndex(paraName) - function.getNumInputOutput();

		if(i < 0 || i >= actualNumFuncPara)
		{
			//throw invalid function parameter exception
		}

		appendParaDependencies(dependencies);

		if(!funcParaIsAssigned[i])
		{
			++numAssignedPara;
			funcParaIsAssigned[i] = true;
		}

		funcParaIsExpr[i] = true;
		funcParaExprList[i] = expr;
	}

	void ComputationOp::setFuncPara(int i, int value)
	{
		if(i < 0 || i >= actualNumFuncPara)
		{
			//throw invalid function parameter exception
		}

		if(!funcParaIsAssigned[i])
		{
			++numAssignedPara;
			funcParaIsAssigned[i] = true;
		}
		funcParaIsExpr[i] = false;
		funcParaValList[i] = value;
	}


	void ComputationOp::setFuncPara(int i, 
		std::string expr,
		std::vector<std::string> dependencies)
	{
		
		if(i < 0 || i >= actualNumFuncPara)
		{
			//throw invalid function parameter exception
		}

		appendParaDependencies(dependencies);

		if(!funcParaIsAssigned[i])
		{
			++numAssignedPara;
			funcParaIsAssigned[i] = true;
		}

		//might be better to throw exception if the value is 
		//already assigned
		funcParaIsExpr[i] = true;
		funcParaExprList[i] = expr;
	}

}
