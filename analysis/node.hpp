#ifndef _P2CL_NODE_HPP_
#define _P2CL_NODE_HPP_

#include <string>
#include "paracontainer.hpp"
#include "typeacc.hpp"
#include "kernelacc.hpp"
#include "bufferacc.hpp"

namespace p2cl{
	
	enum NodeType{ithead, opnode, exthead, voidnode};
	class Executant;

	/*!
		operations, iteration and extension containers are all represented by
		Nodes. Several accumulator objects and probe objects can be used to iterate 
		through the node list to get information or to change the node list.
	*/
	class Node{
		public:
	
			/*!
				@brief
					constructor
				@param upperLayer
					the previous layer of node that contains
					parameters that will be used to compute the parameters
					in this node
			*/
			Node(Node *upperLayer = NULL);

			virtual ~Node(){}

			/*!
				@brief get type of the node
			*/
			virtual NodeType getNodeType() const = 0;
			
			/*!
				@brief accumulate the tuple types that are used in the current
					node
			*/
			virtual void accumulate(TypeAccumulator &) const{}

			/*!
				@brief accumulate the kernel function used in the current node
			*/
			virtual void accumulate(KernelAccumulator &, bool tuning) const{}

			/*!
				@brief accumulate the input and output buffer sizes
			*/
			virtual void accumulate(BufferAccumulator &)const{}

			/*!
				@brief accumulate the operation that need to be performed to Executant
			*/
			virtual void accumulate(Executant &)const{}

			/*!
				@brief next node with iterations
			*/
			virtual Node * nextNode();

			/*!
				@brief get next node without iterations
			*/
			virtual Node * onceNextNode();

			/*!
				@brief validate the parameters associated with the current node
			*/
			void validateAllPara(){para.connect();}

			/*!
				@brief set a parameter as value
			*/
			void setPara(std::string name, int value);

			/*!
				@brief set a parameter as expression
			*/
			void setPara(std::string name, std::string expr,std::string dependencyList);

			/*!
				@brief find a Parameter by its name
				It returns NULL, if the parameter does not exist.
			*/
			const Parameter * findPara(std::string name) const
			{
				return para.findPara(name);
			}

			/*!
				@brief copy a parameter to the current node
			*/
			void copyParameter(const Parameter * pObj)
			{
				para.copyParameter(pObj);
			}

			/*!
				@brief set the direct next node 
			*/
			void setDirectNextNode(Node *);

			/*!
				@brief set the next node that inside the current node
				It is only different from setDirectNextNode when it is an exthead node 
				or an iteration node.
			*/
			virtual void setBodyNextNode(Node *);

			/*!
				@brief get the next node that inside the current node
				It is only different from getDirectNextNode when it is an exthead node 
				or an iteration node.
			*/
			virtual Node * getBodyNextNode() const;

			/*!
				@brief get the next node
			*/
			Node * getDirectNextNode() const;

		protected:

			Node * next;
	
			ParaContainer para;
	};
	
}

#endif
