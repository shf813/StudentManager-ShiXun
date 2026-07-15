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
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QPushButton>
#include <QPixmap>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_stuModel(nullptr), m_currentRole(ROLE_ADMIN),
    m_genderChart(new QChart), m_ageChart(new QChart), m_chartWin(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle("教务学生管理系统");

    // ========== 1. 创建菜单栏（一次性完成） ==========
    QMenuBar *menuBar = this->menuBar();

    // ----- 文件菜单 -----
    m_fileMenu = menuBar->addMenu("文件");
    m_importCsvAction = m_fileMenu->addAction("导入 CSV", this, &MainWindow::onImportCSV);
    m_importExcelAction = m_fileMenu->addAction("导入 Excel", this, &MainWindow::onImportExcel);
    m_fileMenu->addSeparator();
    m_exportCsvAction = m_fileMenu->addAction("导出 CSV", this, &MainWindow::onExportCSV);
    m_exportExcelAction = m_fileMenu->addAction("导出 Excel", this, &MainWindow::onExportExcel);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("退出", this, &QMainWindow::close);

    // ----- 编辑菜单 -----
    m_editMenu = menuBar->addMenu("编辑");
    m_addAction = m_editMenu->addAction("添加学生", this, &MainWindow::onAddStudent);
    m_deleteAction = m_editMenu->addAction("删除学生", this, &MainWindow::onDeleteStudent);
    m_saveAction = m_editMenu->addAction("保存修改", this, &MainWindow::onSaveChange);

    // ----- 视图菜单（所有用户可见）-----
    QMenu *viewMenu = menuBar->addMenu("视图");
    viewMenu->addAction("图表统计", this, &MainWindow::createChartWindow);
    viewMenu->addAction("刷新数据", this, &MainWindow::onRefreshData);

    // ----- 系统菜单（所有用户可见）-----
    QMenu *systemMenu = menuBar->addMenu("系统");
    systemMenu->addAction("修改密码", this, &MainWindow::onChangePassword);
    QAction *toggleThemeAction = systemMenu->addAction("切换主题", this, [this](){
        ui->checkBoxDark->toggle();
    });
    connect(ui->checkBoxDark, &QCheckBox::toggled, this, [toggleThemeAction](bool checked){
        toggleThemeAction->setText(checked ? "切换到亮色主题" : "切换到深色主题");
    });
    toggleThemeAction->setText(ui->checkBoxDark->isChecked() ? "切换到亮色主题" : "切换到深色主题");

    // ----- 帮助菜单（所有用户可见）-----
    QMenu *helpMenu = menuBar->addMenu("帮助");
    helpMenu->addAction("关于", this, &MainWindow::onAbout);

    // ========== 2. 创建学生数据模型（必须在 setUserRole 之前） ==========
    QSqlDatabase db = QSqlDatabase::database();
    m_stuModel = new QSqlTableModel(this, db);
    m_stuModel->setTable("students");
    m_stuModel->setTable("students");
    m_stuModel->setSort(1, Qt::AscendingOrder);   // 按学号（第1列）升序
    m_stuModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_stuModel->select();
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
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // 隐藏左侧行号
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setDefaultSectionSize(28);
    ui->tableView->horizontalHeader()->setHighlightSections(false);

    // ========== 3. 设置按钮样式 ==========
    ui->btnAdd->setProperty("btnType", "success");
    ui->btnDelete->setProperty("btnType", "danger");
    ui->btnSave->setProperty("btnType", "info");
    ui->btnSearch->setProperty("btnType", "purple");
    ui->btnChart->setProperty("btnType", "purple");
    ui->btnExportCsv->setProperty("btnType", "orange");
    ui->btnExportExcel->setProperty("btnType", "orange");
    ui->btnImport->setProperty("btnType", "import");
    ui->btnImportExcel->setProperty("btnType", "import");

    ui->lineEditSearch->setPlaceholderText("输入学号或姓名");

    // ========== 4. 连接信号槽 ==========
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

    // ========== 5. 最后设置权限和主题（此时所有成员已就绪） ==========
    setUserRole(ROLE_ADMIN);                     // 现在 m_stuModel 已存在，安全
    ui->checkBoxDark->setChecked(GlobalStyle::isDarkMode());
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

    // ----- 按钮可见性控制（已有）-----
    ui->btnAdd->setVisible(isAdmin);
    ui->btnDelete->setVisible(isAdmin);
    ui->btnSave->setVisible(isAdmin);
    ui->btnExportCsv->setVisible(isAdmin);
    ui->btnExportExcel->setVisible(isAdmin);
    ui->btnImport->setVisible(isAdmin);
    ui->btnImportExcel->setVisible(isAdmin);

    // 普通用户禁止编辑表格
    ui->tableView->setEditTriggers(isAdmin
                                       ? QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked
                                       : QAbstractItemView::NoEditTriggers);

    // ----- 菜单权限控制（新增）-----
    if (m_fileMenu) {
        // 控制文件菜单中的导入导出项（退出保持可用）
        m_importCsvAction->setVisible(isAdmin);
        m_importExcelAction->setVisible(isAdmin);
        m_exportCsvAction->setVisible(isAdmin);
        m_exportExcelAction->setVisible(isAdmin);
        // 如果希望整个文件菜单对普通用户不可见，可以 m_fileMenu->setEnabled(isAdmin);
        // 但这样也会隐藏“退出”，所以只控制子项
    }
    if (m_editMenu) {
        m_editMenu->setEnabled(isAdmin);  // 整个编辑菜单禁用（或者单独控制每个Action）
    }

    // 应用数据过滤（普通用户只看自己）
    applyUserFilter();
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
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText(m_stuModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);   // ← 添加
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
    }
}

