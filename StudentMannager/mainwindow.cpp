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
#include "excelimportthread.h"
#include <memory>
#include "excelexportthread.h"

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

    //
    ui->btnAdd->setProperty("btnType", "success");
    ui->btnDelete->setProperty("btnType", "danger");
    ui->btnSave->setProperty("btnType", "info");
    ui->btnSearch->setProperty("btnType", "purple");
    ui->btnChart->setProperty("btnType", "purple");
    ui->btnExportCsv->setProperty("btnType", "orange");
    ui->btnExportExcel->setProperty("btnType", "orange");
    ui->btnImport->setProperty("btnType", "import");
    ui->btnImportExcel->setProperty("btnType", "import");

    // 3. 绑定按钮信号槽
    connect(ui->btnImportExcel, &QPushButton::clicked, this, &MainWindow::onImportExcel);
    connect(ui->btnImport, &QPushButton::clicked, this, &MainWindow::onImportCSV);
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

    ui->checkBoxDark->setChecked(GlobalStyle::isDarkMode());
    // 2. 再根据这个状态应用一次主题（确保完全刷新）
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
    QString path = QFileDialog::getSaveFileName(this, "导出Excel（xlsx格式）",
                                                "学生数据.xlsx",
                                                "Excel文件 (*.xlsx)");
    if (path.isEmpty()) return;

    QProgressDialog *pd = new QProgressDialog("正在导出Excel...", "取消", 0, 100, this);
    pd->setWindowModality(Qt::WindowModal);

    ExcelExportThread *thread = new ExcelExportThread(m_stuModel, path, this);
    connect(pd, &QProgressDialog::canceled, thread, &ExcelExportThread::requestInterruption);
    connect(thread, &ExcelExportThread::progressUpdate, pd, &QProgressDialog::setValue);
    connect(thread, &ExcelExportThread::exportFinished, this,
            [this, pd, thread](bool success, const QString &msg) {
                pd->close();
                if (success) {
                    QMessageBox::information(this, "导出结果", msg);
                } else {
                    QMessageBox::critical(this, "导出失败", msg);
                }
                thread->deleteLater();
                pd->deleteLater();
            });

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

    // --- 性别饼图 ---
    query.exec("SELECT gender,COUNT(*) FROM students GROUP BY gender");
    auto pieSer = std::make_unique<QPieSeries>(m_genderChart);
    while(query.next()) {
        QString g = query.value(0).toString();
        int cnt = query.value(1).toInt();
        pieSer->append(g.isEmpty()?"未知":g, cnt);
    }
    m_genderChart->removeAllSeries();
    m_genderChart->addSeries(pieSer.release());
    m_genderChart->setTitle("学生性别比例饼图");

    // --- 年龄柱状图 ---
    query.exec("SELECT age,COUNT(*) FROM students GROUP BY age ORDER BY age");
    auto barSet = std::make_unique<QBarSet>("人数", m_ageChart);
    auto barSer = std::make_unique<QBarSeries>(m_ageChart);

    QStringList ageLabels;
    while(query.next()) {
        int age = query.value(0).toInt();
        int cnt = query.value(1).toInt();
        barSet->append(cnt);
        ageLabels << QString::number(age);
    }
    barSer->append(barSet.release());
    m_ageChart->removeAllSeries();
    m_ageChart->addSeries(barSer.release());
    m_ageChart->setTitle("学生年龄分布柱状图");

    // 图表主题与图例
    m_genderChart->setTheme(QChart::ChartThemeQt);
    m_genderChart->legend()->setAlignment(Qt::AlignBottom);
    m_ageChart->setTheme(QChart::ChartThemeQt);

    // 强制刷新
    m_genderChart->update();
    m_ageChart->update();
}

// ====================== 亮色/暗色主题切换 ======================
void MainWindow::switchTheme(bool isDark)
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    if(isDark)
        GlobalStyle::switchDarkTheme();
    else
        GlobalStyle::switchLightTheme();

    ui->tableView->setAlternatingRowColors(true);
    // 新增：重置表格调色板，清除旧AlternateBase缓存
    QPalette tblPal = ui->tableView->palette();
    tblPal.setBrush(QPalette::Base, Qt::NoBrush);
    tblPal.setBrush(QPalette::AlternateBase, Qt::NoBrush);
    ui->tableView->setPalette(tblPal);

    // 多层刷新：viewport+表格+主窗口
    ui->tableView->viewport()->update();
    ui->tableView->update();
    this->update();
}

