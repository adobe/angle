//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSFragmentShader.h"
#include "ParseHelper.h"

//
// RestrictGLFragColor implementation
//

void RewriteCSSFragmentShader::RestrictGLFragColor::visitSymbol(TIntermSymbol* node)
{
    if (node->getSymbol() == kGLFragColor) {
        ++mRewriter->numErrors;
        mRewriter->sink.prefix(EPrefixError);
        mRewriter->sink << "'" << kGLFragColor << "' access is not permitted.\n";
    }
}

//
// DetermineBlendSymbol implementation
//

// Color matrix overrides blend color. Blend color is the default blend symbol.
void RewriteCSSFragmentShader::DetermineBlendSymbol::visitSymbol(TIntermSymbol* node)
{    
    const TString& symbol = node->getSymbol();
    if (symbol == kCSSColorMatrix) 
        mRewriter->blendSymbol = kCSSColorMatrix;
    else
        mRewriter->blendSymbol = kCSSBlendColor;
}

//
// RewriteCSSFragmentShader implementation
//

const char* const RewriteCSSFragmentShader::kGLFragColor = "gl_FragColor";
const char* const RewriteCSSFragmentShader::kCSSTextureUniform = "css_u_texture";

const char* const RewriteCSSFragmentShader::kCSSBlendColor = "css_BlendColor";
const char* const RewriteCSSFragmentShader::kCSSColorMatrix = "css_ColorMatrix";

void RewriteCSSFragmentShader::rewrite()
{
    RestrictGLFragColor restrictGLFragColor(this);
    root->traverse(&restrictGLFragColor);
    if (numErrors > 0)
        return;
    
    DetermineBlendSymbol determineBlendSymbol(this);
    root->traverse(&determineBlendSymbol);
    ASSERT(blendSymbol);
    
    insertTextureUniform();
    insertTexCoordVarying();
    insertBlendSymbolDeclaration();
    insertBlendingOp();
}

// Inserts something like "vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0)".
void RewriteCSSFragmentShader::insertBlendSymbolDeclaration()
{
    if (blendSymbol == kCSSColorMatrix)
        insertAtTopOfShader(createDeclaration(createGlobalMat4Initialization(kCSSColorMatrix, createMat4IdentityConstant())));
    else
        insertAtTopOfShader(createDeclaration(createGlobalVec4Initialization(kCSSBlendColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "uniform sampler2D css_u_texture".
void RewriteCSSFragmentShader::insertTextureUniform()
{
    insertAtTopOfShader(createDeclaration(createUniformSampler2D(kCSSTextureUniform)));
}

// Inserts "gl_FragColor = css_FragColor * texture2D(s_texture, v_texCoord)"
void RewriteCSSFragmentShader::insertBlendingOp()
{
    // TODO: Maybe add types to the function call, multiply, assign, etc. They don't seem to be necessary, but it might be good.
    TIntermBinary* rhs = createBinary(EOpMul, createGlobalVec4(kCSSBlendColor), createTexture2DCall(kCSSTextureUniform, kCSSTexCoordVarying));
    TIntermBinary* assign = createBinary(EOpAssign, createGlobalVec4(kGLFragColor), rhs);
    insertAtEndOfFunction(assign, findMainFunction());
}
