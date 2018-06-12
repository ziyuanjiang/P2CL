#ifndef _P2CL_ITERATIONNODE_HPP_
#define _P2CL_ITERATIONNODE_HPP_

#include "node.hpp"

namespace p2cl{

	class IterationNode:public Node
	{
		public:
			IterationNode(std::string itName, int num, Node * upperLayer = NULL);

			IterationNode(std::string itName, std::string expr, std::string deplist, Node * upperLayer = NULL);

			virtual Node *nextNode();

			virtual Node * onceNextNode();

			virtual NodeType getNodeType() const;

			virtual ~IterationNode();

			virtual void setBodyNextNode(Node * n);
			
			virtual Node * getBodyNextNode() const
			{
				return bodyNext;
			}
		protected:
			bool in = true;
			Parameter iNum;
			Node * bodyNext;
			int index;
			std::string itName;
	};

}

#endif
