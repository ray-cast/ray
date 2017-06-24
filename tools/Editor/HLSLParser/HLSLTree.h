// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_HLSL_TREE_H_
#define _H_HLSL_TREE_H_

#include "HLSLTypes.h"

struct HLSLNode;
struct HLSLRoot;
struct HLSLStatement;
struct HLSLDeclaration;
struct HLSLStruct;
struct HLSLStructField;
struct HLSLBuffer;
struct HLSLBufferField;
struct HLSLFunction;
struct HLSLArgument;
struct HLSLExpressionStatement;
struct HLSLExpression;
struct HLSLBinaryExpression;
struct HLSLLiteralExpression;
struct HLSLIdentifierExpression;
struct HLSLConstructorExpression;
struct HLSLFunctionCall;
struct HLSLArrayAccess;
struct HLSLArgument;

struct HLSLType
{
	HLSLType(HLSLBaseType _baseType = HLSLBaseType_Unknown)
	{
		baseType = _baseType;
		typeName = NULL;
		array = false;
		arraySize = NULL;
		constant = false;
	}

	HLSLBaseType        baseType;
	const char*         typeName;       // For user defined types.
	bool                array;
	HLSLExpression*     arraySize;
	bool                constant;
};

/** Base class for all nodes in the HLSL AST */
struct HLSLNode
{
	HLSLNodeType        nodeType;
	const char*         fileName;
	int                 line;

	virtual ~HLSLNode()
	{
	}
};

struct HLSLRoot : public HLSLNode
{
	static const HLSLNodeType s_type = HLSLNodeType_Root;
	HLSLRoot() { statement = NULL; }
	HLSLStatement*      statement;          // First statement.

	virtual ~HLSLRoot()
	{
	}
};

struct HLSLStatement : public HLSLNode
{
	HLSLStatement() { nextStatement = NULL; }
	HLSLStatement*      nextStatement;      // next statement in the block.

	virtual ~HLSLStatement()
	{
	}
};

struct HLSLDeclaration : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_Declaration;
	HLSLDeclaration()
	{
		name = NULL;
		nextDeclaration = NULL;
		assignment = NULL;
		registerName = NULL;
	}
	const char*         name;
	HLSLType            type;
	const char*         registerName;
	HLSLDeclaration*    nextDeclaration;    // If multiple variables declared on a line.
	HLSLExpression*     assignment;
};

struct HLSLStruct : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_Struct;
	HLSLStruct()
	{
		name = NULL;
		field = NULL;
	}
	const char*         name;
	HLSLStructField*    field;              // First field in the structure.
};

struct HLSLStructField : public HLSLNode
{
	static const HLSLNodeType s_type = HLSLNodeType_StructField;
	HLSLStructField()
	{
		name = NULL;
		semantic = NULL;
		nextField = NULL;
	}
	const char*         name;
	HLSLType            type;
	const char*         semantic;
	HLSLStructField*    nextField;      // next field in the structure.
};

/** A cbuffer or tbuffer declaration. */
struct HLSLBuffer : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_Buffer;
	HLSLBuffer()
	{
		name = NULL;
		registerName = NULL;
		field = NULL;
	}
	const char*         name;
	const char*         registerName;
	HLSLBufferField*    field;
};

/** Field declaration inside of a cbuffer or tbuffer */
struct HLSLBufferField : public HLSLNode
{
	static const HLSLNodeType s_type = HLSLNodeType_BufferField;
	HLSLBufferField()
	{
		name = NULL;
		nextField = NULL;
	}
	const char*         name;
	HLSLType            type;
	HLSLBufferField*    nextField;      // next field in the cbuffer.
};

/** Function declaration */
struct HLSLFunction : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_Function;
	HLSLFunction()
	{
		name = NULL;
		semantic = NULL;
		statement = NULL;
		argument = NULL;
		numArguments = 0;
	}
	const char*         name;
	HLSLType            returnType;
	const char*         semantic;
	int                 numArguments;
	HLSLArgument*       argument;
	HLSLStatement*      statement;
};

/** Declaration of an argument to a function. */
struct HLSLArgument : public HLSLNode
{
	static const HLSLNodeType s_type = HLSLNodeType_Argument;
	HLSLArgument()
	{
		name = NULL;
		modifier = HLSLArgumentModifier_None;
		semantic = NULL;
		nextArgument = NULL;
	}
	const char*             name;
	HLSLArgumentModifier    modifier;
	HLSLType                type;
	const char*             semantic;
	HLSLArgument*           nextArgument;
};

/** A expression which forms a complete statement. */
struct HLSLExpressionStatement : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_ExpressionStatement;
	HLSLExpressionStatement()
	{
		expression = NULL;
	}
	HLSLExpression*     expression;
};

struct HLSLReturnStatement : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_ReturnStatement;
	HLSLReturnStatement()
	{
		expression = NULL;
	}
	HLSLExpression*     expression;
};

struct HLSLDiscardStatement : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_DiscardStatement;
};

struct HLSLBreakStatement : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_BreakStatement;
};

struct HLSLContinueStatement : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_ContinueStatement;
};

