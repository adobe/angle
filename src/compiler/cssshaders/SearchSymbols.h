//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_CSSSHADERS_SEARCH_SYMBOLS
#define COMPILER_CSSSHADERS_SEARCH_SYMBOLS

#include <stdarg.h>
#include <set>

#include "compiler/intermediate.h"

//
// Searches through the intermediate tree for multiple symbols at a time.
//
class SearchSymbols : public TIntermTraverser
{
public:
    SearchSymbols(int numSymbolNames, ...) : TIntermTraverser(true, false, false)
    {
        va_list symbolNames;
        va_start(symbolNames, numSymbolNames);

        for (int i = 0; i < numSymbolNames; ++i)
        {
            const char* symbolName = va_arg(symbolNames, const char*);
            mMissingSymbolNames.insert(symbolName);
        }
   
        va_end(symbolNames);
    }

    virtual void visitSymbol(TIntermSymbol* node)
    {
        const TString& nodeSymbolName = node->getSymbol();

        for (SymbolNames::iterator iter = mMissingSymbolNames.begin(); iter != mMissingSymbolNames.end(); ++iter)
        {
            const TString& missingSymbolName = *iter;
            if (nodeSymbolName == missingSymbolName)
                mFoundSymbolNames.insert(missingSymbolName);
        }
    }
    
    bool didFindSymbol(const TString& symbolName) const
    {
        return mFoundSymbolNames.find(symbolName) != mFoundSymbolNames.end();
    }

private:
    typedef std::set<TString> SymbolNames;
    
    SymbolNames mMissingSymbolNames;
    SymbolNames mFoundSymbolNames;
};

#endif COMPILER_CSSSHADERS_SEARCH_SYMBOLS
