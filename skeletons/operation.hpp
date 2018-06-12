#ifndef _P2CL_OPERATION_HPP_
#define _P2CL_OPERATION_HPP_
#include <string>
#include <vector>
#include "ctype.hpp"

namespace p2cl
{
	enum SKType {map, reduce, gather, scatter, transpose};
	class CFunction;
	
	/*!
		@class[Operation]
		@brief Base class for various operation classes.
		All of the inherited classes are used to represent operation created by parallel pattern and to generate kernel code.
	*/
	class Operation
	{
		public:
			/*!
				@brief constructor for operation
					The Id is used in kernel name.
				@param type 
					type of operation
				@param id
					It can be set later by setId(int id)
			*/		
			Operation(SKType type, int id = 0);

			/*!
				@brief destructor for operation
				Since Operation class has several child class, 
				it is set to virtual.
			*/	
			virtual ~Operation(){}

			/*!
				@brief get the value of getSKType
			*/		
			SKType getSKType() const
			{
				return opType;
			}

			/*!
				@brief get the name of the constructed kernel
			*/		
			std::string getKernelName() const
			{
				return kernelIdStr;
			}

			/*!
				@brief set id for operation
				@param type 
					type of operation
				@param id
					The Id that used in kernel name
			*/		
			void setId(int id);

			/*!
				@brief basic operation can be extended and apply mapped operation
			*/		
			void enableExtension(){extensionEnable = true;}

			/*!
				@brief disable kernel to perform multiple mapped operation
				on extended input output
			*/		
			void disableExtension(){extensionEnable = false;}

			/*!
				@brief interface for copying Operation on the heap
			*/		
			virtual Operation * clone() const = 0;

			/*!
				@brief get kernel source code
			*/		
			std::string getKernelSrc() const;

			/*!
				@brief get kernel source code for tuning
					currently it is the same as getKernelSrc().
			*/		
			std::string getTuningKernelSrc() const;

			/*!
				@brief get type of the ith input 
				@param i
					index of the input
			*/		
			virtual IOTupleType inputType(int i) const = 0;

			/*!
				@brief get type of the ith output 
				@param i
					index of the output
			*/		
			virtual IOTupleType outputType(int i) const = 0;

			/*!
				@brief get num of input
			*/		
			virtual int numInput() const = 0;

			/*!
				@brief get num of output
			*/		
			virtual int numOutput() const = 0;

			/*!
				@brief 
				Get function parameter declaration
				It is not used in current version
			*/		
			virtual std::string getParaDecl(std::vector<int> para) const;

			/*!
				@brief interface for geting the input length expression
				It is used to calculate shifted length for mapped input. 
			*/		
			virtual std::string inputShiftLengthExpr() const;

			/*!
				@brief interface for geting the output length expression
				It is used to calculate shifted length for mapped output. 
			*/		
			virtual std::string outputShiftLengthExpr() const;

			/*!
				@brief get basetype name of the ith input
			*/		
			std::string inputBaseTypeName(int i) const;

			/*!
				@brief get basetype name of the ith output
			*/		
			std::string outputBaseTypeName(int i) const;

			/*!
				@brief get the name of the IOTupleType of the ith input
			*/		
			std::string inputTupleTypeName(int i) const;

			/*!
				@brief get the name of the IOTupleType of the ith output
			*/		
			std::string outputTupleTypeName(int i) const;

			/*!
				@brief get the num of platform parameter
				It is always zero in current version
			*/		
			virtual int numPlatPara() const
			{
				return 0;
			}

			/*!
				@brief get source code snippet for load single data in an IOTupleType according to the local id
				@param index index of data that need to be loaded
				@param numOfInput num of input of the kernel
			*/		
			virtual std::string partialSrcSingleLoad(std::string index, int numOfInput) const;

			/*!
				@brief get source code snippet for store single data in an IOTupleType according to the local id
				@param index index of location that the data need to be store into
				@param numOfOutput num of output of the kernel
			*/		
			virtual std::string partialSrcSingleStore(std::string index, int numOfOutput) const;

			/*!
				@brief get source code snippet for load
				@param index index of data that need to be loaded
				@param numOfInput num of intput of the kernel
			*/		
			virtual std::string partialSrcLoad(std::string index, int numOfInput) const;

			/*!
				@brief get source code snippet for store 
				@param index index of location that the data need to be store into
				@param numOfOuput num of output of the kernel
			*/		
			virtual std::string partialSrcStore(std::string index, int numOfOutput) const;

			/*!
				@brief get source code snippet for computation
				@param indexName index variable name used for some function that need to know its position
				@param inputVarList list of the input variable name
				@param outputVarList list of the output variable name
			*/		
			virtual std::string partialSrcComputation(std::string indexName, std::vector<std::string> inputVarList, std::vector<std::string> outputVarList) const;

