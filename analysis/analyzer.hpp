#ifndef _P2CL_ANALYZER_HPP_
#define _P2CL_ANALYZER_HPP_

#include <string>
#include "node.hpp"
#include "ctype.hpp"

namespace p2cl{
	
	class Executant;

	typedef struct{
		std::string name;
		bool dirIn;
		CLType type;
		int length;
	}Port;

	/*!
		@brief Analyzer class 
	*/
	class Analyzer{
		public:

			/*!
				@brief constructor for Analyzer
			*/
			Analyzer();
	
			/*!
				@brief get a Executant object 
			*/
			Executant* getExecutant();

			//Analyzer(const Analyzer & obj);
			
			/*!
				@brief set node
			*/
			void setNodeList(Node *);

			/*!
				@brief set addtional function definition
			*/
			void setFunctionDefinition(const std::string & src);

			/*!
				@brief append ports of the process
			*/
			void appendPort(std::string name, bool dirIn, CLType type, int length);
			
			/*!
				@brief get list of ports
			*/
			std::vector<Port> getPortList()
			{
				return portVector;
			}

			/*!
				@brief get the entire kernel source for tuning
			*/
			std::string getTuningKernelSrc() const;

			/*!
				@brief get the entire kernel source
			*/
			std::string getKernelSrc() const;
	
			/*!
				@brief print out ports
			*/
			void printPort();

			/*!
				@brief print list sizes
			*/
			void printListSize();

			/*!
				@brief destructor
			*/
			~Analyzer();
		private:
			int numKernel;

			std::string functionDef;

			std::string typeDecl;

			std::string kernelSrc;

			std::string tuningkernelSrc;

			std::vector<size_t> bufferSizeList;
			
			/*!
				@brief update id for the node list
			*/
			void updateId();

			/*!
				@brief update fuse list
			*/
			void updatefuseList();
	
			/*!
				@brief get type declarations
			*/
			std::string getAllTypeDecl()const;

			/*!
				@brief get kernel sources for all operation 
				excluding type declarations
			*/
			std::string getAllKernelSrc(bool tuning)const;

			/*!
				@brief get Max buffer size list
				
			*/
			std::vector<size_t> getMaxBufferSizes() const;
	
			std::vector<Port> portVector; ///list to store port information

			/*!
				@brief delete node list
			*/
			void deleteNodeList();


			Node * listHead;
	};
}

#endif
