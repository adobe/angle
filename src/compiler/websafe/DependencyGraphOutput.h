//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_DEPENDENCY_GRAPH_OUTPUT_H
#define COMPILER_DEPENDENCY_GRAPH_OUTPUT_H

#include "compiler/websafe/DependencyGraph.h"
#include "compiler/InfoSink.h"

class TDependencyGraphOutput : public TDependencyGraphTraverser {
public:
    TDependencyGraphOutput(TInfoSinkBase& sink) : mSink(sink) {}
    virtual void visitSymbol(TGraphSymbol* symbol);
    virtual void visitArgument(TGraphArgument* parameter);
    virtual void visitFunctionCall(TGraphFunctionCall* functionCall);
    virtual void visitSelection(TGraphSelection* selection);
    virtual void visitLoop(TGraphLoop* loop);
    virtual void visitLogicalOp(TGraphLogicalOp* logicalOp);

    void outputAllSpanningTrees(TDependencyGraph& graph);
private:
    void outputIndentation();

    TInfoSinkBase& mSink;
};

#endif  // COMPILER_DEPENDENCY_GRAPH_OUTPUT_H
