/*
 Copyright (c) 2008-2018, Benoit AUTHEMAN All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author or Destrat.io nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the QuickQanava software library.
//
// \file	qanGraphConfig.h
// \author	benoit@destrat.io
// \date	2016 02 04
//-----------------------------------------------------------------------------

#ifndef qanGraphConfig_h
#define qanGraphConfig_h

#ifdef _MSC_VER
#pragma warning(disable: 4100)  // Disable unreferenced formal parameter warning for Protocol Buffer generated code
#endif

// Qt headers
#include <QObject>
#include <QVector>
#include <QSet>
#include <QQuickItem>

// GTpo headers
#include <GTpo.h>

// QuickContainers headers
#include "../QuickContainers/src/qcmContainer.h"

// QuickQanava headers
#include "./qanNavigable.h"
#include "./qanContainerAdapter.h"

namespace qan { // ::qan

/*! \brief Exception thrown by QuickQanava to notify runtime error (nullptr assert, etc.).
 *
 * Use what() to have a detailled error description.
 */
class runtime_error : public std::runtime_error
{
public:
    explicit runtime_error (const std::string& what_arg) : std::runtime_error( what_arg ) { }
    explicit runtime_error (const char* what_arg) : std::runtime_error( what_arg ) { }
    runtime_error () : runtime_error( "QuickQanava runtime unrecoverable error." ) { }
};

class Node;
class Edge;
class Group;

class GraphConfig final : public gtpo::GraphConfig
{
public:
    using GraphBase = QQuickItem;
    using NodeBase  = QObject;
    using EdgeBase  = QObject;
    using GroupBase = QObject;

    using GraphBehaviours = std::tuple< gtpo::GraphGroupAjacentEdgesBehaviour< qan::GraphConfig > >;
    using GroupBehaviours = std::tuple< gtpo::GroupAdjacentEdgesBehaviour< qan::GraphConfig > >;

    template <typename T>
    using container_adapter = qan::ContainerAdapter<T>;

    using FinalNode     = qan::Node;
    using FinalEdge     = qan::Edge;
    using FinalGroup    = qan::Group;

    template <class ...Args>
    using NodeContainer = qcm::Container< QVector, Args... >;

    template <class ...Args>
    using EdgeContainer = qcm::Container< QVector, Args... >;

    template < class ...Args >
    using SearchContainer = QSet< Args... >;
};

} // ::qan

#endif // qanGraphConfig_h

