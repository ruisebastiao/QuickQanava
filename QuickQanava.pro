TEMPLATE = lib
TARGET = QuickQanava
QT += qml quick
CONFIG += plugin c++14

TARGET = $$qtLibraryTarget($$TARGET)
uri = QuickQanava

DEFINES+=QUICKQANAVA_LIB

PLUGIN_NAME = QuickQanava
PLUGIN_PATH = QuickQanava

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))



include(src/quickqanava.pri)
DISTFILES = src/qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/src/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = src/qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path += /usr/lib
    INSTALLS += target qmldir
}
