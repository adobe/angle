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
    TIntermAggregate* texture2DCall = createFunctionCall(kTexture2D, TType(EbtFloat, EbpUndefined, EvqTemporary, 4));
    
    TIntermSymbol* textureUniform = createSymbol(mTextureUniformName, TType(EbtSampler2D, EbpUndefined, EvqUniform));
    addArgument(texture2DCall, textureUniform);
    
    TIntermSymbol* texCoordVarying = createSymbol(texCoordVaryingName, TType(EbtFloat, EbpHigh, EvqAttribute, 2));
    addArgument(texture2DCall, texCoordVarying);
    
    return texture2DCall;
}

// Inserts "vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0);".
void RewriteCSSFragmentShader::insertBlendColorDeclaration()
{
    TIntermSymbol* blendColor = createSymbol(kBlendColor, TType(EbtFloat, EbpHigh, EvqGlobal, 4));
    TIntermConstantUnion* constant = createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f);
    TIntermAggregate* declaration = createDeclaration(blendColor, constant);
    insertAtBeginningOfShader(declaration);
}

// Inserts "mat4 css_ColorMatrix = mat4(1.0, 0.0, 0.0, 0.0 ...);".
void RewriteCSSFragmentShader::insertColorMatrixDeclaration()
{
    TIntermSymbol* colorMatrix = createSymbol(kColorMatrix, TType(EbtFloat, EbpHigh, EvqGlobal, 4, true));
    TIntermConstantUnion* identityMatrix = createMat4IdentityConstant();
    TIntermAggregate* declaration = createDeclaration(colorMatrix, identityMatrix);
    insertAtBeginningOfShader(declaration);
}

// Inserts "uniform sampler2D css_u_texture_XXX;".
void RewriteCSSFragmentShader::insertTextureUniformDeclaration()
{
    TIntermSymbol* textureUniform = createSymbol(mTextureUniformName, TType(EbtSampler2D, EbpUndefined, EvqUniform));
    TIntermAggregate* declaration = createDeclaration(textureUniform);
    insertAtBeginningOfShader(declaration);
}

// Inserts "varying vec2 css_v_texCoord;".
void RewriteCSSFragmentShader::insertTexCoordVaryingDeclaration()
{
    TIntermSymbol* texCoordVarying = createSymbol(texCoordVaryingName, TType(EbtFloat, EbpHigh, EvqVaryingIn, 2));
    TIntermAggregate* declaration = createDeclaration(texCoordVarying);    
    insertAtBeginningOfShader(declaration);
}

// Inserts "void main() {}" and returns the new main function.
TIntermAggregate* RewriteCSSFragmentShader::insertNewMainFunction()
{
    TIntermAggregate* newMainFunction = createFunction(kMain, TType(EbtVoid, EbpUndefined, EvqGlobal));
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
    const TOperator blendOp = EOpMul;

    TIntermAggregate* texture2DCall = createTexture2DCall(mTextureUniformName, texCoordVaryingName);
    
    TIntermTyped* blendOpLhs = NULL;
    if (usesColorMatrix) {
        TIntermSymbol* colorMatrix = createSymbol(kColorMatrix, TType(EbtFloat, EbpHigh, EvqGlobal, 4, true));
        blendOpLhs = createBinary(EOpMatrixTimesVector, colorMatrix, texture2DCall, TType(EbtFloat, EbpUndefined, EvqTemporary, 4));
    }
    else {
        blendOpLhs = texture2DCall;
    }

    TIntermTyped* assignmentRhs = NULL;
    if (usesBlendColor) {
        TIntermSymbol* blendColor = createSymbol(kColorMatrix, TType(EbtFloat, EbpHigh, EvqGlobal, 4));
        assignmentRhs = createBinary(blendOp, blendOpLhs, blendColor, TType(EbtFloat, EbpUndefined, EvqTemporary, 4));
    }
    else {
        assignmentRhs = blendOpLhs;
    }

    TIntermSymbol* fragColorBuiltin = createSymbol(kFragColor, TType(EbtFloat, EbpMedium, EvqFragColor, 4)); // TODO: Reference the symbol table for the type.
    TIntermBinary* assignment = createBinary(EOpAssign, fragColorBuiltin, assignmentRhs, TType(EbtFloat, EbpUndefined, EvqTemporary, 4));
    insertAtEndOfFunction(function, assignment);
}
