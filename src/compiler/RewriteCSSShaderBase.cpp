//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/FindSymbolUsage.h"
#include "compiler/RenameFunction.h"
#include "compiler/RewriteCSSShaderBase.h"

// TODO: Update gyp.

// TODO: Move these.

//
// RewriteCSSShaderBase implementation
//

void RewriteCSSShaderBase::rewrite()
{
    createRootSequenceIfNeeded();
}

const char* const RewriteCSSShaderBase::kTexCoordVaryingPrefix = "css_v_texCoord";
const char* const RewriteCSSShaderBase::kMain = "main(";

void RewriteCSSShaderBase::insertAtBeginningOfShader(TIntermNode* node)
{
    TIntermAggregate* rootAggregate = root->getAsAggregate();
    
    // Assert that the tree root is a sequence (i.e. createRootSequenceIfNeeded was called).
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence);
    
    TIntermSequence& rootSequence = rootAggregate->getSequence();
    rootSequence.insert(rootSequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfShader(TIntermNode* node)
{
    TIntermAggregate* rootAggregate = root->getAsAggregate();
    
    // Assert that the tree root is a sequence (i.e. createRootSequenceIfNeeded was called).
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence);
    
    rootAggregate->getSequence().push_back(node);
}

void RewriteCSSShaderBase::insertAtBeginningOfFunction(TIntermAggregate* function, TIntermNode* node)
{
    TIntermSequence& bodySequence = getOrCreateFunctionBody(function)->getSequence();
    bodySequence.insert(bodySequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfFunction(TIntermAggregate* function, TIntermNode* node)
{
    getOrCreateFunctionBody(function)->getSequence().push_back(node);
}

// The tree root comes in as either a sequence or a main function declaration.
// If the tree root is a main function declaration, this method creates a new sequence,
// puts the main function declaration inside it, and changes the tree root to point to
// the new sequence.
// Thus, after this function is called, the tree root can only be a sequence.
void RewriteCSSShaderBase::createRootSequenceIfNeeded()
{
    TIntermAggregate* rootAggregate = root->getAsAggregate();

    // The root should come in as either a sequence or a function declaration, both of which are aggregate nodes.
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence || rootAggregate->getOp() == EOpFunction);

    if (rootAggregate->getOp() == EOpFunction) {
        // If the tree root is a function declaration, it should be the main function.
        // Previous compiler steps should have already thrown an error if there is no main function.
        ASSERT(rootAggregate->getName() == kMain);

        TIntermAggregate* newRoot = new TIntermAggregate(EOpSequence);
        TIntermSequence& sequence = newRoot->getSequence();
        sequence.push_back(root);
        root = newRoot;
    }
}

// A function in the shader is an aggregate node that can contain two children in its sequence,
// including a parameter list node and function body node.
// A function with an empty body will not have a body node (e.g. "void main() {}").
// If the function does not have a body node, this method will create one for it.
// If the function does have a body node, this method will just return it.
TIntermAggregate* RewriteCSSShaderBase::getOrCreateFunctionBody(TIntermAggregate* function)
{
    TIntermAggregate* body = NULL;
    TIntermSequence& paramsAndBody = function->getSequence();

    // Functions always have parameters nodes. They might also have a body node.
    ASSERT(paramsAndBody.size() >= 1 || paramsAndBody.size() <= 2);

    if (paramsAndBody.size() == 2) {
        body = paramsAndBody[1]->getAsAggregate();
        
        // If a body node exists, it should be an aggregate sequence node.
        ASSERT(body);
        ASSERT(body->getOp() == EOpSequence);
    } else {
        // If a body node doesn't exist, make one.
        body = new TIntermAggregate(EOpSequence);
        paramsAndBody.push_back(body);
    }
    
    return body;
}

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

bool RewriteCSSShaderBase::isSymbolUsed(const TString& symbolName)
{
    FindSymbolUsage findSymbolUsage(symbolName);
    root->traverse(&findSymbolUsage);
    return findSymbolUsage.symbolUsageFound();
}

void RewriteCSSShaderBase::renameFunction(const TString& oldFunctionName, const TString& newFunctionName)
{
    RenameFunction renameFunction(oldFunctionName, newFunctionName);
    root->traverse(&renameFunction);
}

const TType& RewriteCSSShaderBase::getBuiltinType(const TString& builtinName)
{
    TSymbol* builtinSymbol = symbolTable.find(builtinName);
    ASSERT(builtinSymbol->isVariable());
    TVariable* builtinVariable = static_cast<TVariable*>(builtinSymbol);
    return builtinVariable->getType();
}
