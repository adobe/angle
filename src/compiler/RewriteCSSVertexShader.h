//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_REWRITE_CSS_VERTEX_SHADER
#define COMPILER_REWRITE_CSS_VERTEX_SHADER

#include "GLSLANG/ShaderLang.h"

#include "compiler/RewriteCSSShaderBase.h"
#include "compiler/intermediate.h"
#include "compiler/InfoSink.h"

class TInfoSinkBase;

class RewriteCSSVertexShader : public RewriteCSSShaderBase {
public:
    RewriteCSSVertexShader(TIntermNode* treeRoot, const TString& hiddenSymbolSuffix, TInfoSinkBase& infoSink)
        : RewriteCSSShaderBase(treeRoot, hiddenSymbolSuffix, infoSink) {}
    
    void rewrite();
    
private:
    void insertTexCoordAttribute();
    void insertCSSTexCoordVaryingAssignment(TIntermAggregate* mainFunction);
};

#endif  // COMPILER_REWRITE_CSS_VERTEX_SHADER
