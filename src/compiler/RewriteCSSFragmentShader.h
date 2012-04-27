//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_REWRITE_CSS_FRAGMENT_SHADER
#define COMPILER_REWRITE_CSS_FRAGMENT_SHADER

#include "GLSLANG/ShaderLang.h"

#include "compiler/RewriteCSSShaderBase.h"

//
// Rewrites a CSS fragment shader's intermediate tree into a valid GLSL shader.
//

// Example original fragment shader:
/*

 void main() {
 css_BlendColor = vec4(0.5);
 }

 */

// Example rewritten fragment shader:
// "XXX" represents the hidden symbol suffix.
/*

 vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0);
 varying vec2 css_v_texCoordXXX;
 uniform sampler2D css_u_textureXXX;
 void css_mainXXX(){
 (css_BlendColor = vec4(0.5, 0.5, 0.5, 0.5));
 }
 void main(){
 css_mainXXX();
 (gl_FragColor = (texture2D(css_u_textureXXX, css_v_texCoordXXX) * css_BlendColor));
 }

 */

class TInfoSinkBase;

class RewriteCSSFragmentShader : public RewriteCSSShaderBase {
public:
    RewriteCSSFragmentShader(TIntermNode* treeRoot, const TString& hiddenSymbolSuffix)
        : RewriteCSSShaderBase(treeRoot, hiddenSymbolSuffix)
        , textureUniformName(kTextureUniformPrefix + hiddenSymbolSuffix)
        , userMainFunctionName(kUserMainFunctionPrefix + hiddenSymbolSuffix + "(")
        , usesBlendColor(false)
        , usesColorMatrix(false) {}

    virtual void rewrite();

private:
    static const char* const kBlendColor;
    static const char* const kColorMatrix;
    static const char* const kTextureUniformPrefix;
    static const char* const kUserMainFunctionPrefix;
    static const char* const kFragColor;

    void insertBlendColorDeclaration();
    void insertColorMatrixDeclaration();
    void insertTextureUniformDeclaration();
    void insertNewMainFunction();
    void insertUserMainFunctionCall();
    void insertBlendOp();

    TString textureUniformName;
    TString userMainFunctionName;
    bool usesBlendColor;
    bool usesColorMatrix;
};

#endif  // COMPILER_REWRITE_CSS_FRAGMENT_SHADER
