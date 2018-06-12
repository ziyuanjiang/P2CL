#include <list>
#include "fusedop.hpp"
#include "node.hpp"
#include "opnode.hpp"
#include "fusionprobe.hpp"
#include "arrangementop.hpp"
#include "parameter.hpp"

namespace p2cl{

	FusionProbe::FusionProbe()
	{
		head = NULL;
		tail = NULL;
		lengthPara = NULL;
	}

	void FusionProbe::update()
	{
		Node * pNode1 = NULL;
		Node * pNode2 = NULL;
		if(opList.size() == 0)
		{
		}else if(opList.size() == 1)
		{
			delete *(opList.begin());
		}else{
			FusedOp * pOp = new FusedOp();
			pOp->assign(opList.begin(), opList.end()); 
			head->updateOp(pOp);
			pNode1 = head->getDirectNextNode();
			while(pNode1 != tail && pNode1 != NULL)
			{
				pNode2 = pNode1->getDirectNextNode();
				head->copyParameter(pNode1->findPara("output_range"));
				delete pNode1;
				pNode1 = pNode2;
			}

			if(pNode1 == NULL)
			{
				head->setDirectNextNode(pNode1);
			}else{
				pNode2 = pNode1->getDirectNextNode();
				head->copyParameter(pNode1->findPara("output_range"));
				delete pNode1;
				head->setDirectNextNode(pNode2);
			}
		}

		opList.clear();
		head = NULL;
		tail = NULL;
	}

	void FusionProbe::appendOp(OpNode* op)
	{
		const Operation * pOp = op->getOp();
		SKType currentSktype = pOp->getSKType();
		if(head == NULL)
		{
			head = op;
			tail = op;
			lengthPara = op->getLengthPara();
		}else{
			if(!(lengthPara->equalTo(*(op->getLengthPara()))))
			{
				update();
				opList.clear();
				head = op;
				opList.push_back(pOp->clone());
				tail = op;
				return;
			}
		}

		switch(currentSktype)
		{
			case SKType::transpose:
				update();
				head = op;
				tail = op;
				update();
				break;
			case SKType::map:
				tail = op;
				opList.push_back(pOp->clone());
				break;
			case SKType::reduce:
				{
					opList.push_back(pOp->clone());
					tail = op;
					update();
				}
				break;
			case SKType::gather:
			case SKType::scatter:
				{
					const ArrangementOp * p = dynamic_cast<const ArrangementOp *>(pOp);
					if(p != NULL )
					{
						if(currentSktype == SKType::scatter){
							opList.push_back(pOp->clone());
							tail = op;
							update();
						}else if(currentSktype == SKType::gather)
						{
							update();
							head = op;
							tail = op;
							opList.push_back(pOp->clone());
						}
					}
					return;
				}
				break;
		}
	}
}

