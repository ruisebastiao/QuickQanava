PLUGIN_NAME = QuickQanava
PLUGIN_PATH = QuickQanava
PLUGIN_QML_DIR=$$PWD/src/qml/

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += QUICKQANAVA_LIB

uri = QuickQanava


QML_IMPORT_PATH += \
                $${PLUGIN_DEPLOY_PATH}

message(path:  $${QML_IMPORT_PATH})

linkLocalLibrary(automationstudiocore,   automationstudiocore)

# Source

include($$PWD/src/quickqanava.pri)

