#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "registerdialog.h"
#include "stylehelper.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QSettings>
#include <QStyleFactory>
LoginWidget::LoginWidget(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginWidget), m_loginRole(ROLE_NORMAL)
{
    ui->setupUi(this);
    this->setObjectName("LoginWidget");
    this->setWindowTitle("系统登录");
    this->setFixedSize(360, 280);
    QSettings setting("StudentManager", "loginCfg");
    bool remember = setting.value("remember", false).toBool();
    ui->ckRemember->setChecked(remember);
    if(remember)
    {
        ui->editUser->setText(setting.value("username").toString());
        ui->editPwd->setText(setting.value("password").toString());
    }

    ui->btnLogin->setProperty("btnType", "primary");
    ui->btnRegister->setProperty("btnType", "link");

    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginWidget::onBtnLogin);
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginWidget::onBtnRegister);
    connect(ui->ckDark, &QCheckBox::toggled, this, &LoginWidget::switchTheme);
    qApp->setStyle(QStyleFactory::create("Fusion"));

    ui->ckDark->setChecked(GlobalStyle::isDarkMode());
}
LoginWidget::~LoginWidget()
{
    delete ui;
}
UserRole LoginWidget::getUserRole() const
{
    return m_loginRole;
}
bool LoginWidget::verifyAccount(const QString &user, const QString &pwd)
{
    QSqlQuery query;
    query.prepare("SELECT role FROM sys_user WHERE username = ? AND password = ?");
    query.addBindValue(user);
    query.addBindValue(pwd);
    if(query.exec() && query.next())
    {
        m_loginRole = static_cast<UserRole>(query.value("role").toInt());
        return true;
    }
    return false;
}
void LoginWidget::onBtnLogin()
{
    QString user = ui->editUser->text().trimmed();
    QString pwd = ui->editPwd->text();
    if(user.isEmpty() || pwd.isEmpty())
    {
        QMessageBox::warning(this, "提示", "用户名/密码不能为空");
        return;
    }
    if(!verifyAccount(user, pwd))
    {
        QMessageBox::critical(this, "错误", "账号或密码错误");
        return;
    }
    QSettings setting("StudentManager", "loginCfg");
    if(ui->ckRemember->isChecked())
    {
        setting.setValue("remember", true);
        setting.setValue("username", user);
        setting.setValue("password", pwd);
    }
    else
    {
        setting.clear();
    }
    this->accept();
}
void LoginWidget::onBtnRegister()
{
    RegisterDialog regDlg(this);
    // 同步当前明暗主题给注册窗口
    if(ui->ckDark->isChecked())
        GlobalStyle::switchDarkTheme();
    else
        GlobalStyle::switchLightTheme();
    regDlg.exec();
}
// LoginWidget::switchTheme
void LoginWidget::switchTheme(bool isDark)
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    if (isDark)
    {
        GlobalStyle::switchDarkTheme();
    }
    else
    {
        GlobalStyle::switchLightTheme();
    }
    this->update(); // 弹窗全局重绘
}
