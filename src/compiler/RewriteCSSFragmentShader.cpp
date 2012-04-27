//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSFragmentShader.h"

//
// RewriteCSSFragmentShader implementation
//

void RewriteCSSFragmentShader::rewrite()
{
    RewriteCSSShaderBase::rewrite();

    bool usesBlendColor = isSymbolUsed(kBlendColor);
    bool usesColorMatrix = isSymbolUsed(kColorMatrix);

    insertTextureUniformDeclaration();
    insertTexCoordVaryingDeclaration();
    if (usesBlendColor)
        insertBlendColorDeclaration();
    if (usesColorMatrix)
        insertColorMatrixDeclaration();
    
    renameFunction(kMain, mUserMainFunctionName);
    
    TIntermAggregate* newMainFunction = insertNewMainFunction();
    insertUserMainFunctionCall(newMainFunction);
    insertBlendOp(newMainFunction, usesBlendColor, usesColorMatrix);
}

const char* const RewriteCSSFragmentShader::kBlendColor = "css_BlendColor";
const char* const RewriteCSSFragmentShader::kColorMatrix = "css_ColorMatrix";
const char* const RewriteCSSFragmentShader::kTextureUniformPrefix = "css_u_texture";
const char* const RewriteCSSFragmentShader::kUserMainFunctionPrefix = "css_main";
const char* const RewriteCSSFragmentShader::kFragColor = "gl_FragColor";
const char* const RewriteCSSFragmentShader::kTexture2D = "texture2D(s21;vf2;";

TIntermAggregate* RewriteCSSFragmentShader::createTexture2DCall(const TString& mTextureUniformName, const TString& texCoordVaryingName)
{
    TIntermAggregate* texture2DCall = createFunctionCall(kTexture2D, TType(EbtFloat, EbpUndefined, EvqTemporary, 4));   // TODO: Double check precision.
    addArgument(texture2DCall, createSampler2DUniform(mTextureUniformName));
    addArgument(texture2DCall, createVec2Varying(texCoordVaryingName));
    return texture2DCall;
}

// Inserts "vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0);".
void RewriteCSSFragmentShader::insertBlendColorDeclaration()
{
    insertAtBeginningOfShader(createDeclaration(createVec4GlobalInitialization(kBlendColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "mat4 css_ColorMatrix = mat4(1.0, 0.0, 0.0, 0.0 ...);".
void RewriteCSSFragmentShader::insertColorMatrixDeclaration()
{
    insertAtBeginningOfShader(createDeclaration(createMat4GlobalInitialization(kColorMatrix, createMat4IdentityConstant())));
}

// Inserts "uniform sampler2D css_u_texture_XXX;".
void RewriteCSSFragmentShader::insertTextureUniformDeclaration()
{
    insertAtBeginningOfShader(createDeclaration(createSampler2DUniform(mTextureUniformName)));
}

// Inserts "void main() {}" and returns the new main function.
TIntermAggregate* RewriteCSSFragmentShader::insertNewMainFunction()
{
    TIntermAggregate* newMainFunction = createVoidFunction(kMain);
    insertAtEndOfShader(newMainFunction);
    return newMainFunction;
}

// Inserts "css_mainXXX();" at the beginning of the passed-in function.
void RewriteCSSFragmentShader::insertUserMainFunctionCall(TIntermAggregate* function)
{
    insertAtBeginningOfFunction(function, createFunctionCall(mUserMainFunctionName, TType(EbtVoid, EbpUndefined, EvqTemporary)));
}

// Inserts "gl_FragColor = (css_ColorMatrix * texture2D(css_u_textureXXX, css_v_texCoordXXX)) <BLEND OP> css_FragColor;"
// at the beginning of the passed-in function if both css_BlendColor and css_ColorMatrix are used.
void RewriteCSSFragmentShader::insertBlendOp(TIntermAggregate* function, bool usesBlendColor, bool usesColorMatrix)
{
    // TODO(mvujovic): In the future, we'll support other blend operations besides multiply.
    TOperator blendOp = EOpMul;

    TIntermTyped* blendOpLhs = NULL;
    TIntermAggregate* texture2DCall = createTexture2DCall(mTextureUniformName, texCoordVaryingName);
    if (usesColorMatrix)
        blendOpLhs = createBinaryWithVec4Result(EOpMatrixTimesVector, createMat4Global(kColorMatrix), texture2DCall);
    else
        blendOpLhs = texture2DCall;

    TIntermTyped* assignmentRhs = NULL;
    if (usesBlendColor)
        assignmentRhs = createBinaryWithVec4Result(blendOp, blendOpLhs, createVec4Global(kBlendColor));
    else
        assignmentRhs = blendOpLhs;

    TIntermBinary* assignment = createBinaryWithVec4Result(EOpAssign, createVec4Global(kFragColor), assignmentRhs);
    insertAtEndOfFunction(function, assignment);
}
