#ifndef _P2CL_CTYPE_H_
#define _P2CL_CTYPE_H_
#include <string>

namespace p2cl
{
	enum CLType{
		CHAR,
		UNSIGNED_CHAR,
		SHORT,
		UNSIGNED_SHORT,
		INT,
		UNSIGNED_INT,
		LONG,
		UNSIGNED_LONG,
		FLOAT,
		VOID,
	};
	
	std::string clTypeName(CLType type);

	int clTypeSize(CLType type);

	/*!
		@brief class for tuple type
	*/
	class IOTupleType
	{
		public:
			/*!
				@brief get the declaration of the tuple type struct
			*/
			std::string getStructDecl()const;

			/*!
				@brief get load and store macros
			*/
			static std::string getMacroDecl(int num);

			/*!
				@brief check if the types are the same
			*/
			bool operator ==(const IOTupleType &b) const;

			/*!
				@brief check if the types are different
			*/
			bool operator !=(const IOTupleType &b) const;

			/*!
				@brief constructor that takes the name of the tuple
				@param type type name
			*/
			IOTupleType(std::string type);

			/*!
				@brief constructor that takes baseType and number of tuples 
				@param baseType base type of the tuple
				@param num number of element in the tuple
			*/
			IOTupleType(CLType baseType, int num);

			/*!
				@brief get base type of tuple type
			*/
			CLType getBaseType() const;

			/*!
				@brief get number of element
			*/
			int getEleNum() const;

			/*!
				@brief get tuple type name
			*/
			std::string getIOTupleTypeName() const;

			/*!
				@brief get the size in byte of tuple type
			*/
			int size() const;
		private:
			std::string clTypeNameCap(CLType type) const;

			static std::string vstoreStr(int num);
			static std::string vloadStr(int num);
			static std::string macroVLoad(int start, int num, int totalNum);
			static std::string macroVStore(int start, int num, int totalNum);
			static std::string macroGeneralLoad(int start, int num, int totalNum);
			static std::string macroGeneralStore(int start, int num, int totalNum);


			CLType typeFromString(std::string typeName, int & num) const;
			CLType baseType;

			int eleNum;
	};
}
#endif
