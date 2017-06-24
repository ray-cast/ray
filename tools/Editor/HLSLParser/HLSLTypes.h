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
#ifndef _H_HLSLTYPES_H_
#define _H_HLSLTYPES_H_

#include <ray/string.h>
#include <string>
#include <vector>
#include <memory>
#include <set>

enum HLSLToken
{
	HLSLToken_Begin = 256,
	HLSLToken_Float = HLSLToken_Begin,
	HLSLToken_Float2,
	HLSLToken_Float3,
	HLSLToken_Float4,
	HLSLToken_Float3x3,
	HLSLToken_Float4x4,
	HLSLToken_Half,
	HLSLToken_Half2,
	HLSLToken_Half3,
	HLSLToken_Half4,
	HLSLToken_Half3x3,
	HLSLToken_Half4x4,
	HLSLToken_Bool,
	HLSLToken_Int,
	HLSLToken_Int2,
	HLSLToken_Int3,
	HLSLToken_Int4,
	HLSLToken_Uint,
	HLSLToken_Uint2,
	HLSLToken_Uint3,
	HLSLToken_Uint4,
	HLSLToken_Texture,
	HLSLToken_Sampler2D,
	HLSLToken_SamplerCube,

	HLSLToken_If,
	HLSLToken_Else,
	HLSLToken_For,
	HLSLToken_While,
	HLSLToken_Break,
	HLSLToken_True,
	HLSLToken_False,
	HLSLToken_Void,
	HLSLToken_Struct,
	HLSLToken_CBuffer,
	HLSLToken_TBuffer,
	HLSLToken_Register,
	HLSLToken_Return,
	HLSLToken_Continue,
	HLSLToken_Discard,
	HLSLToken_Const,
	HLSLToken_PackOffset,

	HLSLToken_Uniform,
	HLSLToken_In,
	HLSLToken_InOut,

	HLSLToken_LessEqual,
	HLSLToken_GreaterEqual,
	HLSLToken_EqualEqual,
	HLSLToken_NotEqual,
	HLSLToken_PlusPlus,
	HLSLToken_MinusMinus,
	HLSLToken_PlusEqual,
	HLSLToken_MinusEqual,
	HLSLToken_TimesEqual,
	HLSLToken_DivideEqual,
	HLSLToken_AndAnd,       // &&
	HLSLToken_BarBar,       // ||

	HLSLToken_FloatLiteral,
	HLSLToken_IntLiteral,
	HLSLToken_Identifier,

	HLSLToken_EOF,
};

enum HLSLNodeType
{
	HLSLNodeType_Root,
	HLSLNodeType_Declaration,
	HLSLNodeType_Struct,
	HLSLNodeType_StructField,
	HLSLNodeType_Buffer,
	HLSLNodeType_BufferField,
	HLSLNodeType_Function,
	HLSLNodeType_Argument,
	HLSLNodeType_ExpressionStatement,
	HLSLNodeType_Expression,
	HLSLNodeType_ReturnStatement,
	HLSLNodeType_DiscardStatement,
	HLSLNodeType_BreakStatement,
	HLSLNodeType_ContinueStatement,
	HLSLNodeType_IfStatement,
	HLSLNodeType_ForStatement,
	HLSLNodeType_UnaryExpression,
	HLSLNodeType_BinaryExpression,
	HLSLNodeType_ConditionalExpression,
	HLSLNodeType_CastingExpression,
	HLSLNodeType_LiteralExpression,
	HLSLNodeType_IdentifierExpression,
	HLSLNodeType_ConstructorExpression,
	HLSLNodeType_MemberAccess,
	HLSLNodeType_ArrayAccess,
	HLSLNodeType_FunctionCall,
};

enum HLSLBaseType
{
	HLSLBaseType_Unknown,
	HLSLBaseType_Void,
	HLSLBaseType_Float,
	HLSLBaseType_FirstNumeric = HLSLBaseType_Float,
	HLSLBaseType_Float2,
	HLSLBaseType_Float3,
	HLSLBaseType_Float4,
	HLSLBaseType_Float3x3,
	HLSLBaseType_Float4x4,
	HLSLBaseType_Half,
	HLSLBaseType_Half2,
	HLSLBaseType_Half3,
	HLSLBaseType_Half4,
	HLSLBaseType_Half3x3,
	HLSLBaseType_Half4x4,
	HLSLBaseType_Bool,
	HLSLBaseType_Int,
	HLSLBaseType_Int2,
	HLSLBaseType_Int3,
	HLSLBaseType_Int4,
	HLSLBaseType_Uint,
	HLSLBaseType_Uint2,
	HLSLBaseType_Uint3,
	HLSLBaseType_Uint4,
	HLSLBaseType_LastNumeric = HLSLBaseType_Uint4,
	HLSLBaseType_Texture,
	HLSLBaseType_Sampler2D,
	HLSLBaseType_SamplerCube,
	HLSLBaseType_UserDefined,       // struct

	HLSLBaseType_Count,
	HLSLBaseType_NumericCount = HLSLBaseType_LastNumeric - HLSLBaseType_FirstNumeric + 1
};

enum HLSLBinaryOp
{
	HLSLBinaryOp_And,
	HLSLBinaryOp_Or,
	HLSLBinaryOp_Add,
	HLSLBinaryOp_Sub,
	HLSLBinaryOp_Mul,
	HLSLBinaryOp_Div,
	HLSLBinaryOp_Less,
	HLSLBinaryOp_Greater,
	HLSLBinaryOp_LessEqual,
	HLSLBinaryOp_GreaterEqual,
	HLSLBinaryOp_Equal,
	HLSLBinaryOp_NotEqual,
	HLSLBinaryOp_Assign,
	HLSLBinaryOp_AddAssign,
	HLSLBinaryOp_SubAssign,
	HLSLBinaryOp_MulAssign,
	HLSLBinaryOp_DivAssign,
};

enum HLSLUnaryOp
{
	HLSLUnaryOp_Negative,       // -x
	HLSLUnaryOp_Positive,       // +x
	HLSLUnaryOp_Not,            // !x
	HLSLUnaryOp_PreIncrement,   // ++x
	HLSLUnaryOp_PreDecrement,   // --x
	HLSLUnaryOp_PostIncrement,  // x++
	HLSLUnaryOp_PostDecrement,  // x++
};

enum HLSLArgumentModifier
{
	HLSLArgumentModifier_None,
	HLSLArgumentModifier_In,
	HLSLArgumentModifier_Inout,
	HLSLArgumentModifier_Uniform,
};

#endif