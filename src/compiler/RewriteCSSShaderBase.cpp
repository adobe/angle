//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/FindSymbolUsage.h"
#include "compiler/RenameFunction.h"
#include "compiler/RewriteCSSShaderBase.h"
#include "compiler/RewriteCSSShaderHelper.h"

using namespace RewriteCSSShaderHelper;

// TODO: Update gyp.

void RewriteCSSShaderBase::rewrite()
{
    createRootSequenceIfNeeded();
}

const char* const RewriteCSSShaderBase::kTexCoordVaryingPrefix = "css_v_texCoord";
const char* const RewriteCSSShaderBase::kMain = "main(";

void RewriteCSSShaderBase::insertAtBeginningOfShader(TIntermNode* node)
{
    TIntermAggregate* rootAggregate = mRoot->getAsAggregate();
    
    // Assert that the tree mRoot is a sequence (i.e. createRootSequenceIfNeeded was called).
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence);
    
    TIntermSequence& rootSequence = rootAggregate->getSequence();
    rootSequence.insert(rootSequence.begin(), node);
}

void RewriteCSSShaderBase::insertAtEndOfShader(TIntermNode* node)
{
    TIntermAggregate* rootAggregate = mRoot->getAsAggregate();
    
    // Assert that the tree mRoot is a sequence (i.e. createRootSequenceIfNeeded was called).
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence);
    
    rootAggregate->getSequence().push_back(node);
}

// The tree mRoot comes in as either a sequence or a main function declaration.
// If the tree mRoot is a main function declaration, this method creates a new sequence,
// puts the main function declaration inside it, and changes the tree mRoot to point to
// the new sequence.
// Thus, after this function is called, the tree mRoot can only be a sequence.
void RewriteCSSShaderBase::createRootSequenceIfNeeded()
{
    TIntermAggregate* rootAggregate = mRoot->getAsAggregate();

    // The mRoot should come in as either a sequence or a function declaration, both of which are aggregate nodes.
    ASSERT(rootAggregate);
    ASSERT(rootAggregate->getOp() == EOpSequence || rootAggregate->getOp() == EOpFunction);

    if (rootAggregate->getOp() == EOpFunction) {
        // If the tree mRoot is a function declaration, it should be the main function.
        // Previous compiler steps should have already thrown an error if there is no main function.
        ASSERT(rootAggregate->getName() == kMain);

        TIntermAggregate* newRoot = new TIntermAggregate(EOpSequence);
        TIntermSequence& sequence = newRoot->getSequence();
        sequence.push_back(mRoot);
        mRoot = newRoot;
    }
}

TIntermAggregate* RewriteCSSShaderBase::findFunction(const TString& name) const
{
    TIntermSequence& rootSequence = mRoot->getAsAggregate()->getSequence();
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
    mRoot->traverse(&findSymbolUsage);
    return findSymbolUsage.symbolUsageFound();
}

void RewriteCSSShaderBase::renameFunction(const TString& oldFunctionName, const TString& newFunctionName)
{
    RenameFunction renameFunction(oldFunctionName, newFunctionName);
    mRoot->traverse(&renameFunction);
}

const TType& RewriteCSSShaderBase::getBuiltinType(const TString& builtinName) const
{
    TSymbol* builtinSymbol = mSymbolTable.find(builtinName);
    ASSERT(builtinSymbol->isVariable());
    TVariable* builtinVariable = static_cast<TVariable*>(builtinSymbol);
    return builtinVariable->getType();
}
