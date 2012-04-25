//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSFragmentShader.h"
#include "ParseHelper.h"

//
// RewriteCSSFragmentShader implementation
//

void RewriteCSSFragmentShader::rewrite()
{
    RewriteCSSShaderBase::rewrite();
    
    RestrictFragColor restrictGLFragColor(this);
    root->traverse(&restrictGLFragColor);
    if (numErrors > 0)
        return;
        
    insertTextureUniform();
    insertTexCoordVarying();
    insertBlendSymbolDeclaration();
    insertBlendingOp();
}

const char* const RewriteCSSFragmentShader::kFragColor = "gl_FragColor";
const char* const RewriteCSSFragmentShader::kTextureUniformPrefix = "css_TextureUniform";

const char* const RewriteCSSFragmentShader::kBlendColor = "css_BlendColor";
const char* const RewriteCSSFragmentShader::kColorMatrix = "css_ColorMatrix";

// Inserts something like "vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0)".
void RewriteCSSFragmentShader::insertBlendSymbolDeclaration()
{
    if (blendSymbol == kColorMatrix)
        insertAtTopOfShader(createDeclaration(createGlobalMat4Initialization(kColorMatrix, createMat4IdentityConstant())));
    else
        insertAtTopOfShader(createDeclaration(createGlobalVec4Initialization(kBlendColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "uniform sampler2D css_u_texture_XXX".
void RewriteCSSFragmentShader::insertTextureUniform()
{
    insertAtTopOfShader(createDeclaration(createUniformSampler2D(textureUniformName)));
}

// Inserts "gl_FragColor = css_FragColor * texture2D(s_texture, v_texCoord)"
void RewriteCSSFragmentShader::insertBlendingOp()
{
    // TODO(mvujovic): Maybe I should add types to the binary ops. They don't seem to be necessary, but maybe I'm missing something.
    TIntermSymbol* multiplySymbol = NULL;
    if (blendSymbol == kColorMatrix)
        multiplySymbol = createGlobalMat4(kColorMatrix);
    else
        multiplySymbol = createGlobalVec4(kBlendColor);
    
    TIntermBinary* rhs = createBinary(EOpMul, multiplySymbol, createTexture2DCall(textureUniformName, texCoordVaryingName));
    TIntermBinary* assign = createBinary(EOpAssign, createGlobalVec4(kFragColor), rhs);
    insertAtEndOfFunction(assign, findMainFunction());
}
