//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/FindSymbolUsage.h"
#include "compiler/RenameFunction.h"
#include "compiler/RewriteCSSShaderBase.h"

// TODO: Update gyp.

// TODO: Move these.

bool RewriteCSSShaderBase::isSymbolUsed(const TString& symbolName)
{
    FindSymbolUsage findSymbolUsage(symbolName);
    root->traverse(&findSymbolUsage);
    return findSymbolUsage.symbolUsageFound();
}

void RewriteCSSShaderBase::renameFunction(const TString& oldFunctionName, const TString& newFunctionName)
{
    RenameFunction renameFunction(oldFunctionName, newFunctionName);
    root->traverse(&renameFunction);
}

//
// RewriteCSSShaderBase implementation
//

void RewriteCSSShaderBase::rewrite()
{
    createRootSequenceIfNeeded();
}

const char* const RewriteCSSShaderBase::kTexCoordVaryingPrefix = "css_v_texCoord";
const char* const RewriteCSSShaderBase::kMain = "main(";

TIntermConstantUnion* RewriteCSSShaderBase::createVec4Constant(float x, float y, float z, float w)
{
    ConstantUnion* constantArray = new ConstantUnion[4];
    constantArray[0].setFConst(x);
    constantArray[1].setFConst(y);
    constantArray[2].setFConst(z);
    constantArray[3].setFConst(w);
    return new TIntermConstantUnion(constantArray, TType(EbtFloat, EbpUndefined, EvqConst, 4));
}

TIntermConstantUnion* RewriteCSSShaderBase::createMat4IdentityConstant()
{
    ConstantUnion* constantArray = new ConstantUnion[4 * 4];
    for (int i = 0; i < 4 * 4; i++)
        constantArray[i].setFConst(0.0);

    constantArray[0].setFConst(1.0);
    constantArray[5].setFConst(1.0);
    constantArray[10].setFConst(1.0);
    constantArray[15].setFConst(1.0);

    return new TIntermConstantUnion(constantArray, TType(EbtFloat, EbpUndefined, EvqConst, 4, true));
}

TIntermSymbol* RewriteCSSShaderBase::createVec4Global(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqGlobal, 4));
}

TIntermSymbol* RewriteCSSShaderBase::createMat4Global(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqGlobal, 4, true));
}

TIntermSymbol* RewriteCSSShaderBase::createSampler2DUniform(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtSampler2D, EbpUndefined, EvqUniform));
}

TIntermSymbol* RewriteCSSShaderBase::createVec2Varying(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqVaryingIn, 2));
}

TIntermSymbol* RewriteCSSShaderBase::createVec2Attribute(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqAttribute, 2));
}

TIntermAggregate* RewriteCSSShaderBase::createFunctionCall(const TString& name, const TType& resultType)
{
    TIntermAggregate* functionCall = new TIntermAggregate(EOpFunctionCall);
    functionCall->setName(name);
    functionCall->setType(resultType);
    return functionCall;
}

TIntermBinary* RewriteCSSShaderBase::createBinary(TOperator op, TIntermTyped* left, TIntermTyped* right, const TType& type)
{
    TIntermBinary* binary = new TIntermBinary(op);
    binary->setType(type);
    binary->setLeft(left);
    binary->setRight(right);
    return binary;
}

TIntermBinary* RewriteCSSShaderBase::createBinaryWithVec2Result(TOperator op, TIntermTyped* left, TIntermTyped* right)
{
    return createBinary(op, left, right, TType(EbtFloat, EbpHigh, EvqTemporary, 2));
}

TIntermBinary* RewriteCSSShaderBase::createBinaryWithVec4Result(TOperator op, TIntermTyped* left, TIntermTyped* right)
{
    return createBinary(op, left, right, TType(EbtFloat, EbpHigh, EvqTemporary, 4));
}

TIntermBinary* RewriteCSSShaderBase::createBinaryWithMat4Result(TOperator op, TIntermTyped* left, TIntermTyped* right)
{
    return createBinary(op, left, right, TType(EbtFloat, EbpHigh, EvqTemporary, 4, true));
}

// The child can either be a symbol node or an initialization node.
TIntermAggregate* RewriteCSSShaderBase::createDeclaration(TIntermNode* child)
{
    TIntermAggregate* declaration = new TIntermAggregate(EOpDeclaration);
    declaration->getSequence().push_back(child);
    return declaration;
}

