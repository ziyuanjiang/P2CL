#include "iterationnode.hpp"

namespace p2cl{

	IterationNode::IterationNode(std::string itName, int num, Node * upperLayer):Node(upperLayer), iNum("iterationNum", num)
	{
		this->itName = itName;
		index = -1;
		para.appendValue(itName, -1);
		bodyNext = this;
		para.connectSingle(&iNum);
	}

	IterationNode::IterationNode(std::string itName, std::string expr, std::string deplist, Node * upperLayer):Node(upperLayer), iNum("iterationNum", expr, deplist)
	{
		this->itName = itName;
		index = -1;
		para.appendValue(itName, -1);
		bodyNext = this;
		para.connectSingle(&iNum);
	}

	IterationNode::~IterationNode()
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

	NodeType IterationNode::getNodeType() const
	{
		return NodeType::ithead;
	}

	Node * IterationNode::nextNode()
	{
		++index;
		if(index < iNum.value())
		{
			para.changeValue(itName, index);
			return bodyNext;
		}else{
			index  = -1;
			para.changeValue(itName, index);
			return next;
		}
	}
			
	Node * IterationNode::onceNextNode()
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
			
	void IterationNode::setBodyNextNode(Node * n)
	{
		bodyNext = n;
	}

}
