#include "quickqanavaplugin_plugin.h"
//#include "quickqanava.h"

// GTpo headers
#include "GTpo.h"

// QuickContainers headers
#include "../QuickContainers/src/QuickContainers.h"

// Qt header
#include <QQmlEngine>

// QuickQanava headers
#include "./qanGraphConfig.h"
#include "./qanEdge.h"
#include "./qanEdgeItem.h"
#include "./qanNode.h"
#include "./qanNodeItem.h"
#include "./qanPortItem.h"
#include "./qanConnector.h"
#include "./qanGroup.h"
#include "./qanGroupItem.h"
#include "./qanGraph.h"
#include "./qanNavigable.h"
#include "./qanGrid.h"
#include "./qanGraphView.h"
#include "./qanStyle.h"
#include "./qanStyleManager.h"
#include "./qanBottomRightResizer.h"
#include "./qanNavigablePreview.h"


#include <qqml.h>

void QuickQanavaPluginPlugin::registerTypes(const char *uri)
{
    // @uri QuickQanava
    // qmlRegisterType<QuickQanava>(uri, 1, 0, uri);
    qmlRegisterType< qan::Node >( uri, 2, 0, "AbstractNode");
    qmlRegisterType< qan::NodeItem >( uri, 2, 0, "NodeItem");
    qmlRegisterType< qan::PortItem >( uri, 2, 0, "PortItem");
    qmlRegisterType< qan::Edge >( uri, 2, 0, "AbstractEdge");
    qmlRegisterType< qan::EdgeItem >( uri, 2, 0, "EdgeItem");
    qmlRegisterType< qan::Group >( uri, 2, 0, "AbstractGroup");
    qmlRegisterType< qan::GroupItem >( uri, 2, 0, "GroupItem");
    qmlRegisterType< qan::Connector >( uri, 2, 0, "Connector");

    qmlRegisterType< qan::Graph >( uri, 2, 0, "Graph");
    qmlRegisterType< qan::GraphView >( uri, 2, 0, "AbstractGraphView");
    qmlRegisterType< qan::Navigable >( uri, 2, 0, "Navigable");
    qmlRegisterType< qan::NavigablePreview >( uri, 2, 0, "AbstractNavigablePreview");
    qmlRegisterType< qan::OrthoGrid >( uri, 2, 0, "OrthoGrid");
    qmlRegisterType< qan::PointGrid >( uri, 2, 0, "AbstractPointGrid");
    qmlRegisterType< qan::LineGrid >( uri, 2, 0, "AbstractLineGrid");
    qmlRegisterType< qan::Style >( uri, 2, 0, "Style");
    qmlRegisterType< qan::NodeStyle >( uri, 2, 0, "NodeStyle");
    qmlRegisterType< qan::EdgeStyle >( uri, 2, 0, "EdgeStyle");
    qmlRegisterType< qan::StyleManager >( uri, 2, 0, "StyleManager");
    qmlRegisterType< qan::BottomRightResizer >( uri, 2, 0, "BottomRightResizer" );


}

void QuickQanavaPluginPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
//    //Q_INIT_RESOURCE(QuickQanava);
//#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
//    qWarning() << "QuickQanava::initialize(): Warning: QuickQanava depends on Qt Quick Shapes library available since Qt 5.10.";
//#endif
    engine->addImportPath("qrc:///");

    QuickContainers::initialize();

    if ( engine ) {
        engine->rootContext()->setContextProperty( "defaultNodeStyle", QVariant::fromValue(qan::Node::style()) );
        engine->rootContext()->setContextProperty( "defaultEdgeStyle", QVariant::fromValue(qan::Edge::style()) );
        engine->rootContext()->setContextProperty( "defaultGroupStyle", QVariant::fromValue(qan::Group::style()) );
    }

}

