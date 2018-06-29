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
// \file	qanEdgeItem.cpp
// \author	benoit@destrat.io
// \date	2017 03 02
//-----------------------------------------------------------------------------

// Qt headers
#include <QBrush>
#include <QPainter>

// QuickQanava headers
#include "./qanEdgeItem.h"
#include "./qanNodeItem.h"      // Resolve forward declaration
#include "./qanGroupItem.h"
#include "./qanGraph.h"

namespace qan { // ::qan

/* Edge Object Management *///-------------------------------------------------
EdgeItem::EdgeItem( QQuickItem* parent ) :
    QQuickItem{parent}
{
    setParentItem( parent );
    setAntialiasing( true );
    setFlag( QQuickItem::ItemHasContents, true );
    setAcceptedMouseButtons( Qt::RightButton | Qt::LeftButton );
    setAcceptDrops( true );
    setVisible(false);  // Invisible until there is a valid src/dst

    setStyle( qan::Edge::style() );
    setObjectName( QStringLiteral("qan::EdgeItem") );
}

EdgeItem::~EdgeItem() { /* Nil */ }

auto    EdgeItem::getEdge() noexcept -> qan::Edge* { return _edge.data(); }
auto    EdgeItem::getEdge() const noexcept -> const qan::Edge* { return _edge.data(); }
auto    EdgeItem::setEdge(qan::Edge* edge) noexcept -> void
{
    _edge = edge;
    if ( edge != nullptr&&
         edge->getItem() != this )
        edge->setItem(this);
}

auto    EdgeItem::getGraph() const noexcept -> const qan::Graph* {
    if ( _graph )
        return _graph;
    return _edge ? _edge->getGraph() : nullptr;
}
auto    EdgeItem::getGraph() noexcept -> qan::Graph* {
    if ( _graph )
        return _graph;
    return _edge ? _edge->getGraph() : nullptr;
}
auto    EdgeItem::setGraph(qan::Graph* graph) noexcept -> void {
    _graph = graph; emit graphChanged();
}
//-----------------------------------------------------------------------------

/* Edge Topology Management *///-----------------------------------------------
bool    EdgeItem::isHyperEdge() const noexcept { return ( _edge ? _edge->getHDst().lock() != nullptr : false ); }

auto    EdgeItem::setSourceItem( qan::NodeItem* source ) -> void
{
    if ( source == nullptr )
        return;

    // Connect dst x and y monitored properties change notify signal to slot updateEdge()
    QMetaMethod updateItemSlot = metaObject()->method( metaObject()->indexOfSlot( "updateItemSlot()" ) );
    if ( updateItemSlot.isValid() ) {  // Connect src and dst x and y monitored properties change notify signal to slot updateItemSlot()
        auto srcMetaObj = source->metaObject();
        QMetaProperty srcX      = srcMetaObj->property( srcMetaObj->indexOfProperty( "x" ) );
        QMetaProperty srcY      = srcMetaObj->property( srcMetaObj->indexOfProperty( "y" ) );
        QMetaProperty srcZ      = srcMetaObj->property( srcMetaObj->indexOfProperty( "z" ) );
        QMetaProperty srcWidth  = srcMetaObj->property( srcMetaObj->indexOfProperty( "width" ) );
        QMetaProperty srcHeight = srcMetaObj->property( srcMetaObj->indexOfProperty( "height" ) );
        if ( !srcX.isValid() || !srcX.hasNotifySignal() ) {
            qWarning() << "qan::EdgeItem::setSourceItem(): Error: can't access source x property.";
            return;
        }
        if (  !srcY.isValid() || !srcY.hasNotifySignal() ) {
            qWarning() << "qan::EdgeItem::setSourceItem(): Error: can't access source y property.";
            return;
        }
        if (  !srcWidth.isValid() || !srcWidth.hasNotifySignal() ) {
            qWarning() << "qan::EdgeItem::setSourceItem(): Error: can't access source width property.";
            return;
        }
        if (  !srcHeight.isValid() || !srcHeight.hasNotifySignal() ) {
            qWarning() << "qan::EdgeItem::setSourceItem(): Error: can't access source height property.";
            return;
        }
        connect( source, srcX.notifySignal(),       this, updateItemSlot );
        connect( source, srcY.notifySignal(),       this, updateItemSlot );
        connect( source, srcZ.notifySignal(),       this, updateItemSlot );
        connect( source, srcWidth.notifySignal(),   this, updateItemSlot );
        connect( source, srcHeight.notifySignal(),  this, updateItemSlot );
        _sourceItem = source;
        emit sourceItemChanged();
        if ( source->z() < z() )
            setZ( source->z() - 0.5 );
        updateItem();
    }
}

auto    EdgeItem::setDestinationItem( qan::NodeItem* destination ) -> void
{
    configureDestinationItem( destination );
    _destinationItem = destination;
    emit destinationItemChanged();
    updateItem();
}

void    EdgeItem::setDestinationEdge( qan::EdgeItem* destination )
{
    configureDestinationItem( destination );
    _destinationEdge = destination;
    emit destinationEdgeChanged();
    updateItem();
}

void    EdgeItem::configureDestinationItem( QQuickItem* item )
{
    if ( item == nullptr )
        return;

    // Connect dst x and y monitored properties change notify signal to slot updateItemSlot()
    QMetaMethod updateItemSlot = metaObject( )->method( metaObject( )->indexOfSlot( "updateItemSlot()" ) );
    if ( !updateItemSlot.isValid( ) ) {
        qWarning() << "qan::EdgeItem::setDestinationItem(): Error: no access to edge updateItem slot.";
        return;
    }
    auto dstMetaObj = item->metaObject( );
    QMetaProperty dstX      = dstMetaObj->property( dstMetaObj->indexOfProperty( "x" ) );
    QMetaProperty dstY      = dstMetaObj->property( dstMetaObj->indexOfProperty( "y" ) );
    QMetaProperty dstZ      = dstMetaObj->property( dstMetaObj->indexOfProperty( "z" ) );
    QMetaProperty dstWidth  = dstMetaObj->property( dstMetaObj->indexOfProperty( "width" ) );
    QMetaProperty dstHeight = dstMetaObj->property( dstMetaObj->indexOfProperty( "height" ) );
    if ( !dstX.isValid() || !dstX.hasNotifySignal() ) {
        qWarning() << "qan::EdgeItem::setDestinationItem(): Error: can't access source x property.";
        return;
    }
    if (  !dstY.isValid() || !dstY.hasNotifySignal() ) {
        qWarning() << "qan::EdgeItem::setDestinationItem(): Error: can't access source y property.";
        return;
    }
    if (  !dstWidth.isValid() || !dstWidth.hasNotifySignal() ) {
        qWarning() << "qan::EdgeItem::setDestinationItem(): Error: can't access source width property.";
        return;
    }
    if (  !dstHeight.isValid() || !dstHeight.hasNotifySignal() ) {
        qWarning() << "qan::EdgeItem::setDestinationItem(): Error: can't access source height property.";
        return;
    }
    connect( item, dstX.notifySignal(),       this, updateItemSlot );
    connect( item, dstY.notifySignal(),       this, updateItemSlot );
    connect( item, dstZ.notifySignal(),       this, updateItemSlot );
    connect( item, dstWidth.notifySignal(),   this, updateItemSlot );
    connect( item, dstHeight.notifySignal(),  this, updateItemSlot );
    if ( item->z() < z() )
        setZ( item->z() - 0.5);
}
//-----------------------------------------------------------------------------

/* Edge Drawing Management *///------------------------------------------------
void    EdgeItem::setHidden(bool hidden) noexcept
{
    if ( hidden != _hidden ) {
        _hidden = hidden;
        emit hiddenChanged();
    }
}

void    EdgeItem::updateItem() noexcept
{
    auto cache = generateGeometryCache();
    if ( cache.isValid() ) {
        generateLineGeometry(cache);
        if ( cache.isValid() ) {
            if (cache.lineType == qan::EdgeStyle::LineType::Straight )
                generateArrowGeometry(cache);
            else if ( cache.lineType == qan::EdgeStyle::LineType::Curved ) {
                generateLineControlPoints(cache);
                generateArrowGeometry(cache);
            }
            generateLabelPosition(cache);
        }
    }

    // A valid geometry has been generated, generate a bounding box for edge,
    // and project all geometry in edge CS.
    if ( cache.isValid() )
        applyGeometry(cache);
    else
        setHidden(true);
}

EdgeItem::GeometryCache  EdgeItem::generateGeometryCache() const noexcept
{
    // PRECONDITIONS:
        // _sourceItem can't be nullptr
        // _destinationItem and _destinationEdge can't be _both_ nullptr
    if ( !_sourceItem )
        return EdgeItem::GeometryCache{};
    if ( !_sourceItem && !_destinationEdge )
        return EdgeItem::GeometryCache{};

    EdgeItem::GeometryCache cache{};
    cache.valid = false;

    const QQuickItem*     graphContainerItem = ( getGraph() != nullptr ? getGraph()->getContainerItem() : nullptr );
    if ( graphContainerItem == nullptr ) {
        qWarning() << "qan::EdgeItem::generateEdgeGeometry(): No access to valid graph container item.";
        return cache;    // Return INVALID geometry cache
    }

    const QQuickItem* srcItem = _sourceItem.data();
    if ( srcItem == nullptr ) {
        qWarning() << "qan::EdgeItem::generateEdgeGeometry(): No valid source item.";
        return cache;    // Return INVALID geometry cache
    }

    qan::GroupItem* srcGroupItem = nullptr;
    if(  srcItem != nullptr ) {
        const auto srcNode = static_cast<qan::Node*>(_sourceItem->getNode());
        if ( srcNode != nullptr ) {
            const auto srcNodeGroup = qobject_cast<qan::Group*>(srcNode->getGroup().lock().get());
            if ( srcNodeGroup != nullptr )
                srcGroupItem = srcNodeGroup->getItem();
        }
    }

    qan::NodeItem*  dstNodeItem = qobject_cast<qan::NodeItem*>(_destinationItem);
    if ( dstNodeItem == nullptr &&
         _edge ) {
        qan::Node*  dstNode = static_cast< qan::Node* >( _edge->getDst().lock().get() );
        if ( dstNode != nullptr )
            dstNodeItem = dstNode->getItem();
    }

    // Initialize dstGroupItem: eventual group item for dst item
    qan::GroupItem* dstGroupItem = nullptr;
    if ( dstNodeItem != nullptr &&
         dstNodeItem->getNode() != nullptr ) {
        auto dstNodeGroup = qobject_cast<qan::Group*>( dstNodeItem->getNode()->getGroup().lock().get() );
        if ( dstNodeGroup )
            dstGroupItem = dstNodeGroup->getItem();
    }

    // Initialize dstEdgeItem
    qan::EdgeItem*  dstEdgeItem = _edge &&
                                  _edge->getHDst().lock() ? qobject_cast<qan::EdgeItem*>(_edge->getHDst().lock().get()) : nullptr;

    // Finally, generate dstItem wich either dstNodeItem or dstEdgeItem
    const QQuickItem* dstItem = ( dstNodeItem != nullptr ? qobject_cast<QQuickItem*>(dstNodeItem) :
                                                           qobject_cast<QQuickItem*>(dstEdgeItem) );

    // Check that we have a valid source and destination Quick Item
    if ( srcItem == nullptr || dstItem == nullptr )
        return cache;        // Otherwise, return an invalid cache
    cache.srcItem = srcItem;
    cache.dstItem = dstItem;

    // Generate bounding shapes for source and destination in global CS
    {
        if ( _sourceItem != nullptr ) {        // Generate source bounding shape polygon
            cache.srcBs.resize(_sourceItem->getBoundingShape().size());
            int p = 0;
            for ( const auto& point: _sourceItem->getBoundingShape() )
                cache.srcBs[p++] = _sourceItem->mapToItem( graphContainerItem, point );
        }
        // Generate destination bounding shape polygon
        if ( dstNodeItem != nullptr ) {        // Regular Node -> Node edge
            // Update edge z to source or destination maximum x
            cache.dstBs.resize(dstNodeItem->getBoundingShape().size());
            int p = 0;
            for ( const auto& point: dstNodeItem->getBoundingShape() )
                cache.dstBs[p++] = dstNodeItem->mapToItem( graphContainerItem, point );
        } else if ( dstEdgeItem != nullptr ) {
            // FIXME: generate a dump polygon around destination edge center
            qWarning() << "FIXME: edge does not support destination edge";
        }
    }

    // Verify source and destination bounding shapes
    if ( cache.srcBs.isEmpty() ||
         cache.dstBs.isEmpty() ) {
        qWarning() << "qan::EdgeItem::generateEdgeGeometry(): Invalid source or destination bounding shape.";
        return cache;    // Return INVALID geometry cache
    }

    // Generate edge geometry Z according to actual src and dst z
    const qreal srcZ = srcGroupItem != nullptr ? srcGroupItem->z() + srcItem->z() : srcItem->z();
    const qreal dstZ = dstGroupItem != nullptr ? dstGroupItem->z() + dstItem->z() : dstItem->z();
    cache.z = qMax(srcZ, dstZ) - 0.1;   // Edge z value should be less than src/dst value to ensure port item and selection is on top of edge

    if ( _style )
        cache.lineType = _style->getLineType();

    cache.valid = true;  // Finally, validate cache
    return cache;        // Expecting RVO
}

void    EdgeItem::generateLineGeometry(GeometryCache& cache) const noexcept
{
    // PRECONDITIONS:
        // edGeometry should be valid
        // cache srcBs and dstBs must not be empty (valid bounding shapes are necessary)
    if ( !cache.isValid() )
        return;
    if ( cache.srcBs.isEmpty() ||
         cache.dstBs.isEmpty() ) {
        qWarning() << "EdgeItem::generateLineGeometry(): called with invalid source or destination bounding shape cache.";
        cache.valid = false;
        return;
    }

    // Generate edge line P1 and P2 in global graph CS
    const auto srcBr = cache.srcBs.boundingRect();
    const auto dstBr = cache.dstBs.boundingRect();
    const QPointF srcBrCenter = srcBr.center(); // Keep theses value in processor cache
    const QPointF dstBrCenter = dstBr.center();
    cache.srcBr = srcBr;
    cache.dstBr = dstBr;
    cache.srcBrCenter = srcBr.center();
    cache.dstBrCenter = dstBr.center();

    const QLineF line = getLineIntersection( srcBrCenter, dstBrCenter, cache.srcBs, cache.dstBs );

    // Update hidden
    {
        {
            const auto arrowSize = getStyle() != nullptr ? getStyle()->getArrowSize() : 4.0;
            const auto arrowLength = arrowSize * 3.;
            if ( line.length() < 2.0 + arrowLength )
                cache.hidden = true;
            if ( cache.hidden )  // Fast exit if edge is hidden
                return;
        }
        const QRectF lineBr = QRectF{line.p1(), line.p2()}.normalized();  // Generate a Br with intersection points
        cache.hidden = ( srcBr.contains(lineBr) ||    // Hide edge if the whole line is contained in either src or dst BR
                         dstBr.contains(lineBr) );
        if ( cache.hidden )  // Fast exit if edge is hidden
            return;
    }

    // Save generated p1 and p2 to gometry cache
    const auto p1 = line.p1();  // Keep a fast cache access to theses coordinates
    const auto p2 = line.p2();
    cache.p1 = p1;
    cache.p2 = p2;

    {   // Take dock configuraiton into account to correct p1 and p2 when connected to/from a dock.

        // Correction is in fact a "point culling":
        // *Left dock*:    |
        // valid position  O invalid position (since node usually lay here for left docks)
        //                 |             With y beeing culled to either br.top or br.bottom
        //
        // *Top dock*:   valid position
        //                  ---O---      With x beeing culled to either br.left or br.right
        //               invalid position (since node usually lay here for top docks)

        auto correctPortPoint = [](const auto& cache, auto dockType, const auto& p,
                                   const auto& brCenter, const auto& br) -> QPointF {
            QPointF c{p}; // c corrected point
            if ( cache.lineType == qan::EdgeStyle::LineType::Straight ) {
                switch ( dockType ) {
                case qan::NodeItem::Dock::Left:
                    if ( p.x() > brCenter.x() )
                        c = QPointF{brCenter.x(), p.y() > brCenter.y() ? br.bottom() : br.top()};
                    break;
                case qan::NodeItem::Dock::Top:
                    if ( p.y() > brCenter.y() )
                        c = QPointF{p.x() > brCenter.x() ? br.right() : br.left(), brCenter.y()};
                    break;
                case qan::NodeItem::Dock::Right:
                    if ( p.x() < brCenter.x() )
                        c = QPointF{brCenter.x(), p.y() > brCenter.y() ? br.bottom() : br.top()};
                    break;
                case qan::NodeItem::Dock::Bottom:
                    if ( p.y() < brCenter.y() )
                        c = QPointF{p.x() > brCenter.x() ? br.right() : br.left(), brCenter.y()};
                    break;
                }
             } else {    // qan::EdgeStyle::LineType::Curved, for curved line, do not intersection, generate point according to port type.
                switch ( dockType ) {
                    case qan::NodeItem::Dock::Left:
                        c = QPointF{br.left(), brCenter.y() };
                        break;
                    case qan::NodeItem::Dock::Top:
                        c = QPointF{brCenter.x(), br.top()};
                        break;
                    case qan::NodeItem::Dock::Right:
                        c = QPointF{br.right(), brCenter.y()};
                        break;
                    case qan::NodeItem::Dock::Bottom:
                        c = QPointF{brCenter.x(), br.bottom() };
                        break;
                }
            }
            return c; // Expect RVO
        }; // correctPortPoint()

        const auto srcPort = qobject_cast<const qan::PortItem*>(cache.srcItem);
        if ( srcPort != nullptr )
            cache.p1 = correctPortPoint(cache, srcPort->getDockType(), p1, srcBrCenter, srcBr );

        const auto dstPort = qobject_cast<const qan::PortItem*>(cache.dstItem);
            if ( dstPort != nullptr )
                cache.p2 = correctPortPoint(cache, dstPort->getDockType(), p2, dstBrCenter, dstBr );
     } // dock configuration block
}

void    EdgeItem::generateArrowGeometry(GeometryCache& cache) const noexcept
{
    // PRECONDITIONS:
        // cache should be valid
    if ( !cache.isValid() )
        return;

    const qreal arrowSize = getStyle() != nullptr ? getStyle()->getArrowSize() : 4.0;
    const qreal arrowLength = arrowSize * 3.;

    cache.dstA1 = QPointF{ 0.,           -arrowSize  };
    cache.dstA2 = QPointF{ arrowLength,  0.          };
    cache.dstA3 = QPointF{ 0.,           arrowSize   };

    static constexpr    qreal MinLength = 0.00001;          // Correct line dst point to take into account the arrow geometry
    const QLineF line{cache.p1, cache.p2};    // Generate dst arrow line angle
    if ( cache.lineType == qan::EdgeStyle::LineType::Straight ) {
        if ( line.length() > MinLength )    // Protect line.length() DIV0
            cache.p2 = line.pointAt( 1.0 - (arrowLength/line.length()) );
        cache.dstAngle = lineAngle(line);
    } else if ( cache.lineType == qan::EdgeStyle::LineType::Curved ) {
        // Generate arrow orientation:
        // General case: get cubic tangent at line end.
        // Special case: when line length is small (ie less than 4 time arrow length), curve might
        //               have very sharp orientation, average tangent at curve end AND line angle to avoid
        //               arrow orientation that does not fit the average curve angle.
        static constexpr auto averageDstAngleFactor = 4.0;
        if ( line.length() > averageDstAngleFactor * arrowLength )         // General case
            cache.dstAngle = cubicCurveAngleAt(0.99, cache.p1, cache.p2, cache.c1, cache.c2);
        else {                                                          // Special case
            cache.dstAngle = ( 0.4 * cubicCurveAngleAt(0.99, cache.p1, cache.p2, cache.c1, cache.c2) +
                               0.6 * lineAngle(line) );
        }

        // Use dst angle to generate an end point translated by -arrowLength...
        // Build a (P2, C2) vector
        QVector2D dstVector( QPointF{cache.c2.x() - cache.p2.x(), cache.c2.y() - cache.p2.y()} );
        dstVector.normalize();
        dstVector *= static_cast<float>(arrowLength);
        cache.p2 = cache.p2 + dstVector.toPointF();
    }
}

void    EdgeItem::generateLineControlPoints(GeometryCache& cache) const noexcept
{
    // PRECONDITIONS:
        // edGeometry should be valid
        // cache srcBs and dstBs must not be empty (valid bounding shapes are necessary)
        // cache style must be straight line
    if ( !cache.isValid() )
        return;
    if ( cache.lineType != qan::EdgeStyle::LineType::Curved )
        return;

    const auto srcPort = qobject_cast<const qan::PortItem*>(cache.srcItem);
    const auto dstPort = qobject_cast<const qan::PortItem*>(cache.dstItem);

    const auto xDelta = cache.p2.x() - cache.p1.x();
    const auto xDeltaAbs = std::abs(xDelta);
    const auto yDelta = cache.p2.y() - cache.p1.y();
    const auto yDeltaAbs = std::abs(yDelta);

    const QLineF line{cache.p1, cache.p2};
    const auto lineLength = line.length();

    if ( srcPort == nullptr ||      // If there is a connection to a non-port item, generate a control point for it
         dstPort == nullptr ) {
        // SIMPLE CASE: Generate cubic curve control points with no dock, just use line center and normal
        // Heuristic: bounded to ]0;40.], larger when line length is large (line.length()), small when
        //            line is either vertical or horizontal (xDelta/yDelta near 0)
        const auto distance = std::min( line.length(), std::min(xDeltaAbs / 2., yDeltaAbs / 2.) );
        const auto controlPointDistance = qBound( 0.001, distance, 40. );

        const QPointF center{ ( cache.p1.x() + cache.p2.x() ) / 2.,           // (P1,P2) Line center
                              ( cache.p1.y() + cache.p2.y() ) / 2. };
        // Invert if:
            // Top left quarter:     do not invert (xDelta < 0 && yDelta < 0)
            // Top right quarter:    xDelta > 0 && yDelta < 0
            // Bottom rigth quarter: do not invert (xDelta > 0 && yDelta >0)
            // Bottom left quarter:  xDelta < 0 && yDelta > 0
        const auto invert =  ( xDelta > 0 && yDelta < 0 ) ||
                             ( xDelta < 0 && yDelta > 0 ) ? -1. : 1.;
        const QPointF normal = QPointF{ -line.dy(), line.dx() } / ( lineLength * invert );

        if ( srcPort == nullptr )
            cache.c1 = center + ( normal * controlPointDistance );
        if ( dstPort == nullptr )
            cache.c2 = center - ( normal * controlPointDistance );
    }
    if ( srcPort != nullptr ||      // If there is a connection to a port item, generate a control point for it
         dstPort != nullptr ) {
        static constexpr qreal maxOffset = 40.;
        auto offset = [](auto deltaAbs) -> auto {
            // Heuristic: for [0, maxOffset] delta, return a percentage of maxOffset, return value
            //             is between [4.;maxOffset]
            return std::max(10., std::min(deltaAbs, 100.) / 100. * maxOffset);
        };

        // Offset / Correction:
        //  Offset is translation in port direction (left for left port, top for top port and so on....)
        //  Correction is translation in respect to dX/dY of edge line.
        //
        //          C1 o <-offset->
        //             ^
        //             |
        //         correction
        //             |
        //             v          +----------+
        //                       O|   NODE   |
        //              left port +----------+
        //

        // Heuristics:
            // 1. xCorrection should be really small when xDeltaAbs is small (small == < average bounding rect width)
                // otherwise, it should be proportionnal to one fitfh of xDeltaAbs and always less than maxOffset.
            // 2. yCorrection should be really small when yDeltaAbs is small (small == < average bounding rect height)
                // otherwise, it should be proportionnal to one fitfh of yDeltaAbs and always less than maxOffset.
            // 3. Do not apply correction on out port
        const auto xOffset = offset(xDeltaAbs);
        const auto yOffset = offset(yDeltaAbs);


        static constexpr auto maxCorrection = 100.;
        Q_UNUSED(maxCorrection);
        auto correction = [](auto deltaAbs, auto maxSize) -> auto {
            //           c = correction
            //           ^
            //           |
            // 3*maxSize * p0 (0,maxSize*3)
            //           |   *
            //       p.y +     * p(x, y)
            //           |       *  p1 (maxSize, maxSize)
            // maxSize   +          *****************************
            //           |
            //           +-----+----+---------------------------->   delta
            //           0    p.x maxSize
            //
            // on x in [O,maxSize] we want to interpolate linearly on (p0, p1):
            // p.x = deltaAbs
            //
            // See https://en.wikipedia.org/wiki/Linear_interpolation, then simplify to a polynomial form for our special use-case:
            //
            //       p0.y (p1.x-x) + p1.y(x - p0.x)    3 * maxSize * (maxSize-x) + (maxSize * x)    (3*maxSize²) - (3*maxSize*x) + (maxSize*x)
            // p.y = ------------------------------ = ------------------------------------------ =  ------------------------------------------
            //               p1.x - p0.x                               maxSize                                      maxSize
            qreal c = maxSize;
            if ( deltaAbs < maxSize ) {
                const auto deltaAbsMaxSize = deltaAbs * maxSize;
                const auto maxSize2 = maxSize * maxSize;
                c = ( (3*maxSize2) - (3*deltaAbsMaxSize) + deltaAbsMaxSize ) / maxSize;
            }
            return c;
        };
        const auto maxBrWidth = 100.;
        //const auto maxBrWidth = std::max(cache.srcBr.width(), cache.dstBr.width());
        qreal xCorrection = ( std::signbit(xDelta) ? -1. : 1. ) * correction(xDeltaAbs, maxBrWidth);

        const auto maxBrHeight = 50.;
        //const auto maxBrHeight = std::max(cache.srcBr.height(), cache.dstBr.height());
        qreal yCorrection = ( std::signbit(yDelta) ? -1. : 1. ) * correction(yDeltaAbs, maxBrHeight);
        //qDebug() << "maxBrWidth=" << maxBrWidth << "\tmaxBrHeight=" << maxBrHeight;
        //qDebug() << "xDelta=" << xDelta << "\txCorrection=" << xCorrection;
        //qDebug() << "yDelta=" << yDelta << "\tyCorrection=" << yCorrection;

                                           // Left Tp Rgt Bot None
        qreal xCorrect[5][5] =             { { 1,  0,  0,  1, 0 },     // Dock:Left
                                             { 0,  1,  0,  0, 0 },     // Dock::Top
                                             { 0,  0, -1, -1, 0 },     // Dock::Right
                                             { 1,  0, -1, -1, 0 },     // Dock::Bottom
                                             { 0,  0,  0,  0, 0 } };   // None

        qreal yCorrect[5][5] =             { { 1,  1,  0,  0, 0 },     // Dock:Left
                                             { 1,  0,  0,  0, 0 },     // Dock::Top
                                             { 0,  0,  0, -1, 0 },    // Dock::Right
                                             { 0,  0, -1,  0, 0 },    // Dock::Bottom
                                             { 0,  0,  0,  0, 0 } };   // None

        unsigned int previous = srcPort != nullptr ? static_cast<unsigned int>(srcPort->getDockType()) : 4;;  // 4 = None
        unsigned int next = dstPort != nullptr ? static_cast<unsigned int>(dstPort->getDockType()) : 4;      // 4 = None

        using Dock = qan::NodeItem::Dock;
        const double xSmooth = qBound(-100., xDelta, 100.) / 100.;
        const double ySmooth = qBound(-100., yDelta, 100.) / 100.;
        if ( srcPort != nullptr ) {     // Generate control point for src (C1)
            const auto xCorrectionFinal = xCorrection * xCorrect[previous][next] * ySmooth;
            const auto yCorrectionFinal = yCorrection * yCorrect[previous][next] * xSmooth;
            switch ( srcPort->getDockType() ) {
            case Dock::Left:     cache.c1 = cache.p1 + QPointF{ -xOffset,           yCorrectionFinal  };  break;
            case Dock::Top:      cache.c1 = cache.p1 + QPointF{ xCorrectionFinal,  -yOffset      };  break;
            case Dock::Right:    cache.c1 = cache.p1 + QPointF{ xOffset,            yCorrectionFinal  };  break;
            case Dock::Bottom:   cache.c1 = cache.p1 + QPointF{ xCorrectionFinal,   yOffset      };  break;
            }
        }
        if ( dstPort != nullptr ) {     // Generate control point for dst (C2)
            const auto xCorrectionFinal = xCorrection * xCorrect[previous][next] * ySmooth;
            const auto yCorrectionFinal = yCorrection * yCorrect[previous][next] * xSmooth;
            switch ( dstPort->getDockType() ) {
            case Dock::Left:     cache.c2 = cache.p2 + QPointF{ -xOffset,           yCorrectionFinal };  break;
            case Dock::Top:      cache.c2 = cache.p2 + QPointF{ xCorrectionFinal,   -yOffset    };  break;
            case Dock::Right:    cache.c2 = cache.p2 + QPointF{ xOffset,            yCorrectionFinal };  break;
            case Dock::Bottom:   cache.c2 = cache.p2 + QPointF{ xCorrectionFinal,   yOffset     };  break;
            }
        }
    }

    // Finally, modify p1 and p2 according to c1 and c2
    cache.p1 = getLineIntersection( cache.c1, cache.srcBrCenter, cache.srcBs);
    cache.p2 = getLineIntersection( cache.c2, cache.dstBrCenter, cache.dstBs);
}

void    EdgeItem::generateLabelPosition(GeometryCache& cache) const noexcept
{
    // PRECONDITIONS:
        // cache should be valid
    if ( !cache.isValid() )
        return;

    const QLineF line{cache.p1, cache.p2};
    if ( cache.lineType == qan::EdgeStyle::LineType::Straight ) {
        cache.labelPosition = line.pointAt(0.5) + QPointF{10., 10.};
    } else if ( cache.lineType == qan::EdgeStyle::LineType::Curved ) {
        // Get the barycenter of polygon p1/p2/c1/c2
        QPolygonF p{ {cache.p1, cache.p2, cache.c1, cache.c2 } };
        if (!p.isEmpty())
            cache.labelPosition = p.boundingRect().center();
    }
}

void    EdgeItem::applyGeometry(const GeometryCache& cache) noexcept
{
    // PRECONDITIONS:
        // cache should be valid
        // edge should not be hidden
    if (!cache.isValid())
        return;

    if ( cache.hidden ) {    // Apply hidden property
        setVisible(false);
        setHidden(true);
        return;
    }

    const QQuickItem*   graphContainerItem = getGraph() != nullptr ? getGraph()->getContainerItem() : nullptr;
    if ( graphContainerItem != nullptr ) {
        QPolygonF edgeBrPolygon;
        edgeBrPolygon << cache.p1 << cache.p2;
        if ( cache.lineType == qan::EdgeStyle::LineType::Curved )
            edgeBrPolygon << cache.c1 << cache.c2;
        //QRectF lineBr = QRectF{cache.p1, cache.p2}.normalized();  // Generate a Br with intersection points
        const QRectF edgeBr = edgeBrPolygon.boundingRect();
        setPosition( edgeBr.topLeft() );    // Note: setPosition() call must occurs before mapFromItem()
        setSize( edgeBr.size() );

        _p1 = mapFromItem(graphContainerItem, cache.p1);
        _p2 = mapFromItem(graphContainerItem, cache.p2);
        emit lineGeometryChanged();

        {   // Apply arrow geometry
            _dstAngle = cache.dstAngle;
            emit dstAngleChanged(); // Note: Update dstAngle before arrow geometry.

            _dstA1 = cache.dstA1;    // Arrow geometry is already expressed in edge "local CS"
            _dstA2 = cache.dstA2;
            _dstA3 = cache.dstA3;
            emit arrowGeometryChanged();
        }

        if ( cache.lineType == qan::EdgeStyle::LineType::Curved ) { // Apply control point geometry
            _c1 = mapFromItem(graphContainerItem, cache.c1);
            _c2 = mapFromItem(graphContainerItem, cache.c2);
            emit controlPointsChanged();
        }

        setZ(cache.z);
        setLabelPos( mapFromItem(graphContainerItem, cache.labelPosition) );
    }

    // Edge item geometry is now valid, set the item visibility to true and "unhide" it
    setVisible(true);
    setHidden(false);
}

qreal   EdgeItem::lineAngle(const QLineF& line) const noexcept
{
    static constexpr    qreal Pi = 3.141592653;
    static constexpr    qreal TwoPi = 2. * Pi;
    static constexpr    qreal MinLength = 0.00001;
    const qreal lineLength = line.length();
    if ( lineLength < MinLength )
        return -1.;
    double angle = std::acos( line.dx() / lineLength );
    if ( line.dy() < 0. )
        angle = TwoPi - angle;
    return angle * ( 360. / TwoPi );
}

void    EdgeItem::setLine( QPoint src, QPoint dst )
{
    _p1 = src;
    _p2 = dst;
    emit lineGeometryChanged();
}

QPointF  EdgeItem::getLineIntersection( const QPointF& p1, const QPointF& p2,
                                        const QPolygonF& polygon ) const noexcept
{
    const QLineF line{p1, p2};
    QPointF source{p1};
    QPointF intersection;
    for ( auto p = 0; p < polygon.length() - 1 ; ++p ) {
        const QLineF polyLine( polygon[p], polygon[p + 1] );
        if ( line.intersect( polyLine, &intersection ) == QLineF::BoundedIntersection ) {
            source = intersection;
            break;
        }
    }
    return source;
}

QLineF  EdgeItem::getLineIntersection( const QPointF& p1, const QPointF& p2,
                                       const QPolygonF& srcBp, const QPolygonF& dstBp ) const noexcept
{
    const QLineF line{p1, p2};
    QPointF source{p1};
    QPointF intersection;
    for ( auto p = 0; p < srcBp.length() - 1 ; ++p ) {
        const QLineF polyLine( srcBp[p], srcBp[p + 1] );
        if ( line.intersect( polyLine, &intersection ) == QLineF::BoundedIntersection ) {
            source = intersection;
            break;
        }
    }
    QPointF destination{p2};
    for ( auto p = 0; p < dstBp.length() - 1 ; ++p ) {
        const QLineF polyLine( dstBp[p], dstBp[p + 1] );
        if ( line.intersect( polyLine, &intersection ) == QLineF::BoundedIntersection ) {
            destination = intersection;
            break;
        }
    }
    return QLineF{source, destination};
}
//-----------------------------------------------------------------------------


/* Curve Control Points Management *///----------------------------------------
qreal   EdgeItem::cubicCurveAngleAt(qreal pos, const QPointF& start, const QPointF& end, const QPointF& c1, const QPointF& c2 ) const noexcept
{
    // Preconditions:
        // pos in [0., 1.0]
    if ( pos < 0. || pos > 1. )
        return -1;

    // FIXME 20170914: That code need more testing, std::atan2 return value should be handled with
    // more care: https://stackoverflow.com/questions/1311049/how-to-map-atan2-to-degrees-0-360

    // This code is (C) 2014 Paul W.
    // Description:
        // http://www.paulwrightapps.com/blog/2014/9/4/finding-the-position-and-angle-of-points-along-a-bezier-curve-on-ios

    // Compute cubic polynomial coefficients
    const QPointF coeff3{end.x() - (3. * c2.x()) + (3. * c1.x()) - start.x(),
                         end.y() - (3. * c2.y()) + (3. * c1.y()) - start.y()};

    const QPointF coeff2{(3. * c2.x()) - (6. * c1.x()) + (3. * start.x()),
                         (3. * c2.y()) - (6. * c1.y()) + (3. * start.y())};

    const QPointF coeff1{(3. * c1.x()) - (3. * start.x()),
                         (3. * c1.y()) - (3. * start.y())};

    const auto pos2 = pos * pos;
    const auto dxdt = (3. * coeff3.x()*pos2) + (2. * coeff2.x()*pos) + coeff1.x();
    const auto dydt = (3. * coeff3.y()*pos2) + (2. * coeff2.y()*pos) + coeff1.y();

    static constexpr    qreal Pi = 3.141592653;
    auto degrees = std::atan2(dxdt, dydt) * 180. / Pi;
    if (degrees > 90.)
        degrees = 450. - degrees;
    else
        degrees = 90. - degrees;
    return degrees;
}
//-----------------------------------------------------------------------------


/* Mouse Management *///-------------------------------------------------------
void    EdgeItem::mouseDoubleClickEvent( QMouseEvent* event )
{
    const qreal d = distanceFromLine( event->localPos(), QLineF{_p1, _p2} );
    if ( d > -0.0001 && d < 5. &&
         event->button() == Qt::LeftButton ) {
        emit edgeDoubleClicked( this, event->localPos() );
        event->accept();
    }
    else
        event->ignore();
    QQuickItem::mouseDoubleClickEvent( event );
}

void    EdgeItem::mousePressEvent( QMouseEvent* event )
{
    const qreal d = distanceFromLine( event->localPos( ), QLineF{_p1, _p2} );
    if ( d > -0.0001 && d < 5. ) {
        if ( event->button() == Qt::LeftButton ) {
            emit edgeClicked( this, event->localPos() );
            event->accept();
        }
        else if ( event->button() == Qt::RightButton ) {
            emit edgeRightClicked( this, event->localPos() );
            event->accept();
            return;
        }
    }
    else
        event->ignore();
}

qreal   EdgeItem::distanceFromLine( const QPointF& p, const QLineF& line ) const noexcept
{
    // Inspired by DistancePointLine Unit Test, Copyright (c) 2002, All rights reserved
    // Damian Coventry  Tuesday, 16 July 2002
    static constexpr    qreal MinLength = 0.00001;
    const qreal lLength = line.length();
    if ( lLength < MinLength )
        return -1.; // Protect generation of u from DIV0
    const qreal u  = ( ( ( p.x() - line.x1() ) * ( line.x2() - line.x1() ) ) +
                     ( ( p.y() - line.y1() ) * ( line.y2() - line.y1() ) ) ) / ( lLength * lLength );
    if ( u < 0. || u > 1. )
        return -1.;
    const QPointF i { line.x1() + u * ( line.x2() - line.x1() ),
                      line.y1() + u * ( line.y2() - line.y1() ) };
    return QLineF{p, i}.length();
}
//-----------------------------------------------------------------------------

/* Style and Properties Management *///----------------------------------------
void    EdgeItem::setStyle( EdgeStyle* style ) noexcept
{
    if ( style != _style ) {
        if ( _style != nullptr )  // Every style that is non default is disconnect from this node
            QObject::disconnect( _style, 0, this, 0 );
        _style = style;
        if ( _style ) {
            connect( _style,    &QObject::destroyed,    // Monitor eventual style destruction
                     this,      &EdgeItem::styleDestroyed );
            // Note 20170909: _style.styleModified() signal is _not_ binded to updateItem() slot, since
            // it would be very unefficient to update edge for properties change affecting only
            // edge visual item (for example, _stye.lineWidth modification is watched directly
            // from edge delegate). Since arrowSize affect concrete edge geometry, bind it manually to
            // updateItem().
            connect( _style,    &qan::EdgeStyle::arrowSizeChanged,
                     this,      &EdgeItem::updateItem );
            connect( _style,    &qan::EdgeStyle::lineTypeChanged,
                     this,      &EdgeItem::updateItem );
        }
        emit styleChanged( );
    }
}

void    EdgeItem::styleDestroyed( QObject* style )
{
    if ( style != nullptr )
        setStyle( nullptr );
}
//-----------------------------------------------------------------------------


/* Drag'nDrop Management *///--------------------------------------------------
bool    EdgeItem::contains( const QPointF& point ) const
{
    const qreal d = distanceFromLine( point, QLineF{_p1, _p2} );
    return ( d > 0. && d < 5. );
}

void    EdgeItem::dragEnterEvent( QDragEnterEvent* event )
{
    // Note 20160218: contains() is used so enter event is necessary generated "on the edge line"
    if ( _acceptDrops ) {
        if ( event->source() == nullptr ) {
            event->accept(); // This is propably a drag initated with type=Drag.Internal, for exemple a connector drop node trying to create an hyper edge, accept by default...
            QQuickItem::dragEnterEvent( event );
            return;
        }
        if ( event->source() != nullptr ) { // Get the source item from the quick drag attached object received
            QVariant source = event->source()->property( "source" );
            if ( source.isValid() ) {
                QQuickItem* sourceItem = source.value< QQuickItem* >( );
                QVariant draggedStyle = sourceItem->property( "draggedEdgeStyle" ); // The source item (usually a style node or edge delegate must expose a draggedStyle property.
                if ( draggedStyle.isValid() ) {
                    event->accept();
                    return;
                }
            }
        }
        event->ignore();
        QQuickItem::dragEnterEvent( event );
    }
    QQuickItem::dragEnterEvent( event );
}

void	EdgeItem::dragMoveEvent( QDragMoveEvent* event )
{
    if ( getAcceptDrops() ) {
        qreal d = distanceFromLine( event->posF( ), QLineF{_p1, _p2} );
        if ( d > 0. && d < 5. )
            event->accept();
        else event->ignore();
    }
    QQuickItem::dragMoveEvent( event );
}

void	EdgeItem::dragLeaveEvent( QDragLeaveEvent* event )
{
    QQuickItem::dragLeaveEvent( event );
    if ( getAcceptDrops() )
        event->ignore();
}

void    EdgeItem::dropEvent( QDropEvent* event )
{
    if ( getAcceptDrops() && event->source() != nullptr ) { // Get the source item from the quick drag attached object received
        QVariant source = event->source()->property( "source" );
        if ( source.isValid() ) {
            QQuickItem* sourceItem = source.value< QQuickItem* >( );
            QVariant draggedStyle = sourceItem->property( "draggedEdgeStyle" ); // The source item (usually a style node or edge delegate must expose a draggedStyle property.
            if ( draggedStyle.isValid() ) {
                qan::EdgeStyle* draggedEdgeStyle = draggedStyle.value< qan::EdgeStyle* >( );
                if ( draggedEdgeStyle != nullptr ) {
                    setStyle( draggedEdgeStyle );
                    event->accept();
                }
            }
        }
    }
    QQuickItem::dropEvent( event );
}
//-----------------------------------------------------------------------------

} // ::qan
