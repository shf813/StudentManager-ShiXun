#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "commondef.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void onBtnOkClicked();
    void onBtnCancelClicked();

private:
    Ui::RegisterDialog *ui;
    bool checkUserExist(const QString& username);
    bool insertNewUser(const QString& user, const QString& pwd, UserRole role);
    bool validateStudent(const QString& stuNo, const QString& name, const QString& gender, const QString& idCard);
};

#endif // REGISTERDIALOG_H
