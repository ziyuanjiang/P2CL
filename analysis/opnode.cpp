#include <string>
#include <iostream>
#include <functional>
#include "executant.hpp"
#include "operation.hpp"
#include "paracontainer.hpp"
#include "exception.hpp"
#include "opnode.hpp"
#include "computationop.hpp"
#include "arrangementop.hpp"
#include "fusionprobe.hpp"
#include "transposeop.hpp"

namespace p2cl{

	OpNode::OpNode(Operation * op, Node * upperLayer):Node(upperLayer)
	{
		pOp = op;
	}

	void OpNode::setId(int id)
	{
		this->id = id;
		if(pOp!= NULL)
			pOp->setId(id);
	}
			
	void OpNode::updateOp(Operation * op)
	{
		if(pOp != NULL)
			delete pOp;

		pOp = op;
	}
			
	const Parameter * OpNode::getLengthPara() const
	{
		return para.findPara("length");
	}

	void OpNode::accumulate(BufferAccumulator &acc)const
	{
		if(pOp == NULL)
			return;

		SKType type = pOp->getSKType();
	
		const Parameter * pPara;
		int extNum = acc.getCurrentExtNum();
		int inputListSize = 0;
		int outputListSize = 0;
		int length = 0;
		int input_range = 0;
		int output_range = 0;
		int width = 0;
		int height = 0;

		if(extNum != 1)
			pOp->enableExtension();

	
		switch(type)
		{
			case SKType::map:
			case SKType::reduce:
				length = para.evalPara("length");
				pPara = para.findPara("input_range");
				if(pPara != NULL)
				{
					input_range = pPara->value();
				}else{
					input_range = length;
				}

				pPara = para.findPara("output_range");
				if(pPara != NULL)
				{
					output_range = pPara->value();
				}else{
					output_range = length;
				}

				for(int i = 0; i < pOp->numInput(); ++i)
				{
				
					inputListSize = extNum;
					inputListSize *= pOp->inputType(i).size();
					inputListSize *= input_range;

					acc.append(i, inputListSize);
				}

				for(int i = 0; i < pOp->numOutput(); ++i)
				{

					outputListSize = extNum;
					outputListSize *= pOp->outputType(i).size();

					if(type == SKType::map)
						outputListSize *= output_range;

					acc.append(i, outputListSize);
				}

				break;
			case SKType::gather:
			case SKType::scatter:
				length = para.evalPara("length");
				if(type == SKType::gather)
				{
					input_range = para.evalPara("input_range");
					output_range = length;
				}else{
					output_range = para.evalPara("output_range");
					input_range = length;
				}

				for(int i = 0; i < pOp->numInput(); ++i)
				{
					inputListSize = extNum;
					inputListSize *= pOp->inputType(i).size();
					inputListSize *= input_range;

					acc.append(i, inputListSize);
				}

				for(int i = 0; i < pOp->numOutput(); ++i)
				{
					outputListSize = extNum;
					outputListSize *= pOp->outputType(i).size();
					outputListSize *= output_range;

					acc.append(i, outputListSize);
				}
				break;
			case SKType::transpose:
				width = para.evalPara("width");
				height = para.evalPara("height");
				length = width * height; 
				for(int i = 0; i < pOp->numInput(); ++i)
				{
					inputListSize = extNum;
					inputListSize *= pOp->inputVarType(i).size();
					inputListSize *= length;
					acc.append(i, inputListSize);
				}
				break;
		}
	}

	void OpNode::accumulate(KernelAccumulator &acc, bool tuning) const
	{
		if(pOp == NULL)
			return;
		if(tuning)
		{
			acc.append(pOp->getTuningKernelSrc());
		}else{
			acc.append(pOp->getKernelSrc());
		}
	}

	std::vector<int> OpNode::evalParaList(std::vector<std::string> paraNameList)const
	{
		std::vector<int> retVal;
		for(std::vector<std::string>::iterator it = paraNameList.begin();
			it != paraNameList.end(); ++it)
		{
			retVal.push_back(para.evalExpr(*it, *it));
		}
		return retVal;
	}

	void OpNode::accumulate(Executant & acc)const
	{
		if(pOp == NULL)
			return;

		switch(pOp->getSKType())
		{
			case SKType::map:
				acc.appendMapOp(pOp->getKernelName(), pOp->numInput(), pOp->numOutput(), para.evalPara("length") , evalParaList(pOp->getDependencyList()), acc.tempraryExtensionNum);
				break;
			case SKType::reduce:
				acc.appendReduceOp(pOp->getKernelName(), std::function<void(void *, void *, void *)>(),
				pOp->numInput(), 
				para.evalPara("length"), 
				pOp->outputType(0).size(), 
				evalParaList(pOp->getDependencyList()), 
				acc.tempraryExtensionNum);
				break;
			case SKType::gather:
				{
					ArrangementOp * pArrOp = dynamic_cast<ArrangementOp *>(pOp);
					if(pArrOp != NULL)
					{
						acc.appendGatherOp(pArrOp->getKernelName(), 
						pArrOp->numInput(), para.evalPara("length"), 
						pArrOp->getTupleLength(), 
						evalParaList(pOp->getDependencyList()),
						acc.tempraryExtensionNum);
					}
				}
				break;
			case SKType::scatter:
				{
					ArrangementOp * pArrOp = dynamic_cast<ArrangementOp *>(pOp);
					if(pArrOp != NULL)
					{
						acc.appendGatherOp(pArrOp->getKernelName(), 
						pArrOp->numInput(), para.evalPara("length"), 
						pArrOp->getTupleLength(), 
						evalParaList(pOp->getDependencyList()),
						acc.tempraryExtensionNum);
					}
				}
				break;
			case SKType::transpose:
				{	
					TransposeOp * pTransOp = dynamic_cast<TransposeOp*>(pOp);
					std::vector<size_t> sizeList;
					if(pTransOp != NULL)
					{
						for(int i = 0; i < pTransOp->numInput(); ++i)
						{
							sizeList.push_back(clTypeSize(pTransOp->inputType(i).getBaseType()));
						}

						acc.appendTransposeOp(pTransOp->getKernelName(), 
						pTransOp->numInput(), 
						para.evalPara("width"), 
						para.evalPara("height"), 
						sizeList,
						pTransOp->getTupleLength(),
						acc.tempraryExtensionNum);
					}
				}
				break;

		}

	}

	void OpNode::accumulate(TypeAccumulator &acc) const
	{
		if(pOp == NULL)
			return;
		switch(pOp->getSKType())
		{
			case SKType::map:
			case SKType::reduce:
				{
					ComputationOp * p = dynamic_cast<ComputationOp*>(pOp);
					if(p != NULL)
					{
						for(int i = 0; i < p->numInput(); ++i)
						{
							acc.append(p->inputType(i));
						}
						for(int i = 0; i < p->numOutput(); ++i)
						{
							acc.append(p->outputType(i));
						}
					}
				}
				break;
				
			case SKType::scatter:
			case SKType::gather:
				{
					ArrangementOp * p = dynamic_cast<ArrangementOp *>(pOp);
					if(p != NULL)
					{

						for(int i = 0; i < p->numInput(); ++i)
						{
							acc.append(p->inputVarType(i));
						}


						for(int i = 0; i < p->numInput(); ++i)
						{
							acc.append(p->inputType(i));
						}
					}
				}
				break;
		}
	}

	NodeType OpNode::getNodeType() const
	{
		return NodeType::opnode;
	}

	OpNode::~OpNode()
	{
		if(pOp != NULL)
			delete pOp;
	}


}
