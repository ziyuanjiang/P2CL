#ifndef _P2CL_REDUCEOP_HPP_
#define _P2CL_REDUCEOP_HPP_

#include "computationop.hpp"

namespace p2cl{

	class ReduceOp:public ComputationOp
	{
		public:
			ReduceOp(const CFunction & rfunc, int id =0);

			virtual ReduceOp * clone() const;

			virtual ~ReduceOp(){}

			virtual IOTupleType inputType(int i) const override;
			virtual IOTupleType outputType(int i) const override;

			virtual int numInput() const override;

			virtual int numOutput() const override;

			virtual std::string partialSrcReduce(std::string resultName, std::string inputName) const;

			virtual std::string inputShiftLengthExpr() const override;

			virtual std::string outputShiftLengthExpr() const override;

		protected:
		private:
	};
}

#endif
