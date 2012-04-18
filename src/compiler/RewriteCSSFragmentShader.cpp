//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/RewriteCSSFragmentShader.h"
#include "ParseHelper.h"

void RewriteCSSFragmentShader::rewrite()
{
    insertCSSFragColorDeclaration();
    
    // Replace all "gl_FragColor" with "css_FragColor".
    GlobalParseContext->treeRoot->traverse(this);
}

// TODO: Pool allocate strings.
// TODO: What precision?
void RewriteCSSFragmentShader::insertCSSFragColorDeclaration()
{
    // LHS
    TIntermSymbol* symbol = new TIntermSymbol(0, "css_FragColor", TType(EbtFloat, EbpHigh, EvqGlobal, 4));
    
    // RHS
    ConstantUnion* constArray = new ConstantUnion[4];
    for (int i = 0; i < 4; i++)
        constArray[i].setFConst(1.0);
    
    TIntermConstantUnion* constUnion = new TIntermConstantUnion(constArray, TType(EbtFloat, EbpUndefined, EvqConst, 4));
    
    // Declaration
    TIntermBinary* initialization = new TIntermBinary(EOpInitialize);
    initialization->setType(TType(EbtFloat, EbpHigh, EvqTemporary, 4));
    initialization->setLeft(symbol);
    initialization->setRight(constUnion);
    
    TIntermAggregate* declaration = new TIntermAggregate(EOpDeclaration);
    TIntermSequence& declarationSequence = declaration->getSequence();
    declarationSequence.insert(declarationSequence.begin(), initialization);
    
    TIntermSequence& globalSequence = GlobalParseContext->treeRoot->getAsAggregate()->getSequence();
    globalSequence.insert(globalSequence.begin(), declaration);
}

void RewriteCSSFragmentShader::visitSymbol(TIntermSymbol* node)
{
    const char* kFragColor = "gl_FragColor";
    if (node->getSymbol() == kFragColor) {
        
        node->setId(0);
        node->getTypePointer()->setQualifier(EvqGlobal);
        node->setSymbol("css_FragColor");
    }
}

bool RewriteCSSFragmentShader::visitBinary(Visit visit, TIntermBinary* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitUnary(Visit visit, TIntermUnary* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitSelection(Visit visit, TIntermSelection* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitAggregate(Visit visit, TIntermAggregate* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitLoop(Visit visit, TIntermLoop* node)
{
    return true;
}

bool RewriteCSSFragmentShader::visitBranch(Visit visit, TIntermBranch* node)
{
    return true;
}
