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
    RewriteCSSFragmentShader(TInfoSinkBase& infoSink) : RewriteCSSShaderBase(infoSink), blendSymbol(NULL) {}
    virtual void rewrite();
    
private:  
    static const char* const kGLFragColor;
    static const char* const kCSSTextureUniform;

    // Blend symbols.
    static const char* const kCSSBlendColor;
    static const char* const kCSSColorMatrix;
    
    const char* blendSymbol;
    
    void insertBlendSymbolDeclaration();
    void insertTextureUniform();
    void insertBlendingOp();
    
    //
    // Generates errors for references to gl_FragColor.
    //
    class RestrictGLFragColor : public TIntermTraverser
    {
    public:
        RestrictGLFragColor(RewriteCSSFragmentShader* rewriter) : TIntermTraverser(true, false, false), mRewriter(rewriter) {}
        virtual void visitSymbol(TIntermSymbol*);
    private:
        RewriteCSSFragmentShader* mRewriter;
    };
    
    //
    // Determines which blend symbol is used. 
    // If multiple blend symbols are used, chooses one of them according to a precedence order.
    //
    class DetermineBlendSymbol : public TIntermTraverser
    {
    public:
        DetermineBlendSymbol(RewriteCSSFragmentShader* rewriter) : TIntermTraverser(true, false, false), mRewriter(rewriter) {}
        virtual void visitSymbol(TIntermSymbol*);
    private:
        RewriteCSSFragmentShader* mRewriter;
    };
};

#endif  // COMPILER_REWRITE_CSS_FRAGMENT_SHADER