void MainWindow::onDeleteStudent()
{
    auto selects = ui->tableView->selectionModel()->selectedRows();
    if (selects.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("请选中要删除的学生");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    QMessageBox confirmBox(this);
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setWindowTitle("确认删除");
    confirmBox.setText(QString("确定删除%1条数据？").arg(selects.size()));
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    GlobalStyle::setupMessageBoxButton(&confirmBox, QMessageBox::Yes, "danger");
    GlobalStyle::setupMessageBoxButton(&confirmBox, QMessageBox::No, "cancel");
    if (confirmBox.exec() == QMessageBox::No)
        return;

    // 从后往前删除
    for (int i = selects.size() - 1; i >= 0; --i) {
        m_stuModel->removeRow(selects[i].row());
    }

    // 立即提交删除操作
    if (!m_stuModel->submitAll()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("删除失败");
        msgBox.setText(m_stuModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        m_stuModel->revertAll(); // 回滚删除
        return;
    }

    // 刷新模型，确保视图与数据库同步（可选，但推荐）
    m_stuModel->select();

    // 成功提示（可选）
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("成功");
    msgBox.setText(QString("已成功删除 %1 条数据").arg(selects.size()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
    msgBox.exec();
}

void MainWindow::onSaveChange()
{
    if(m_stuModel->submitAll())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("成功");
        msgBox.setText("所有修改已保存到数据库");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
    }
    else
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("保存失败");
        msgBox.setText(m_stuModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
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

// ====================== 多线程导出 CSV / Excel ======================
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
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle("导出结果");
                    msgBox.setText(msg);
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
                    msgBox.exec();
                } else {
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.setWindowTitle("导出失败");
                    msgBox.setText(msg);
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
                    msgBox.exec();
                }
                thread->deleteLater();
                pd->deleteLater();
            });

    thread->start();
}

void MainWindow::onExportResult(bool success, const QString &msg)
{
    if(success) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("导出结果");
        msgBox.setText(msg);
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
    } else {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("导出失败");
        msgBox.setText(msg);
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
    }
}

