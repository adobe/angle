//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSShaderBase.h"
#include "ParseHelper.h"

static const char* kGLFragColor = "gl_FragColor";
static const char* kCSSGLFragColor = "css_gl_FragColor";
static const char* kCSSUTexture = "css_u_texture";
static const char* kCSSVTexCoord = "css_v_texCoord";
static const char* kTexture2D = "texture2D(s21;vf2;";
static const char* kMain = "main(";

TIntermConstantUnion* RewriteCSSShaderBase::createVec4Constant(float x, float y, float z, float w)
{
    ConstantUnion* constantArray = new ConstantUnion[4];
    constantArray[0].setFConst(x);
    constantArray[1].setFConst(y);
    constantArray[2].setFConst(z);
    constantArray[3].setFConst(w);
    return new TIntermConstantUnion(constantArray, TType(EbtFloat, EbpUndefined, EvqConst, 4));    
}

TIntermSymbol* RewriteCSSShaderBase::createGlobalVec4(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqGlobal, 4));
}

TIntermSymbol* RewriteCSSShaderBase::createUniformSampler2D(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtSampler2D, EbpUndefined, EvqUniform));
}

TIntermSymbol* RewriteCSSShaderBase::createVaryingVec2(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqVaryingIn, 2));
}

TIntermAggregate* RewriteCSSShaderBase::createFunctionCall(const TString& name)
{
    TIntermAggregate* functionCall = new TIntermAggregate(EOpFunctionCall);
    functionCall->setName(name);
    return functionCall;
}

TIntermBinary* RewriteCSSShaderBase::createBinary(TOperator op, TIntermTyped* left, TIntermTyped* right)
{
    TIntermBinary* binary = new TIntermBinary(op);
    binary->setLeft(left);
    binary->setRight(right);
    return binary; 
}

TIntermAggregate* RewriteCSSShaderBase::createTexture2DCall(const TString& textureUniformName, const TString& texCoordVaryingName)
{
    TIntermAggregate* texture2DCall = createFunctionCall(kTexture2D); // TODO: Maybe pool allocate strings?
    addArgument(createUniformSampler2D(textureUniformName), texture2DCall);
    addArgument(createVaryingVec2(texCoordVaryingName), texture2DCall);
    return texture2DCall;
}

TIntermAggregate* RewriteCSSShaderBase::createDeclaration(TIntermNode* child)
{
    TIntermAggregate* declaration = new TIntermAggregate(EOpDeclaration);
    declaration->getSequence().push_back(child);
    return declaration;    
}

TIntermBinary* RewriteCSSShaderBase::createGlobalVec4Initialization(const TString& symbolName, TIntermTyped* rhs)
{
    TIntermBinary* initialization = createBinary(EOpInitialize, createGlobalVec4(symbolName), rhs);
    initialization->setType(TType(EbtFloat, EbpHigh, EvqTemporary, 4)); // TODO: What precision?
    return initialization;
}

void RewriteCSSShaderBase::addArgument(TIntermNode* argument, TIntermAggregate* functionCall)
{
    functionCall->getSequence().push_back(argument);
}

// Inserts "varying vec2 css_v_texCoord".
void RewriteCSSShaderBase::insertTexCoordVarying()
{
    insertAtTopOfShader(createDeclaration(createVaryingVec2(kCSSVTexCoord)));
}

void RewriteCSSShaderBase::insertAtTopOfShader(TIntermNode* node)
{
    TIntermSequence& globalSequence = GlobalParseContext->treeRoot->getAsAggregate()->getSequence();
    globalSequence.insert(globalSequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfFunction(TIntermNode* node, TIntermAggregate* function)
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
    
    body->getSequence().push_back(node);
}
