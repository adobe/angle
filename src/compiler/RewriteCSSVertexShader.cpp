//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSVertexShader.h"
#include "ParseHelper.h"

const char* const RewriteCSSVertexShader::kTexCoordAttributePrefix = "css_TexCoordAttribute";

void RewriteCSSVertexShader::rewrite()
{
    RewriteCSSShaderBase::rewrite();
    
    insertTexCoordVaryingDeclaration();
    insertTexCoordAttribute();
    insertCSSTexCoordVaryingAssignment();
}

void RewriteCSSVertexShader::insertTexCoordAttribute()
{
    insertAtBeginningOfShader(createDeclaration(createAttributeVec2(texCoordAttributeName)));
}

void RewriteCSSVertexShader::insertCSSTexCoordVaryingAssignment()
{
    insertAtBeginningOfFunction(findFunction(kMain), createBinary(EOpAssign, createVaryingVec2(texCoordVaryingName), createAttributeVec2(texCoordAttributeName)));
}
