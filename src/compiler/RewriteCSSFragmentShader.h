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
        , blendSymbol(NULL) {}
    
    virtual void rewrite();
    
    const TString& getTextureUniformName() { return textureUniformName; } 
    
private:  
    static const char* const kFragColor;
    static const char* const kTextureUniformPrefix;

    // Blend symbols.
    static const char* const kBlendColor;
    static const char* const kColorMatrix;
        
    void insertBlendSymbolDeclaration();
    void insertTextureUniform();
    void insertBlendingOp();
    
    TString textureUniformName;
    const char* blendSymbol;
    
    //
    // Generates errors for references to gl_FragColor.
    //
    class RestrictFragColor : public TIntermTraverser
    {
    public:
        RestrictFragColor(RewriteCSSFragmentShader* rewriter) : TIntermTraverser(true, false, false), mRewriter(rewriter) {}
        virtual void visitSymbol(TIntermSymbol*);
    private:
        RewriteCSSFragmentShader* mRewriter;
    };    
};

#endif  // COMPILER_REWRITE_CSS_FRAGMENT_SHADER
