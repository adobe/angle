//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_REMOVE_UNUSED_SYMBOLS
#define COMPILER_REMOVE_UNUSED_SYMBOLS

#include <set>

#include "compiler/intermediate.h"

typedef std::set<const char*> SymbolNames;

//
// Removes the symbols that are not used in the shader from a vector of symbol names.
//
class RemoveUnusedSymbolsFromSet : public TIntermTraverser
{
public:
    RemoveUnusedSymbolsFromSet(SymbolNames& symbolNames)
    : TIntermTraverser(true, false, false)
    , mFoundSymbolNames(symbolNames)
    {
        mMissingSymbolNames.insert(symbolNames.begin(), symbolNames.end());
        mFoundSymbolNames.clear();
    }

    virtual void visitSymbol(TIntermSymbol* node)
    {
        const char* const nodeSymbolName = node->getSymbol().c_str();

        for (SymbolNames::iterator iter = mMissingSymbolNames.begin(); iter != mMissingSymbolNames.end(); ++iter)
        {
            const char* const missingSymbolName = *iter;
            if (strcmp(nodeSymbolName, missingSymbolName) == 0) {
                mFoundSymbolNames.insert(missingSymbolName);
                mMissingSymbolNames.erase(iter++);
            }
        }
    }

private:
    SymbolNames& mFoundSymbolNames;
    SymbolNames mMissingSymbolNames;
};

#endif COMPILER_REMOVE_UNUSED_SYMBOLS
