//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSShaderHelper.h"

namespace RewriteCSSShaderHelper {

const TType voidType(TQualifier qualifier)
{
    return TType(EbtVoid, EbpUndefined, qualifier);
}

const TType vec2Type(TQualifier qualifier)
{
    return TType(EbtFloat, getDefaultPrecision(qualifier), qualifier, 2);
}

const TType vec4Type(TQualifier qualifier)
{
    return TType(EbtFloat, getDefaultPrecision(qualifier), qualifier, 4);
}

const TType mat4Type(TQualifier qualifier)
{
    return TType(EbtFloat, getDefaultPrecision(qualifier), qualifier, 4, true);
}

const TType sampler2DType()
{
    return TType(EbtSampler2D, EbpUndefined, EvqUniform);
}

const TPrecision getDefaultPrecision(TQualifier qualifier)
{
    return qualifier == EvqTemporary || qualifier == EvqConst ?  EbpUndefined : EbpHigh;
}

TIntermConstantUnion* createVec4Constant(float x, float y, float z, float w)
{
    ConstantUnion* constantArray = new ConstantUnion[4];
    constantArray[0].setFConst(x);
    constantArray[1].setFConst(y);
    constantArray[2].setFConst(z);
    constantArray[3].setFConst(w);
    return new TIntermConstantUnion(constantArray, vec4Type(EvqConst));
}

TIntermConstantUnion* createMat4IdentityConstant()
{
    ConstantUnion* constantArray = new ConstantUnion[4 * 4];
    for (int i = 0; i < 4 * 4; i++)
        constantArray[i].setFConst(0.0);
    
    constantArray[0].setFConst(1.0);
    constantArray[5].setFConst(1.0);
    constantArray[10].setFConst(1.0);
    constantArray[15].setFConst(1.0);
    
    return new TIntermConstantUnion(constantArray, mat4Type(EvqConst));
}

TIntermSymbol* createSymbol(const TString& name, const TType& type)
{
    return new TIntermSymbol(0, name, type);
}

TIntermAggregate* createFunctionCall(const TString& name, const TType& returnType)
{
    TIntermAggregate* functionCall = new TIntermAggregate(EOpFunctionCall);
    functionCall->setName(name);
    functionCall->setType(returnType);
    return functionCall;
}

TIntermAggregate* createFunctionCall(const TString& name, TIntermNode* argument1, TIntermNode* argument2, const TType& returnType)
{
    TIntermAggregate* functionCall = createFunctionCall(name, returnType);
    addArgument(functionCall, argument1);
    addArgument(functionCall, argument2);
    return functionCall;
}

void addArgument(TIntermAggregate* functionCall, TIntermNode* argument)
{
    functionCall->getSequence().push_back(argument);
}

TIntermBinary* createBinary(TOperator op, TIntermTyped* left, TIntermTyped* right, const TType& returnType)
{
    TIntermBinary* binary = new TIntermBinary(op);
    binary->setType(returnType);
    binary->setLeft(left);
    binary->setRight(right);
    return binary;
}

TIntermAggregate* createDeclaration(TIntermNode* child)
{
    TIntermAggregate* declaration = new TIntermAggregate(EOpDeclaration);
    declaration->getSequence().push_back(child);
    return declaration;
}

TIntermAggregate* createDeclaration(TIntermSymbol* symbol, TIntermTyped* rhs)
{
    // The initialization node has the same type as the symbol, except with undefined precision.
    TType type(symbol->getType());
    type.setPrecision(EbpUndefined);
    
    // The initialization node sets the symbol equal to the right hand side.
    TIntermBinary* initialization = createBinary(EOpInitialize, symbol, rhs, type);
    
    // The declaration node contains the initialization node.
    return createDeclaration(initialization);
}

TIntermAggregate* createFunction(const TString& name, const TType& returnType)
{
    TIntermAggregate* function = new TIntermAggregate(EOpFunction);
    function->setName(name);
    function->setType(returnType);
    
    TIntermSequence& paramsAndBody = function->getSequence();
    
    TIntermAggregate* parameters = new TIntermAggregate(EOpParameters);
    paramsAndBody.push_back(parameters);
    
    TIntermAggregate* body = new TIntermAggregate(EOpSequence);
    paramsAndBody.push_back(body);
    
    return function;
}

}  // namespace
