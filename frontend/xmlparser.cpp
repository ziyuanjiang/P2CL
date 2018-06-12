#include <sstream>
#include <iostream>
#include <regex>
#include <map>
#include "operation.hpp"
#include "mapop.hpp"
#include "reduceop.hpp"
#include "scatterop.hpp"
#include "gatherop.hpp"
#include "transposeop.hpp"
#include "voidnode.hpp"
#include "extheadnode.hpp"
#include "node.hpp"
#include "opnode.hpp"
#include "iterationnode.hpp"
#include "xmlparser.hpp"
#include "exception.hpp"

namespace p2cl{
	XmlParser::XmlParser(const std::string & fileName)
	{
		std::stringstream ss;
		tinyxml2::XMLError err = doc.LoadFile(fileName.c_str());
		if(tinyxml2::XML_SUCCESS != err)
		{
			ss<<"cannot open "
			<<fileName<<std::endl;
			ss<<doc.GetErrorStr1()
			<<doc.GetErrorStr2();
			throw Exception("xmlError",
			 ss.str());
		}
		
		tinyxml2::XMLElement * rootEle = doc.FirstChildElement();

		functionDesRoot = rootEle->FirstChildElement("function_descriptions");
	
		//document without function description is legal
		if(functionDesRoot != NULL)		
		{
			CFunction func;
			tinyxml2::XMLElement* functionEle;
			functionEle = functionDesRoot->FirstChildElement("function");
			while(functionEle != NULL)
			{
				func = extractCFunction(functionEle);
				functionTable[func.getName()] = func;
				functionEle = functionEle->NextSiblingElement("function");
			}
		}
		
		processRoot = rootEle->FirstChildElement("process");
		if(processRoot == NULL)
		{
			throw Exception("xmlError", "no process");
		}
	}

	Analyzer * XmlParser::getAnalyzer() const
	{
		std::string name;
		bool dirIn;
		CLType type;
		int length;

		const tinyxml2::XMLElement * ele;
		bool outPortAvailable = false;
		Analyzer * p = new Analyzer();
		Node * pNode = new VoidNode();

		loadMultipleParameter(processRoot, pNode);

		pNode->validateAllPara();

		ele = processRoot->FirstChildElement("port");
		//port info
		if(ele == NULL)
		{
			throw Exception("xmlError", "no port in process");
		}

		while(ele != NULL)
		{
			getPort(ele, name, dirIn, type, length);
			p->appendPort(name, dirIn, type, length);
			outPortAvailable |= !dirIn;
			ele = ele->NextSiblingElement("port");
		}

		if(!outPortAvailable)
		{
			throw Exception("xmlError", "no output port in process");
		}

		pNode->setDirectNextNode(loadChildOperationList(processRoot, pNode));
		
		p->setNodeList(pNode);

		return p;
	}

	Node * XmlParser::loadExtNode(const tinyxml2::XMLElement * ele, Node * upperLayer) const
	{
		ParaData data;
		std::string itName;
		Node * pNodeHead = NULL;
		Node * pNodeChild = NULL;
		Node * pNodeLast = NULL;
		const tinyxml2::XMLElement * childEle; 
		childEle = ele->FirstChildElement("num_of_extensions");
		if(childEle == NULL)
		{
			throw Exception("xmlError", "no num_of_extensions inside high-order map pattern");
		}

		data = loadParaData(childEle, "num_of_extensions");

		if(data.isExpr)
		{
			pNodeHead = new ExtHeadNode(data.expr, data.depList, upperLayer);
		}else{
			pNodeHead = new ExtHeadNode(data.value, upperLayer);
		}
		
		loadMultipleParameter(ele, pNodeHead);
		pNodeHead->validateAllPara();
		pNodeChild = loadChildOperationList(ele, pNodeHead);

		pNodeHead->setBodyNextNode(pNodeChild); 
		pNodeLast = getNodeListLast(pNodeChild);
		pNodeLast->setDirectNextNode(pNodeHead);
		return pNodeHead;
	}

	Node * XmlParser::getNodeListLast(Node * node) const
	{
		Node * currentNode = node;
		Node * nextNode;
		if(currentNode == NULL)
			return NULL;

		nextNode = currentNode->getDirectNextNode();

		while(nextNode != NULL)
		{
			currentNode = nextNode;
			nextNode = currentNode->getDirectNextNode();
		}

		return currentNode;
	}

