#ifndef _P2CL_COMPUTATION_HPP_
#define _P2CL_COMPUTATION_HPP_

#include <vector>
#include "operation.hpp"
#include "cfunction.hpp"

namespace p2cl{

	/*!
		@brief base class for map and reduce operation
		this class provide APIs to set infomation for map and reduce
	*/
	class ComputationOp:public Operation
	{
		public:
			/*!
				@brief constructor for ComputationOp
				@param type parallel pattern type 
				@param func function used to create computation using parallel pattern
				@param id unique id that used for kernel operation
					It can be set later by setId(int id)
			*/
			ComputationOp(SKType type, const CFunction & func, int id = 0);

			/*!
				@brief deconstructor for ComputationOp
			*/
			virtual ~ComputationOp(){}

			/*!
				@brief set function parameter as value
			*/
			void setFuncPara(std::string paraName, int value);

			/*!
				@brief set function parameter as expression
			*/
			void setFuncPara(std::string paraName, std::string expr, std::string dependencies);

			/*!
				@brief set function parameter as expression
				@param paraName name of the parameter
			*/
			void setFuncPara(std::string paraName, 
				std::string expr,
				std::vector<std::string> dependencies);

			/*!
				@brief set function parameter as value
				@param i index
				@param value value to be set
			*/
			void setFuncPara(int i, int value);

			/*!
				@brief set function parameter as value
				@param i index
				@param expr expression string
				@param dependencies parameter string that separated by space
			*/
			void setFuncPara(int i, std::string expr, std::string dependencies);

			/*!
				@brief set function parameter as value
				@param i index
				@param expr expression string
				@param dependencies string list 
			*/
			void setFuncPara(int i, 
				std::string expr,
				std::vector<std::string> dependencies);

			/*!
				@brief check if function parameters are all set
			*/
			bool checkFuncPara() const;
			
			//virtual IOTupleType inputType(int i) const;
			//virtual IOTupleType outputType(int i) const;

			/*!
				@brief set index to the i-th input
			*/
			virtual void assignIndexList(int i) ;

			/*!
				@brief set index to the input whose name is paraName 
			*/
			virtual void assignIndexList(std::string paraName) ;
			//virtual int numInput() const;
			//virtual int numOutput() const;
			//virtual std::string partialSrcComputation(std::string paraList) const;
		protected:
			int actualNumFuncPara;
			CFunction function;
			std::vector<int> funcParaValList;
			std::vector<std::string> funcParaExprList;
			std::vector<bool> funcParaIsExpr;
			std::vector<bool> funcParaIsAssigned;

			int numAssignedPara;

			bool indexListAvailable;
			int indexListInput;
		private:
	};
}

#endif
