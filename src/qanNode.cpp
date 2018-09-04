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
// \file	qanNode.cpp
// \author	benoit@destrat.io
// \date	2004 February 15
//-----------------------------------------------------------------------------

// Qt headers
#include <QPainter>
#include <QPainterPath>

// QuickQanava headers
#include "./qanNode.h"
#include "./qanNodeItem.h"
#include "./qanPortItem.h"
#include "./qanGroup.h"
#include "./qanGraph.h"

namespace qan { // ::qan

/* Node Object Management *///-------------------------------------------------
Node::Node(QObject* parent) :
    gtpo::node< qan::Config >{}
{
    Q_UNUSED(parent)
}

Node::~Node()
{
    if ( _item )
        _item->deleteLater();
}

qan::Graph* Node::getGraph() noexcept {
    return qobject_cast< qan::Graph* >( gtpo::node< qan::Config >::get_graph() );
}

const qan::Graph* Node::getGraph() const noexcept {
    return qobject_cast< const qan::Graph* >( gtpo::node< qan::Config >::get_graph() );
}

bool    Node::operator==( const qan::Node& right ) const
{
    return getLabel() == right.getLabel();
}

qan::NodeItem*          Node::getItem() noexcept { return _item.data(); }
const qan::NodeItem*    Node::getItem() const noexcept { return _item.data(); }

void    Node::setItem(qan::NodeItem* nodeItem) noexcept
{
    if ( nodeItem != nullptr ) {
        _item = nodeItem;
        if ( nodeItem->getNode() != this )
            nodeItem->setNode(this);
    }
}
//-----------------------------------------------------------------------------

/* Node Static Factories *///--------------------------------------------------
QQmlComponent*  Node::delegate(QQmlEngine& engine) noexcept
{
    static std::unique_ptr<QQmlComponent>   delegate;
    if ( !delegate )
        delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/QuickQanava/Node.qml");
    return delegate.get();
}

qan::NodeStyle* Node::style() noexcept
{
    static std::unique_ptr<qan::NodeStyle>  qan_Node_style;
    if ( !qan_Node_style )
        qan_Node_style = std::make_unique<qan::NodeStyle>();
    return qan_Node_style.get();
}
//-----------------------------------------------------------------------------

/* Behaviours Management *///--------------------------------------------------
void    Node::installBehaviour( std::unique_ptr<qan::NodeBehaviour> behaviour )
{
    // PRECONDITIONS:
        // behaviour can't be nullptr
    if ( !behaviour )
        return;
    behaviour->setHost(this);
    add_dynamic_node_behaviour( std::move(behaviour) );
}
//-----------------------------------------------------------------------------

/* Appearance Management *///--------------------------------------------------
void    Node::setLabel( const QString& label ) noexcept
{
    if ( label != _label ) {
        _label = label;
        emit labelChanged();
    }
}
//-----------------------------------------------------------------------------

/* Dock Management *///--------------------------------------------------------
//-----------------------------------------------------------------------------

} // ::qan
