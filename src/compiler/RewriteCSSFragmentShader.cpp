//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSFragmentShader.h"
#include "ParseHelper.h"

static const char* kGLFragColor = "gl_FragColor";
static const char* kCSSGLFragColor = "css_gl_FragColor";
static const char* kCSSUTexture = "css_u_texture";
static const char* kCSSVTexCoord = "css_v_texCoord";
static const char* kTexture2D = "texture2D(s21;vf2;";
static const char* kMain = "main(";

void RewriteCSSFragmentShader::rewrite()
{
    insertTextureUniform();
    insertTexCoordVarying();
    insertCSSFragColorDeclaration();
    
    // Replace all kGLFragColor with kCSSGLFragColor.
    GlobalParseContext->treeRoot->traverse(this);
}

void RewriteCSSFragmentShader::insertCSSFragColorDeclaration()
{
    insertAtTopOfShader(createDeclaration(createGlobalVec4Initialization(kCSSGLFragColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "uniform sampler2D css_u_texture".
void RewriteCSSFragmentShader::insertTextureUniform()
{
    insertAtTopOfShader(createDeclaration(createUniformSampler2D(kCSSUTexture)));
}

// TODO: Maybe add types to the function call, multiply, assign, etc. They don't seem to be necessary, but it might be good.
// Inserts "gl_FragColor = css_FragColor * texture2D(s_texture, v_texCoord)"
void RewriteCSSFragmentShader::insertBlendingOp(TIntermAggregate* mainFunction)
{
    TIntermBinary* rhs = createBinary(EOpMul, createGlobalVec4(kCSSGLFragColor), createTexture2DCall(kCSSUTexture, kCSSVTexCoord));
    TIntermBinary* assign = createBinary(EOpAssign, createGlobalVec4(kGLFragColor), rhs);
    insertAtEndOfFunction(assign, mainFunction);
}

void RewriteCSSFragmentShader::visitSymbol(TIntermSymbol* node)
{
    if (node->getSymbol() == kGLFragColor) {  
        node->setId(0);
        node->getTypePointer()->setQualifier(EvqGlobal);
        node->setSymbol(kCSSGLFragColor);
    }
}

bool RewriteCSSFragmentShader::visitBinary(Visit visit, TIntermBinary* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitUnary(Visit visit, TIntermUnary* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitSelection(Visit visit, TIntermSelection* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitAggregate(Visit visit, TIntermAggregate* node)
{
    if (node->getOp() == EOpFunction && node->getName() == kMain)
        insertBlendingOp(node);
    
    return true;
}

bool RewriteCSSFragmentShader::visitLoop(Visit visit, TIntermLoop* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitBranch(Visit visit, TIntermBranch* node)
{
    return true;
}
