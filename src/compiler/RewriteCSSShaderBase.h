//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_REWRITE_CSS_SHADER_BASE
#define COMPILER_REWRITE_CSS_SHADER_BASE

#include "GLSLANG/ShaderLang.h"

#include "compiler/intermediate.h"
#include "compiler/InfoSink.h"

class TInfoSinkBase;

class RewriteCSSShaderBase : public TIntermTraverser {
public:
    RewriteCSSShaderBase(TInfoSinkBase& infoSink) : TIntermTraverser(false, false, true), sink(infoSink) {}
    virtual ~RewriteCSSShaderBase() {}
    
    virtual void rewrite() = 0;
    virtual int numErrors() = 0;
protected:
    TIntermConstantUnion* createVec4Constant(float x, float y, float z, float w);
    TIntermSymbol* createGlobalVec4(const TString& name);
    TIntermSymbol* createUniformSampler2D(const TString& name);
    TIntermSymbol* createVaryingVec2(const TString& name);
    TIntermAggregate* createFunctionCall(const TString& name);
    void addArgument(TIntermNode* argument, TIntermAggregate* functionCall);
    TIntermBinary* createBinary(TOperator op, TIntermTyped* left, TIntermTyped* right);
    TIntermAggregate* createTexture2DCall(const TString& textureUniformName, const TString& texCoordVaryingName);
    TIntermAggregate* createDeclaration(TIntermNode* child);
    TIntermBinary* createGlobalVec4Initialization(const TString& symbolName, TIntermTyped* rhs);
    
    void insertAtTopOfShader(TIntermNode* node);
    void insertAtEndOfFunction(TIntermNode* node, TIntermAggregate* function);
    
    void insertTexCoordVarying();
    
    TInfoSinkBase& sink;
};

#endif  // COMPILER_REWRITE_CSS_SHADER_BASE
