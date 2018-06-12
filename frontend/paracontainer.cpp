#include <map>
#include "exception.hpp"
#include "tinyexpr.h"
#include "paracontainer.hpp"

namespace p2cl{

	ParaContainer::ParaContainer()
	{
		upperLayer = NULL;
	}

	ParaContainer::~ParaContainer()
	{
		std::map<std::string, Parameter *>::iterator it;
		for(it=paraList.begin(); it != paraList.end(); ++it)
		{
			delete it->second;
		}
	}

	void ParaContainer::copyParameter(const Parameter * pObj)
	{
		if(pObj == NULL)
		{
			return;
		}else{
			Parameter * p = pObj->clone();
			std::string name = p->paraName();
			std::map<std::string, Parameter *>::iterator it;
			it = paraList.find(name);

			if(it != paraList.end())
			{
				delete paraList[name];
			}

			paraList[name] = p;
			exprList[name] = p;
			connectSingle(p);
		}
	}

	void ParaContainer::transferFrom(ParaContainer & a)
	{
		std::map<std::string, Parameter *>::iterator it;
		for(it=paraList.begin(); it != paraList.end(); ++it)
		{
			delete it->second;
		}

		upperLayer = a.upperLayer;
		paraList = a.paraList;
		a.paraList = std::map<std::string, Parameter *>();
		exprList = a.exprList;
		a.exprList = std::map<std::string, Parameter *>();
	}

	const Parameter * ParaContainer::findPara(std::string name) const
	{
		std::map<std::string, Parameter *>::const_iterator it;
		it = paraList.find(name);

		if(it != paraList.end())
		{
			return it->second;
		}else if(upperLayer != NULL){
			const Parameter * pp = upperLayer->findPara(name);
			if(pp != NULL)
			{
				return pp;
			}
			else
			{
				return NULL;
			}

		}else{
			return NULL;
		}
	}

	void ParaContainer::setUpperLayer(const ParaContainer * parent)
	{
		upperLayer = parent;
	}


	void ParaContainer::connectSingle(Parameter * p) const
	{
		std::map<std::string, Parameter *>::const_iterator it;
		for(int i = 0; i < p->dependencyNum(); ++i)
		{
			std::string tmp;
			tmp = p->dependencyParaName(i);
			it = paraList.find(tmp);
			if(it != paraList.end())
			{
				p->connectParent(it->second);
			}else if(upperLayer != NULL)
			{
				const Parameter * pp = upperLayer->findPara(tmp);
				if(pp != NULL)
				{
					p->connectParent(pp);
				}
				else
				{
					throw Exception("parameterError", 
					"cannot find parameter " + tmp);
				}
			}
			else
			{
				throw Exception("parameterError", 
					"cannot find parameter " + tmp);
			}
		}
	}

	void ParaContainer::connect()
	{
		std::map<std::string, Parameter *>::iterator it;
		for (it=exprList.begin(); it!=exprList.end(); ++it)
		{
			Parameter * p = it->second;
			connectSingle(p);
		}
	}

	int ParaContainer::evalPara(std::string name) const
	{
		return paraList.at(name)->value();
	}

	int ParaContainer::evalExpr(std::string expr, std::string dependencyList) const
	{
		Parameter a("temp", expr, dependencyList);
		connectSingle(&a);
		return a.value();
	}

	void ParaContainer::changeValue(std::string name, int value)
	{
		std::map<std::string, Parameter *>::iterator it;
		it = paraList.find(name);

		if(it != paraList.end())
		{
			it->second->setValue(value);
		}
	}

	void ParaContainer::appendValue(std::string name, int value)
	{
		std::map<std::string, Parameter *>::iterator it;
		it = paraList.find(name);
		if(it != paraList.end())
		{
			it->second->setValue(value);
		}else{
			Parameter * p = new Parameter(name, value);
			paraList[name] = p;
			valueList[name] = p;
		}
	}

	void ParaContainer::appendExpr(std::string name, std::string expr, std::string dependencyList)
	{
		std::map<std::string, Parameter *>::iterator it;
		it = paraList.find(name);
		if(it != paraList.end())
		{
			delete paraList[name];
		}

		Parameter * p = new Parameter(name, expr, dependencyList);
		paraList[name] = p;
		exprList[name] = p;
	}

	void ParaContainer::pop(std::string name)
	{
		delete paraList.at(name);
		paraList.erase(name);
		valueList.erase(name);
		exprList.erase(name);
	}

}
