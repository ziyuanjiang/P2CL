#ifndef _P2CL_MAPOP_HPP_
#define _P2CL_MAPOP_HPP_

#include "computationop.hpp"

namespace p2cl{

	/*!
		@brief class for the operation created by map parallel pattern
	*/
	class MapOp:public ComputationOp
	{
		public:
			/*!
				@brief constructor for MapOp
				@param rfunc function that used to created map parallel pattern
				@param id 
			*/
			MapOp(const CFunction & rfunc, int id =0);

			virtual MapOp * clone() const;

			/*!
				@brief deconstructor for MapOp
			*/
			virtual ~MapOp(){}

			/*!
				@brief get the type of i-th input
			*/
			virtual IOTupleType inputType(int i) const override;

			/*!
				@brief get the type of i-th output
			*/
			virtual IOTupleType outputType(int i) const override;

			virtual int numInput() const override;

			virtual int numOutput() const override;

			virtual std::string partialSrcComputation(std::string indexName, std::vector<std::string> inputVarList, std::vector<std::string> outputVarList) const override;
		protected:
		private:
	};
}

#endif
