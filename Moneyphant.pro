QT       += core gui widgets printsupport

CONFIG   += c++11

QMAKE_CFLAGS_WARN_ON -= -W
QMAKE_CXXFLAGS_WARN_ON -= -W

DEFINES *= QT_USE_QSTRINGBUILDER

TARGET = Moneyphant
TEMPLATE = app

ICON = graphics/icon.icns
QMAKE_INFO_PLIST = Info.plist
OTHER_FILES += Info.plist

GIT_DIR=$$PWD
include(config.pri)
include($$MR_INCLUDE/../gitversion.pri)

SOURCES += src/main.cpp \
	src/Evolutions.cpp \
	src/StatementReader.cpp \
	src/TagHelper.cpp \
	src/TransferStats.cpp \
	src/Updater.cpp \
	src/util.cpp \
	src/version.cpp \
	src/model/Account.cpp \
	src/model/AccountModel.cpp \
	src/model/AccountProxyModel.cpp \
        src/model/Currency.cpp \
        src/model/CurrencyWithAmount.cpp \
	src/model/StatementFileFormat.cpp \
		#src/model/TagModel.cpp \
	src/model/Transfer.cpp \
	src/model/TransferModel.cpp \
	src/model/TransferProxyModel.cpp \
	src/ui/ColumnChooser.cpp \
	src/ui/MainWindow.cpp \
	src/ui/TagEdit.cpp \
		#src/ui/TagField.cpp \
	src/ui-dialogs/AboutDialog.cpp \
	src/ui-dialogs/PreferenceDialog.cpp \
	src/ui-dialogs/StatementImporterDialog.cpp \
	src/ui-tabs/AccountTab.cpp \
	src/ui-tabs/BalanceTab.cpp \
	src/ui-tabs/ExpenseTab.cpp \
	src/ui-tabs/TransferTab.cpp \
	lib/qcustomplot.cpp

HEADERS += src/db.h \
	src/Evolutions.h \
	src/Iban.h \
	src/sql.h \
	src/StatementReader.h \
	src/TagHelper.h \
	src/TransferStats.h \
	src/Updater.h \
	src/util.h \
	src/version.h \
	src/model/Account.h \
	src/model/AccountModel.h \
	src/model/AccountProxyModel.h \
        src/model/Currency.h \
        src/model/CurrencyWithAmount.h \
	src/model/StatementFileFormat.h \
		#src/model/TagModel.h \
	src/model/Transfer.h \
	src/model/TransferModel.h \
	src/model/TransferProxyModel.h \
	src/ui/ColumnChooser.h \
	src/ui/MainWindow.h \
	src/ui/TagEdit.h \
		#src/ui/TagField.h \
	src/ui-dialogs/AboutDialog.h \
	src/ui-dialogs/PreferenceDialog.h \
	src/ui-dialogs/StatementImporterDialog.h \
	src/ui-tabs/AccountTab.h \
	src/ui-tabs/BalanceTab.h \
	src/ui-tabs/ExpenseTab.h \
	src/ui-tabs/Tab.h \
	src/ui-tabs/TransferTab.h \
	lib/qcustomplot.h

FORMS += src/ui/MainWindow.ui \
	src/ui-dialogs/AboutDialog.ui \
	src/ui-dialogs/PreferenceDialog.ui \
	src/ui-dialogs/StatementImporterDialog.ui \
	src/ui-tabs/AccountTab.ui \
	src/ui-tabs/BalanceTab.ui \
	src/ui-tabs/ExpenseTab.ui \
	src/ui-tabs/TransferTab.ui

RESOURCES += sql/evolutions.qrc \
	graphics/graphics.qrc

# mr lib
INCLUDEPATH += $$MR_INCLUDE
LIBS += $$MR_LIB

# sqlpp11 + sqlite3 connector
LIBS += $$SQLPP11_SQLITE3_CONNECTOR_LIB $$SQLITE3_LIB
INCLUDEPATH += $$SQLPP11_INCLUDE $$SQLPP11_SQLITE3_CONNECTOR_INCLUDE $$DATE_INCLUDE
PRE_TARGETDEPS += $$SQLPP11_SQLITE3_CONNECTOR_LIB
