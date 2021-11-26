/*
 Copyright (c) 2008-2021, Benoit AUTHEMAN All rights reserved.

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
// \file	qanNavigable.cpp
// \author	benoit@destrat.io
// \date	2015 07 19
//-----------------------------------------------------------------------------

// Qt headers
// Nil

// QuickQanava headers
#include "./qanNavigable.h"

namespace qan { // ::qan

/* Navigable Object Management *///--------------------------------------------
Navigable::Navigable( QQuickItem* parent ) :
    QQuickItem{parent}
{
    _containerItem = new QQuickItem{this};
<<<<<<< HEAD
    _containerItem->setTransformOrigin( TransformOrigin::TopLeft );

    connect( _containerItem, &QQuickItem::childrenRectChanged,  // Listenning to children rect changes to update containerItem size.
             [this]() {
        if ( this->_containerItem && m_containerSizeLocked==false ) {

=======
    _containerItem->setTransformOrigin(TransformOrigin::TopLeft);
    _containerItem->setAcceptTouchEvents(true);
    connect(_containerItem, &QQuickItem::childrenRectChanged,  // Listen to children rect changes to update containerItem size
             [this]() {
        if (this->_containerItem != nullptr) {
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
            const auto cr = this->_containerItem->childrenRect();

            this->_containerItem->setProperty("width",QVariant::fromValue(cr.width()));
            this->_containerItem->setProperty("height",QVariant::fromValue(cr.height()));
        }
    });
    setAcceptedMouseButtons(Qt::RightButton | Qt::LeftButton);
    setTransformOrigin(TransformOrigin::TopLeft);

    _defaultGrid = std::make_unique<qan::Grid>();
    setGrid(_defaultGrid.get());
    setAcceptTouchEvents(true);
}
//-----------------------------------------------------------------------------

/* Navigation Management *///--------------------------------------------------
void    Navigable::setNavigable( bool navigable ) noexcept
{
    if ( navigable != _navigable ) {
        _navigable = navigable;
        emit navigableChanged();
    }
}

void    Navigable::centerOn(QQuickItem* item)
{
<<<<<<< HEAD
    // Algorithm:
    // 1. Project navigable view center in container item CS.
    // 2. Compute vector from navigable view center to item center in container item CS.
    // 3. Translate container by (-) this vector.
    if ( _containerItem == nullptr ||
         item == nullptr )
        return;
    if ( item->parentItem() != getContainerItem() )
=======
    if (_containerItem == nullptr ||
        item == nullptr)
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
        return;

    // ALGORITHM:
        // 1. Map item center in container CS, then translate view by this scaled center to set
        //    the top left corner of item at view origin (0, 0)
        // 2. Then compute a vector to move item at center of view using an (origin, view venter) vector
        //    in container item coordinate system.

    const qreal zoom = _containerItem->scale();

    // 1.
    const QPointF itemPos = _containerItem->mapToItem(item, QPointF{-item->width() / 2., -item->height() / 2.});
    QPointF containerPos{0., 0.};
    containerPos.rx() = itemPos.x() * zoom;
    containerPos.ry() = itemPos.y() * zoom;
    _containerItem->setPosition(containerPos);

    // 2.
    const QPointF viewCenter = QPointF{width() / 2., height() / 2.};
    const QPointF viewCenterContainerCs = mapToItem(_containerItem, viewCenter);
    const QPointF viewOriginContainerCs = mapToItem(_containerItem, QPointF{0, 0});
    const QPointF translationContainerCs = viewCenterContainerCs - viewOriginContainerCs;
    _containerItem->setPosition(_containerItem->position() + (translationContainerCs * zoom));

    updateGrid();
}

void    Navigable::centerOnPosition(QPointF position)
{
    QPointF navigableCenter{ width() / 2., height() / 2. };
    QPointF navigableCenterContainerCs = mapToItem( _containerItem, navigableCenter );
<<<<<<< HEAD
    QPointF itemCenterContainerCs{ item->mapToItem( _containerItem, QPointF{ item->width() / 2., item->height() / 2. } ) };
    QPointF translation{ navigableCenterContainerCs - itemCenterContainerCs };
    //    _containerItem->setPosition( QPointF{ _containerItem->x() + translation.x(),
    //                                          _containerItem->y() + translation.y() } );
    _containerItem->setProperty("x",QVariant::fromValue(_containerItem->x() + translation.x()));
    _containerItem->setProperty("y",QVariant::fromValue(_containerItem->y() + translation.y()));


=======
    QPointF translation{ navigableCenterContainerCs - position };
    _containerItem->setPosition(QPointF{_containerItem->x() + translation.x(),
                                         _containerItem->y() + translation.y()});
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
    updateGrid();
}

void    Navigable::fitInView( )
{
    QRectF content = _containerItem->childrenRect();
    if (!content.isEmpty()) { // Protect against div/0, can't fit if there is no content...
        const qreal viewWidth = width();
        const qreal viewHeight = height();

        const qreal fitWidthZoom = viewWidth / content.width();
        const qreal fitHeightZoom = viewHeight / content.height();

        qreal fitZoom = fitWidthZoom;
        if (content.height() * fitWidthZoom > viewHeight)
            fitZoom = fitHeightZoom;
<<<<<<< HEAD
        //        qDebug( ) << "\tfitZoom=" << fitZoom;


        // Don't use setZoom() because we force a TopLeft scale
        if ( isValidZoom(fitZoom) ) {

=======
        fitZoom -= 0.1; // Add margins

        // FIXME: Buggy, do something similar to  centerOn() 2.0...
        QPointF contentPos{0., 0.};
        /*if (content.width() * fitZoom < viewWidth)      // Center zoomed content horizontally
            contentPos.rx() = ( viewWidth - (content.width() * fitZoom) ) / 2.;
        if (content.height() * fitZoom < viewHeight)    // Center zoomed content horizontally
            contentPos.ry() = ( viewHeight - (content.height() * fitZoom) ) / 2.;
        //_containerItem->setPosition(contentPos);
        */
        contentPos = content.topLeft() * fitZoom;
        _containerItem->setPosition(-contentPos);
        _panModified = false;
        _zoomModified = false;

        // Don't use setZoom() because we force a TopLeft scale
        if (isValidZoom(fitZoom)) {
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
            _zoom = fitZoom;

            QPointF contentPos{0., 0.};
            if ( content.width() * fitZoom < viewWidth ) {  // Center zoomed content horizontally
                contentPos.rx() = ( viewWidth - ( content.width() * fitZoom ) ) / 2.;
            }
            if ( content.height() * fitZoom < viewHeight ) {   // Center zoomed content horizontally
                contentPos.ry() = ( viewHeight - ( content.height() * fitZoom ) ) / 2.;
            }
            //        _containerItem->setPosition( contentPos );
            _containerItem->setProperty("x",QVariant::fromValue(contentPos.x()));
            _containerItem->setProperty("y",QVariant::fromValue(contentPos.y()));

            _panModified = false;
            _zoomModified = false;


            _containerItem->setProperty("scale",QVariant::fromValue(_zoom));
            emit zoomChanged();
            emit containerItemModified();
            navigableContainerItemModified();
            updateGrid();
        }
    }
}

void Navigable::fitInItem(QQuickItem *item,int margin)
{
    qreal x=width()/2;
    qreal y=height()/2;


    QPointF centerPoint(x,y);

    qreal fitWidthZoom = 1.0;
    qreal fitHeightZoom = 1.0;
    fitWidthZoom = width() / (item->width()+margin);
    fitHeightZoom = height()/ (item->height()+margin);
    //qDebug( ) << "\tviewWidth=" << viewWidth << "  viewHeight=" << viewHeight;
    //qDebug( ) << "\tfitWidthZoom=" << fitWidthZoom << "  fitHeightZoom=" << fitHeightZoom;

    qreal fitZoom = fitWidthZoom;
    if ( item->height() * fitWidthZoom > height() )
        fitZoom = fitHeightZoom;

    QRectF itemScaledRect(item->x()*fitZoom,item->y()*fitZoom,item->width()*fitZoom,item->height()*fitZoom);


    QPointF itemCenterPoint(centerPoint.x()-itemScaledRect.center().x(),centerPoint.y()-itemScaledRect.center().y());



    this->_containerItem->setProperty("x",QVariant::fromValue(itemCenterPoint.x()));
    this->_containerItem->setProperty("y",QVariant::fromValue(itemCenterPoint.y()));
    this->_containerItem->setProperty("scale",QVariant::fromValue(fitZoom));

    _zoom = fitZoom;
    _zoomModified = true;
    _panModified = true;
    emit zoomChanged();
    emit containerItemModified();
    navigableContainerItemModified();
    updateGrid();

}

void    Navigable::setAutoFitMode( AutoFitMode autoFitMode )
{
    if ( _autoFitMode != AutoFit &&
         autoFitMode == AutoFit )
        fitInView();    // When going to auto fit mode from another mode, force fitInView()

    _autoFitMode = autoFitMode;
    emit autoFitModeChanged();
}

void    Navigable::setZoom(qreal zoom)
{
<<<<<<< HEAD
    if(qFuzzyCompare(zoom,_zoom)){
        return;
    }
    if ( isValidZoom( zoom ) ) {
        switch ( _zoomOrigin ) {
=======
    if (isValidZoom(zoom)) {
        switch (_zoomOrigin) {
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
        case QQuickItem::Center: {
            zoomOn(QPointF{width() / 2., height() / 2.},
                   zoom);
        }
            break;
        case QQuickItem::TopLeft:
        default:
            _zoom = zoom;
<<<<<<< HEAD
            _containerItem->setProperty("scale",QVariant::fromValue(_zoom));
=======
            _containerItem->setScale(_zoom);
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
            _zoomModified = true;
            emit zoomChanged();
            emit containerItemModified();
            navigableContainerItemModified();
        }
    }
}

void    Navigable::zoomOn(QPointF center, qreal zoom)
{
    // Get center coordinates in container CS, it is our
    // zoom application point
    qreal containerCenterX = center.x() - _containerItem->x();
    qreal containerCenterY = center.y() - _containerItem->y();



    qreal lastZoom = _zoom;

    // Don't apply modification if new zoom is not valid (probably because it is not in zoomMin, zoomMax range)
    if ( isValidZoom( zoom ) ) {
        // Get center coordinate in container CS with the new zoom
        qreal oldZoomX = containerCenterX * ( zoom / lastZoom );
        qreal oldZoomY = containerCenterY * ( zoom / lastZoom );

        // Compute a container position correction to have a fixed "zoom
        // application point"
        qreal zoomCorrectionX = containerCenterX - oldZoomX;
        qreal zoomCorrectionY = containerCenterY - oldZoomY;

        qreal newX=_containerItem->x() + zoomCorrectionX;
        qreal newY=_containerItem->y() + zoomCorrectionY;




        // Correct container position and set the appropriate scaling
        _containerItem->setX( newX );
        _containerItem->setY(newY);
        _containerItem->setScale( zoom );

        _zoom = zoom;
        _zoomModified = true;
        _panModified = true;
        emit zoomChanged();
        emit containerItemModified();
        navigableContainerItemModified();
        updateGrid();
    }
}

bool    Navigable::isValidZoom(qreal zoom) const
{
<<<<<<< HEAD
    if ( qFuzzyCompare( zoom , _zoomMin) )
        return true;

    if ( qFuzzyCompare( zoom , _zoomMax) )
        return true;


=======
    if (qFuzzyCompare(1. + zoom - _zoom, 1.0))
        return false;
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
    if ( ( zoom > _zoomMin ) &&    // Don't zoom less than zoomMin
         ( _zoomMax < 0. ||   // Don't zoom more than zoomMax except if zoomMax is infinite
           zoom < _zoomMax ) )
        return true;
    return false;
}

void    Navigable::setZoomOrigin( QQuickItem::TransformOrigin zoomOrigin )
{
    if ( zoomOrigin != _zoomOrigin ) {
        _zoomOrigin = zoomOrigin;
        emit zoomOriginChanged();
    }
}

void    Navigable::setZoomMax( qreal zoomMax )
{
    if ( qFuzzyCompare( 1. + zoomMax - _zoomMax, 1.0 ) )
        return;
    _zoomMax = zoomMax;
    emit zoomMaxChanged();
}

void    Navigable::setZoomMin( qreal zoomMin )
{
    if ( qFuzzyCompare( 1. + zoomMin - _zoomMin, 1.0 ) )
        return;
    if ( zoomMin < 0.01 )
        return;
    _zoomMin = zoomMin;
    emit zoomMinChanged();
}

void    Navigable::setDragActive(bool dragActive) noexcept
{
    if (dragActive != _dragActive) {
        _dragActive = dragActive;
        emit dragActiveChanged();
    }
}

<<<<<<< HEAD

=======
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
void    Navigable::geometryChanged( const QRectF& newGeometry, const QRectF& oldGeometry )
#else
void    Navigable::geometryChange( const QRectF& newGeometry, const QRectF& oldGeometry )
#endif
{
    if (getNavigable()) {
        // Apply fitInView if auto fitting is set to true and the user has not applyed a custom zoom or pan
        if ( _autoFitMode == AutoFit &&
             ( !_panModified || !_zoomModified ) ) {
            fitInView();
        }
        // In AutoFit mode, try centering the content when the view is resized and the content
        // size is less than the view size (it is no fitting but centering...)
        if ( _autoFitMode == AutoFit ) {
            bool centerWidth = false;
            bool centerHeight = false;
            // Container item children Br mapped in root CS.
            QRectF contentBr = mapRectFromItem( _containerItem, _containerItem->childrenRect( ) );
            if ( newGeometry.contains( contentBr ) ) {
                centerWidth = true;
                centerHeight = true;
            } else {
                if ( contentBr.top() > newGeometry.top() &&
                     contentBr.bottom() < newGeometry.bottom() )
                    centerHeight = true;
                if ( contentBr.left() > newGeometry.left() &&
                     contentBr.right() < newGeometry.right() )
                    centerWidth = true;
            }
            if (centerWidth) {
                qreal cx = ( newGeometry.width() - contentBr.width() ) / 2.;
                _containerItem->setX( cx );
                // _containerItem->setProperty("x", QVariant::fromValue(cx));
            }
            if (centerHeight) {
                qreal cy = ( newGeometry.height() - contentBr.height() ) / 2.;
                _containerItem->setY( cy );
                // _containerItem->setProperty("y", QVariant::fromValue(cy));
            }
        }

        // In AutoFit mode, try anchoring the content to the visible border when the content has
        // custom user zoom
        if ( _autoFitMode == AutoFit &&
             ( _panModified || _zoomModified ) ) {
            bool anchorRight = false;
            bool anchorLeft = false;

            // Container item children Br mapped in root CS.
            QRectF contentBr = mapRectFromItem( _containerItem, _containerItem->childrenRect() );
            if ( contentBr.width() > newGeometry.width() &&
                 contentBr.right() < newGeometry.right() ) {
                anchorRight = true;
            }
            if ( contentBr.width() > newGeometry.width() &&
                 contentBr.left() > newGeometry.left() ) {
                anchorLeft = true;
            }
            if ( anchorRight ) {
                qreal xd = newGeometry.right() - contentBr.right();
                _containerItem->setX( _containerItem->x() + xd );
                // _containerItem->setProperty("x",  QVariant::fromValue(_containerItem->x() + xd));
            } else if ( anchorLeft ) {  // Right anchoring has priority over left anchoring...
                qreal xd = newGeometry.left( ) - contentBr.left( );
                _containerItem->setX( _containerItem->x() + xd );
                // _containerItem->setProperty("x",QVariant::fromValue(  _containerItem->x() + xd));
            }
        }

        updateGrid();
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QQuickItem::geometryChanged( newGeometry, oldGeometry );
#else
    QQuickItem::geometryChange( newGeometry, oldGeometry );
#endif
}

<<<<<<< HEAD
void Navigable::panOffset(QPointF delta){

    QPointF p{ QPointF{ _containerItem->x(), _containerItem->y() } - delta };

    qreal x=p.x();
    qreal y=p.y();

    _containerItem->setProperty("x",QVariant::fromValue(x));

    _containerItem->setProperty("y", QVariant::fromValue(y));
    emit containerItemModified();
    navigableContainerItemModified();
}

void Navigable::panTo(QPointF target){
    QPointF delta = viewPosition() - target;
    setDragActive(true);

    panOffset(delta);

    _panModified = true;
    setViewPosition(target);


    updateGrid();
}
void    Navigable::mouseMoveEvent( QMouseEvent* event )
{
    if ( getNavigable() && isDraggable() ) {
        if ( _leftButtonPressed && !viewPosition().isNull() ) {
            panTo(event->localPos());
        }
=======
void    Navigable::mouseMoveEvent(QMouseEvent* event)
{
    if (!getNavigable() ||
        !_containerItem) {
        QQuickItem::mouseMoveEvent(event);
        return;
    }
    if (_leftButtonPressed &&               // Left click panning /////////////
        !_lastPan.isNull()) {
        const QPointF delta = _lastPan - event->localPos();
        const auto p = QPointF{_containerItem->x(),
                               _containerItem->y()} - delta;
        _containerItem->setX(p.x());
        _containerItem->setY(p.y());
        emit containerItemModified();
        navigableContainerItemModified();
        _panModified = true;
        _lastPan = event->localPos();
        setDragActive(true);

        updateGrid();
    } else if (_selectionRectItem != nullptr &&
               _ctrlLeftButtonPressed) {    // Ctrl+Left click selection //////
        const auto& p = event->localPos();
        _selectionRectItem->setX(std::min(p.x(), _startSelectRect.x()));
        _selectionRectItem->setY(std::min(p.y(), _startSelectRect.y()));
        _selectionRectItem->setWidth(std::abs(p.x() - _startSelectRect.x()));
        _selectionRectItem->setHeight(std::abs(p.y() - _startSelectRect.y()));
        _lastSelectRect = event->localPos();
        const auto selectionRect = mapRectToItem(_containerItem,
                                                 QRectF{_selectionRectItem->x(), _selectionRectItem->y(),
                                                        _selectionRectItem->width(), _selectionRectItem->height()});
        selectionRectActivated(selectionRect);
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
    }
    QQuickItem::mouseMoveEvent(event);
}

void    Navigable::mousePressEvent(QMouseEvent* event)
{
<<<<<<< HEAD
    if ( getNavigable() ) {
        if ( event->button() == Qt::LeftButton ) {
            _leftButtonPressed = true;
            setViewPosition(event->localPos());
=======
    if (!getNavigable()) {
        event->ignore();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        if (getSelectionRectEnabled() &&
            event->modifiers() == Qt::ControlModifier) {
            _ctrlLeftButtonPressed = true;          // SELECT = Left button + CTRL //////
            _lastSelectRect = event->localPos();
            _startSelectRect = event->localPos();
            if (_selectionRectItem) {
                _selectionRectItem->setX(event->localPos().x());
                _selectionRectItem->setY(event->localPos().y());
                _selectionRectItem->setWidth(1.);
                _selectionRectItem->setHeight(1.);
                _selectionRectItem->setVisible(true);
                _selectRectActive = true;
            }
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
            event->accept();
            return;
        } else {
            _leftButtonPressed = true;              // PAN = Left button ////////////////
            _lastPan = event->localPos();
            event->accept();
            return;
        }
    }
    if (event->button() == Qt::RightButton) {       // Right clicks are caught //////////
        event->accept();
        return;
    }
    event->ignore();
}

void    Navigable::mouseReleaseEvent( QMouseEvent* event )
{
    if (getNavigable()) {
        if (event->button() == Qt::LeftButton &&
            !_dragActive &&
            !_selectRectActive) {       // Do not emit clicked when dragging occurs
            emit clicked(event->localPos());
            navigableClicked(event->localPos());
        } else if (event->button() == Qt::RightButton) {
            emit rightClicked(event->localPos());
            navigableRightClicked(event->localPos());
        }
        setDragActive(false);
        _leftButtonPressed = false;

        _ctrlLeftButtonPressed = false;     // End selection rect resizing
        _selectRectActive = false;
        if (_selectionRectItem) {
            _selectionRectItem->setVisible(false);
            selectionRectEnd();
        }
    }
    QQuickItem::mouseReleaseEvent(event);
}

void    Navigable::wheelEvent(QWheelEvent* event)
{
    if (getNavigable()) {
        qreal zoomFactor = (event->angleDelta().y() > 0. ? _zoomIncrement : -_zoomIncrement);
        zoomOn(event->position(), getZoom() + zoomFactor);
    }
    updateGrid();
    // Note 20160117: NavigableArea is opaque for wheel events, do not call QQuickItem::wheelEvent(event);
}
//-----------------------------------------------------------------------------


/* Selection Rectangle Management *///-----------------------------------------
void    Navigable::setSelectionRectEnabled(bool selectionRectEnabled) noexcept
{
    if (selectionRectEnabled != _selectionRectEnabled) {
        _selectionRectEnabled = selectionRectEnabled;
        _ctrlLeftButtonPressed = false; // Reset the selection rect state
        _selectRectActive = false;
        _lastSelectRect = QPointF{};
        _startSelectRect = QPointF{};
        if (!_selectionRectEnabled)
            selectionRectEnd();
        emit selectionRectEnabledChanged();
    }
}

void    Navigable::setSelectionRectItem(QQuickItem* selectionRectItem) noexcept { _selectionRectItem = selectionRectItem; }

void    Navigable::selectionRectActivated(const QRectF& rect) { Q_UNUSED(rect); }

void    Navigable::selectionRectEnd() { }
//-----------------------------------------------------------------------------


/* Grid Management *///--------------------------------------------------------
void    Navigable::setGrid(qan::Grid* grid) noexcept
{
    if ( grid != _grid ) {
        if ( _grid ) {                    // Hide previous grid
            disconnect( _grid, nullptr,
                        this, nullptr );  // Disconnect every update signals from grid to this navigable
        }
        _grid = grid;                       // Configure new grid
        if ( _grid ) {
            _grid->setParentItem( this );
            _grid->setZ( -1.0 );
            _grid->setAntialiasing(false);
            _grid->setScale(1.0);
            connect(grid,   &QQuickItem::visibleChanged, // Force updateGrid when visibility is changed to eventually
                    this,   &Navigable::updateGrid);    // take into account any grid property change while grid was hidden.
        }
        if (!_grid)
            _grid = _defaultGrid.get(); // Do not connect default grid, it is an "empty grid"
        updateGrid();
        emit gridChanged();
    }
}

void    Navigable::updateGrid() noexcept
{
<<<<<<< HEAD
    if(lockGridUpdate()==false){
        if ( _grid &&
             _containerItem != nullptr ) {
            // Generate a view rect to update grid
            QRectF viewRect{ _containerItem->mapFromItem(this, {0.,0.}),
                        _containerItem->mapFromItem(this, {width(), height()}) };
            _grid->updateGrid(viewRect, *_containerItem, *this );
        }
=======
    if ( _grid &&
         _containerItem != nullptr ) {
        // Generate a view rect to update grid
        QRectF viewRect{ _containerItem->mapFromItem(this, {0.,0.}),
                         _containerItem->mapFromItem(this, {width(), height()}) };
        if (!viewRect.isEmpty())
            _grid->updateGrid(viewRect, *_containerItem, *this );
>>>>>>> ab88d77ec62175b9fd499a154ffaf92f7bf23989
    }
}

void Navigable::touchEvent(QTouchEvent *event)
{

    QQuickItem::touchEvent(event);
}
//-----------------------------------------------------------------------------

} // ::qan
