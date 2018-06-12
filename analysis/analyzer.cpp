#include <vector>
#include <regex>
#include <iostream>
#include "analyzer.hpp"
#include "exception.hpp"
#include "node.hpp"
#include "ctype.hpp"
#include "typeacc.hpp"
#include "kernelacc.hpp"
#include "bufferacc.hpp"
#include "fusionprobe.hpp"
#include "executant.hpp"

namespace p2cl{
	
	Analyzer::Analyzer()
	{
		listHead = NULL;
	}

	Analyzer::~Analyzer()
	{
		deleteNodeList();
	}

	Executant* Analyzer::getExecutant()
	{
		Node * pNode = listHead;
		Executant * pVal;
		if(pNode == NULL)
			return NULL;

		pVal = new Executant(bufferSizeList);

		while(pNode != NULL)
		{
			pNode->accumulate(*pVal);
			pNode = pNode->nextNode();
		}


		for(int i = 0; i < portVector.size(); ++i)
		{
			if(portVector[i].dirIn)
			{
				pVal->appendInputSize(
				portVector[i].length * clTypeSize(portVector[i].type));
			}else{
				pVal->appendOutputSize(
				portVector[i].length * clTypeSize(portVector[i].type));
			}
		}

		return pVal;
	}

	std::string Analyzer::getAllTypeDecl() const
	{
		TypeAccumulator acc;
		Node * pNode = listHead;
		while(pNode != NULL)
		{
			pNode->accumulate(acc);
			pNode = pNode->nextNode();
		}
		return acc.getAllTypeDecl();
	}

	std::vector<size_t> Analyzer::getMaxBufferSizes() const
	{
		BufferAccumulator acc;
		Node * pNode = listHead;
		int inIndex = 0;
		int outIndex = 0;
		for(int i = 0; i < portVector.size(); ++i)
		{
			if(portVector[i].dirIn)
			{
				acc.append(inIndex++
					, 
				portVector[i].length * clTypeSize(portVector[i].type));
			}else{
				acc.append(outIndex++
					, 
				portVector[i].length * clTypeSize(portVector[i].type));
			}
		}

		while(pNode != NULL)
		{
			pNode->accumulate(acc);
			pNode = pNode->nextNode();
		}
		return acc.getList();
	}


	std::string Analyzer::getAllKernelSrc(bool tuning) const
	{
		KernelAccumulator acc;
		Node * pNode = listHead;
		while(pNode != NULL)
		{
			pNode->accumulate(acc, tuning);
			pNode = pNode->onceNextNode();
		}
		return acc.getAllKernelSrc();
	}
	

	void Analyzer::setFunctionDefinition(const std::string & src)
	{
		functionDef = src;
	}

	void Analyzer::updateId()
	{
		int id = 0;
		Node * pNode = listHead;
		OpNode * pOpNode; 
		while(pNode != NULL)
		{
			pOpNode = 
			dynamic_cast<OpNode*>(pNode);
			pNode = pNode->onceNextNode();
			if(NULL != pOpNode)
			{
				pOpNode->setId(id++);
			}
		}
		numKernel = id;
	}

	void Analyzer::updatefuseList()
	{
		FusionProbe probe;
		Node * pNode = listHead;
		OpNode * pOpNode; 
		while(pNode != NULL)
		{
			pOpNode = 
			dynamic_cast<OpNode*>(pNode);
			pNode = pNode->onceNextNode();
			if(NULL != pOpNode)
			{
				probe.appendOp(pOpNode);
			}else{
				probe.update();
			}
		}
		probe.update();
	}

	void Analyzer::appendPort(std::string name, bool dirIn, CLType type, int length)
	{
		Port temp;
		temp.name = name;
		temp.dirIn = dirIn;
		temp.type = type;
		temp.length = length;
		portVector.push_back(temp);
	}

	void Analyzer::deleteNodeList()
	{
		if(listHead == NULL)
			return;
		
		Node * p1 = listHead;
		Node * p2 = listHead;
		listHead = NULL;
		
		while(p1 != NULL)
		{
			p2 = p1->getDirectNextNode();
			delete p1;
			p1 = p2;
		}
	}

	std::string Analyzer::getTuningKernelSrc() const
	{
		std::stringstream ss;
		if(listHead == NULL)
			return std::string("");

		ss<<typeDecl<<std::endl;
		ss<<functionDef<<std::endl;
		ss<<tuningkernelSrc<<std::endl;
		return ss.str();
	}

	std::string Analyzer::getKernelSrc() const
	{
		std::stringstream ss;
		if(listHead == NULL)
			return std::string("");

		ss<<typeDecl<<std::endl;
		ss<<functionDef<<std::endl;
		ss<<kernelSrc<<std::endl;
		return ss.str();
	}

	void Analyzer::printListSize()
	{
		std::cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$"<<std::endl;
		for(int i = 0; i < bufferSizeList.size(); ++i)
		{
			std::cout<<i<<" "<<bufferSizeList[i]<<std::endl;
		}
		std::cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$"<<std::endl;
	}

	void Analyzer::printPort()
	{
		for(int i = 0; i < portVector.size(); ++i)
		{
			Port info = portVector[i];
			std::cout<<info.name<<"\t"
				<<"dirin: "<<info.dirIn
				<<" cltype: "<<clTypeName(info.type)
				<<" length: "<<info.length<<std::endl;
		}
	}

	void Analyzer::setNodeList(Node * head)
	{
		if(listHead != NULL)
		{
			throw Exception("AnalyzerError", "cannot assign another nodelist to a already assigned process.");
		}else if(head->getNodeType() != voidnode){
			
			throw Exception("AnalyzerError", "nodelist does start with voidnode");
		}else{
			listHead = head;
			typeDecl = getAllTypeDecl();
			updatefuseList();
			updateId();
			bufferSizeList = getMaxBufferSizes();
			tuningkernelSrc = getAllKernelSrc(true);
			kernelSrc = getAllKernelSrc(false);
		}
	}

}
