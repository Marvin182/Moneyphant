QT       += core gui widgets

CONFIG   += c++11

QMAKE_CFLAGS_WARN_ON -= -W
QMAKE_CXXFLAGS_WARN_ON -= -W

TARGET = Moneyphant
TEMPLATE = app

ICON = graphics/icon.icns
QMAKE_INFO_PLIST = Info.plist
OTHER_FILES += Info.plist

include(gitversion.pri)

SOURCES += src/main.cpp\
    src/MainWindow.cpp \
    src/Account.cpp \
    src/Evolutions.cpp \
    src/StatementReader.cpp \
    src/Transfer.cpp \
    src/AccountModel.cpp \
    src/AccountProxyModel.cpp \
    src/TagEdit.cpp \
    src/TagHelper.cpp \
    src/TransferModel.cpp \
    src/TransferProxyModel.cpp \
    src/TransferStats.cpp \
    src/ui/AboutDialog.cpp \
    src/ui/PreferenceDialog.cpp \
    src/ui/StatementImporterDialog.cpp \
    src/ui/ColumnChooser.cpp \
    src/model/StatementFileFormat.cpp

HEADERS  += src/MainWindow.h \
    src/Account.h \
    src/Evolutions.h \
    src/db.h \
    src/StatementReader.h \
    src/Transfer.h \
    src/AccountModel.h \
    src/AccountProxyModel.h \
    src/TagEdit.h \
    src/TagHelper.h \
    src/TransferModel.h \
    src/TransferProxyModel.h \
    src/TransferStats.h \
    src/ui/AboutDialog.h \
    src/ui/PreferenceDialog.h \
    src/ui/StatementImporterDialog.h \
    src/ui/ColumnChooser.h \
    src/sql.h \
    src/model/StatementFileFormat.h

FORMS    += src/ui/MainWindow.ui \
    src/ui/AboutDialog.ui \
    src/ui/PreferenceDialog.ui \
    src/ui/StatementImporterDialog.ui

RESOURCES += sql/evolutions.qrc \
    graphics/graphics.qrc

INCLUDEPATH += ../mr/include
LIBS += -L../mr/release -lmr

# sqlpp11
LIBS += -L$$PWD/../../lib/sqlpp11-connector-sqlite3/build/src/ -lsqlpp11-connector-sqlite3 \
            -L$$PWD/../../../../usr/lib/ -lsqlite3
INCLUDEPATH += $$PWD/../../lib/sqlpp11/include \
                $$PWD/../../lib/sqlpp11-connector-sqlite3/include
DEPENDPATH += $$PWD/../../lib/sqlpp11-connector-sqlite3/include
PRE_TARGETDEPS += $$PWD/../../lib/sqlpp11-connector-sqlite3/build/src/libsqlpp11-connector-sqlite3.a
