#ifndef ACCOUNT_TAB_H
#define ACCOUNT_TAB_H

#include "Tab.h"
#include "../sql.h"
#include "../model/AccountModel.h"
#include "../model/AccountProxyModel.h"
#include "../TagHelper.h"

namespace Ui {
	class AccountTab;
}

class AccountTab : public Tab {
	Q_OBJECT
public:
	explicit AccountTab(QWidget* parent = 0);
	~AccountTab();

	using Tab::init;
	virtual void init(Db db, std::shared_ptr<AccountModel> accountModel);

public slots:
	virtual void refresh() {}
	virtual void focusSearchField();

	void setInitialBalance(double d);
	void mergeAccounts();

signals:
	void mergeAccountsEnabled(bool enabled);

private:
	Ui::AccountTab *ui;
	
	std::shared_ptr<AccountModel> model;

	TagHelper tagHelper;

	int currentAccountId;
	AccountProxyModel* proxyModel;

private slots:
	void setCurrentAccount(const QModelIndex& index);
	void showSelectedAccounts(const QItemSelection& selected, const QItemSelection& deseceted);
	void updateAccountInfo();
	void updateAccountDetails();
};

#endif // ACCOUNT_TAB_H
