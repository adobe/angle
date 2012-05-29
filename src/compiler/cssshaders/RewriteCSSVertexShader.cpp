//
// Copyright (c) 2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/cssshaders/RewriteCSSShaderHelper.h"
#include "compiler/cssshaders/RewriteCSSVertexShader.h"
#include "compiler/cssshaders/SearchSymbols.h"

using namespace RewriteCSSShaderHelper;

const char* const RewriteCSSVertexShader::kUserTexCoordAttrName = "a_texCoord";
const char* const RewriteCSSVertexShader::kHiddenTexCoordAttrPrefix = "css_a_texCoord";

void RewriteCSSVertexShader::rewrite()
{
    RewriteCSSShaderBase::rewrite();

    insertTexCoordVaryingDeclaration();

    SearchSymbols search(1, kUserTexCoordAttrName);
    getRootAggregate()->traverse(&search);

    bool isTexCoordAttrUserDefined = search.didFindSymbol(kUserTexCoordAttrName);
    if (isTexCoordAttrUserDefined) {
        insertTexCoordVaryingAssignment(kUserTexCoordAttrName);
    } else {
        insertTexCoordAttrDeclaration();
        insertTexCoordVaryingAssignment(mHiddenTexCoordAttrName);
    }
}

// Inserts "attribute vec2 css_a_texCoordXXX;".
void RewriteCSSVertexShader::insertTexCoordAttrDeclaration()
{
    TIntermSymbol* texCoordAttr = createSymbol(mHiddenTexCoordAttrName, vec2Type(EvqAttribute));
    TIntermAggregate* declaration = createDeclaration(texCoordAttr);
    insertAtBeginningOfShader(declaration);
}

// Inserts "varying vec2 css_v_texCoordXXX;".
void RewriteCSSVertexShader::insertTexCoordVaryingDeclaration()
{
    TIntermSymbol* texCoordVarying = createSymbol(getTexCoordVaryingName(), vec2Type(EvqVaryingIn));
    TIntermAggregate* declaration = createDeclaration(texCoordVarying);
    insertAtBeginningOfShader(declaration);
}

// Inserts "css_v_texCoordXXX = a_texCoord;" or "css_v_texCoordXXX = css_a_texCoordXXX"
// as the first line of the main function.
void RewriteCSSVertexShader::insertTexCoordVaryingAssignment(const TString& texCoordAttrName)
{
    TIntermSymbol* texCoordVarying = createSymbol(getTexCoordVaryingName(), vec2Type(EvqVaryingIn));
    TIntermSymbol* texCoordAttribute = createSymbol(texCoordAttrName, vec2Type(EvqAttribute));
    TIntermBinary* assignment = createBinary(EOpAssign,
                                             texCoordVarying,
                                             texCoordAttribute,
                                             vec2Type(EvqTemporary));
    insertAtBeginningOfFunction(findFunction(kMain), assignment);
}