	Node * XmlParser::loadChildOperationList(const tinyxml2::XMLElement * root, Node * upperLayer) const
	{
		//the first node will not be returned, it is used to mark the head
		Node * pNodeFakeHead = new VoidNode();
		pNodeFakeHead->setDirectNextNode(NULL);
		Node * pNodeHead;
		Node * pNode1 = pNodeFakeHead;
		Node * pNode2 = pNodeFakeHead;

		std::string eleName;
		const tinyxml2::XMLElement * ele;
		ele = root->FirstChildElement();
		std::cout<<"++++++++++++++++++++++++"<<std::endl;
		while(ele != NULL)
		{
			eleName = ele->Value();
			{
				if(eleName.compare("stage_generate") == 0)
				{
					std::cout<<"stage_generate+"<<std::endl;
					pNode2 = loadIterationNode(ele, upperLayer);
					pNode1->setDirectNextNode(pNode2);
					pNode1 = pNode2;
					std::cout<<"stage_generate-"<<std::endl;
				}else if(eleName.compare("map") == 0){
					std::cout<<"high-order map+"<<std::endl;
					pNode2 = loadExtNode(ele, upperLayer);
					pNode1->setDirectNextNode(pNode2);
					pNode1 = pNode2;
					std::cout<<"high-order map-"<<std::endl;
				}else if(eleName.compare("operation") == 0){
					std::cout<<"operation+"<<std::endl;
					pNode2 = loadOperationNode(ele, upperLayer);
					pNode1->setDirectNextNode(pNode2);
					pNode1 = pNode2;
					std::cout<<"operation-"<<std::endl;
				}
			}
			ele = ele->NextSiblingElement();
		}
		std::cout<<"-----------------------"<<std::endl;

		pNodeHead  = pNodeFakeHead->getDirectNextNode();
		pNodeFakeHead->setDirectNextNode(NULL);
		delete pNodeFakeHead;
		return pNodeHead;
	}

