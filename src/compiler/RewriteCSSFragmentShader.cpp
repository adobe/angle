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

    usesColorMatrix = isSymbolUsed(kColorMatrix);
    
    insertTextureUniformDeclaration();
    insertTexCoordVaryingDeclaration();
    insertBlendColorDeclaration();
    if (usesColorMatrix)
        insertColorMatrixDeclaration();
    renameFunction(kMain, userMainFunctionName);
    insertNewMainFunction();
    insertUserMainFunctionCall();
    insertBlendOp();
}

const char* const RewriteCSSFragmentShader::kBlendColor = "css_BlendColor";
const char* const RewriteCSSFragmentShader::kColorMatrix = "css_ColorMatrix";
const char* const RewriteCSSFragmentShader::kTextureUniformPrefix = "css_u_texture";
const char* const RewriteCSSFragmentShader::kUserMainFunctionPrefix = "css_main";
const char* const RewriteCSSFragmentShader::kFragColor = "gl_FragColor";

// Inserts "vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0)".
void RewriteCSSFragmentShader::insertBlendColorDeclaration()
{
    insertAtBeginningOfShader(createDeclaration(createVec4GlobalInitialization(kBlendColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "mat4 css_ColorMatrix = mat4(1.0, 0.0, 0.0, 0.0 ...)".
void RewriteCSSFragmentShader::insertColorMatrixDeclaration()
{
    insertAtBeginningOfShader(createDeclaration(createMat4GlobalInitialization(kColorMatrix, createMat4IdentityConstant())));
}

// Inserts "uniform sampler2D css_u_texture_XXX".
void RewriteCSSFragmentShader::insertTextureUniformDeclaration()
{
    insertAtBeginningOfShader(createDeclaration(createSampler2DUniform(textureUniformName)));
}

// Inserts "void main() {}".
void RewriteCSSFragmentShader::insertNewMainFunction()
{
    insertAtEndOfShader(createVoidFunction(kMain));
}

void RewriteCSSFragmentShader::insertUserMainFunctionCall()
{
    insertAtBeginningOfFunction(findFunction(kMain), createFunctionCall(userMainFunctionName));
}

// If css_ColorMatrix is used, inserts "gl_FragColor = css_ColorMatrix * texture2D(s_texture, v_texCoord) <BLEND OP> css_FragColor"
// Otherwise, inserts "gl_FragColor = texture2D(s_texture, v_texCoord) <BLEND OP> css_FragColor "
void RewriteCSSFragmentShader::insertBlendOp()
{
    // FIXME(mvujovic): Eventually, we'd like to support other blend operations besides multiply.
    TOperator blendOp = EOpMul;
    
    TIntermTyped* blendOpLhs = NULL;
    TIntermAggregate* texture2DCall = createTexture2DCall(textureUniformName, texCoordVaryingName);
    if (usesColorMatrix)
        blendOpLhs = createBinaryWithVec4Result(EOpMatrixTimesVector, createMat4Global(kColorMatrix), texture2DCall);
    else
        blendOpLhs = texture2DCall;
    
    TIntermBinary* assignmentRhs = createBinaryWithVec4Result(blendOp, blendOpLhs, createVec4Global(kBlendColor));
    TIntermBinary* assignment = createBinaryWithVec4Result(EOpAssign, createVec4Global(kFragColor), assignmentRhs);
    insertAtEndOfFunction(findFunction(kMain), assignment);
}
