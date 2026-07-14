#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QStyleFactory>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_stuModel(nullptr), m_currentRole(ROLE_ADMIN),
    m_genderChart(new QChart), m_ageChart(new QChart), m_chartWin(nullptr)
{


    ui->setupUi(this);
    this->setWindowTitle("教务学生管理系统");


    // 2. 创建学生数据模型
    QSqlDatabase db = QSqlDatabase::database();
    m_stuModel = new QSqlTableModel(this, db);
    m_stuModel->setTable("students");
    m_stuModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_stuModel->select();

    // 中文表头
    m_stuModel->setHeaderData(0, Qt::Horizontal, "ID");
    m_stuModel->setHeaderData(1, Qt::Horizontal, "学号");
    m_stuModel->setHeaderData(2, Qt::Horizontal, "姓名");
    m_stuModel->setHeaderData(3, Qt::Horizontal, "性别");
    m_stuModel->setHeaderData(4, Qt::Horizontal, "年龄");
    m_stuModel->setHeaderData(5, Qt::Horizontal, "专业");
    m_stuModel->setHeaderData(6, Qt::Horizontal, "身份证号");
    m_stuModel->setHeaderData(7, Qt::Horizontal, "手机号");

    ui->tableView->setModel(m_stuModel);
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setAlternatingRowColors(true);

    // 隐藏左侧行号，统一行高，质感大幅提升
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setDefaultSectionSize(28);
    ui->tableView->horizontalHeader()->setHighlightSections(false);

    // 3. 绑定按钮信号槽
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddStudent);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteStudent);
    connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::onSaveChange);
    connect(ui->btnSearch, &QPushButton::clicked, this, &MainWindow::onSearchStudent);
    connect(ui->lineEditSearch, &QLineEdit::returnPressed, ui->btnSearch, &QPushButton::click);
    connect(ui->btnExportCsv, &QPushButton::clicked, this, &MainWindow::onExportCSV);
    connect(ui->btnExportExcel, &QPushButton::clicked, this, &MainWindow::onExportExcel);
    connect(ui->btnChart, &QPushButton::clicked, this, &MainWindow::createChartWindow);
    connect(ui->checkBoxDark, &QCheckBox::toggled, this, &MainWindow::switchTheme);

    // 默认管理员权限
    setUserRole(ROLE_ADMIN);

    switchTheme(ui->checkBoxDark->isChecked());
}

MainWindow::~MainWindow()
{
    delete ui;
}



// ====================== 权限控制：管理员/普通用户按钮可见性 ======================
void MainWindow::setUserRole(UserRole role)
{
    m_currentRole = role;
    bool isAdmin = (role == ROLE_ADMIN);
    // 管理员按钮显示控制
    ui->btnAdd->setVisible(isAdmin);
    ui->btnDelete->setVisible(isAdmin);
    ui->btnSave->setVisible(isAdmin);
    ui->btnExportCsv->setVisible(isAdmin);
    ui->btnExportExcel->setVisible(isAdmin);

    // 普通用户禁止编辑表格
    ui->tableView->setEditTriggers(isAdmin
                                       ? QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked
                                       : QAbstractItemView::NoEditTriggers);
}

// ====================== 学生增删改查 ======================
void MainWindow::onAddStudent()
{
    int newRow = m_stuModel->rowCount();
    if(m_stuModel->insertRow(newRow))
    {
        QModelIndex idx = m_stuModel->index(newRow, 1);
        ui->tableView->setCurrentIndex(idx);
        ui->tableView->edit(idx);
    }
    else
        QMessageBox::warning(this, "错误", m_stuModel->lastError().text());
}

void MainWindow::onDeleteStudent()
{
    auto selects = ui->tableView->selectionModel()->selectedRows();
    if(selects.isEmpty()){
        QMessageBox::warning(this, "提示", "请选中要删除的学生");
        return;
    }
    if(QMessageBox::No == QMessageBox::question(this, "确认删除", QString("确定删除%1条数据？").arg(selects.size())))
        return;

    for(int i = selects.size()-1; i >= 0; i--)
        m_stuModel->removeRow(selects[i].row());
}

void MainWindow::onSaveChange()
{
    if(m_stuModel->submitAll())
        QMessageBox::information(this, "成功", "所有修改已保存到数据库");
    else
    {
        QMessageBox::critical(this, "保存失败", m_stuModel->lastError().text());
        m_stuModel->revertAll();
    }
}

void MainWindow::onSearchStudent()
{
    QString key = ui->lineEditSearch->text().trimmed().replace("'","''");
    if(key.isEmpty())
        m_stuModel->setFilter("");
    else
        m_stuModel->setFilter(QString("stu_no LIKE '%%1%' OR name LIKE '%%1%'").arg(key));
    m_stuModel->select();
}

// ====================== 多线程导出 CSV / Excel（制表符版） ======================
void MainWindow::onExportCSV()
{
    QString path = QFileDialog::getSaveFileName(this, "导出CSV", "学生数据.csv", "CSV文件(*.csv)");
    if(path.isEmpty()) return;

    QProgressDialog* pd = new QProgressDialog("正在导出...", "取消", 0, 100, this);
    pd->setWindowModality(Qt::WindowModal);

    ExportThread* thread = new ExportThread(m_stuModel, path, true, this);
    connect(pd, &QProgressDialog::canceled, thread, &ExportThread::requestInterruption);
    connect(thread, &ExportThread::progressUpdate, pd, &QProgressDialog::setValue);
    connect(thread, &ExportThread::exportFinished, this, &MainWindow::onExportResult);
    connect(thread, &ExportThread::finished, thread, &ExportThread::deleteLater);
    connect(thread, &ExportThread::finished, pd, &QProgressDialog::deleteLater);
    thread->start();
}

