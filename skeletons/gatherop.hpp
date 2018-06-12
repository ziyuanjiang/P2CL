#ifndef _P2CL_GATHER_HPP_
#define _P2CL_GATHER_HPP_

#include "arrangementop.hpp"

namespace p2cl{

	class GatherOp:public ArrangementOp
	{
		public:
			GatherOp(int id =0);
			virtual GatherOp * clone() const;
			virtual ~GatherOp(){}

			virtual IOTupleType inputType(int i) const;
			virtual IOTupleType outputType(int i) const;
			virtual std::string partialSrcLoad(std::string index, int numOfInput) const;
			virtual std::string partialSrcSingleLoad(std::string index, int numOfInput) const;
			virtual std::string partialSrcSingleStore(std::string index, int numOfOutput) const;
			virtual IOTupleType inputVarType(int i) const;

			virtual std::string inputShiftLengthExpr() const override;

		protected:
		private:
	};
}

#endif
