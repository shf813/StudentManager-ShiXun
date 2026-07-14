#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "stylehelper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    this->setObjectName("RegisterDialog"); // 绑定ID给全局样式选择器
    this->setWindowTitle("用户注册");
    this->setFixedSize(320, 260);
    // 填充角色下拉框
    ui->cbxRole->addItem("管理员", ROLE_ADMIN);
    ui->cbxRole->addItem("普通用户", ROLE_NORMAL);
    connect(ui->btnOk, &QPushButton::clicked, this, &RegisterDialog::onBtnOkClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &RegisterDialog::onBtnCancelClicked);
    // 默认加载亮色样式
    ui->btnOk->setProperty("btnType", "primary");
    ui->btnCancel->setProperty("btnType", "cancel");
}
RegisterDialog::~RegisterDialog()
{
    delete ui;
}
bool RegisterDialog::checkUserExist(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM sys_user WHERE username = ?");
    query.addBindValue(username);
    query.exec();
    return query.next();
}
bool RegisterDialog::insertNewUser(const QString &user, const QString &pwd, UserRole role)
{
    QSqlQuery query;
    query.prepare("INSERT INTO sys_user(username, password, role) VALUES (?, ?, ?)");
    query.addBindValue(user);
    query.addBindValue(pwd);
    query.addBindValue(role);
    bool ok = query.exec();
    if(!ok)
        QMessageBox::critical(this, "数据库错误", query.lastError().text());
    return ok;
}
void RegisterDialog::onBtnOkClicked()
{
    QString user = ui->regUser->text().trimmed();
    QString pwd = ui->regPwd->text();
    QString pwd2 = ui->regPwd2->text();
    UserRole role = static_cast<UserRole>(ui->cbxRole->currentData().toInt());
    if(user.isEmpty() || pwd.isEmpty())
    {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空");
        return;
    }
    if(pwd != pwd2)
    {
        QMessageBox::warning(this, "提示", "两次输入密码不一致");
        return;
    }
    if(checkUserExist(user))
    {
        QMessageBox::warning(this, "提示", "该用户名已存在");
        return;
    }
    if(insertNewUser(user, pwd, role))
    {
        QMessageBox::information(this, "成功", "注册完成，请登录！");
        this->accept();
    }
}
void RegisterDialog::onBtnCancelClicked()
{
    this->reject();
}
