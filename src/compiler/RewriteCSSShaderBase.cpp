//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSShaderBase.h"
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

bool RewriteCSSShaderBase::isSymbolUsed(const TString& symbolName)
{
    FindSymbolUsage findSymbolUsage(symbolName);
    root->traverse(&findSymbolUsage);
    return findSymbolUsage.symbolUsageFound();
}

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

void RewriteCSSShaderBase::renameFunction(const TString& oldFunctionName, const TString& newFunctionName)
{
    RenameFunction renameFunction(oldFunctionName, newFunctionName);
    root->traverse(&renameFunction);
}

//
// RewriteCSSShaderBase implementation
//

void RewriteCSSShaderBase::rewrite()
{
    createRootSequenceIfNeeded();
}

const char* const RewriteCSSShaderBase::kTexCoordVaryingPrefix = "css_TexCoordVarying";
const char* const RewriteCSSShaderBase::kTexture2D = "texture2D(s21;vf2;";
const char* const RewriteCSSShaderBase::kMain = "main(";

TIntermConstantUnion* RewriteCSSShaderBase::createVec4Constant(float x, float y, float z, float w)
{
    ConstantUnion* constantArray = new ConstantUnion[4];
    constantArray[0].setFConst(x);
    constantArray[1].setFConst(y);
    constantArray[2].setFConst(z);
    constantArray[3].setFConst(w);
    return new TIntermConstantUnion(constantArray, TType(EbtFloat, EbpUndefined, EvqConst, 4));    
}

TIntermConstantUnion* RewriteCSSShaderBase::createMat4IdentityConstant()
{
    ConstantUnion* constantArray = new ConstantUnion[4 * 4];
    for (int i = 0; i < 4 * 4; i++)
        constantArray[i].setFConst(0.0);
    
    constantArray[0].setFConst(1.0);
    constantArray[5].setFConst(1.0);
    constantArray[10].setFConst(1.0);
    constantArray[15].setFConst(1.0);
    
    return new TIntermConstantUnion(constantArray, TType(EbtFloat, EbpUndefined, EvqConst, 4, true));    
}

// TODO(mvujovic): Is symbol id 0 ok? Or do we need to to insert these in the symbol table?
TIntermSymbol* RewriteCSSShaderBase::createGlobalVec4(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqGlobal, 4));
}

TIntermSymbol* RewriteCSSShaderBase::createGlobalMat4(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqGlobal, 4, true));
}

TIntermSymbol* RewriteCSSShaderBase::createUniformSampler2D(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtSampler2D, EbpUndefined, EvqUniform));
}

TIntermSymbol* RewriteCSSShaderBase::createVaryingVec2(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqVaryingIn, 2));
}

TIntermSymbol* RewriteCSSShaderBase::createAttributeVec2(const TString& name)
{
    return new TIntermSymbol(0, name, TType(EbtFloat, EbpHigh, EvqAttribute, 2));
}

TIntermAggregate* RewriteCSSShaderBase::createFunctionCall(const TString& name)
{
    TIntermAggregate* functionCall = new TIntermAggregate(EOpFunctionCall);
    functionCall->setName(name);
    return functionCall;
}

TIntermBinary* RewriteCSSShaderBase::createBinary(TOperator op, TIntermTyped* left, TIntermTyped* right)
{
    TIntermBinary* binary = new TIntermBinary(op);
    binary->setLeft(left);
    binary->setRight(right);
    return binary; 
}

TIntermAggregate* RewriteCSSShaderBase::createTexture2DCall(const TString& textureUniformName, const TString& texCoordVaryingName)
{
    TIntermAggregate* texture2DCall = createFunctionCall(kTexture2D); // TODO(mvujovic): Should I be pool allocating strings?
    addArgument(createUniformSampler2D(textureUniformName), texture2DCall);
    addArgument(createVaryingVec2(texCoordVaryingName), texture2DCall);
    return texture2DCall;
}

TIntermAggregate* RewriteCSSShaderBase::createDeclaration(TIntermNode* child)
{
    TIntermAggregate* declaration = new TIntermAggregate(EOpDeclaration);
    declaration->getSequence().push_back(child);
    return declaration;    
}

TIntermBinary* RewriteCSSShaderBase::createGlobalVec4Initialization(const TString& symbolName, TIntermTyped* rhs)
{
    TIntermBinary* initialization = createBinary(EOpInitialize, createGlobalVec4(symbolName), rhs);
    initialization->setType(TType(EbtFloat, EbpHigh, EvqTemporary, 4)); // TODO(mvujovic): What precision?
    return initialization;
}

