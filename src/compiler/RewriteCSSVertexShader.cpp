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
    insertTexCoordVaryingAssignment();
}

void RewriteCSSVertexShader::insertTexCoordAttribute()
{
    insertAtBeginningOfShader(createDeclaration(createVec2Attribute(texCoordAttributeName)));
}

void RewriteCSSVertexShader::insertTexCoordVaryingAssignment()
{
    insertAtBeginningOfFunction(findFunction(kMain), createBinaryWithVec2Result(EOpAssign, createVec2Varying(texCoordVaryingName), createVec2Attribute(texCoordAttributeName)));
}
