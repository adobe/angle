//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSVertexShader.h"
#include "ParseHelper.h"

const char* kCSSTexCoordAttribute = "css_a_texCoord";

void RewriteCSSVertexShader::rewrite()
{
    insertTexCoordVarying();
    insertTexCoordAttribute();
    insertCSSTexCoordVaryingAssignment();
    
    GlobalParseContext->treeRoot->traverse(this);
}

void RewriteCSSVertexShader::insertTexCoordAttribute()
{
    insertAtTopOfShader(createDeclaration(createAttributeVec2(kCSSTexCoordAttribute)));
}

void RewriteCSSVertexShader::insertCSSTexCoordVaryingAssignment()
{
    // TODO: Implement.
}

void RewriteCSSVertexShader::visitSymbol(TIntermSymbol* node)
{
}

bool RewriteCSSVertexShader::visitBinary(Visit visit, TIntermBinary* node)
{
    return true;
}

bool RewriteCSSVertexShader::visitUnary(Visit visit, TIntermUnary* node)
{
    return true;
}

bool RewriteCSSVertexShader::visitSelection(Visit visit, TIntermSelection* node)
{
    return true;
}

bool RewriteCSSVertexShader::visitAggregate(Visit visit, TIntermAggregate* node)
{   
    return true;
}

bool RewriteCSSVertexShader::visitLoop(Visit visit, TIntermLoop* node)
{
    return true;
}

bool RewriteCSSVertexShader::visitBranch(Visit visit, TIntermBranch* node)
{
    return true;
}
