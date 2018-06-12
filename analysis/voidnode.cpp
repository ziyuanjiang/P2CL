#include "voidnode.hpp"

namespace p2cl{

	VoidNode::VoidNode(Node * upperLayer):Node(upperLayer)
	{
	}

	NodeType VoidNode::getNodeType() const
	{
		return NodeType::voidnode;
	}

}
