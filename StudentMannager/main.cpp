#include "mainwindow.h"
#include"loginwidget.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include<QSqlError>
#include"stylehelper.h"

// 全局数据库初始化函数
bool initGlobalDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("school_db.db");
    if (!db.open())
    {
        QMessageBox::critical(nullptr, "数据库打开失败", db.lastError().text());
        return false;
    }

    QSqlQuery query;
    // 创建学生表
    QString stuSql = "CREATE TABLE IF NOT EXISTS students ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                     "stu_no TEXT NOT NULL UNIQUE, "
                     "name TEXT NOT NULL, gender TEXT, age INTEGER, major TEXT, id_card TEXT, phone TEXT)";
    query.exec(stuSql);

    // 创建用户权限表
    QString userSql = "CREATE TABLE IF NOT EXISTS sys_user ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "username TEXT UNIQUE NOT NULL, "
                      "password TEXT NOT NULL, "
                      "role INTEGER NOT NULL)";
    query.exec(userSql);

    // 插入默认账号（不存在才插入）
    query.exec("INSERT OR IGNORE INTO sys_user(username,password,role) VALUES('admin','123456',0)");
    query.exec("INSERT OR IGNORE INTO sys_user(username,password,role) VALUES('user','123456',1)");

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 它会自动读取上次保存的主题并应用
    GlobalStyle::initTheme();
    // 第一步：提前初始化数据库
    if (!initGlobalDatabase()) return -1;

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "StudentManager_" + QLocale(locale).name();
        if (translator.load(baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    LoginWidget login;


    // 使用 exec() 以模态方式显示登录框
    if(login.exec() == QDialog::Accepted)
    {
        MainWindow w;
        w.setUserRole(login.getUserRole());
         w.setCurrentUsername(login.getLoginUsername());

        // 关键修改：将主窗口的主题切换信号连接到登录窗口的槽
        // 这样当在主窗口切换主题时，登录窗口（如果再次打开）也会应用相同的主题
        QObject::connect(&w, &MainWindow::switchTheme, &login, &LoginWidget::switchTheme);

        w.show();
        return a.exec();
    }
    return 0;
}
