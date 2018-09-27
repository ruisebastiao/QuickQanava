TEMPLATE = lib
TARGET = QuickQanava
QT += qml quick opengl
CONFIG += plugin c++14

uri = QuickQanava

DEFINES+=QUICKQANAVA_LIB

RESOURCES       += src/QuickQanava.qrc

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir

include(src/quickqanava.pri)


DISTFILES = qml/qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qml/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

include(../../plugin.pri)

#qmldir.files = qmldir
#unix {
#    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
#    qmldir.path = $$installPath
#    target.path = $$installPath
#    INSTALLS += target qmldir


#}


