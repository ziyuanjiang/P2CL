#ifndef _F2C_XMLPARSER_HPP_
#define _F2C_XMLPARSER_HPP_

#include <string>
#include <map>
#include <vector>
#include "tinyxml2.h"
#include "cfunction.hpp"
#include "analyzer.hpp"

namespace p2cl{
	

	/*!
		class for parsing xml files and get Analyzer objects
	*/
	class XmlParser{
		public:
			typedef struct{
				int value;
				bool isExpr;
				std::string expr;
				std::string depList;
			}ParaData;

			/*!
				constructor
				@param fileName the file name of the xml file
			*/
			XmlParser(const std::string & fileName);

			/*!
				@brief get an Analyzer object
			*/
			Analyzer * getAnalyzer() const;

			/*!
				@brief print all the information of all functions
			*/
			void printFunction() const;

		protected:
		private:
			/*!
				load
			*/
			Node * loadOperationNode(const tinyxml2::XMLElement * ele, Node * upperLayer) const;
			Node * getNodeListLast(Node * node) const;
			void loadMultipleParameter(const tinyxml2::XMLElement * root,
			Node * node) const;

			void getPort(const tinyxml2::XMLElement * ele, std::string &name, bool & dirIn, CLType &type, int & length) const;

			CFunction extractCFunction(const tinyxml2::XMLNode * node) const;

			void loadParameter(const tinyxml2::XMLElement * ele,
				Node * node) const;

			ParaData loadParaData(const tinyxml2::XMLElement * ele, std::string name) const;

			Node * loadChildOperationList(const tinyxml2::XMLElement * root, Node * upperLayer = NULL) const;

			Node * loadIterationNode(const tinyxml2::XMLElement * ele, Node * upperLayer = NULL) const;

			Node * loadExtNode(const tinyxml2::XMLElement * ele, Node * upperLayer = NULL) const;

			tinyxml2::XMLDocument doc;		

			tinyxml2::XMLElement* functionDesRoot;

			tinyxml2::XMLElement* processRoot;
			
			std::map<std::string, CFunction> functionTable;
	};
}


#endif
