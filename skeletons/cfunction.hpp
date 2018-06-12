#ifndef _P2CL_CFUNCTION_HPP_
#define _P2CL_CFUNCTION_HPP_

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "ctype.hpp"

namespace p2cl
{
	/*!
		@brief class for CFunctions
	*/
	class CFunction{
		public:
			/*!
				@brief empty constructor for CFunction
			*/
			CFunction();

			/*!
				@brief constructor for CFunction
				@param func the string of the whole function or just the function declaration
				@param nInput the number of inputs
				@param nOutput the number of outputs
				@param nFuncPara the number of function parameters
			*/
			CFunction(const std::string & func, int nInput, int nOutput, int nFuncPara);

			/*!
				@brief setFunction for function declaration
			*/
			void setFunction(const std::string & func, int nInput, int nOutput, int nFuncPara);
	
			/*!
				@brief get function name
			*/
			std::string getName() const;

			/*!
				@brief get input type of the i-th input
			*/
			IOTupleType getInputType(int index) const;

			/*!
				@brief get output type of the i-th output
			*/
			IOTupleType getOutputType(int index) const;

			/*!
				@brief get the number of input
			*/
			int getNumInput() const;

			/*!
				@brief get the number of output
			*/
			int getNumOutput() const;

			/*!
				@brief get the total number of input and output
			*/
			int getNumInputOutput() const;

			/*!
				@brief get the number of function parameter
			*/
			int getNumFuncPara() const;

			/*!
				@brief get the index of parameter
			*/
			int paraNameIndex(std::string paraName) const;
			
			/*!
				@brief template function that generate 
					function call expression with any numbers of arguments
			*/
			template<typename T, typename... Args>
			std::string getFunctionCall(T value, Args... args) const
			{
				std::stringstream ss;
				int numPara;
				std::string paraList = getParaList(numPara , value, args...);
				ss<<functionName<<'('<<paraList<<')'<<';';
				//std::cout<<"number of parameter:"<<numPara<<std::endl;
				return ss.str();
			}

		private:

			/*!
				@brief variadic template function that generate 
					parameter list expression with any numbers of parameters
			*/
			template<typename T, typename... Args>
			std::string getParaList(int & numPara, T value, Args... args) const
			{
				std::stringstream ss;
				std::string subList;
				subList = getParaList(numPara, args...);
				++numPara;
				if(subList.length() == 0)
				{
					ss<<value;
				}else
				{
					ss<<value<<", "<<subList;
				}
				return ss.str();
			}

			/*!
				@brief base case for the variadic template function std::string getParaList(int & numPara, T value, Args... args)
			*/
			std::string getParaList(int & numPara) const
			{
				return std::string("");
			}
	
			

			/*!
				Extract parameter type name, parameter names and function names of given function
				@param function
					function del
				@param functionName
					extracted function name
				@return 
					
			*/
			bool parameterExtract(const std::string & function, std::string & functionName, std::vector<std::string>& typeName,std::vector<std::string> & paraName) const;

			std::string functionName;
			std::string functionDecl;

			std::vector<IOTupleType> typeList;
			std::vector<std::string> paraNameList;

			int numInput;
			int numOutput;
			int numFuncPara;
			
			int totalNumPara;

	};
}

#endif