TIntermBinary* RewriteCSSShaderBase::createGlobalMat4Initialization(const TString& symbolName, TIntermTyped* rhs)
{
    TIntermBinary* initialization = createBinary(EOpInitialize, createGlobalMat4(symbolName), rhs);
    initialization->setType(TType(EbtFloat, EbpHigh, EvqTemporary, 4, true));
    return initialization;
}

TIntermAggregate* RewriteCSSShaderBase::createVoidFunction(const TString& name)
{
    TIntermAggregate* function = new TIntermAggregate(EOpFunction);
    function->setName(name);
    function->setType(TType(EbtVoid, EbpUndefined, EvqGlobal));
    
    TIntermSequence& paramsAndBody = function->getSequence();
    
    TIntermAggregate* parameters = new TIntermAggregate(EOpParameters);
    paramsAndBody.push_back(parameters);
    
    TIntermAggregate* body = new TIntermAggregate(EOpSequence);
    paramsAndBody.push_back(body);
    
    return function;
}

void RewriteCSSShaderBase::addArgument(TIntermNode* argument, TIntermAggregate* functionCall)
{
    functionCall->getSequence().push_back(argument);
}

// Inserts "varying vec2 css_v_texCoord".
void RewriteCSSShaderBase::insertTexCoordVaryingDeclaration()
{
    insertAtTopOfShader(createDeclaration(createVaryingVec2(texCoordVaryingName)));
}

void RewriteCSSShaderBase::insertAtTopOfShader(TIntermNode* node)
{
    TIntermSequence& globalSequence = root->getAsAggregate()->getSequence();
    globalSequence.insert(globalSequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfShader(TIntermNode* node)
{
    root->getAsAggregate()->getSequence().push_back(node);
}

void RewriteCSSShaderBase::insertAtTopOfFunction(TIntermNode* node, TIntermAggregate* function)
{
    TIntermSequence& bodySequence = getOrCreateFunctionBody(function)->getSequence();
    bodySequence.insert(bodySequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfFunction(TIntermNode* node, TIntermAggregate* function)
{
    getOrCreateFunctionBody(function)->getSequence().push_back(node);
}

// If there is only a main() function in the global scope, the main function will be the tree root.
// However, we'd like to insert declarations above the main function, so we must wrap it in a sequence.
// The new tree root must be that root sequence.
void RewriteCSSShaderBase::createRootSequenceIfNeeded()
{
    TIntermAggregate* rootAggregate = root->getAsAggregate();
 
    // The root should be a sequence or a function declaration, both of which are aggregate nodes.
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence || rootAggregate->getOp() == EOpFunction);
    
    if (rootAggregate->getOp() == EOpFunction) {
        // If the tree root is a function declaration, it should be the main function.
        ASSERT(rootAggregate->getName() == kMain);
        
        TIntermAggregate* newRoot = new TIntermAggregate(EOpSequence);
        TIntermSequence& sequence = newRoot->getSequence();
        sequence.push_back(root);
        root = newRoot;
    }
}

TIntermAggregate* RewriteCSSShaderBase::getOrCreateFunctionBody(TIntermAggregate* function)
{
    TIntermAggregate* body = NULL;
    TIntermSequence& paramsAndBody = function->getSequence();
    
    // The function should have parameters and may have a body.
    ASSERT(paramsAndBody.size() == 1 || paramsAndBody.size() == 2);
    
    if (paramsAndBody.size() == 2) {
        body = paramsAndBody[1]->getAsAggregate();
    } else {
        // Make a function body if necessary.
        body = new TIntermAggregate(EOpSequence);
        paramsAndBody.push_back(body);
    }
    
    // The function body should be an aggregate node.
    ASSERT(body);

    return body;
}

// TODO: Maybe find the main function once and cache it.
TIntermAggregate* RewriteCSSShaderBase::findFunction(const TString& name)
{
    TIntermSequence& rootSequence = root->getAsAggregate()->getSequence();
    for (TIntermSequence::const_iterator iter = rootSequence.begin(); iter != rootSequence.end(); ++iter) {
        TIntermNode* node = *iter;
        TIntermAggregate* aggregate = node->getAsAggregate();
        if (aggregate && aggregate->getOp() == EOpFunction && aggregate->getName() == name)
            return aggregate;
    }
    return NULL;
}
