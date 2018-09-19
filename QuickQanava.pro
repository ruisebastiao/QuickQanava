TEMPLATE = lib
TARGET = QuickQanava
QT += qml quick
CONFIG += plugin c++14

TARGET = automationmodule
uri = QuickQanava

DEFINES+=QUICKQANAVA_LIB

PLUGIN_NAME = QuickQanava
PLUGIN_PATH = QuickQanava

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))


OTHER_FILES += \
    qml/*.qml \
    qml/qmldir

include(src/quickqanava.pri)
DISTFILES = +=qml/qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/src/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}


