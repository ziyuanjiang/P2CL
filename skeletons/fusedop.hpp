#ifndef _P2CL_FUSEDOP_HPP_
#define _P2CL_FUSEDOP_HPP_

#include <vector>
#include "operation.hpp"
#include "mapop.hpp"
#include "reduceop.hpp"
#include "cfunction.hpp"

namespace p2cl{

	/*!
		@brief class for basic operation fusion
	*/
	class FusedOp:public Operation
	{
		public:
			/*!
				@brief constructor for FusedOp
			*/
			FusedOp(int id = 0);

			/*!
				@brief copy constructor for FusedOp
			*/
			FusedOp(const FusedOp& obj);

			/*!
				@brief deconstructor
			*/
			virtual ~FusedOp();

			virtual FusedOp* clone() const;

			/*!
				@brief assign operations that needs to be fused
			*/
			template <class InputIterator>
		        void assign(InputIterator first, InputIterator last)
			{
				clearOpsList();
				opsList.assign(first, last);
				update();
			}

			virtual IOTupleType inputType(int i) const;

			virtual IOTupleType outputType(int i) const;
			virtual int numInput() const;
			virtual int numOutput() const;

			virtual std::string partialSrcSingleLoad(std::string index, int numOfInput) const;
			virtual std::string partialSrcSingleStore(std::string index, int numOfOutput) const;
			virtual std::string partialSrcLoad(std::string index, int numOfInput) const;
			virtual std::string partialSrcStore(std::string index, int numOfOutput) const;
			virtual std::string partialSrcComputation(std::string indexName, std::vector<std::string> inputVarList, std::vector<std::string> outputVarList) const override;
			virtual std::string reduceLoad(std::string varName, std::string offset) const;

			std::string partialSrcReduce(std::string resultName, std::string inputName) const override;

			virtual std::string inputShiftLengthExpr() const override;

			virtual std::string outputShiftLengthExpr() const override;

		protected:
			virtual IOTupleType inputVarType(int i) const;
			virtual IOTupleType outputVarType(int i) const;
		private:
			/*!
				@brief analyze and update states of FusedOp after assigning operations
			*/
			void update();
			void clearOpsList();
			SKType lastOpType;
			std::vector<MapOp*> mapOpsList;
			std::vector<Operation*> opsList;
			ReduceOp * pReduceObj;
	};
}

#endif