// ====================== 数据可视化 ======================
void MainWindow::createChartWindow()
{
    if (m_chartWin) {
        if (m_chartWin->isMinimized()) {
            m_chartWin->showNormal();
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
    m_chartWin->resize(1000, 650);

    // 主布局：垂直
    QVBoxLayout *mainLayout = new QVBoxLayout(m_chartWin);

    // --- 图表容器（用于截图） ---
    m_chartContainer = new QWidget(m_chartWin);
    m_chartContainer->setObjectName("chartContainer");
    // 可选：设置背景色与窗口一致（主题会覆盖）
    QHBoxLayout *chartLayout = new QHBoxLayout(m_chartContainer);
    chartLayout->setContentsMargins(0, 0, 0, 0); // 去掉边距，避免截图有空白

    m_genderChart = new QChart();
    m_ageChart = new QChart();

    QChartView *pieView = new QChartView(m_genderChart);
    QChartView *barView = new QChartView(m_ageChart);
    pieView->setRenderHint(QPainter::Antialiasing);
    barView->setRenderHint(QPainter::Antialiasing);

    chartLayout->addWidget(pieView);
    chartLayout->addWidget(barView);

    mainLayout->addWidget(m_chartContainer, 1); // 拉伸占据所有空间

    // --- 底部按钮 ---
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton *exportBtn = new QPushButton("导出图表为图片");
    exportBtn->setProperty("btnType", "primary");
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::onExportChart);
    btnLayout->addWidget(exportBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_chartWin, &QWidget::destroyed, this, [this]() {
        m_chartWin = nullptr;
        m_chartContainer = nullptr; // 容器随窗口销毁
    });

    refreshChartData();
    m_chartWin->show();
}

void MainWindow::refreshChartData() {
    if (!m_genderChart || !m_ageChart) return;

    QSqlQuery query;

    // ========== 性别饼图（添加百分比标签） ==========
    query.exec("SELECT gender,COUNT(*) FROM students GROUP BY gender");
    auto pieSer = std::make_unique<QPieSeries>();
    int total = 0;

    // 先收集数据，计算总人数
    QList<QPair<QString, int>> genderData;
    while(query.next()) {
        QString g = query.value(0).toString();
        int cnt = query.value(1).toInt();
        genderData.append({g.isEmpty() ? "未知" : g, cnt});
        total += cnt;
    }

    // 填充系列并设置百分比标签
    for (const auto &pair : genderData) {
        QPieSlice *slice = pieSer->append(pair.first, pair.second);
        if (total > 0) {
            double percent = 100.0 * pair.second / total;
            // 标签格式：性别 + 百分比（保留1位小数）
            slice->setLabel(QString("%1\n%2%").arg(pair.first).arg(percent, 0, 'f', 1));
            slice->setLabelVisible(true);
            // 可选：将标签放在切片外，避免文字重叠（根据情况调整）
            slice->setLabelPosition(QPieSlice::LabelOutside);
        }
    }

    m_genderChart->removeAllSeries();
    m_genderChart->addSeries(pieSer.release());
    m_genderChart->setTitle("学生性别比例饼图");
    m_genderChart->legend()->setAlignment(Qt::AlignBottom);

    // ========== 年龄柱状图（坐标轴） ==========
    query.exec("SELECT age,COUNT(*) FROM students GROUP BY age ORDER BY age");

    auto barSet = std::make_unique<QBarSet>("人数");
    auto barSeries = std::make_unique<QBarSeries>();
    QStringList ageLabels;
    int maxCount = 0;

    while(query.next()) {
        int age = query.value(0).toInt();
        int cnt = query.value(1).toInt();
        *barSet << cnt;
        ageLabels << QString::number(age);
        if (cnt > maxCount) maxCount = cnt;
    }

    m_ageChart->removeAllSeries();
    if (barSet->count() > 0) {
        barSeries->append(barSet.release());
        m_ageChart->addSeries(barSeries.release());

        // X轴：年龄分类
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(ageLabels);
        axisX->setTitleText("年龄");
        m_ageChart->setAxisX(axisX, m_ageChart->series().last());

        // Y轴：人数
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("人数");
        axisY->setRange(0, maxCount + 1);
        axisY->setLabelFormat("%d");
        m_ageChart->setAxisY(axisY, m_ageChart->series().last());
    } else {
        m_ageChart->setTitle("暂无年龄数据");
    }

    m_ageChart->setTitle("学生年龄分布柱状图");
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
    QPalette tblPal = ui->tableView->palette();
    tblPal.setBrush(QPalette::Base, Qt::NoBrush);
    tblPal.setBrush(QPalette::AlternateBase, Qt::NoBrush);
    ui->tableView->setPalette(tblPal);

    ui->tableView->viewport()->update();
    ui->tableView->update();
    this->update();
}

// ====================== 导入功能 ======================
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
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("导入结果");
            msgBox.setText(msg);
            msgBox.setStandardButtons(QMessageBox::Ok);
            GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
            msgBox.exec();
            m_stuModel->select();
        } else {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("导入失败");
            msgBox.setText(msg);
            msgBox.setStandardButtons(QMessageBox::Ok);
            GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
            msgBox.exec();
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
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle("导入结果");
                    msgBox.setText(msg);
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
                    msgBox.exec();
                    m_stuModel->select();
                } else {
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.setWindowTitle("导入失败");
                    msgBox.setText(msg);
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
                    msgBox.exec();
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

    box.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QPushButton *okBtn = box.addButton(QMessageBox::Ok);
    QPushButton *cancelBtn = box.addButton(QMessageBox::Cancel);
    okBtn->setProperty("btnType", "primary");
    cancelBtn->setProperty("btnType", "cancel");
    okBtn->style()->unpolish(okBtn);
    okBtn->style()->polish(okBtn);
    cancelBtn->style()->unpolish(cancelBtn);
    cancelBtn->style()->polish(cancelBtn);

    box.setDefaultButton(QMessageBox::Ok);
    return box.exec() == QMessageBox::Ok;
}