TIntermBinary* RewriteCSSShaderBase::createVec4GlobalInitialization(const TString& symbolName, TIntermTyped* rhs)
{
    return createBinaryWithVec4Result(EOpInitialize, createVec4Global(symbolName), rhs);
}

TIntermBinary* RewriteCSSShaderBase::createMat4GlobalInitialization(const TString& symbolName, TIntermTyped* rhs)
{
    return createBinaryWithMat4Result(EOpInitialize, createMat4Global(symbolName), rhs);
}

TIntermAggregate* RewriteCSSShaderBase::createVoidFunction(const TString& name)
{
    TIntermAggregate* function = new TIntermAggregate(EOpFunction);
    function->setName(name);
    function->setType(TType(EbtVoid, EbpUndefined, EvqGlobal));

    TIntermSequence& paramsAndBody = function->getSequence();

    TIntermAggregate* parameters = new TIntermAggregate(EOpParameters);
    paramsAndBody.push_back(parameters);

    TIntermAggregate* body = new TIntermAggregate(EOpSequence);
    paramsAndBody.push_back(body);

    return function;
}

void RewriteCSSShaderBase::addArgument(TIntermAggregate* functionCall, TIntermNode* argument)
{
    functionCall->getSequence().push_back(argument);
}

// Inserts "varying vec2 css_v_texCoord;".
void RewriteCSSShaderBase::insertTexCoordVaryingDeclaration()
{
    insertAtBeginningOfShader(createDeclaration(createVec2Varying(texCoordVaryingName)));
}

void RewriteCSSShaderBase::insertAtBeginningOfShader(TIntermNode* node)
{
    TIntermSequence& rootSequence = root->getAsAggregate()->getSequence();
    rootSequence.insert(rootSequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfShader(TIntermNode* node)
{
    root->getAsAggregate()->getSequence().push_back(node);
}

void RewriteCSSShaderBase::insertAtBeginningOfFunction(TIntermAggregate* function, TIntermNode* node)
{
    TIntermSequence& bodySequence = getOrCreateFunctionBody(function)->getSequence();
    bodySequence.insert(bodySequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfFunction(TIntermAggregate* function, TIntermNode* node)
{
    getOrCreateFunctionBody(function)->getSequence().push_back(node);
}

// Call this at the beginning of rewriting to wrap the main function in a sequence, if it isn't already
// wrapped in one.
// All of the other methods in this class and any subclasses will assume that the root is a sequence.
// Wrapping is required when the shader has only a main() function in the global scope, which makes the
// main function the tree root.
// In general, we want the main function wrapped in a sequence because we will need to insert declarations
// in the global scope around it.
void RewriteCSSShaderBase::createRootSequenceIfNeeded()
{
    TIntermAggregate* rootAggregate = root->getAsAggregate();

    // The root should be a sequence or a function declaration, both of which should be aggregate nodes.
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence || rootAggregate->getOp() == EOpFunction);

    if (rootAggregate->getOp() == EOpFunction) {
        // If the tree root is a function declaration, it should be the main function.
        ASSERT(rootAggregate->getName() == kMain);

        TIntermAggregate* newRoot = new TIntermAggregate(EOpSequence);
        TIntermSequence& sequence = newRoot->getSequence();
        sequence.push_back(root);
        root = newRoot;
    }
}

TIntermAggregate* RewriteCSSShaderBase::getOrCreateFunctionBody(TIntermAggregate* function)
{
    TIntermAggregate* body = NULL;
    TIntermSequence& paramsAndBody = function->getSequence();

    // The function should have parameters and may have a body.
    ASSERT(paramsAndBody.size() == 1 || paramsAndBody.size() == 2);

    if (paramsAndBody.size() == 2) {
        body = paramsAndBody[1]->getAsAggregate();
    } else {
        // Make a function body if necessary.
        body = new TIntermAggregate(EOpSequence);
        paramsAndBody.push_back(body);
    }

    // The function body should be an aggregate node.
    ASSERT(body);

    return body;
}

TIntermAggregate* RewriteCSSShaderBase::findFunction(const TString& name)
{
    TIntermSequence& rootSequence = root->getAsAggregate()->getSequence();
    for (TIntermSequence::const_iterator iter = rootSequence.begin(); iter != rootSequence.end(); ++iter) {
        TIntermNode* node = *iter;
        TIntermAggregate* aggregate = node->getAsAggregate();
        if (aggregate && aggregate->getOp() == EOpFunction && aggregate->getName() == name)
            return aggregate;
    }
    return NULL;
}
