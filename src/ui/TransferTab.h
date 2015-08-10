#ifndef TRANSFER_TAB_H
#define TRANSFER_TAB_H

#include <QWidget>
#include <mr/common>
#include "../sql.h"
#include "../TransferModel.h"
#include "../TransferProxyModel.h"
#include "../TransferStats.h"
#include "../TagHelper.h"

namespace Ui {
	class TransferTab;
}
class AccountModel;

class TransferTab : public QWidget {
	Q_OBJECT
public:
	explicit TransferTab(QWidget *parent = 0);
	~TransferTab();

	void init(Db db, AccountModel* accountModel);

public slots:
	void reloadCache();

private:
	Ui::TransferTab *ui;
	
	Db db;
	TagHelper tagHelper;
	AccountModel* accountModel;

	int currentTransferId;
	TransferModel* model;
	TransferProxyModel* proxyModel;
	TransferStats stats;

private slots:
	void createFilterMonthLinks();
	void clickedFilterMonthLink();
	
	void setCurrentTransfer(const QModelIndex& index);
	void showSelected(const QItemSelection& selected, const QItemSelection& deseceted);
	void checkSelected();
	
	void updateTags();
	void updateDetails();
	void saveNote();

	void addToStats(int transferId) { updateStats(transferId, 1); }
	void removeFromStats(int transferId) { updateStats(transferId, -1); }
	void updateStats(int transferId = -1, int mode = 0);
	void resetStats();
};

#endif // TRANSFER_TAB_H
