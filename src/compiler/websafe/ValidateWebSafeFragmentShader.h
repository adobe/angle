//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_VALIDATE_WEB_SAFE_FRAGMENT_SHADER_H_
#define COMPILER_VALIDATE_WEB_SAFE_FRAGMENT_SHADER_H_

#include "GLSLANG/ShaderLang.h"

#include "compiler/intermediate.h"
#include "compiler/websafe/DependencyGraph.h"

class TInfoSinkBase;

class ValidateWebSafeFragmentShader : TDependencyGraphTraverser {
public:
    ValidateWebSafeFragmentShader(TInfoSinkBase& sink, const TString& restrictedSymbol)
        : mSink(sink)
        , mRestrictedSymbol(restrictedSymbol)
        , mNumErrors(0) {}

    void validate(const TDependencyGraph& graph);
    int numErrors() const { return mNumErrors; }

    virtual void visitArgument(TGraphArgument* parameter);
    virtual void visitSelection(TGraphSelection* selection);
    virtual void visitLoop(TGraphLoop* loop);
    virtual void visitLogicalOp(TGraphLogicalOp* logicalOp);

private:
    void beginError(const TIntermNode* node);
    void validateUserDefinedFunctionCallUsage(const TDependencyGraph& graph);

	TInfoSinkBase& mSink;
    const TString mRestrictedSymbol;
    int mNumErrors;
};

#endif  // COMPILER_VALIDATE_WEB_SAFE_FRAGMENT_SHADER_H_
