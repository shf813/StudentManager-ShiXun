#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QBarSeries>
#include <QBarSet>
#include "exportthread.h"
#include "commondef.h"
#include<QCategoryAxis>
#include<QBarSeries>
#include<QBarSet>
#include"stylehelper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 权限设置
    void setUserRole(UserRole role);
    // 初始化数据库（学生表+用户表）
    bool initDatabase();
    // 构建图表窗口
    void createChartWindow();

public slots:
    // 主题切换
    void switchTheme(bool isDark);

private slots:
    // 学生操作
    void onAddStudent();
    void onDeleteStudent();
    void onSaveChange();
    void onSearchStudent();
    // 导出文件
    void onExportCSV();
    void onExportExcel();
    void onExportResult(bool success, const QString& msg);
    // 图表刷新
    void refreshChartData();

private:
    Ui::MainWindow *ui;
    QSqlTableModel* m_stuModel;
    UserRole m_currentRole;
    QChart* m_genderChart;
    QChart* m_ageChart;
    QWidget* m_chartWin = nullptr;
};



#endif // MAINWINDOW_H
