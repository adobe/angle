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

// TODO(mvujovic): Insert example.

class TInfoSinkBase;

class RewriteCSSFragmentShader : public RewriteCSSShaderBase {
public:
    RewriteCSSFragmentShader(TIntermNode* treeRoot, const TString& hiddenSymbolSuffix, TInfoSinkBase& infoSink) 
        : RewriteCSSShaderBase(treeRoot, hiddenSymbolSuffix, infoSink)
        , textureUniformName(kTextureUniformPrefix + hiddenSymbolSuffix)
        , userMainFunctionName(kUserMainFunctionPrefix + hiddenSymbolSuffix + "(")
        , usesColorMatrix(false) {}
    
    virtual void rewrite();
    
private:  
    static const char* const kBlendColor;
    static const char* const kColorMatrix;
    static const char* const kTextureUniformPrefix;
    static const char* const kUserMainFunctionPrefix;
    static const char* const kFragColor;
    
    void insertBlendColorDeclaration();
    void insertColorMatrixDeclaration();
    void insertTextureUniformDeclaration();
    void insertNewMainFunction();
    void insertUserMainFunctionCall();
    void insertBlendOp();
    
    TString textureUniformName;
    TString userMainFunctionName;
    bool usesColorMatrix;
};

#endif  // COMPILER_REWRITE_CSS_FRAGMENT_SHADER
