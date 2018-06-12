#ifndef _P2CL_PARACONTAINER_HPP_
#define _P2CL_PARACONTAINER_HPP_

#include <map>
#include "parameter.hpp"

namespace p2cl{

	/*!
		@brief container that manage many parameters
	*/
	class ParaContainer
	{
		public:
			/*!
				@brief constructor
			*/
			ParaContainer();

			/*!
				@brief destructor
			*/
			~ParaContainer();

			/*!
				@brief find a parameter by its name
			*/
			const Parameter * findPara(std::string name) const;

			/*!
				@brief copy a parameter object to the container
			*/
			void copyParameter(const Parameter * pObj);

			/*!
				@brief validate all of the parameters inside the container
			*/
			void connect();

			/*!
				@brief transfer all the parameter of another ParaContainer
				to the current container
			*/
			void transferFrom(ParaContainer & a);

			/*!
				@brief delete a parameter by its name
			*/
			void pop(std::string name);

			/*!
				@brief change a value of a parameter
				@param name name of the parameter
			*/
			void changeValue(std::string name, int value);

			/*!
				@brief create and append a new Parameter with a value
			*/
			void appendValue(std::string name, int value);

			/*!
				@brief create and append a new Parameter with a expression
			*/
			void appendExpr(std::string name, std::string expr, std::string dependencyList);

			/*!
				@brief evaluate an expression
			*/
			int evalExpr(std::string name, std::string dependencyList) const;

			/*!
				@brief evaluate the value of a parameter 
				@param name name of the paramter that needs to be evaluate
			*/
			int evalPara(std::string name) const;

			/*!
				@brief set another Parameter as its upperLayer
				All the parameters in the upperLayer can be accessed by the current container.
				If there are Parameters in both containers,
				the parameter in the current container have higher priority
			*/
			void setUpperLayer(const ParaContainer * parent);

			/*!
				@brief link all the dependencies of a Parameter to the container
			*/
			void connectSingle(Parameter *) const;
		private:

			const ParaContainer * upperLayer;

			std::map<std::string, Parameter *> paraList;

			std::map<std::string, Parameter *> valueList;

			std::map<std::string, Parameter *> exprList;
	};
}

#endif
