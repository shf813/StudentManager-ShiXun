/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBoxDark;
    QSplitter *splitter;
    QPushButton *btnSearch;
    QLineEdit *lineEditSearch;
    QTableView *tableView;
    QSplitter *splitter_5;
    QSplitter *splitter_2;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
    QPushButton *btnSave;
    QSplitter *splitter_3;
    QPushButton *btnExportCsv;
    QPushButton *btnExportExcel;
    QPushButton *btnChart;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(450, 487);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        checkBoxDark = new QCheckBox(centralwidget);
        checkBoxDark->setObjectName("checkBoxDark");

        verticalLayout->addWidget(checkBoxDark);

        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        btnSearch = new QPushButton(splitter);
        btnSearch->setObjectName("btnSearch");
        splitter->addWidget(btnSearch);
        lineEditSearch = new QLineEdit(splitter);
        lineEditSearch->setObjectName("lineEditSearch");
        splitter->addWidget(lineEditSearch);

        verticalLayout->addWidget(splitter);

        tableView = new QTableView(centralwidget);
        tableView->setObjectName("tableView");

        verticalLayout->addWidget(tableView);

        splitter_5 = new QSplitter(centralwidget);
        splitter_5->setObjectName("splitter_5");
        splitter_5->setOrientation(Qt::Orientation::Vertical);
        splitter_2 = new QSplitter(splitter_5);
        splitter_2->setObjectName("splitter_2");
        splitter_2->setOrientation(Qt::Orientation::Horizontal);
        btnAdd = new QPushButton(splitter_2);
        btnAdd->setObjectName("btnAdd");
        splitter_2->addWidget(btnAdd);
        btnDelete = new QPushButton(splitter_2);
        btnDelete->setObjectName("btnDelete");
        splitter_2->addWidget(btnDelete);
        btnSave = new QPushButton(splitter_2);
        btnSave->setObjectName("btnSave");
        splitter_2->addWidget(btnSave);
        splitter_5->addWidget(splitter_2);
        splitter_3 = new QSplitter(splitter_5);
        splitter_3->setObjectName("splitter_3");
        splitter_3->setOrientation(Qt::Orientation::Horizontal);
        btnExportCsv = new QPushButton(splitter_3);
        btnExportCsv->setObjectName("btnExportCsv");
        splitter_3->addWidget(btnExportCsv);
        btnExportExcel = new QPushButton(splitter_3);
        btnExportExcel->setObjectName("btnExportExcel");
        splitter_3->addWidget(btnExportExcel);
        splitter_5->addWidget(splitter_3);
        btnChart = new QPushButton(splitter_5);
        btnChart->setObjectName("btnChart");
        splitter_5->addWidget(btnChart);

        verticalLayout->addWidget(splitter_5);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 450, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        checkBoxDark->setText(QCoreApplication::translate("MainWindow", "\346\232\227\350\211\262\344\270\273\351\242\230", nullptr));
        btnSearch->setText(QCoreApplication::translate("MainWindow", "\346\220\234\347\264\242", nullptr));
        lineEditSearch->setText(QCoreApplication::translate("MainWindow", "(\346\220\234\347\264\242\350\276\223\345\205\245\346\241\206\357\274\211", nullptr));
        btnAdd->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\345\255\246\347\224\237", nullptr));
        btnDelete->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\345\255\246\347\224\237", nullptr));
        btnSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\344\277\256\346\224\271", nullptr));
        btnExportCsv->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272CSV", nullptr));
        btnExportExcel->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272Excel", nullptr));
        btnChart->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\273\237\350\256\241\345\233\276\350\241\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
