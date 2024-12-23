
CONFIG      += warn_on qt thread c++14
QT          += core widgets gui qml quick


include(../GTpo/src/gtpo.pri)
include(../QuickContainers/src/quickcontainers.pri)

DEPENDPATH      += $$PWD
INCLUDEPATH     += $$PWD


QML_IMPORT_PATH += \
                $$PWD

HEADERS +=  $$PWD/QuickQanava.h             \
            $$PWD/qanUtils.h                \
            $$PWD/qanGraphConfig.h          \
            $$PWD/qanGraphView.h            \
            $$PWD/qanEdge.h                 \
            $$PWD/qanEdgeItem.h             \
            $$PWD/qanNode.h                 \
            $$PWD/qanNodeItem.h             \
            $$PWD/qanPortItem.h             \
            $$PWD/qanSelectable.h           \
            $$PWD/qanDraggable.h            \
            $$PWD/qanAbstractDraggableCtrl.h\
            $$PWD/qanDraggableCtrl.h        \
            $$PWD/qanDraggableCtrl.hpp      \
            $$PWD/qanConnector.h            \
            $$PWD/qanBehaviour.h            \
            $$PWD/qanGroup.h                \
            $$PWD/qanGroupItem.h            \
            $$PWD/qanGraph.h                \
            $$PWD/qanGraph.hpp              \
            $$PWD/qanStyle.h                \
            $$PWD/qanStyleManager.h         \
            $$PWD/qanNavigable.h            \
            $$PWD/qanNavigablePreview.h     \
            $$PWD/qanGrid.h                 \
            $$PWD/qanContainerAdapter.h     \
            $$PWD/qanBottomRightResizer.h   \
            $$PWD/quickqanavaplugin_plugin.h\
            $$PWD/quickqanavaglobal.h


SOURCES +=  $$PWD/qanGraphView.cpp          \
            $$PWD/qanUtils.cpp              \
            $$PWD/qanEdge.cpp               \
            $$PWD/qanEdgeItem.cpp           \
            $$PWD/qanNode.cpp               \
            $$PWD/qanNodeItem.cpp           \
            $$PWD/qanPortItem.cpp           \
            $$PWD/qanSelectable.cpp         \
            $$PWD/qanDraggable.cpp          \
            $$PWD/qanConnector.cpp          \
            $$PWD/qanBehaviour.cpp          \
            $$PWD/qanGraph.cpp              \
            $$PWD/qanGroup.cpp              \
            $$PWD/qanGroupItem.cpp          \
            $$PWD/qanStyle.cpp              \
            $$PWD/qanStyleManager.cpp       \
            $$PWD/qanNavigable.cpp          \
            $$PWD/qanNavigablePreview.cpp   \
            $$PWD/qanGrid.cpp               \
            $$PWD/qanBottomRightResizer.cpp \
            $$PWD/quickqanavaplugin_plugin.cpp \



OTHER_FILES +=  $$PWD/QuickQanava \
    $$PWD/qml/*.qml \
    $$PWD/qml/qmldir

#CONFIG      += use_graphviz
use_graphviz {
    DEFINES     += USE_GRAPHVIZ
    unix: CONFIG += link_pkgconfig
    unix: PKGCONFIG += libgvc

    GRAPHVIZ_DIR = "C:\graphviz"

    # note these are x64 libs, graphviz only distributes x86 ones
    win32: LIBS += $$GRAPHVIZ_DIR/lib/gvc.lib
    win32: LIBS += $$GRAPHVIZ_DIR/lib/cgraph.lib
    win32: LIBS += $$GRAPHVIZ_DIR/lib/cdt.lib
    win32: LIBS += $$GRAPHVIZ_DIR/lib/gvplugin_dot_layout.lib
    win32: INCLUDEPATH += "$$GRAPHVIZ_DIR/include/graphviz"
    win32: DEPENDPATH += "$$GRAPHVIZ_DIR/include/graphviz"
    win32: PRE_TARGETDEPS += $$GRAPHVIZ_DIR/lib/gvc.lib
    win32: PRE_TARGETDEPS += $$GRAPHVIZ_DIR/lib/cdt.lib
    win32: PRE_TARGETDEPS += $$GRAPHVIZ_DIR/lib/cgraph.lib
}
