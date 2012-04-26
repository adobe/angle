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

// TODO(mvujovic): Give an example.

class RewriteCSSVertexShader : public RewriteCSSShaderBase {
public:
    RewriteCSSVertexShader(TIntermNode* treeRoot, const TString& hiddenSymbolSuffix, TInfoSinkBase& infoSink)
        : RewriteCSSShaderBase(treeRoot, hiddenSymbolSuffix, infoSink)
        , texCoordAttributeName(kTexCoordAttributePrefix + hiddenSymbolSuffix) {}    
    void rewrite();
    
private:
    // TODO: This should be fixed to "a_texCoord", and defined if its not defined already.
    static const char* const kTexCoordAttributePrefix;
    
    void insertTexCoordAttribute();
    void insertCSSTexCoordVaryingAssignment();
    
    TString texCoordAttributeName;
};

#endif  // COMPILER_REWRITE_CSS_VERTEX_SHADER
