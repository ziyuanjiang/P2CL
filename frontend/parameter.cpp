#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "tinyexpr.h"
#include "parameter.hpp"
#include "exception.hpp"

namespace p2cl
{
	Parameter::Parameter(std::string name,std::string expr, std::string dependencyList)
	{
		this->name = name;
		is_expr = true;
		this->expr = expr;
		dependList = splitParaList(dependencyList);
		dependVal = new double[dependList.size()];

		
		teVars = new te_variable[dependList.size()]();

		int err;

		for(int i = 0; i < dependList.size(); ++i)
		{
			teVars[i].name = dependList[i].c_str();
			teVars[i].address = &(dependVal[i]);
		}

		te_expr * exprtemp = te_compile(expr.c_str(), teVars, dependList.size(), &err);
		teExpr = exprtemp;
		if(exprtemp == NULL)
		{
			throw Exception("xmlError", expr + " evaluation failed "
				+"dependency list: " + dependencyList);
			//error
		}
	
		parentList.resize(dependList.size(), NULL);
	}

	Parameter::Parameter(std::string name, int value)
	{
		this->name = name;
		is_expr = false;
		this->pri_value = value;
		teVars = NULL;
		teExpr = NULL;
		dependVal = NULL;
	}

	Parameter::Parameter(const Parameter & obj)
	{
		parentList = obj.parentList;
		dependList = obj.dependList;
		name = obj.name;
		expr = obj.expr;
		is_expr = obj.is_expr;
		pri_value = obj.pri_value;

		//
		if(is_expr)
		{
			dependVal = new double[dependList.size()];
			teVars = new te_variable[dependList.size()]();

			for(int i = 0; i < dependList.size(); ++i)
			{
				teVars[i].name = dependList[i].c_str();
				teVars[i].address = &(dependVal[i]);
			}

			int err;
			te_expr * exprtemp = te_compile(expr.c_str(), teVars, dependList.size(), &err);
			teExpr = exprtemp;
			if(exprtemp == NULL)
			{
				throw Exception("xmlError", expr + " evaluation failed");
				//error
			}
		}else{
			teVars = NULL;
			teExpr = NULL;
			dependVal = NULL;
		}
	}

	Parameter::~Parameter()
	{
		if(is_expr)
		{
			te_free(teExpr);
			delete[] teVars;
			delete[] dependVal;
		}
	}

	void Parameter::setValue(int v)
	{
		is_expr = false;
		this->pri_value = v;
		if(teVars != NULL)
		{
			delete[] teVars;
			teVars = NULL;
		}
		if(teExpr != NULL)
		{
			te_free(teExpr);
			teExpr = NULL;
		}
		if(dependVal != NULL)
		{
			delete[] dependVal;
			dependVal = NULL;
		}
	}

	bool Parameter::isExpr() const
	{
		return is_expr;
	}
			
	bool Parameter::equalTo(const Parameter &b) const
	{
		bool equal = true;
		std::string expr1;
		std::string expr2;
		
		if(is_expr)
		{
			if(b.is_expr)
			{
				expr1 = expr;
				expr2 = b.expr;
				expr1.erase(std::remove(expr1.begin(), expr1.end(), ' '), expr1.end());
				expr1.erase(std::remove(expr1.begin(), expr1.end(), '\t'), expr1.end());
				expr1.erase(std::remove(expr1.begin(), expr1.end(), '\r'), expr1.end());
				expr1.erase(std::remove(expr1.begin(), expr1.end(), '\n'), expr1.end());
				expr2.erase(std::remove(expr2.begin(), expr2.end(), ' '), expr2.end());
				expr2.erase(std::remove(expr2.begin(), expr2.end(), '\t'), expr2.end());
				expr2.erase(std::remove(expr2.begin(), expr2.end(), '\r'), expr2.end());
				expr2.erase(std::remove(expr2.begin(), expr2.end(), '\n'), expr2.end());
				equal = expr1.compare(expr2) == 0;

				if(equal)
				{
					for(int i = 0; i < parentList.size(); ++i)
					{
						int j;
						for(j = 0; j < b.parentList.size(); ++j)
						{
							if(parentList[i] == parentList[j])
							{
								break;
							}
						}

						if(j == b.parentList.size())
						{
							equal &= false;
						}
					}
				}

			}else{
				equal = false;
			}
		}else{
			if(b.is_expr)
			{
				equal = false;
			}else{
				equal = pri_value == pri_value;
			}
		}
		return equal;
	}

	std::string Parameter::dependencyParaName(int i) const
	{
		return dependList[i];
	}

	int Parameter::dependencyNum() const
	{
		return dependList.size();
	}

	std::string Parameter::paraName() const
	{
		return name;
	}

	void Parameter::connectParent(const Parameter * parent)
	{
		int i;
		std::string searchName = parent->paraName();
		if(parent == NULL)
			return;

		for(i = 0; dependList.size(); ++i)
		{
			if(dependList[i].compare(searchName) == 0)
			{
				break;
			}
		}

		if(i < dependList.size())
		{
			parentList[i] = parent;
		}
	}

	int Parameter::value() const
	{
		if(is_expr)
		{
			double retVal;
			for(int i = 0; i < parentList.size(); ++i)
			{
				if(parentList[i] != NULL)
				{
					dependVal[i] = parentList[i]->value();
				}else{
					throw Exception("paraError", name + " dependency list is not complete");
					//exception
				}
			}

			retVal = te_eval(teExpr);
			return (int)retVal;
		}else{
			return pri_value;
		}
	}

}

