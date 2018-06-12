#ifndef _P2CL_ARRANGEMENTOP_HPP_
#define _P2CL_ARRANGEMENTOP_HPP_

#include <vector>
#include "ctype.hpp"
#include "operation.hpp"
#include "cfunction.hpp"

namespace p2cl{

	/*!
		@brief base class for gather and scatter operation
		this class provide APIs to set infomation for gather and scatter
	*/
	class ArrangementOp:public Operation
	{
		public:
			/*!
				@brief constructor for ArrangementOp
			*/
			ArrangementOp(SKType type, int id = 0);

			/*!
				@brief destructor for ArrangementOp
			*/
			virtual ~ArrangementOp(){}
			
			/*!
				@brief append base type 
			*/
			void appendBaseType(CLType type);

			/*!
				@brief clear base type
			*/
			void clearBaseType();

			/*!
				@brief set tupleLength
			*/
			void setTupleLength(int num);

			/*!
				@brief set range as value 
				@param value range value
			*/
			void setRange(int num);

			/*!
				@brief append offset as expression
				@param expr expression
				@param dependencies parameters of expression
			*/
			void setRange(std::string expr, std::vector<std::string> dependencies);

			/*!
				@brief set range as expr
				@param expr expression
				@param depList parameters of expression

			*/
			void setRange(std::string expr, std::string dependencies);

			/*!
				@brief append offset as value 
				@param value offset value
			*/
			ArrangementOp& appendOffset(int value);

			/*!
				@brief append offset as expression
				@param expr expression
				@param depList parameters of expression
			*/
			ArrangementOp& appendOffset(std::string expr, std::string depList);

			/*!
				@brief append offset as expression
				@param expr expression
				@param dependencies parameters of expression
			*/
			ArrangementOp& appendOffset(std::string expr, std::vector<std::string> dependencies);
			/*!
				@brief clear offsets
			*/
			void clearOffsets();

			/*!
				@brief number of input 
			*/
			virtual int numInput() const;

			/*!
				@brief number of output
			*/
			virtual int numOutput() const;

			/*!
				@brief get number of offset
			*/
			int getOffsetNum() const{return offsetNum;}

			/*!
				@brief get tuple length
			*/
			int getTupleLength() const{return tupleLength;}

		protected:
			/*!
				@brief get base type
			*/
			CLType getBaseType(int i) const;

			std::vector<int> offsetValList;
			std::vector<bool> offsetIsExpr;
			std::vector<std::string> offsetExprList;
			int tupleLength;

			int range;
			bool rangeIsExpr;
			std::string rangeExpr;
		private:
			std::vector<CLType> baseTypeList;
			int offsetNum;
	};
}

#endif