void MainWindow::onExportChart()
{
    if (!m_chartContainer) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("图表窗口未打开");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    QString path = QFileDialog::getSaveFileName(this,
                                                "保存图表为图片",
                                                "图表.png",
                                                "PNG 图片 (*.png);;JPEG 图片 (*.jpg *.jpeg);;BMP 图片 (*.bmp)");
    if (path.isEmpty())
        return;

    // 截取图表容器（不含按钮）
    QPixmap pixmap = m_chartContainer->grab();
    if (pixmap.isNull()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("截图失败，请重试");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    if (pixmap.save(path)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("导出成功");
        msgBox.setText(QString("图表已保存至：%1").arg(path));
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
    } else {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("导出失败");
        msgBox.setText("保存图片失败，请检查路径权限");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
    }
}

void MainWindow::setCurrentUsername(const QString& username)
{
    m_currentUser = username;
    applyUserFilter();
}

void MainWindow::applyUserFilter()
{
    if (m_currentRole == ROLE_ADMIN) {
        m_stuModel->setFilter("");
    } else {
        if (!m_currentUser.isEmpty()) {
            QString filter = QString("stu_no = '%1'").arg(m_currentUser.replace("'", "''"));
            m_stuModel->setFilter(filter);
        } else {
            m_stuModel->setFilter("1=0");
        }
    }
    // 无论角色如何，都按学号排序
    m_stuModel->setSort(1, Qt::AscendingOrder);
    m_stuModel->select();
}

void MainWindow::onChangePassword()
{
    QDialog dlg(this);
    dlg.setWindowTitle("修改密码");
    dlg.setFixedSize(300, 180);

    QFormLayout *form = new QFormLayout(&dlg);

    QLineEdit *oldPwdEdit = new QLineEdit;
    oldPwdEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *newPwdEdit = new QLineEdit;
    newPwdEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *confirmPwdEdit = new QLineEdit;
    confirmPwdEdit->setEchoMode(QLineEdit::Password);

    form->addRow("旧密码:", oldPwdEdit);
    form->addRow("新密码:", newPwdEdit);
    form->addRow("确认新密码:", confirmPwdEdit);

    // ===== 创建按钮盒，并设置按钮样式 =====
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    // 获取“确定”和“取消”按钮，设置 btnType 属性
    QPushButton *okBtn = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelBtn = buttonBox->button(QDialogButtonBox::Cancel);
    if (okBtn) {
        okBtn->setProperty("btnType", "primary");
        okBtn->style()->polish(okBtn);   // 强制刷新样式
    }
    if (cancelBtn) {
        cancelBtn->setProperty("btnType", "cancel");
        cancelBtn->style()->polish(cancelBtn);
    }

    connect(buttonBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    form->addRow(buttonBox);

    if (dlg.exec() != QDialog::Accepted)
        return;

    // 获取输入
    QString oldPwd = oldPwdEdit->text();
    QString newPwd = newPwdEdit->text();
    QString confirmPwd = confirmPwdEdit->text();

    // ----- 非空验证 -----
    if (oldPwd.isEmpty() || newPwd.isEmpty() || confirmPwd.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("所有字段不能为空");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }
    if (newPwd != confirmPwd) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("两次输入的新密码不一致");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }
    if (newPwd == oldPwd) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("新密码不能与旧密码相同");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // ----- 验证旧密码 -----
    QSqlQuery query;
    query.prepare("SELECT password FROM sys_user WHERE username = ?");
    query.addBindValue(m_currentUser);
    if (!query.exec() || !query.next()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("用户不存在，请重新登录");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }
    QString storedPwd = query.value(0).toString();
    if (storedPwd != oldPwd) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("错误");
        msgBox.setText("旧密码错误");
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // ----- 更新密码 -----
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE sys_user SET password = ? WHERE username = ?");
    updateQuery.addBindValue(newPwd);
    updateQuery.addBindValue(m_currentUser);
    if (!updateQuery.exec()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("密码更新失败：" + updateQuery.lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
        msgBox.exec();
        return;
    }

    // ----- 成功提示 -----
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("成功");
    msgBox.setText("密码已修改，下次登录请使用新密码");
    msgBox.setStandardButtons(QMessageBox::Ok);
    GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
    msgBox.exec();
}

void MainWindow::onRefreshData()
{
    m_stuModel->select();
    // 可选：显示提示
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("刷新完成");
    msgBox.setText("数据已从数据库刷新");
    msgBox.setStandardButtons(QMessageBox::Ok);
    GlobalStyle::setupMessageBoxButton(&msgBox, QMessageBox::Ok, "primary");
    msgBox.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "关于教务学生管理系统",
                       "<h2>教务学生管理系统 v1.0</h2>"
                       "<p>基于 Qt 5/6 开发，支持学生信息管理、导入导出、数据可视化等功能。</p>"
                       "<p>作者: 孙浩凡<br>"
                       "License: GPL v3</p>");
}
