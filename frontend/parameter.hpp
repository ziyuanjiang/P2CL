#ifndef _P2CL_PARA_HPP_
#define _P2CL_PARA_HPP_

#include <string>
#include <vector>
#include "tinyexpr.h"
#include "frontendutils.hpp"

namespace p2cl
{
	class Parameter
	{
		public:
			
			/*!
				@brief constructor.
				
				create parameter by expression
				
			*/
			Parameter(std::string name,std::string expr, std::string dependencyList);

			/*!
				@brief constructor.
				
				create parameter by value
			*/
			Parameter(std::string name,int value);

			/*!
				@brief copy constructor
			*/
			Parameter(const Parameter & obj);

			/*!
				@brief interface for copying Operation on the heap
			*/		
			Parameter * clone() const
			{
				return new Parameter(*this);
			}

			~Parameter();

			/*!
				@brief check if the parameter is an expression
			*/
			bool isExpr() const;

			/*!
				@brief check if two parameter are equal
			*/
			bool equalTo(const Parameter &b) const;

			/*!
				@brief get the i-th parameter that the current 
				parameter is depend on
			*/
			std::string dependencyParaName(int i) const;

			/*!
				@brief get the total number of dependencies
			*/
			int dependencyNum() const;

			/*!
				@brief get the name of the parameter
			*/
			std::string paraName() const;

			/*!
				@brief connect the dependencies of the parameter to a ParaContainer
			*/
			void connectParent(const Parameter * parent);

			/*!
				@brief get the value of the parameter
			*/
			int value() const;
	
			/*!
				@brief set value to the parameter
			*/
			void setValue(int v);

		private:

			std::vector<const Parameter *> parentList;

			std::vector<std::string> dependList;

			std::string name;

			std::string expr;

			bool is_expr;
			int pri_value;
			te_variable *teVars;
			double * dependVal;
			te_expr* teExpr;
	};
}

#endif
