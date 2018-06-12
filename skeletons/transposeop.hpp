#ifndef _P2CL_TRANSPOSEOP_HPP_
#define _P2CL_TRANSPOSEOP_HPP_

#include <vector>
#include "operation.hpp"
#include "cfunction.hpp"

namespace p2cl{

	class TransposeOp:public Operation
	{
		public:
			TransposeOp(int id = 0);

			TransposeOp(const TransposeOp & obj);

			virtual ~TransposeOp(){}

			virtual TransposeOp* clone() const;

			virtual IOTupleType inputType(int i) const;

			virtual IOTupleType outputType(int i) const;

			virtual int numInput() const;

			virtual int numOutput() const;

			virtual std::string inputShiftLengthExpr() const override;

			virtual std::string outputShiftLengthExpr() const override;

			void appendBaseType(CLType type);

			void clearBaseType();

			void setTupleLength(int num);

			int getTupleLength() const{return tupleLength;}

			CLType getBaseType(int i) const;

			virtual std::string transposeSrc() const override;
		protected:
		private:
			std::vector<CLType> baseTypeList;
			int tupleLength;
	};
}

#endif
