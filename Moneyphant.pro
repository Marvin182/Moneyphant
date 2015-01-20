QT       += core gui widgets

CONFIG   += c++11

QMAKE_CFLAGS_WARN_ON -= -W
QMAKE_CXXFLAGS_WARN_ON -= -W

TARGET = Moneyphant
TEMPLATE = app

ICON = icon.icns
QMAKE_INFO_PLIST = Info.plist
OTHER_FILES += Info.plist

SOURCES += src/main.cpp\
        src/MainWindow.cpp \
        src/Account.cpp \
        src/Evolutions.cpp \
    src/StatementReader.cpp \
    src/Transfer.cpp \
    src/AccountModel.cpp \
    src/TagEdit.cpp \
    src/TagHelper.cpp \
    src/TransferModel.cpp \
    src/TransferProxyModel.cpp \
    src/TransferStats.cpp \
    src/globals/pempek_assert.cpp \
    src/globals/string-util.cpp \
    src/globals/assert.cpp

HEADERS  += src/MainWindow.h \
        src/Account.h \
        src/Evolutions.h \
        src/db.h \
    src/StatementReader.h \
    src/Transfer.h \
    src/AccountModel.h \
    src/TagEdit.h \
    src/TagHelper.h \
    src/TransferModel.h \
    src/TransferProxyModel.h \
    src/TransferStats.h \
    src/globals/all.h \
    src/globals/assert.h \
    src/globals/pempek_assert.h \
    src/globals/sqlpp.h \
    src/globals/string-util.h

FORMS    += ui/mainwindow.ui

# sqlpp11
LIBS += -L$$PWD/../../lib/sqlpp11-connector-sqlite3/build/src/ -lsqlpp11-connector-sqlite3 \
            -L$$PWD/../../../../usr/lib/ -lsqlite3
INCLUDEPATH +=  /Users/marvin/lib/sqlpp11/include \
                $$PWD/../../lib/sqlpp11-connector-sqlite3/include
DEPENDPATH += $$PWD/../../lib/sqlpp11-connector-sqlite3/include
PRE_TARGETDEPS += $$PWD/../../lib/sqlpp11-connector-sqlite3/build/src/libsqlpp11-connector-sqlite3.a

# libc
INCLUDEPATH += /Users/marvin/lib/clang2/llvm/projects/libcxx/include/

RESOURCES += \
    sql/evolutions.qrc