			/*!
				@brief get source code snippet for reduce computation
				@param resultName name of the variable that store and collect reduce result
				@param inputName name of the variable that needs to be combined
			*/		
			virtual std::string partialSrcReduce(std::string resultName, std::string inputName) const;

			/*!
				@brief get source code snippet for load data for reduce
				@param varName name of the variable that is used to store loaded data
				@param offset offset expression for load
			*/		
			virtual std::string reduceLoad(std::string varName, std::string offset) const;

			/*!
				@brief get IOTupleType of the input variable
					This is different from inputType for FusedOperation
				@param i index of the input
			*/		
			virtual IOTupleType inputVarType(int i) const;

			/*!
				@brief get IOTupleType name of the input variable
					input variable type is different from input type for FusedOperation
				@param i index of the input
			*/		
			std::string inputVarTypeName(int i) const;

			/*!
				@brief get basetype name for IOTupleType of the input variable
					input variable type is different from input type for FusedOperation
				@param i index of the input
			*/		
			std::string inputVarBaseTypeName(int i) const;

			/*!
				@brief get IOTupleType name of the output variable
					output variable type is different from output type for FusedOperation
				@param i index of the output
			*/		
			virtual IOTupleType outputVarType(int i) const;

			/*!
				@brief get type name for IOTupleType of the output variable
					output variable type is different from output type for FusedOperation
				@param i index of the output
			*/		
			std::string outputVarTypeName(int i) const;

			/*!
				@brief get base type name for IOTupleType of the output variable
					output variable type is different from output type for FusedOperation
				@param i index of the output
			*/		
			std::string outputVarBaseTypeName(int i) const;

			/*!
				@brief get the addtional parameter names for the kernel source code
			*/		
			std::vector<std::string> getDependencyList() const;
		protected:

			/*!
				@brief get source code snippet for shifting input and output pointer
				It is used when the basic operation is extended.
			*/		
			std::string shiftInputOutput() const;

			/*!
				@brief create code for a while loop 
				@param expr condition expression of the while loop
				@param content content of the while loop
			*/
			std::string whileWrap(std::string expr, std::string content) const;

			/*!
				@brief create code for a if statement
				@param expr condition expression of the if statement
				@param content content of the if statement
			*/
			std::string ifWrap(std::string expr, std::string content) const;

			/*!
				@brief create code for a if-else statement
				@param expr condition expression of the if statement
				@param trueContent content of the if statement for true branch
				@param falseContent content of the if statement for false branch
			*/
			std::string ifElseWrap(std::string expr, std::string trueContent, std::string falseContent) const;

			/*!
				@brief size of bytes for IOTupleType of the ith input
				@param i index of the input
			*/
			int inputTupleSize(int i) const;

			/*!
				@brief size of bytes for IOTupleType of the ith output
				@param i index of the output
			*/
			int outputTupleSize(int i) const;

			/*!
			*/
			virtual std::string tuplePointerLoadOutofRange(std::string varName, int n) const;

			/*!
				@brief create code for load tupletype data from the input at the given index 
					to the location pointed by the given pointer
				@param varPtrName name of the pointer that point to the input variable 
				@param index index name 
				@param pName pointer name of the input
				@param n tuple size
			*/
			std::string tuplePointerLoad(std::string varPtrName, std::string index, std::string pName, int n) const;

			/*!
				@brief create code for store tupletype data from the location pointed by the variable pointer 
					to the output at the given index
				@param varPtrName name of the pointer that point to the output variable 
				@param index index name 
				@param pName pointer name of the output
				@param n tuple size
			*/
			std::string tuplePointerStore(std::string varPtrName, std::string index, std::string pName, int n) const;

			/*!
				@brief code for declaring local barrier
			*/
			std::string localBarrier() const;

			/*!
				@brief code for declaring global barrier
			*/
			std::string globalBarrier() const;

			/*!
				@brief temporary variable Name 
				temporary variable is used when the variable is not directly linked to kernel input or output
				@param i id of temporary variable
			*/
			std::string tempVarName(int i) const;

			/*!
				@brief input variable Name 
				The name should be consistent among different layers of the code.
				Use of this function in different layers guarantees consistency.
			*/
			std::string inputVarName(int i) const;

			/*!
				@brief input variable Pointer Name 
				The name should be consistent among different layers of the code.
				Use of this function in different layers guarantees consistency.
			*/
			std::string inputVarPointerName(int i) const;

			/*!
				@brief output variable Name 
				The name should be consistent among different layers of the code.
				Use of this function in different layers guarantees consistency.
			*/
			std::string outputVarName(int i) const;


			/*!
				@brief output variable Pointer Name 
				The name should be consistent among different layers of the code.
				Use of this function in different layers guarantees consistency.
			*/
			std::string outputVarPointerName(int i) const;