void MainWindow::onExportExcel()
{
    QString path = QFileDialog::getSaveFileName(this, "导出Excel（制表符格式）", "学生数据.xls", "表格文件(*.xls)");
    if(path.isEmpty()) return;

    QProgressDialog* pd = new QProgressDialog("正在导出...", "取消", 0, 100, this);
    pd->setWindowModality(Qt::WindowModal);

    ExportThread* thread = new ExportThread(m_stuModel, path, false, this);
    connect(pd, &QProgressDialog::canceled, thread, &ExportThread::requestInterruption);
    connect(thread, &ExportThread::progressUpdate, pd, &QProgressDialog::setValue);
    connect(thread, &ExportThread::exportFinished, this, &MainWindow::onExportResult);
    connect(thread, &ExportThread::finished, thread, &ExportThread::deleteLater);
    connect(thread, &ExportThread::finished, pd, &QProgressDialog::deleteLater);
    thread->start();
}

void MainWindow::onExportResult(bool success, const QString &msg)
{
    if(success)
        QMessageBox::information(this, "导出结果", msg);
    else
        QMessageBox::critical(this, "导出失败", msg);
}

// ====================== 数据可视化：性别饼图 + 年龄柱状图 ======================
void MainWindow::createChartWindow()
{
    if(m_chartWin)
    {
        if (m_chartWin->isMinimized()) {
            m_chartWin->showNormal(); // 从最小化恢复
        }
        m_ageChart->show();
        m_chartWin->raise();
        m_chartWin->activateWindow();
        refreshChartData();
        return;
    }

    m_chartWin = new QWidget(nullptr);
    m_chartWin->setAttribute(Qt::WA_DeleteOnClose);
    m_chartWin->setWindowTitle("学生数据可视化统计");
    m_chartWin->resize(1000,600);
    QHBoxLayout* layout = new QHBoxLayout(m_chartWin);

    m_genderChart = new QChart();
    m_ageChart = new QChart();

    QChartView* pieView = new QChartView(m_genderChart);
    QChartView* barView = new QChartView(m_ageChart);
    pieView->setRenderHint(QPainter::Antialiasing);
    barView->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(pieView);
    layout->addWidget(barView);

    connect(m_chartWin, &QWidget::destroyed, this, [this](){
        m_chartWin = nullptr;
    });

    refreshChartData();
    m_chartWin->show();
}

void MainWindow::refreshChartData() {
    // 空指针保护
    if (!m_genderChart || !m_ageChart) return;

    QSqlQuery query;

    // --- 修复性别饼图部分 ---
    query.exec("SELECT gender,COUNT(*) FROM students GROUP BY gender");
    // 1. 创建 QPieSeries 时，将 m_genderChart 作为其父对象
    QScopedPointer<QPieSeries> pieSer(new QPieSeries(m_genderChart));
    while(query.next()) {
        QString g = query.value(0).toString();
        int cnt = query.value(1).toInt();
        pieSer->append(g.isEmpty()?"未知":g, cnt);
    }
    m_genderChart->removeAllSeries();
    m_genderChart->addSeries(pieSer.take());
    m_genderChart->setTitle("学生性别比例饼图");

    // --- 修复年龄柱状图部分 ---
    query.exec("SELECT age,COUNT(*) FROM students GROUP BY age ORDER BY age");
    // 2. 创建 QBarSet 和 QBarSeries 时，将 m_ageChart 作为其父对象
    QScopedPointer<QBarSet> barSet(new QBarSet("人数", m_ageChart));
    QScopedPointer<QBarSeries> barSer(new QBarSeries(m_ageChart));

    QStringList ageLabels;
    while(query.next()) {
        int age = query.value(0).toInt();
        int cnt = query.value(1).toInt();
        barSet->append(cnt);
        ageLabels << QString::number(age);
    }
    barSer->append(barSet.take());
    m_ageChart->removeAllSeries();
    m_ageChart->addSeries(barSer.take());
    m_ageChart->setTitle("学生年龄分布柱状图");

    // ... 其余代码保持不变 ...
    m_genderChart->setTheme(QChart::ChartThemeQt);
    m_genderChart->legend()->setAlignment(Qt::AlignBottom);
    m_ageChart->setTheme(QChart::ChartThemeQt);

    // 强制刷新图表视图
    m_genderChart->update();
    m_ageChart->update();
}

// ====================== 亮色/暗色主题切换 ======================
void MainWindow::switchTheme(bool isDark)
{
    this->setStyleSheet("") ;
    qApp->setStyle(QStyleFactory::create("Fusion"));
    if(isDark)
    {
        GlobalStyle::setDarkPalette();
        QString darkStyle = GlobalStyle::getDarkBaseStyle();
        this->setStyleSheet(darkStyle);
    }
    else
    {
        GlobalStyle::setLightPalette();
        QString lightStyle = GlobalStyle::getLightBaseStyle();
        this->setStyleSheet(lightStyle);
    }
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->viewport()->update();
}
