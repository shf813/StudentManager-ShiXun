#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QDialog>
#include "commondef.h"

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QDialog
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

    // 获取登录用户角色，给main调用
    UserRole getUserRole() const;
    // 主题切换槽，与主窗口联动
public slots:
    void switchTheme(bool isDark);

private slots:
    void onBtnLogin();
    void onBtnRegister();

private:
    Ui::LoginWidget *ui;
    UserRole m_loginRole;
    bool verifyAccount(const QString& user, const QString& pwd);
};

#endif // LOGINWIDGET_H
