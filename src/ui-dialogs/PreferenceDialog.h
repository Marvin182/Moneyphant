#ifndef UI_PREFERENCE_DIALOG_H
#define UI_PREFERENCE_DIALOG_H

#include <QDialog>

namespace Ui {
class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
	Q_OBJECT
public:
	explicit PreferenceDialog(QWidget *parent = 0);
	~PreferenceDialog();

private:
	Ui::PreferenceDialog *ui;
};

#endif // UI_PREFERENCE_DIALOG_H
