#ifndef _P2CL_OPNODE_HPP_
#define _P2CL_OPNODE_HPP_

#include "node.hpp"
#include "operation.hpp"
#include "parameter.hpp"

namespace p2cl
{
	class OpNode: public Node
	{
		public:
			OpNode(Operation * op, Node * upperLayer = NULL);

			void updateOp(Operation * op);
			
			void setId(int id);

			const Operation * getOp() const
			{
				return pOp;
			}

			const Parameter * getLengthPara() const;
			
			virtual void accumulate(Executant &)const override;

			virtual void accumulate(KernelAccumulator &, bool tuning) const;

			virtual void accumulate(BufferAccumulator &)const;

			NodeType getNodeType() const;

			virtual void accumulate(TypeAccumulator &acc) const;

			virtual ~OpNode();
	
		protected:
			std::vector<int> evalParaList(std::vector<std::string> paraNameList)const;
			int id;
			Operation * pOp;
	};
}

#endif
