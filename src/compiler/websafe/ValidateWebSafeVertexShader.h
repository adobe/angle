//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_VALIDATE_WEB_SAFE_VERTEX_SHADER_H_
#define COMPILER_VALIDATE_WEB_SAFE_VERTEX_SHADER_H_

#include "GLSLANG/ShaderLang.h"

#include "compiler/intermediate.h"
#include "compiler/InfoSink.h"

class TInfoSinkBase;

class ValidateWebSafeVertexShader : public TIntermTraverser {
public:
    ValidateWebSafeVertexShader(TInfoSinkBase& sink, const TString& restrictedSymbol)
        : TIntermTraverser(true, false, false)
        , mSink(sink)
        , mRestrictedSymbol(restrictedSymbol)
        , mFoundRestrictedSymbol(false) {}

    void validate(TIntermNode* root) { root->traverse(this); }
    int numErrors() { return mFoundRestrictedSymbol ? 1 : 0; }

    virtual void visitSymbol(TIntermSymbol*);
    virtual bool visitBinary(Visit visit, TIntermBinary*) { return false; }
    virtual bool visitUnary(Visit visit, TIntermUnary*) { return false; }
    virtual bool visitSelection(Visit visit, TIntermSelection*) { return false; }
    virtual bool visitAggregate(Visit visit, TIntermAggregate*);
    virtual bool visitLoop(Visit visit, TIntermLoop*) { return false; };
    virtual bool visitBranch(Visit visit, TIntermBranch*) { return false; };
private:
    TInfoSinkBase& mSink;
    const TString mRestrictedSymbol;
    bool mFoundRestrictedSymbol;
};

#endif  // COMPILER_VALIDATE_WEB_SAFE_VERTEX_SHADER_H_