struct HLSLIfStatement : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_IfStatement;
	HLSLIfStatement()
	{
		condition = NULL;
		statement = NULL;
		elseStatement = NULL;
	}
	HLSLExpression*     condition;
	HLSLStatement*      statement;
	HLSLStatement*      elseStatement;
};

struct HLSLForStatement : public HLSLStatement
{
	static const HLSLNodeType s_type = HLSLNodeType_ForStatement;
	HLSLForStatement()
	{
		initialization = NULL;
		condition = NULL;
		increment = NULL;
		statement = NULL;
	}
	HLSLDeclaration*    initialization;
	HLSLExpression*     condition;
	HLSLExpression*     increment;
	HLSLStatement*      statement;
};

/** Base type for all types of expressions. */
struct HLSLExpression : public HLSLNode
{
	static const HLSLNodeType s_type = HLSLNodeType_Expression;
	HLSLExpression()
	{
		nextExpression = NULL;
	}
	HLSLType            expressionType;
	HLSLExpression*     nextExpression; // Used when the expression is part of a list, like in a function call.
};

struct HLSLUnaryExpression : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_UnaryExpression;
	HLSLUnaryExpression()
	{
		expression = NULL;
	}
	HLSLUnaryOp         unaryOp;
	HLSLExpression*     expression;
};

struct HLSLBinaryExpression : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_BinaryExpression;
	HLSLBinaryExpression()
	{
		expression1 = NULL;
		expression2 = NULL;
	}
	HLSLBinaryOp        binaryOp;
	HLSLExpression*     expression1;
	HLSLExpression*     expression2;
};

/** ? : construct */
struct HLSLConditionalExpression : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_ConditionalExpression;
	HLSLConditionalExpression()
	{
		condition = NULL;
		trueExpression = NULL;
		falseExpression = NULL;
	}
	HLSLExpression*     condition;
	HLSLExpression*     trueExpression;
	HLSLExpression*     falseExpression;
};

struct HLSLCastingExpression : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_CastingExpression;
	HLSLCastingExpression()
	{
		expression = NULL;
	}
	HLSLType            type;
	HLSLExpression*     expression;
};

/** Float, integer, boolean, etc. literal constant. */
struct HLSLLiteralExpression : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_LiteralExpression;
	HLSLBaseType        type;   // Note, not all types can be literals.
	union
	{
		bool            bValue[4];
		float           fValue[4];
		int             iValue[4];
	};
};

/** An identifier, typically a variable name or structure field name. */
struct HLSLIdentifierExpression : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_IdentifierExpression;
	HLSLIdentifierExpression()
	{
		name = NULL;
		global = false;
	}
	const char*         name;
	bool                global; // This is a global variable.
};

/** float2(1, 2) */
struct HLSLConstructorExpression : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_ConstructorExpression;
	HLSLConstructorExpression()
	{
		argument = NULL;
	}
	HLSLType            type;
	HLSLExpression*     argument;
};

/** object.member **/
struct HLSLMemberAccess : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_MemberAccess;
	HLSLMemberAccess()
	{
		object = NULL;
		field = NULL;
	}
	HLSLExpression*     object;
	const char*         field;
};

/** array[index] **/
struct HLSLArrayAccess : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_ArrayAccess;
	HLSLArrayAccess()
	{
		array = NULL;
		index = NULL;
	}
	HLSLExpression*     array;
	HLSLExpression*     index;
};

struct HLSLFunctionCall : public HLSLExpression
{
	static const HLSLNodeType s_type = HLSLNodeType_FunctionCall;
	HLSLFunctionCall()
	{
		function = NULL;
		argument = NULL;
		numArguments = 0;
	}
	const HLSLFunction* function;
	int                 numArguments;
	HLSLExpression*     argument;
};

/**
 * Abstract syntax tree for parsed HLSL code.
 */
class HLSLTree
{
public:

	explicit HLSLTree();
	~HLSLTree();

	/** Adds a string to the string pool used by the tree. */
	const char* AddString(const char* string);

	/** Returns true if the string is contained within the tree. */
	bool GetContainsString(const char* string) const;

	/** Returns the root block in the tree */
	HLSLRoot* GetRoot() const;

	/** Adds a new node to the tree with the specified type. */
	template <class T>
	T* AddNode(const char* fileName, int line)
	{
		HLSLNode* node = new (AllocateMemory(sizeof(T))) T();
		node->nodeType = T::s_type;
		node->fileName = fileName;
		node->line = line;
		return static_cast<T*>(node);
	}

private:

	void* AllocateMemory(size_t size);
	void  AllocatePage();

private:

	static const size_t s_nodePageSize = 1024 * 4;

	struct NodePage
	{
		std::shared_ptr<NodePage>   next;
		char        buffer[s_nodePageSize];
	};

	HLSLRoot*       m_root;

	size_t          m_currentPageOffset;

	std::shared_ptr<NodePage>       m_firstPage;
	std::shared_ptr<NodePage>       m_currentPage;
	std::set<std::string>      m_stringPool;
};

#endif