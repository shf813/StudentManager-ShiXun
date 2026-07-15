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
    this->setObjectName("RegisterDialog");
    this->setWindowTitle("学生注册");
    this->setFixedSize(300, 350);

    // ----- 初始化性别下拉框 -----
    ui->cbxGender->addItem("请选择性别", "");      // 默认占位项，值为空
    ui->cbxGender->addItem("男", "男");
    ui->cbxGender->addItem("女", "女");
    ui->cbxGender->setCurrentIndex(0);              // 默认选中"请选择性别"

    // ----- 初始化角色下拉框（固定为普通用户）-----
    ui->cbxRole->addItem("管理员", ROLE_ADMIN);
    ui->cbxRole->addItem("学生", ROLE_NORMAL);
    ui->cbxRole->setCurrentIndex(1);                // 默认普通用户
    ui->cbxRole->setEnabled(false);                 // 禁止修改

    ui->editStuNo->setPlaceholderText("请输入学号");
    ui->editName->setPlaceholderText("请输入姓名");
    ui->editIdCard->setPlaceholderText("请输入18位身份证号");
    ui->regPwd->setPlaceholderText("设置密码");
    ui->regPwd2->setPlaceholderText("再次输入密码确认");

    // 设置按钮样式
    ui->btnOk->setProperty("btnType", "primary");
    ui->btnCancel->setProperty("btnType", "cancel");

    // 连接信号槽
    connect(ui->btnOk, &QPushButton::clicked, this, &RegisterDialog::onBtnOkClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &RegisterDialog::onBtnCancelClicked);
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

bool RegisterDialog::validateStudent(const QString& stuNo, const QString& name,
                                     const QString& gender, const QString& idCard)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM students WHERE stu_no = ? AND name = ? AND gender = ? AND id_card = ?");
    query.addBindValue(stuNo);
    query.addBindValue(name);
    query.addBindValue(gender);
    query.addBindValue(idCard);
    if (!query.exec()) {
        QMessageBox::critical(this, "数据库错误", query.lastError().text());
        return false;
    }
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
    if(!ok) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("数据库错误");
        msgBox.setText(query.lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
    }
    return ok;
}

void RegisterDialog::onBtnOkClicked()
{
    // 获取输入：学号、姓名、性别、身份证、密码、确认密码
    QString stuNo = ui->editStuNo->text().trimmed();      // 假设命名为 editStuNo
    QString name  = ui->editName->text().trimmed();
    QString gender = ui->cbxGender->currentText();
    QString idCard = ui->editIdCard->text().trimmed();
    QString pwd = ui->regPwd->text();
    QString pwd2 = ui->regPwd2->text();

    // ----- 验证性别是否已选择（不为"请选择性别"）-----
    if (ui->cbxGender->currentIndex() == 0) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("请选择性别");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // 基本非空验证
    if (stuNo.isEmpty() || name.isEmpty() || gender.isEmpty() || idCard.isEmpty() || pwd.isEmpty()) {
        // 弹窗提示
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("所有字段均为必填");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // 检查两次密码是否一致
    if (pwd != pwd2) {
        // 提示不一致
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("两次输入密码不一致");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // 验证学生信息是否匹配
    if (!validateStudent(stuNo, name, gender, idCard)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("验证失败");
        msgBox.setText("学号、姓名、性别、身份证信息不匹配，请确认输入正确");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // 检查该学号是否已注册（因为 username 即学号）
    if (checkUserExist(stuNo)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("该学号已注册，请直接登录");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // 插入新用户，角色固定为普通用户（ROLE_NORMAL）
    if (insertNewUser(stuNo, pwd, ROLE_NORMAL)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("成功");
        msgBox.setText("注册完成，请使用学号和密码登录！");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        this->accept();
    }
}

void RegisterDialog::onBtnCancelClicked()
{
    this->reject();
}
