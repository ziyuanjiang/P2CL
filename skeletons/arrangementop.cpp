#include <string>
#include <sstream>
#include <limits.h>
#include "arrangementop.hpp"
#include "cfunction.hpp"
#include "ctype.hpp"
#include "frontendutils.hpp"

namespace p2cl{

	void ArrangementOp::appendBaseType(CLType type)
	{
		baseTypeList.push_back(type);
	}

	void ArrangementOp::clearBaseType()
	{
		baseTypeList.clear();
	}

	ArrangementOp::ArrangementOp(SKType type, int id):Operation(type, id)
	{
		offsetNum = 0;
		tupleLength = 1;
	}

	void ArrangementOp::setTupleLength(int num)
	{
		this->tupleLength = num;
	}

	void ArrangementOp::setRange(int num)
	{
		range = num;
		rangeIsExpr = false;
	}

	void ArrangementOp::setRange(std::string expr, std::vector<std::string> dependencies)
	{
		appendParaDependencies(dependencies);
		rangeIsExpr = true;
		rangeExpr = expr;
	}

	void ArrangementOp::setRange(std::string expr, std::string dependencies)
	{
		setRange(expr, splitParaList(dependencies));
	}

	void ArrangementOp::clearOffsets()
	{
		offsetValList.clear();
		offsetExprList.clear();
		offsetIsExpr.clear();
		offsetNum = 0;
	}

	CLType ArrangementOp::getBaseType(int i) const
	{
		int num  = baseTypeList.size();
		if(num ==0)
		{
			return CLType::FLOAT;
		}else if(i < num){
			return baseTypeList[i];
		}else{
			return CLType::VOID;
		}
	}

	ArrangementOp& ArrangementOp::appendOffset(int value)
	{
		offsetExprList.push_back("");
		offsetValList.push_back(value);
		offsetIsExpr.push_back(false);
		++offsetNum;
		return *this;
	}
			
	ArrangementOp& ArrangementOp::appendOffset(std::string expr, std::string depList)
	{
		return appendOffset(expr, splitParaList(depList));
	}

	ArrangementOp& ArrangementOp::appendOffset(std::string expr,
		std::vector<std::string> dependencies)
	{
		appendParaDependencies(dependencies);
		offsetExprList.push_back(expr);
		offsetValList.push_back(0);
		offsetIsExpr.push_back(true);
		++offsetNum;
		return *this;
	}

	int ArrangementOp::numInput() const
	{
		int num = baseTypeList.size();
		if(num == 0)
		{
			return 1;
		}else{
			return num;
		}
	}

	int ArrangementOp::numOutput() const
	{
		int num = baseTypeList.size();
		if(num == 0)
		{
			return 1;
		}else{
			return num;
		}
	}
	
}
