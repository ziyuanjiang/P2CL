#ifndef _P2CL_SCATTER_HPP_
#define _P2CL_SCATTER_HPP_

#include "arrangementop.hpp"
#include "ctype.hpp"

namespace p2cl{

	class ScatterOp:public ArrangementOp
	{
		public:
			ScatterOp(int id =0);
			virtual ScatterOp * clone() const;
			virtual ~ScatterOp(){}
			virtual IOTupleType inputType(int i) const;
			virtual IOTupleType outputType(int i) const;

			virtual std::string partialSrcStore(std::string index, int numOfOutput) const;
			virtual std::string partialSrcSingleStore(std::string index, int numOfOutput) const;
			virtual std::string partialSrcSingleLoad(std::string index, int numOfOutput) const;

			virtual IOTupleType inputVarType(int i) const;

			virtual std::string outputShiftLengthExpr() const override;
		protected:
		private:
	};
}

#endif
