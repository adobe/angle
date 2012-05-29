//
// Copyright (c) 2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_CSSSHADERS_REWRITE_CSS_SHADER_BASE
#define COMPILER_CSSSHADERS_REWRITE_CSS_SHADER_BASE

#include "GLSLANG/ShaderLang.h"

#include "compiler/intermediate.h"
#include "compiler/SymbolTable.h"

class TInfoSinkBase;

class RewriteCSSShaderBase {
public:
    RewriteCSSShaderBase(TIntermNode* root,
                         const TSymbolTable& symbolTable,
                         const TString& hiddenSymbolSuffix);

    virtual void rewrite();
    TIntermNode* getNewRoot() { return mRoot; }

    virtual ~RewriteCSSShaderBase() {}

protected:
    static const char* const kMain;

    void insertAtBeginningOfShader(TIntermNode* node);
    void insertAtEndOfShader(TIntermNode* node);

    TIntermAggregate* findFunction(const TString& name) const;
    void renameFunction(const TString& oldFunctionName, const TString& newFunctionName);

    const TType& getBuiltinType(const TString& builtinName) const;
    const TString& getTexCoordVaryingName() const { return mTexCoordVaryingName; }

    TIntermAggregate* getRootAggregate() const;

private:
    static const char* const kTexCoordVaryingPrefix;

    TIntermAggregate* mRoot;
    const TSymbolTable& mSymbolTable;
    TString mTexCoordVaryingName;

    void createRootSequenceIfNeeded();
};

#endif  // COMPILER_CSSSHADERS_REWRITE_CSS_SHADER_BASE