			/*!
				@brief platform parameter Name 
				It is not used in current version.
			*/
			std::string platParaName(int i) const;

			/*!
				@brief the ith input pointer Name 
			*/
			std::string inputName(int i) const;

			/*!
				@brief the ith output pointer Name 
			*/
			std::string outputName(int i) const;

			/*!
				@brief the ith localSpaceName
			*/
			std::string localSpaceName(int i) const;

			/*!
				@brief code that load groupid of the workitem to a variable
				@param varName variable name
				@param dim dimension
			*/
			std::string groupIdSt(const std::string &varName, int dim) const;

			/*!
				@brief code that load global id of the workitem to a variable
				@param varName variable name
				@param dim dimension
			*/
			std::string globalIdSt(const std::string &varName, int dim) const;

			/*!
				@brief code that load local id of the workitem to a variable
				@param varName variable name
				@param dim dimension
			*/
			std::string localIdSt(const std::string &varName, int dim) const;

			/*!
				@brief code that load local id of the workitem to a variable
				@param varName variable name
				@param dim dimension
			*/
			std::string workGroupSize(const std::string &varName, int dim) const;

			/*!
				@brief code that load local id of the workitem to a variable
				@param varName variable name
				@param dim dimension
			*/
			std::string globalSize(const std::string &varName, int dim) const;

			/*!
				@brief kernel declaration
				@param tuning switch for kernelDecl
				It is not used in current version.
			*/
			std::string kernelDecl(bool tuning) const;

			/*!
				@brief helper function that combine kernel declaration and kernelBody
				@param tuning switch for kernelDecl
				It is not used in current version.
			*/
			std::string kernelWrap(const std::string &kernelDecl, const std::string & kernelBody) const;

			/*!
				@brief function that generate a loop
				@param functionCall loop body
				@param initial initial index
				@param length length of the loop
			*/
			std::string loopWrap(const std::string & functionCall,int initial, int length) const;

			/*!
				@brief function that generate a loop
				@param functionCall loop body
				@param initial initial index
				@param length length of the loop
			*/
			std::string loopWrap(const std::string & functionCall,const std::string& initial, const std::string & length) const;

			/*!
				@brief function that generate a loop whose index is incremented by a stride
				@param functionCall loop body
				@param initial initial index
				@param length length of the loop
			*/
			std::string loopWrapStride(const std:: string &functionCall, const std::string& initial,const std::string& endIndex, const std::string& stride) const;

			/*!
				@brief indent the code
				@param srcBody source code body
			*/
			std::string indent(const std::string & srcBody) const;

			/*!
				@brief create a string where element in the given list 
				is separated by comma
				@param list
			*/
			std::string commaSeparatedList(std::vector<std::string> list) const;

			/*!
				@brief create a string where element in the given list 
				is separated by comma. 
				Addtional '&' character is appended to each variable name.
				@param list
			*/
			std::string commaSeparatedListOutput(std::vector<std::string> list) const;

			/*!
				@brief set skeleton type
				@param type skeleton type
			*/
			void setSKType(SKType type){
				opType = type;
			}

			/*!
				@brief append list of addtional parameters of the kernel function 
				@param list parameter name list
			*/
			void appendParaDependencies(std::vector<std::string> list);

			/*!
				@brief append list of addtional parameters of the kernel function
				@param list string where each element is separated by space
			*/
			void appendParaDependency(std::string para);

			/*!
				@brief reset parameters to the kernel function
			*/
			void resetDependencyList();

			/*!
				@brief load data from input to a variable whose name is generated by inputVarName(index)
			*/
			std::string completeLoadSrc(std::string index) const;

			/*!
				@brief load data from input to a variable whose name is generated by tempVarName(index)
			*/
			std::string completeLoad2TempSrc(std::string index) const;

			/*!
				@brief store data from to a variable whose name is generated by outputVarName(index)
			*/
			std::string completeStoreSrc(std::string index) const;

			/*!
				@brief transpose kernel body
				Since the tranpose parallel pattern cannot fuse with any other operations,
				the kernel source code is handled by its own class
			*/
			virtual std::string transposeSrc() const;
		private:

			/*!
				@brief get kernel body source code
			*/
			std::string kernelBodySrc() const;

			/*!
				@brief get source code for map parallel pattern
			*/
			std::string mapSrc() const;

			/*!
				@brief get source code for arrangement operation
			*/
			std::string arrangeSrc() const;

			/*!
				@brief get source code for reduce operation
			*/
			std::string reduceSrc() const;

			int rangeIsExpr;
			int range;

			int operationID;
			std::string kernelIdStr;
			SKType opType;
			std::vector<std::string> dependencyList;
			bool extensionEnable;
	};
}

#endif
