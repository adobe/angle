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

class RewriteCSSShaderBase {
public:
    RewriteCSSShaderBase(TIntermNode* treeRoot, const TString& hiddenSymbolSuffix, TInfoSinkBase& infoSink)
        : root(treeRoot)
        , texCoordVaryingName(kTexCoordVaryingPrefix + hiddenSymbolSuffix)
        , sink(infoSink) {}

    virtual void rewrite();
    TIntermNode* getNewTreeRoot() { return root; }

    virtual ~RewriteCSSShaderBase() {}

protected:
    static const char* const kTexCoordVaryingPrefix;
    static const char* const kTexture2D;
    static const char* const kMain;

    TIntermConstantUnion* createVec4Constant(float x, float y, float z, float w);
    TIntermConstantUnion* createMat4IdentityConstant();
    TIntermSymbol* createVec4Global(const TString& name);
    TIntermSymbol* createMat4Global(const TString& name);
    TIntermSymbol* createSampler2DUniform(const TString& name);
    TIntermSymbol* createVec2Varying(const TString& name);
    TIntermSymbol* createVec2Attribute(const TString& name);
    TIntermAggregate* createFunctionCall(const TString& name);
    void addArgument(TIntermAggregate* functionCall, TIntermNode* argument);
    TIntermBinary* createBinary(TOperator op, TIntermTyped* left, TIntermTyped* right);
    TIntermBinary* createBinaryWithVec2Result(TOperator op, TIntermTyped* left, TIntermTyped* right);
    TIntermBinary* createBinaryWithVec4Result(TOperator op, TIntermTyped* left, TIntermTyped* right);
    TIntermBinary* createBinaryWithMat4Result(TOperator op, TIntermTyped* left, TIntermTyped* right);
    TIntermAggregate* createTexture2DCall(const TString& textureUniformName, const TString& texCoordVaryingName);
    TIntermAggregate* createDeclaration(TIntermNode* child);
    TIntermBinary* createVec4GlobalInitialization(const TString& symbolName, TIntermTyped* rhs);
    TIntermBinary* createMat4GlobalInitialization(const TString& symbolName, TIntermTyped* rhs);
    TIntermAggregate* createVoidFunction(const TString& name);

    void insertAtBeginningOfShader(TIntermNode* node);
    void insertAtEndOfShader(TIntermNode* node);
    void insertAtBeginningOfFunction(TIntermAggregate* function, TIntermNode* node);
    void insertAtEndOfFunction(TIntermAggregate* function, TIntermNode* node);
    void insertTexCoordVaryingDeclaration();

    TIntermAggregate* findFunction(const TString& name);
    void renameFunction(const TString& oldFunctionName, const TString& newFunctionName);
    bool isSymbolUsed(const TString& symbolName);

    TIntermNode* root;
    TString texCoordVaryingName;
    TInfoSinkBase& sink;

private:
    void createRootSequenceIfNeeded();
    TIntermAggregate* getOrCreateFunctionBody(TIntermAggregate* function);
};

#endif  // COMPILER_REWRITE_CSS_SHADER_BASE
