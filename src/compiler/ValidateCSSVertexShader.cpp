//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/ValidateCSSVertexShader.h"

void ValidateCSSVertexShader::visitSymbol(TIntermSymbol* node)
{
    const char* kRestrictedSymbol = "css_u_texture";
    if (node->getSymbol() == kRestrictedSymbol) {
        mFoundRestrictedSymbol = true;
        mSink.prefix(EPrefixError);
        mSink.location(node->getLine());
        mSink << "Definition of a global symbol by the name '" << kRestrictedSymbol << "' is not permitted in vertex shaders.\n";
    }
}

bool ValidateCSSVertexShader::visitBinary(Visit visit, TIntermBinary* node)
{
    return false;
}

bool ValidateCSSVertexShader::visitUnary(Visit visit, TIntermUnary* node)
{
    return false;
}

bool ValidateCSSVertexShader::visitSelection(Visit visit, TIntermSelection* node)
{
    return false;
}

bool ValidateCSSVertexShader::visitAggregate(Visit visit, TIntermAggregate* node)
{   
    // Don't keep exploring if we've found the restricted symbol,
    // and don't explore anything besides the global scope (i.e. don't explore function definitions).
    if (mFoundRestrictedSymbol || node->getOp() == EOpFunction)
        return false;
    
    return true;
}

bool ValidateCSSVertexShader::visitLoop(Visit visit, TIntermLoop* node)
{
    return false;
}

bool ValidateCSSVertexShader::visitBranch(Visit visit, TIntermBranch* node)
{
    return false;
}