	Node * XmlParser::loadOperationNode(const tinyxml2::XMLElement * ele, Node * upperLayer) const
	{
		std::stringstream ss;
		Node * pNode;
		std::regex mapReg("^[ \t\r\n]*(map)[ \t\r\n]*$");
		std::regex reduceReg("^[ \t\r\n]*(reduce)[ \t\r\n]*$");
		std::regex gatherReg("^[ \t\r\n]*(gather)[ \t\r\n]*$");
		std::regex scatterReg("^[ \t\r\n]*(scatter)[ \t\r\n]*$");
		std::regex transposeReg("^[ \t\r\n]*(transpose)[ \t\r\n]*$");
		const tinyxml2::XMLElement * childEle; 
		std::string skeletonType;
		childEle = ele->FirstChildElement("skeleton_type");
		if(childEle == NULL)
		{
			throw Exception("xmlError", "one operation's skeleton_type is unclear");
		}
		skeletonType = childEle->GetText();
		SKType skt;

		if(regex_match(skeletonType, mapReg))
		{
			skt = SKType::map;
		}else if(regex_match(skeletonType, reduceReg))
		{
			skt = SKType::reduce;
		}else if(regex_match(skeletonType, gatherReg))
		{
			skt = SKType::gather;
		}
		else if(regex_match(skeletonType, scatterReg)){
			skt = SKType::scatter;
		}else if(regex_match(skeletonType, transposeReg)){
			skt = SKType::transpose;
		}else{
			throw Exception("xmlError", "one operation have invalid skeleton_type");
		}

		ParaData data;
		std::map<std::string, CFunction>::const_iterator it;
		switch(skt)
		{
			case SKType::transpose:
				{
					ParaData tupleSizeData;
					ParaData widthData;
					ParaData heightData;
					std::vector<std::string> typeVector;
					const tinyxml2::XMLElement * typeEle; 
					int numBaseType = 0;

					childEle = ele->FirstChildElement("tuple_size");
					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType
							+ "operation does not specify tuple_size");
					}

					tupleSizeData = loadParaData(childEle, "tuple_size");


					childEle = ele->FirstChildElement("width");
					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType
							+ "operation does not specify width");
					}

					widthData = loadParaData(childEle, "width");

					childEle = ele->FirstChildElement("height");
					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType
							+ "operation does not specify height");
					}

					heightData = loadParaData(childEle, "height");

					childEle = ele->FirstChildElement("basetype");
					if(childEle != NULL)
					{
						ss.str("");
						ss.clear();
						numBaseType = 0;
						ss<<"type"<<numBaseType;
						typeEle = childEle->FirstChildElement(ss.str().c_str());

						while(typeEle != NULL)
						{
							std::string basetypeName;
							basetypeName = typeEle->GetText();
							typeVector.push_back(basetypeName);
							++numBaseType;
							ss.str("");
							ss.clear();
							ss<<"type"<<numBaseType;
							typeEle = childEle->FirstChildElement(ss.str().c_str());
						}
					}


					TransposeOp* pOp = new TransposeOp();

					for(int i = 0; i < typeVector.size(); ++i)
					{
						std::string basetypeName;
						basetypeName = typeVector[i];
						IOTupleType tempType(basetypeName);
						CLType basetype = tempType.getBaseType();
						int numEle = tempType.getEleNum();
						if(basetype == CLType::VOID || numEle != 1)
							throw Exception("xmlError", std::string("invalid base type for ") + skeletonType +" operation.");
						pOp->appendBaseType(basetype);
					}

					if(tupleSizeData.isExpr)
					{
						throw Exception("xmlError", std::string("tuple size for ")+ skeletonType + std::string(" operation can only be a fixed value.") );
						/*
						pOp->setTupleLength(tupleSizeData.expr, tupleSizeData.depList);
						pNode = new OpNode(pOp, upperLayer);
						pNode->setPara("localLength", tupleSizeData.expr, tupleSizeData.depList);
						*/
					}
					else	
					{
						pOp->setTupleLength(tupleSizeData.value);
						pNode = new OpNode(pOp, upperLayer);
					}

					if(widthData.isExpr)
					{
						pNode->setPara("width", widthData.expr, widthData.depList);
					}else{
						pNode->setPara("width", widthData.value);
					}

					if(heightData.isExpr)
					{
						pNode->setPara("height", heightData.expr, heightData.depList);
					}else{
						pNode->setPara("height", heightData.value);
					}


				}
				break;
			case SKType::map:
			case SKType::reduce:
				{
					ParaData lengthData;
					int numFuncPara;
					int numXMLFuncPara = 0;

					std::string functionName;
					const tinyxml2::XMLElement * funParaEle; 
					std::vector<ParaData> paraVector;

					childEle = ele->FirstChildElement("length");
					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType+ " operation does not specify length");
					}
					
					lengthData = loadParaData(childEle, "length");

					childEle = ele->FirstChildElement("function");
					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType
							+ " operation does not specify function");
					}

					functionName = childEle->GetText();
					it = functionTable.find(functionName);
					if(it == functionTable.end())
					{
						throw Exception("xmlError", std::string("function ")
							+ functionName + " is not declared");
					}
					numFuncPara = it->second.getNumFuncPara();
					childEle = ele->FirstChildElement("func_para");
					if(childEle != NULL)
					{
						ss.str("");
						ss.clear();
						numXMLFuncPara = 0;
						ss<<"para"<<numXMLFuncPara;
						funParaEle = childEle->FirstChildElement(ss.str().c_str());

						while(funParaEle != NULL)
						{
							data = loadParaData(funParaEle, ss.str());
							paraVector.push_back(data);
							++numXMLFuncPara;
							ss.str("");
							ss.clear();
							ss<<"para"<<numXMLFuncPara;
							funParaEle = childEle->FirstChildElement(ss.str().c_str());
						}
					}

					if(numFuncPara != paraVector.size())
					{
						throw Exception("xmlError", skeletonType +
							"operation's parameter num mismatch");
					}

					
					ComputationOp * pOp;
					if(skt == SKType::map)
					{
						pOp = new MapOp(it->second);
					}else{
						pOp = new ReduceOp(it->second);
					}


					int indexListIndex = it->second.paraNameIndex("index");
					int numInput = it->second.getNumInput();
					if(indexListIndex != -1 )
					{
						if(numInput <= indexListIndex)
						{
							throw Exception("xmlError", "function " + it->second.getName() 
								+ " have parameter named index which is not input");
						}
						if(it->second.getInputType(indexListIndex) != IOTupleType("int"))
						{
							throw Exception("xmlError", "function " + it->second.getName() 
								+ " have parameter named index which is not int type");
						}
						pOp->assignIndexList(indexListIndex);
					}

					for(int i = 0; i < paraVector.size(); ++i)
					{
						data = paraVector[i];
						if(data.isExpr)
							pOp->setFuncPara(i, data.expr, data.depList);
						else
							pOp->setFuncPara(i, data.value);
					}

					pNode = new OpNode(pOp, upperLayer);

					if(lengthData.isExpr)
					{
						pNode->setPara("length", lengthData.expr, lengthData.depList);
					}else{
						pNode->setPara("length", lengthData.value);
					}
				}

				break;
			case SKType::gather:
			case SKType::scatter:
				{
					ParaData lengthData;
					ParaData tupleSizeData;
					ParaData rangeData;
					std::vector<ParaData> offsetVector;
					std::vector<std::string> typeVector;
					const tinyxml2::XMLElement * offsetEle; 
					const tinyxml2::XMLElement * typeEle; 
					int numBaseType = 0;

					childEle = ele->FirstChildElement("length");
					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType+ " operation does not specify length");
					}
					
					lengthData = loadParaData(childEle, "length");

					childEle = ele->FirstChildElement("tuple_size");
					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType
							+ "operation does not specify tuple_size");
					}

					tupleSizeData = loadParaData(childEle, "tuple_size");

					offsetEle = ele->FirstChildElement("offset");

					while(offsetEle != NULL)
					{
						data = loadParaData(offsetEle, std::string("offset") );
						offsetVector.push_back(data);
						offsetEle = offsetEle->NextSiblingElement("offset");
					}

					childEle = ele->FirstChildElement("basetype");
					if(childEle != NULL)
					{
						ss.str("");
						ss.clear();
						numBaseType = 0;
						ss<<"type"<<numBaseType;
						typeEle = childEle->FirstChildElement(ss.str().c_str());

						while(typeEle != NULL)
						{
							std::string basetypeName;
							basetypeName = typeEle->GetText();
							typeVector.push_back(basetypeName);
							++numBaseType;
							ss.str("");
							ss.clear();
							ss<<"type"<<numBaseType;
							typeEle = childEle->FirstChildElement(ss.str().c_str());
						}
					}

					std::string  rangeEleName;
					
					if(skt == SKType::gather)
					{
						rangeEleName = "input_range";
					}else{
						rangeEleName = "output_range";
					}
					childEle = ele->FirstChildElement(rangeEleName.c_str());

					if(childEle == NULL)
					{
						throw Exception("xmlError", skeletonType
							+ std::string("operation does not specify ") 
							+ rangeEleName);
					}


					rangeData = loadParaData(childEle, rangeEleName);

					ArrangementOp * pOp;
					if(skt == SKType::gather)
					{
						pOp = new GatherOp();
					}else{
						pOp = new ScatterOp();
					}

					for(int i = 0; i < offsetVector.size(); ++i)
					{
						data = offsetVector[i];
						if(data.isExpr)
							pOp->appendOffset(data.expr, data.depList);
						else
							pOp->appendOffset(data.value);
					}

					for(int i = 0; i < typeVector.size(); ++i)
					{
						std::string basetypeName;
						basetypeName = typeVector[i];
						IOTupleType tempType(basetypeName);
						CLType basetype = tempType.getBaseType();
						int numEle = tempType.getEleNum();
						if(basetype == CLType::VOID || numEle != 1)
							throw Exception("xmlError", std::string("invalid base type for ") + skeletonType +" operation.");
						pOp->appendBaseType(basetype);
					}

					if(rangeData.isExpr)
					{
						pOp->setRange(rangeData.expr, rangeData.depList);
					}else
					{
						pOp->setRange(rangeData.value);
					}

					if(tupleSizeData.isExpr)
					{
						throw Exception("xmlError", std::string("tuple size for ")+ skeletonType + std::string(" operation can only be a fixed value.") );
						/*
						pOp->setTupleLength(tupleSizeData.expr, tupleSizeData.depList);
						pNode = new OpNode(pOp, upperLayer);
						pNode->setPara("localLength", tupleSizeData.expr, tupleSizeData.depList);
						*/
					}
					else	
					{
						pOp->setTupleLength(tupleSizeData.value);
						pNode = new OpNode(pOp, upperLayer);
					}

					if(rangeData.isExpr)
					{
						pNode->setPara(rangeEleName, rangeData.expr, rangeData.depList);
					}else{
						pNode->setPara(rangeEleName, rangeData.value);
					}

					if(lengthData.isExpr)
					{
						pNode->setPara("length", lengthData.expr, lengthData.depList);
					}else{
						pNode->setPara("length", lengthData.value);
					}

				}

				break;
		}

		pNode->validateAllPara();
		return pNode;
	}

	Node * XmlParser::loadIterationNode(const tinyxml2::XMLElement * ele, Node * upperLayer) const
	{
		ParaData data;
		std::string itName;
		Node * pNodeHead = NULL;
		Node * pNodeChild = NULL;
		Node * pNodeLast = NULL;
		const tinyxml2::XMLElement * childEle; 
		childEle = ele->FirstChildElement("num_of_stages");
		if(childEle == NULL)
		{
			throw Exception("xmlError", "no num_of_stages inside stage_generate");
		}
		data = loadParaData(childEle, "num_of_stages");

		childEle = ele->FirstChildElement("iterator_name");
		if(childEle == NULL)
			throw Exception("xmlError", "no iterator_name inside stage_generate");
		else{
			itName = childEle->GetText();
		}

		if(data.isExpr)
		{
			pNodeHead = new IterationNode(itName, data.expr, data.depList, upperLayer);
		}else{
			pNodeHead = new IterationNode(itName, data.value, upperLayer);
		}
		
		loadMultipleParameter(ele, pNodeHead);
		pNodeHead->validateAllPara();
		
		pNodeChild = loadChildOperationList(ele, pNodeHead);

		pNodeHead->setBodyNextNode(pNodeChild); 
		pNodeLast = getNodeListLast(pNodeChild);
		pNodeLast->setDirectNextNode(pNodeHead);
		return pNodeHead;
	}

	void XmlParser::loadMultipleParameter(const tinyxml2::XMLElement * root,
			Node * node) const
	{
		const tinyxml2::XMLElement * ele; 
		ele = root->FirstChildElement("shared_variable");
		while(ele != NULL)
		{
			loadParameter(ele, node);
			ele = ele->NextSiblingElement("shared_variable");
		}
	}

	XmlParser::ParaData XmlParser::loadParaData(const tinyxml2::XMLElement * ele, std::string name) const
	{
		ParaData data;
		data.isExpr = false;
		data.value = 0;
		std::regex valueReg("^[ \t\r\n]*(value)[ \t\r\n]*$");
		std::regex exprReg("^[ \t\r\n]*(expr)[ \t\r\n]*$");

		const tinyxml2::XMLAttribute * pAttr;
		pAttr = ele->FindAttribute("value_type");
		if(pAttr == NULL)
		{
			tinyxml2::XMLError err;
			err = ele->QueryIntText(&(data.value));
			if(err != tinyxml2::XML_SUCCESS)
			{
				throw Exception("xmlError", std::string("parameter ") + name + "data invalid");
			}
			data.isExpr = false;
		}else{
			std::string type;
			type = pAttr->Value();

			if(regex_match(type, valueReg))
			{
				tinyxml2::XMLError err;
				err = ele->QueryIntText(&(data.value));
				if(err != tinyxml2::XML_SUCCESS)
				{
					throw Exception("xmlError", std::string("parameter ") + name + "data invalid. Parameter value should be integer");
				}
				data.isExpr = false;
			}else if(regex_match(type, exprReg))
			{
				data.expr = ele->GetText();
				data.isExpr = true;
				pAttr = ele->FindAttribute("para_list");
				if(pAttr == NULL)
				{
					throw Exception("xmlError", std::string("parameter ") + name + " is expression, but it does not specify dependency list.");
				}
				data.depList = pAttr->Value();
			}else{
				throw Exception("xmlError", std::string("parameter ") + name + " value_type invalid");
			}
		}
		return data;
	}

	void XmlParser::loadParameter(const tinyxml2::XMLElement * ele,
			Node * node) const
	{
		std::string name;
		ParaData data;

		const tinyxml2::XMLAttribute * pAttr = ele->FindAttribute("name");
		if(pAttr == NULL)
		{
			throw Exception("xmlError", "unknown parameter incomplete");
		}else{
			name = pAttr->Value();
		}

		data = loadParaData(ele, name);
		if(data.isExpr)
		{
			node->setPara(name, data.expr, data.depList);
		}else{
			node->setPara(name, data.value);
		}
	}

	void XmlParser::getPort(const tinyxml2::XMLElement * ele, std::string &name, bool & dirIn, CLType &type, int & length) const
	{
		std::regex inreg("^[ \t\r\n]*((in)|(IN))[ \t\r\n]*$");
		std::regex outreg("^[ \t\r\n]*((out)|(out))[ \t\r\n]*$");

		//std::string name;
		std::string direction;
		//bool dirIn;
		std::string typeName;
		//CLType type
		
		//int length;
		
		const tinyxml2::XMLAttribute * pAttr = ele->FindAttribute("name");
		if(pAttr == NULL)
		{
			throw Exception("xmlError", "unknown port incomplete");
		}else{
			name = pAttr->Value();
		}
		pAttr = ele->FindAttribute("direction");

		if(pAttr == NULL)
		{
			throw Exception("xmlError", std::string("port ") + name +" misses direction attribute");
		}else{
			direction = pAttr->Value();
			if(regex_match(direction, inreg))
			{
				dirIn = true;
			}else if(regex_match(direction, outreg))
			{
				dirIn = false;
			}else{
				throw Exception("xmlError", std::string("port ") + name + " direction invalid");
			}
		}

		pAttr = ele->FindAttribute("type");

		if(pAttr == NULL)
		{
			throw Exception("xmlError", std::string("port ") + name +" misses type attribute");
		}else{
			typeName = pAttr->Value();
			IOTupleType tempType(typeName);
			if(tempType.getEleNum() == 1)
			{
				type = tempType.getBaseType();
			}else{
				throw Exception("xmlError", std::string("port ") + name + " type invalid");
			}
		}

		pAttr = ele->FindAttribute("length");

		if(pAttr == NULL)
		{
			throw Exception("xmlError", std::string("port ") + name +" misses type attribute");
		}else{
			tinyxml2::XMLError err;
			err = pAttr->QueryIntValue(&length);
			if(tinyxml2::XML_SUCCESS != err)
				throw Exception("xmlError", std::string("port ") + name + "length invalid");
		}
	}

	CFunction XmlParser::extractCFunction(const tinyxml2::XMLNode * node) const
	{
		std::string functionDecl;
		std::istringstream iss;
		int numInput;
		int numOutput;
		int numFuncPara;
		const tinyxml2::XMLElement* ele = node->FirstChildElement("decl");
		if(ele == NULL)
		{
			throw Exception("xmlError", "description of unknown function" 
			"does not specify decl");
		}
		functionDecl = ele->GetText();
		
		ele = node->FirstChildElement("num_input");
		if(NULL == ele)
		{
			throw Exception("xmlError", std::string("description of \"") + functionDecl
			+ "\" does not specify num_input");
		}
		iss.clear();
		iss.str(ele->GetText());
		iss>>numInput;

		ele = node->FirstChildElement("num_output");
		if(NULL == ele)
		{
			throw Exception("xmlError", std::string("description of \"") + functionDecl
			+ "\" does not specify num_output");
		}

		iss.clear();
		iss.str(ele->GetText());
		iss>>numOutput;

		ele = node->FirstChildElement("num_para");
		if(NULL == ele)
		{
			throw Exception("xmlError", std::string("description of \"") + functionDecl
			+ "\" does not specify num_para");
		}
		iss.clear();
		iss.str(ele->GetText());
		iss>>numFuncPara;
		
		return CFunction(functionDecl, numInput, numOutput, numFuncPara);
	}
}
