#ifndef TRANSFER_TAB_H
#define TRANSFER_TAB_H

#include <mr/common>
#include <mr/qt>
#include "Tab.h"
#include "../sql.h"
#include "../model/AccountModel.h"
#include "../model/TransferModel.h"
#include "../model/TransferProxyModel.h"
#include "../TransferStats.h"
#include "../TagHelper.h"

namespace Ui {
	class TransferTab;
}

class TransferTab : public Tab {
	Q_OBJECT
public:
	explicit TransferTab(QWidget* parent = 0);
	~TransferTab();

	using Tab::init;
	virtual void init(Db db, std::shared_ptr<TransferModel> transferModel);

public slots:
	virtual void refresh() {}
	virtual void focusSearchField();

	void toggleCheckedOfSelected();
	void toggleInternalOfSelected();
	void removeSelected();

private:
	Ui::TransferTab *ui;
	
	std::shared_ptr<TransferModel> model;

	TagHelper tagHelper;

	int currentTransferId;
	TransferProxyModel* proxyModel;
	TransferStats stats;

private slots:
	void createFilterMonthLinks();
	void clickedFilterMonthLink();
	
	void setCurrentTransfer(const QModelIndex& index);
	void showSelected(const QItemSelection& selected, const QItemSelection& deseceted);
	
	void updateTags();
	void updateDetails();
	void saveNote();

	void addToStats(int transferId) { updateStats(transferId, 1); }
	void removeFromStats(int transferId) { updateStats(transferId, -1); }
	void updateStats(int transferId = -1, int mode = 0);
	void resetStats();
};

#endif // TRANSFER_TAB_H
