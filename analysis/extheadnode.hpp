#ifndef _P2CL_EXTHEADNODE_HPP_
#define _P2CL_EXTHEADNODE_HPP_

#include "node.hpp"

namespace p2cl{

	/*!
		@brief node that store the information of extension.
		All the nodes inside its body are extended with the extension number.
	*/
	class ExtHeadNode: public Node
	{
		public:
			/*!
				@brief constructor with number of extension as value
				@param num num of extension
				@param upperLayer the upper layer of this node
				It is used to get parameters in a higher layer.
			*/
			ExtHeadNode(int num, Node * upperLayer = NULL);

			/*!
				@brief constructor with number of extension as expression
				@param expr expression
				@param dependencyList dependency list that separated by space
			*/
			ExtHeadNode(std::string expr, std::string dependencyList, Node * upperLayer = NULL);

			virtual ~ExtHeadNode();

			/*!
				@brief get type of this node
			*/
			virtual NodeType getNodeType() const;

			/*!
				@brief get the next node
			*/
			virtual Node *nextNode();

			/*!
				@brief set the next body node
				the following nodes are all extended with the extension number
			*/
			virtual void setBodyNextNode(Node * n);

			virtual void accumulate(BufferAccumulator &acc)const;

			virtual void accumulate(Executant &)const override;

			/*!
				@brief set the next body node
				the operations in the body nodes are all extended with the extension number
			*/
			virtual Node * getBodyNextNode() const
			{
				return bodyNext;
			}


		protected:
			Node * bodyNext;

			Parameter extNum;

			bool in;
	};
}

#endif
