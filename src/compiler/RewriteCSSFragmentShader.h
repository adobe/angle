//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_REWRITE_CSS_FRAGMENT_SHADER
#define COMPILER_REWRITE_CSS_FRAGMENT_SHADER

#include "GLSLANG/ShaderLang.h"

#include "compiler/intermediate.h"
#include "compiler/InfoSink.h"

class TInfoSinkBase;

class RewriteCSSFragmentShader : public TIntermTraverser {
public:
    RewriteCSSFragmentShader(TInfoSinkBase& sink) : TIntermTraverser(false, false, true), mSink(sink) {}
    void rewrite();
    int numErrors() { return 0; } // TODO: Implement.
    
    virtual void visitSymbol(TIntermSymbol*);
    virtual bool visitBinary(Visit visit, TIntermBinary*);
    virtual bool visitUnary(Visit visit, TIntermUnary*);
    virtual bool visitSelection(Visit visit, TIntermSelection*);
    virtual bool visitAggregate(Visit visit, TIntermAggregate*);
    virtual bool visitLoop(Visit visit, TIntermLoop*);
    virtual bool visitBranch(Visit visit, TIntermBranch*);
private:
    TIntermConstantUnion* createVec4Constant(float x, float y, float z, float w);
    TIntermSymbol* createGlobalVec4(const TString& name);
    TIntermSymbol* createUniformSampler2D(const TString& name);
    TIntermSymbol* createVaryingVec2(const TString& name);
    TIntermAggregate* createFunctionCall(const TString& name);
    void addArgument(TIntermNode* argument, TIntermAggregate* functionCall);
    TIntermBinary* createBinary(TOperator op, TIntermTyped* left, TIntermTyped* right);
    TIntermAggregate* createTexture2DCall(const TString& textureUniformName, const TString& texCoordVaryingName);
    
    void insertAtTopOfShader(TIntermNode* node);
    void insertAtEndOfFunction(TIntermNode* node, TIntermAggregate* function);
    
    void insertCSSFragColorDeclaration();
    void insertTextureUniform();
    void insertTexCoordVarying();
    void insertBlendingOp(TIntermAggregate* mainFunction);
    
    TInfoSinkBase& mSink;
};

#endif  // COMPILER_REWRITE_CSS_FRAGMENT_SHADER