void MainWindow::onImportCSV()
{
    if (!showImportFormatHint()) return;
    QString filePath = QFileDialog::getOpenFileName(this, "选择导入的CSV文件",
                                                    QString(), "CSV文件 (*.csv);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    QProgressDialog *pd = new QProgressDialog("正在导入数据...", "取消", 0, 100, this);
    pd->setWindowModality(Qt::WindowModal);

    ImportThread *thread = new ImportThread(filePath, this);
    connect(pd, &QProgressDialog::canceled, thread, &ImportThread::requestInterruption);
    connect(thread, &ImportThread::progressUpdate, pd, &QProgressDialog::setValue);
    connect(thread, &ImportThread::importFinished, this, [this, pd, thread](bool success, const QString &msg) {
        pd->close();
        if (success) {
            QMessageBox::information(this, "导入结果", msg);
            // 重新加载表格数据
            m_stuModel->select();
        } else {
            QMessageBox::critical(this, "导入失败", msg);
        }
        thread->deleteLater();
        pd->deleteLater();
    });

    thread->start();
}

void MainWindow::onImportExcel()
{
    if (!showImportFormatHint()) return;
    QString filePath = QFileDialog::getOpenFileName(this, "选择Excel文件",
                                                    QString(), "Excel文件 (*.xlsx);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    QProgressDialog *pd = new QProgressDialog("正在导入Excel数据...", "取消", 0, 100, this);
    pd->setWindowModality(Qt::WindowModal);

    ExcelImportThread *thread = new ExcelImportThread(filePath, this);
    connect(pd, &QProgressDialog::canceled, thread, &ExcelImportThread::requestInterruption);
    connect(thread, &ExcelImportThread::progressUpdate, pd, &QProgressDialog::setValue);
    connect(thread, &ExcelImportThread::importFinished, this,
            [this, pd, thread](bool success, const QString &msg) {
                pd->close();
                if (success) {
                    QMessageBox::information(this, "导入结果", msg);
                    m_stuModel->select();  // 刷新表格
                } else {
                    QMessageBox::critical(this, "导入失败", msg);
                }
                thread->deleteLater();
                pd->deleteLater();
            });

    thread->start();
}

bool MainWindow::showImportFormatHint()
{
    QMessageBox box(this);
    box.setWindowTitle("导入格式说明");
    box.setTextFormat(Qt::RichText);

    // 提示内容（之前修改过的转置表格）
    QString msg =
        "<h3>导入文件格式要求</h3>"
        "<p>请确保 Excel/CSV 的列顺序如下（<b>无多余列，第一列即学号</b>）：</p>"
        "<table border='1' cellpadding='6' cellspacing='0' style='text-align:center;'>"
        "<tr style='background-color:#4472C4; color:white;'>"
        "<th>列序</th><th>A</th><th>B</th><th>C</th><th>D</th><th>E</th><th>F</th><th>G</th>"
        "</tr>"
        "<tr>"
        "<td><b>字段名</b></td>"
        "<td>学号</td><td>姓名</td><td>性别</td><td>年龄</td><td>专业</td><td>身份证号</td><td>手机号</td>"
        "</tr>"
        "<tr>"
        "<td><b>说明</b></td>"
        "<td>必填，<br>不可重复</td>"
        "<td>必填</td>"
        "<td>男/女</td>"
        "<td>整数，<br>可为空</td>"
        "<td></td><td></td><td></td>"
        "</tr>"
        "</table>"
        "<p><b>注意：</b></p>"
        "<ul>"
        "<li>第一行可以是标题（含“学号”字样会自动跳过），也可以直接从数据开始。</li>"
        "<li>学号已存在时，将<b>更新</b>该学生的其他信息。</li>"
        "<li>年龄请填写整数，留空则存储为 0。</li>"
        "<li>文件不要包含多余的 ID 列或其他无关列。</li>"
        "</ul>";
    box.setText(msg);

    // ----- 去掉右上角的关闭按钮，只能通过按钮关闭 -----
    box.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    // ----- 手动添加按钮，并设置属性以匹配主题 -----
    QPushButton *okBtn = box.addButton(QMessageBox::Ok);
    QPushButton *cancelBtn = box.addButton(QMessageBox::Cancel);

    // 赋予按钮样式类型：确定用 primary（主色），取消用 cancel（灰色/亮暗）
    okBtn->setProperty("btnType", "primary");
    cancelBtn->setProperty("btnType", "cancel");

    // 刷新按钮样式（因为属性在构造后才设置，需要手动触发样式更新）
    okBtn->style()->unpolish(okBtn);
    okBtn->style()->polish(okBtn);
    cancelBtn->style()->unpolish(cancelBtn);
    cancelBtn->style()->polish(cancelBtn);

    box.setDefaultButton(QMessageBox::Ok);
    return box.exec() == QMessageBox::Ok;
}
