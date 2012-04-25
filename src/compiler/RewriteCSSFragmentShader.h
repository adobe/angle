//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_REWRITE_CSS_FRAGMENT_SHADER
#define COMPILER_REWRITE_CSS_FRAGMENT_SHADER

#include "GLSLANG/ShaderLang.h"

#include "compiler/RewriteCSSShaderBase.h"
#include "compiler/intermediate.h"
#include "compiler/InfoSink.h"

class TInfoSinkBase;

class RewriteCSSFragmentShader : public RewriteCSSShaderBase {
public:
    RewriteCSSFragmentShader(TIntermNode* treeRoot, const TString& hiddenSymbolSuffix, TInfoSinkBase& infoSink) 
        : RewriteCSSShaderBase(treeRoot, hiddenSymbolSuffix, infoSink)
        , textureUniformName(kTextureUniformPrefix + hiddenSymbolSuffix)
        , useColorMatrix(false) {}
    
    virtual void rewrite();
    
    const TString& getTextureUniformName() { return textureUniformName; } 
    
private:  
    static const char* const kCSSMain;
    static const char* const kFragColor;
    static const char* const kTextureUniformPrefix;
    static const char* const kBlendColor;
    static const char* const kColorMatrix;
        
    void insertBlendSymbolDeclaration();
    void insertTextureUniform();
    void insertBlendOp();
    
    TString textureUniformName;
    bool useColorMatrix;
};

#endif  // COMPILER_REWRITE_CSS_FRAGMENT_SHADER
