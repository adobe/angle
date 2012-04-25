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

    useColorMatrix = isSymbolUsed(kColorMatrix);
    
    insertTextureUniformDeclaration();
    insertTexCoordVaryingDeclaration();
    insertBlendSymbolDeclaration();
    renameFunction(kMain, cssMainFunctionName);
    insertNewMainFunction();
    insertCSSMainCall();
    insertBlendOp();
}

const char* const RewriteCSSFragmentShader::kCSSMainPrefix = "css_Main";
const char* const RewriteCSSFragmentShader::kFragColor = "gl_FragColor";
const char* const RewriteCSSFragmentShader::kTextureUniformPrefix = "css_TextureUniform";
const char* const RewriteCSSFragmentShader::kBlendColor = "css_BlendColor";
const char* const RewriteCSSFragmentShader::kColorMatrix = "css_ColorMatrix";

// Inserts something like "vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0)".
void RewriteCSSFragmentShader::insertBlendSymbolDeclaration()
{
    if (useColorMatrix)
        insertAtTopOfShader(createDeclaration(createGlobalMat4Initialization(kColorMatrix, createMat4IdentityConstant())));
    else
        insertAtTopOfShader(createDeclaration(createGlobalVec4Initialization(kBlendColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "uniform sampler2D css_u_texture_XXX".
void RewriteCSSFragmentShader::insertTextureUniformDeclaration()
{
    insertAtTopOfShader(createDeclaration(createUniformSampler2D(textureUniformName)));
}

// Inserts "void main() {}".
void RewriteCSSFragmentShader::insertNewMainFunction()
{
    insertAtEndOfShader(createVoidFunction(kMain));
}

void RewriteCSSFragmentShader::insertCSSMainCall()
{
    insertAtTopOfFunction(createFunctionCall(cssMainFunctionName), findFunction(kMain));
}

// Inserts "gl_FragColor = css_FragColor * texture2D(s_texture, v_texCoord)"
void RewriteCSSFragmentShader::insertBlendOp()
{
    // TODO(mvujovic): Maybe I should add types to the binary ops. They don't seem to be necessary, but maybe I'm missing something.
    TIntermSymbol* multiplySymbol = useColorMatrix ? createGlobalMat4(kColorMatrix) : createGlobalVec4(kBlendColor);
    TIntermBinary* rhs = createBinary(EOpMul, multiplySymbol, createTexture2DCall(textureUniformName, texCoordVaryingName));
    TIntermBinary* assign = createBinary(EOpAssign, createGlobalVec4(kFragColor), rhs);
    insertAtEndOfFunction(assign, findFunction(kMain));
}
