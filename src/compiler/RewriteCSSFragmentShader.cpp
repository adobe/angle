//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSFragmentShader.h"
#include "ParseHelper.h"

//
// Determines if a symbol is used.
//
class FindSymbolUsage : public TIntermTraverser
{
public:
    FindSymbolUsage(const TString& symbolName)
        : TIntermTraverser(true, false, false)
        , mSymbolName(symbolName)
        , mSymbolUsageFound(false) {}

    bool symbolUsageFound() { return mSymbolUsageFound; }
    
    virtual void visitSymbol(TIntermSymbol* node)
    {
        if (node->getSymbol() == mSymbolName)
            mSymbolUsageFound = true;
    }
    
    virtual bool visitBinary(Visit visit, TIntermBinary*) {return shouldKeepLooking();}
    virtual bool visitUnary(Visit visit, TIntermUnary*) {return shouldKeepLooking();}
    virtual bool visitSelection(Visit visit, TIntermSelection*) {return shouldKeepLooking();}
    virtual bool visitAggregate(Visit visit, TIntermAggregate*) {return shouldKeepLooking();}
    virtual bool visitLoop(Visit visit, TIntermLoop*) {return shouldKeepLooking();}
    virtual bool visitBranch(Visit visit, TIntermBranch*) {return shouldKeepLooking();}
    
private:
    bool shouldKeepLooking() { return !mSymbolUsageFound; }
    
    const TString& mSymbolName;
    bool mSymbolUsageFound;
};    

//
// Renames a function, including its declaration and any calls to it. 
//
class RenameFunction : public TIntermTraverser
{
public:
    RenameFunction(const TString& oldFunctionName, const TString& newFunctionName)
        : TIntermTraverser(true, false, false)
        , mOldFunctionName(oldFunctionName)
        , mNewFunctionName(newFunctionName) {}
    
    virtual bool visitAggregate(Visit visit, TIntermAggregate* node)
    {
        TOperator op = node->getOp();
        if ((op == EOpFunction || op == EOpFunctionCall) && node->getName() == mOldFunctionName)
            node->setName(mNewFunctionName);
        return true;
    }
    
private:
    const TString& mOldFunctionName;
    const TString& mNewFunctionName;
};

//
// RewriteCSSFragmentShader implementation
//

void RewriteCSSFragmentShader::rewrite()
{
    RewriteCSSShaderBase::rewrite();

    FindSymbolUsage findColorMatrixUsage(kColorMatrix);
    root->traverse(&findColorMatrixUsage);
    useColorMatrix = findColorMatrixUsage.symbolUsageFound();
    
    RenameFunction renameMain(kMain, kCSSMain);
    root->traverse(&renameMain);
    
    insertTextureUniform();
    insertTexCoordVarying();
    insertBlendSymbolDeclaration();
    insertBlendOp();
}

const char* const RewriteCSSFragmentShader::kCSSMain = "css_main(";
const char* const RewriteCSSFragmentShader::kFragColor = "gl_FragColor";
const char* const RewriteCSSFragmentShader::kTextureUniformPrefix = "css_TextureUniform";
const char* const RewriteCSSFragmentShader::kBlendColor = "css_BlendColor";
const char* const RewriteCSSFragmentShader::kColorMatrix = "css_ColorMatrix";

// Inserts something like "vec4 css_BlendColor = vec4(1.0, 1.0, 1.0, 1.0)".
void RewriteCSSFragmentShader::insertBlendSymbolDeclaration()
{
    if (useColorMatrix)
        insertAtTopOfShader(createDeclaration(createGlobalMat4Initialization(kColorMatrix, createMat4IdentityConstant())));
    else
        insertAtTopOfShader(createDeclaration(createGlobalVec4Initialization(kBlendColor, createVec4Constant(1.0f, 1.0f, 1.0f, 1.0f))));
}

// Inserts "uniform sampler2D css_u_texture_XXX".
void RewriteCSSFragmentShader::insertTextureUniform()
{
    insertAtTopOfShader(createDeclaration(createUniformSampler2D(textureUniformName)));
}

// Inserts "gl_FragColor = css_FragColor * texture2D(s_texture, v_texCoord)"
void RewriteCSSFragmentShader::insertBlendOp()
{
    // TODO(mvujovic): Maybe I should add types to the binary ops. They don't seem to be necessary, but maybe I'm missing something.
    TIntermSymbol* multiplySymbol = useColorMatrix ? createGlobalMat4(kColorMatrix) : createGlobalVec4(kBlendColor);
    TIntermBinary* rhs = createBinary(EOpMul, multiplySymbol, createTexture2DCall(textureUniformName, texCoordVaryingName));
    TIntermBinary* assign = createBinary(EOpAssign, createGlobalVec4(kFragColor), rhs);
    insertAtEndOfFunction(assign, findFunction(kCSSMain));
}
