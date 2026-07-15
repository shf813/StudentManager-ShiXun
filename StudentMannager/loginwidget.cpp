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
    ui->editUser->setPlaceholderText("请输入学号或账号");
    ui->editPwd->setPlaceholderText("请输入密码");

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
    if(query.exec() && query.next()) {
        m_loginRole = static_cast<UserRole>(query.value("role").toInt());
        m_loginUsername = user;   // 保存用户名（学号）
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
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("用户名/密码不能为空");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }
    if(!verifyAccount(user, pwd))
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("账号或密码错误");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
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
    if(ui->ckDark->isChecked())
        GlobalStyle::switchDarkTheme();
    else
        GlobalStyle::switchLightTheme();
    regDlg.exec();
}

void LoginWidget::switchTheme(bool isDark)
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    if (isDark)
        GlobalStyle::switchDarkTheme();
    else
        GlobalStyle::switchLightTheme();
    this->update();
}
