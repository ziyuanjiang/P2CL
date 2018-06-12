#include <string>
#include <sstream>
#include <limits.h>
#include "transposeop.hpp"
#include "ctype.hpp"
#include "frontendutils.hpp"

namespace p2cl{
	TransposeOp::TransposeOp(int id):Operation(SKType::transpose, id)
	{
		tupleLength = 1;
	}

	TransposeOp::TransposeOp(const TransposeOp  & obj):Operation(obj)
	{
		baseTypeList = obj.baseTypeList;
		tupleLength = obj.tupleLength;
	}

	TransposeOp* TransposeOp::clone() const
	{
		return new TransposeOp(*this);
	}

	void TransposeOp::appendBaseType(CLType type)
	{
		baseTypeList.push_back(type);
	}

	CLType TransposeOp::getBaseType(int i) const
	{
		int num  = baseTypeList.size();
		if(num ==0)
		{
			return CLType::FLOAT;
		}else if(i < num){
			return baseTypeList[i];
		}else{
			return CLType::VOID;
		}
	}

	void TransposeOp::clearBaseType()
	{
		baseTypeList.clear();
	}

	void TransposeOp::setTupleLength(int num)
	{
		this->tupleLength = num;
	}


	IOTupleType TransposeOp::inputType(int i) const
	{
		
		return IOTupleType(getBaseType(i), getTupleLength());
	}

	IOTupleType TransposeOp::outputType(int i) const
	{
		return IOTupleType(getBaseType(i), getTupleLength());
	}


	int TransposeOp::numInput() const
	{
		int num = baseTypeList.size();
		if(num == 0)
		{
			return 1;
		}else{
			return num;
		}
	}

	int TransposeOp::numOutput() const
	{
		int num = baseTypeList.size();
		if(num == 0)
		{
			return 1;
		}else{
			return num;
		}
	}

	std::string TransposeOp::inputShiftLengthExpr() const
	{
		std::stringstream ss;
		ss<<"(width * height)";
		return ss.str();
	}

	std::string TransposeOp::outputShiftLengthExpr() const
	{
		std::stringstream ss;
		ss<<"(width * height)";
		return ss.str();
	}


	std::string TransposeOp::transposeSrc() const
	{
		std::stringstream ss;
		std::stringstream ssTmp;
		ss<<groupIdSt("gp0", 0)<<std::endl;
		ss<<shiftInputOutput()<<std::endl;
		ss<<localIdSt("l0", 0)<<std::endl;

		if(tupleLength != 1)
		{
			ss<<"int tuple_index = l0 % "<<tupleLength<<';'<<std::endl;
			ss<<"l0 = l0 / "<<tupleLength<<';'<<std::endl;
		}
		ss<<"int block_size_x = ceil((float)width/ group_dimx);"<<std::endl;
		ss<<"int block_x = gp0 % (block_size_x);"<<std::endl;
		ss<<"int block_y = gp0 / (block_size_x);"<<std::endl;
		ss<<std::endl;
		ss<<"int local_x = l0 % group_dimx;"<<std::endl;
		ss<<"int local_y = l0 / group_dimx;"<<std::endl;
		ss<<std::endl;
		ss<<"int in_x = mad24(block_x, group_dimx, local_x);"<<std::endl;
		ss<<"int in_y = mad24(block_y, group_dimy, local_y);"<<std::endl;

		//ss<<"printf(\"%d\\t%d\\t%d\\t%d\\t%d\\t%d\\n\", width, height, group_dimx, group_dimy, in_x, in_y);"<<std::endl;

		ssTmp<<"int input_index = mad24(in_y, width, in_x);"<<std::endl;
		ssTmp<<std::endl;
		ssTmp<<"int local_input = mad24(local_y, group_dimx";
		if(tupleLength != 1)
		{
			ssTmp<<" * "<<tupleLength;
		}
		ssTmp<<" + 1, local_x";

		if(tupleLength != 1)
		{
			ssTmp<<" * "<<tupleLength;
		}
		ssTmp<<");"<<std::endl;

		ssTmp<<std::endl;
		if(tupleLength != 1)
		{
			ssTmp<<"local_input += tuple_index;"<<std::endl;
			ssTmp<<"input_index = input_index * "<<tupleLength
				<<" + tuple_index;"<<std::endl;
		}

		for(int i = 0; i < numInput(); ++i)
		{
			ssTmp<<localSpaceName(i)<<"[local_input] = "
			<<inputName(i)<<"[input_index];"<<std::endl;
		}
		//ss<<"printf(\"in:\\t%d\\t%d\\t%d\\t%d\\n\", get_global_id(0),block_x, block_y, l0);"<<std::endl;

		ss<<ifWrap("in_x < width && in_y < height", ssTmp.str())<<std::endl;
		ss<<"local_x = l0 % group_dimy;"<<std::endl;
		ss<<"local_y = l0 / group_dimy;"<<std::endl;
		ss<<std::endl;
		ss<<"int out_x = mad24(block_y, group_dimy, local_x);"<<std::endl;
		ss<<"int out_y = mad24(block_x, group_dimx, local_y);"<<std::endl;
		ss<<std::endl;
		ss<<localBarrier()<<std::endl;
		ssTmp.str("");
		ssTmp.clear();

		ssTmp<<"int output_index = mad24(out_y, height, out_x);"<<std::endl;

		ssTmp<<"int local_output = mad24(local_x, group_dimx";
		if(tupleLength != 1)
		{
			ssTmp<<" * "<<tupleLength;
		}
		ssTmp<<" + 1, local_y";
		if(tupleLength != 1)
		{
			ssTmp<<" * "<<tupleLength;
		}

		ssTmp<<");"<<std::endl;
		ssTmp<<std::endl;
		if(tupleLength != 1)
		{
			ssTmp<<"local_output += tuple_index;"<<std::endl;
			ssTmp<<"output_index = output_index * ";
			ssTmp<<tupleLength<<" + tuple_index;"<<std::endl;
		}

		//ssTmp<<"printf(\"out:\\t%d\\t%d\\t%d\\t%d\\n\", get_global_id(0),out_x, out_y, local_output);"<<std::endl;

		for(int i = 0; i < numInput(); ++i)
		{
			ssTmp<<outputName(i)<<"[output_index] = "
			<<localSpaceName(i)<<"[local_output];"<<std::endl;
		}
		ss<<ifWrap("out_x < height && out_y < width", ssTmp.str())<<std::endl;
		return ss.str();
	}
}
