#include <iostream>
#include "extheadnode.hpp"
#include "executant.hpp"

namespace p2cl{


	NodeType ExtHeadNode::getNodeType() const
	{
		return NodeType::exthead;
	}

	Node * ExtHeadNode::nextNode()
	{
		if(in)
		{
			in = false;
			return bodyNext;
		}else{
			in = true;
			return next;
		}
	}

	void ExtHeadNode::accumulate(BufferAccumulator &acc)const
	{
		int previousExtNum = acc.getCurrentExtNum();
		int currentExtNum = extNum.value();
		int nextExtNum = 0;

		if(in)
		{
			nextExtNum = previousExtNum * currentExtNum;
		}else{
			nextExtNum = previousExtNum / currentExtNum;
		}

		acc.setExt(nextExtNum);
	}
			
	void ExtHeadNode::accumulate(Executant & obj)const
	{
		int previousExtNum = obj.tempraryExtensionNum;
		int currentExtNum = extNum.value();
		if(in)
		{
			obj.tempraryExtensionNum = 
			previousExtNum * currentExtNum;
		}else{
			obj.tempraryExtensionNum = 
			previousExtNum / currentExtNum;
		}
	}

	ExtHeadNode::~ExtHeadNode()
	{
		Node * p1 = bodyNext;
		Node * p2 = bodyNext;
		bodyNext = this;
		while(p1 != this && p1 != NULL)
		{
			p2 = p1->getDirectNextNode();
			delete p1;
			p1 = p2;
		}
	}

	ExtHeadNode::ExtHeadNode(int num, Node * upperLayer):Node(upperLayer),extNum(std::string("extensionNum"), num)

	{
		bodyNext = this;
		in = true;
		para.connectSingle(&extNum);
	}
			
	ExtHeadNode::ExtHeadNode(std::string expr, std::string dependencyList, Node * upperLayer):Node(upperLayer), extNum(std::string("extensionNum"), expr, dependencyList)
	{
		bodyNext = this;
		in = true;
		para.connectSingle(&extNum);
	}
			
	void ExtHeadNode::setBodyNextNode(Node * n)
	{
		bodyNext = n;
	}

}
