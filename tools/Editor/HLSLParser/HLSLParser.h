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
#ifndef _H_HLSL_PARSER_H_
#define _H_HLSL_PARSER_H_

#include "HLSLTokenizer.h"
#include "HLSLTree.h"

class HLSLParser final
{
public:
	HLSLParser(const char* fileName, const char* buffer, std::size_t length);

	bool Parse(HLSLTree* tree);

private:
	bool Accept(int token);
	bool Accept(const char* token);
	bool AcceptInt(int& value);
	bool AcceptFloat1(float value[1]);
	bool AcceptFloat2(float value[2]);
	bool AcceptFloat3(float value[3]);
	bool AcceptFloat4(float value[4]);
	bool AcceptIdentifier(const char*& identifier);

	bool AcceptType(bool allowVoid, HLSLBaseType& type, const char*& typeName, bool* constant);
	bool AcceptBinaryOperator(int priority, HLSLBinaryOp& binaryOp);
	bool AcceptUnaryOperator(bool pre, HLSLUnaryOp& unaryOp);
	bool AcceptAssign(HLSLBinaryOp& binaryOp);
	bool AcceptDeclaration(bool allowUnsizedArray, HLSLType& type, const char*& name);

	bool Expect(int token);
	bool ExpectIdentifier(const char*& identifier);
	bool ExpectType(bool allowVoid, HLSLBaseType& type, const char*& typeName, bool* constant);
	bool ExpectDeclaration(bool allowUnsizedArray, HLSLType& type, const char*& name);

	bool ParseTopLevel(HLSLStatement*& statement);
	bool ParseBlock(HLSLStatement*& firstStatement, const HLSLType& returnType);
	bool ParseStatementOrBlock(HLSLStatement*& firstStatement, const HLSLType& returnType);
	bool ParseStatement(HLSLStatement*& statement, const HLSLType& returnType);
	bool ParseDeclaration(HLSLDeclaration*& declaration);
	bool ParseFieldDeclaration(HLSLStructField*& field);
	bool ParseBufferFieldDeclaration(HLSLBufferField*& field);
	bool ParseExpression(HLSLExpression*& expression);
	bool ParseBinaryExpression(int priority, HLSLExpression*& expression);
	bool ParseTerminalExpression(HLSLExpression*& expression, bool& needsEndParen);
	bool ParseExpressionList(int endToken, bool allowEmptyEnd, HLSLExpression*& firstExpression, int& numExpressions);
	bool ParseArgumentList(int endToken, HLSLArgument*& firstArgument, int& numArguments);
	bool ParseDeclarationAssignment(HLSLDeclaration* declaration);
	bool ParsePartialConstructor(HLSLExpression*& expression, HLSLBaseType type, const char* typeName);

	bool CheckForUnexpectedEndOfStream(int endToken);

	const HLSLStruct* FindUserDefinedType(const char* name) const;

	void BeginScope();
	void EndScope();

	void DeclareVariable(const char* name, const HLSLType& type);
	void DeclareVariable(const char* name, const HLSLType& type, int value);
	void DeclareVariable(const char* name, const HLSLType& type, float value);
	void DeclareVariable(const char* name, const HLSLType& type, const char* value);

	const HLSLType* FindVariable(const char* name, bool& global) const;

	const HLSLFunction* FindFunction(const char* name) const;

	bool GetIsFunction(const char* name) const;
	const HLSLFunction* MatchFunctionCall(const HLSLFunctionCall* functionCall, const char* name);

	bool GetMemberType(const HLSLType& objectType, const char* fieldName, HLSLType& memberType);

	bool CheckTypeCast(const HLSLType& srcType, const HLSLType& dstType);

	const char* GetFileName();
	int GetLineNumber() const;

private:
	struct Variable
	{
		Variable()
		{
		}

		const char*     name;
		HLSLType        type;
		union
		{
			int ivalue;
			float fvalue;
		};

		std::string string;
	};

	HLSLTokenizer				m_tokenizer;
	std::vector<HLSLStruct*>    m_userTypes;
	std::vector<Variable>       m_variables;
	std::vector<HLSLFunction*>  m_functions;
	int							m_numGlobals;
	HLSLTree*					m_tree;
};

#endif