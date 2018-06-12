#include "node.hpp"

namespace p2cl{

	Node::Node(Node * upperLayer)
	{
		next = NULL;
		if(upperLayer != NULL)
			para.setUpperLayer(&(upperLayer->para));
	}

	Node * Node::nextNode()
	{
		return next;
	}

	Node * Node::onceNextNode()
	{
		return nextNode();
	}

	void Node::setPara(std::string name, int value)
	{
		para.appendValue(name, value);
	}

	void Node::setPara(std::string name, std::string expr,std::string dependencyList)
	{
		para.appendExpr(name, expr, dependencyList);
	}

	void Node::setDirectNextNode(Node * n)
	{
		next = n;
	}

	void Node::setBodyNextNode(Node * n)
	{
		setDirectNextNode(n);
	}

	Node * Node::getBodyNextNode() const
	{
		return getDirectNextNode();
	}

	Node * Node::getDirectNextNode() const
	{
		return next;
	}

}
