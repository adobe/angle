//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSVertexShader.h"
#include "ParseHelper.h"

const char* const RewriteCSSVertexShader::kTexCoordAttributeName = "a_texCoord";

void RewriteCSSVertexShader::rewrite()
{
    RewriteCSSShaderBase::rewrite();

    // FIXME(mvujovic): We rely on the shader defining a_texCoord.
    // In the future, all of the attributes and uniforms from the CSS Shaders spec will be built-ins,
    // and we will insert declarations for them here.
    insertTexCoordVaryingDeclaration();
    insertTexCoordVaryingAssignment();
}

// Inserts "css_v_texCoordXXX = a_texCoord;" as the first line of the main function.
void RewriteCSSVertexShader::insertTexCoordVaryingAssignment()
{
    insertAtBeginningOfFunction(findFunction(kMain), createBinaryWithVec2Result(EOpAssign, createVec2Varying(texCoordVaryingName), createVec2Attribute(kTexCoordAttributeName)));
}
