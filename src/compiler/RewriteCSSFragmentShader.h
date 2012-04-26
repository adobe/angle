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
        , cssMainFunctionName(kCSSMainPrefix + hiddenSymbolSuffix + "(")
        , usesColorMatrix(false) {}
    
    virtual void rewrite();
    
    const TString& getTextureUniformName() { return textureUniformName; } 
    
private:  
    static const char* const kCSSMainPrefix;
    static const char* const kFragColor;
    static const char* const kTextureUniformPrefix;
    static const char* const kBlendColor;
    static const char* const kColorMatrix;
    
    void insertBlendColorDeclaration();
    void insertColorMatrixDeclaration();
    void insertTextureUniformDeclaration();
    void insertNewMainFunction();
    void insertCSSMainCall();
    void insertBlendOp();
    
    TString textureUniformName;
    TString cssMainFunctionName;
    bool usesColorMatrix;
};

#endif  // COMPILER_REWRITE_CSS_FRAGMENT_SHADER
