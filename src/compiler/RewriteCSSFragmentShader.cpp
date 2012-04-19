//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSFragmentShader.h"
#include "ParseHelper.h"

//
// ReplaceGLFragColor implementation
//

void RewriteCSSFragmentShader::ReplaceGLFragColor::visitSymbol(TIntermSymbol* node)
{
    if (node->getSymbol() == kGLFragColor) {  
        node->setId(0);
        node->getTypePointer()->setQualifier(EvqGlobal);
        node->setSymbol(kCSSGLFragColor);
    }
}

//
// RewriteCSSFragmentShader implementation
//

void RewriteCSSFragmentShader::rewrite()
{
    RewriteCSSShaderBase::rewrite();
    
    ReplaceGLFragColor replaceGLFragColor;
    GlobalParseContext->treeRoot->traverse(&replaceGLFragColor);
    
    insertTextureUniform();
    insertTexCoordVarying();
    insertCSSFragColorDeclaration();
    insertBlendingOp();
}

void RewriteCSSFragmentShader::insertCSSFragColorDeclaration()
{
    insertAtTopOfShader(createDeclaration(createGlobalVec4Initialization(kCSSGLFragColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "uniform sampler2D css_u_texture".
void RewriteCSSFragmentShader::insertTextureUniform()
{
    insertAtTopOfShader(createDeclaration(createUniformSampler2D(kCSSTextureUniformTexture)));
}

// TODO: Maybe add types to the function call, multiply, assign, etc. They don't seem to be necessary, but it might be good.
// Inserts "gl_FragColor = css_FragColor * texture2D(s_texture, v_texCoord)"
void RewriteCSSFragmentShader::insertBlendingOp()
{
    TIntermBinary* rhs = createBinary(EOpMul, createGlobalVec4(kCSSGLFragColor), createTexture2DCall(kCSSTextureUniformTexture, kCSSTexCoordVarying));
    TIntermBinary* assign = createBinary(EOpAssign, createGlobalVec4(kGLFragColor), rhs);
    insertAtEndOfFunction(assign, findMainFunction());
}
