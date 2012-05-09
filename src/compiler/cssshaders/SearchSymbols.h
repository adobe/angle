//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_REMOVE_UNUSED_SYMBOLS
#define COMPILER_REMOVE_UNUSED_SYMBOLS

#include <set>

#include "compiler/intermediate.h"

typedef std::set<TString> SymbolNames;

//
// Searches through the intermediate tree for multiple symbols at a time.
//
class SearchSymbols : public TIntermTraverser
{
public:
    SearchSymbols(const SymbolNames& symbolNames)
        : TIntermTraverser(true, false, false)
        , mSymbolNames(symbolNames) {}

    virtual void visitSymbol(TIntermSymbol* node)
    {
        const TString& nodeSymbolName = node->getSymbol();

        for (SymbolNames::iterator iter = mSymbolNames.begin(); iter != mSymbolNames.end(); ++iter)
        {
            const TString& searchSymbolName = *iter;
            if (nodeSymbolName == searchSymbolName)
                mFoundSymbolNames.insert(searchSymbolName);
        }
    }
    
    const SymbolNames& getFoundSymbolNames() const
    {
        return mFoundSymbolNames;
    }

private:
    const SymbolNames& mSymbolNames;
    SymbolNames mFoundSymbolNames;
};

#endif COMPILER_REMOVE_UNUSED_SYMBOLS
