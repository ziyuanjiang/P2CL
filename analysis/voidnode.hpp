#ifndef _P2CL_VOIDNODE_HPP_
#define _P2CL_VOIDNODE_HPP_

#include "node.hpp"

namespace p2cl{
	/*!
		A type node type that contains no additional information but parameters.
	*/
	class VoidNode: public Node
	{
		public:
			VoidNode(Node * upperLayer = NULL);
			virtual ~VoidNode(){}
			NodeType getNodeType() const;
		protected:
	};
}

#endif
