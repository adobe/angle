//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_VALIDATE_CSS_VERTEX_SHADER
#define COMPILER_VALIDATE_CSS_VERTEX_SHADER

#include "GLSLANG/ShaderLang.h"

#include "compiler/RewriteCSSShaderBase.h"
#include "compiler/intermediate.h"
#include "compiler/InfoSink.h"

class TInfoSinkBase;

class RewriteCSSVertexShader : public RewriteCSSShaderBase {
public:
    RewriteCSSVertexShader(TInfoSinkBase& infoSink) : RewriteCSSShaderBase(infoSink) {}
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
    void insertTexCoordAttribute();
};

#endif  // COMPILER_VALIDATE_CSS_VERTEX_SHADER
