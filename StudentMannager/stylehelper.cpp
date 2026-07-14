#include "stylehelper.h"
#include <QPalette>
#include <QApplication>
#include<QStyle>

QString GlobalStyle::getLightBaseStyle()
{
    QString base = R"(
* {
    font-family:%1;
    font-size:%2px;
}
QLabel {
    color:#334155;
}
QLineEdit {
    border:1px solid %3;
    border-radius:%4px;
    padding:%5px %6px;
    background:#f9fafb;
}
QLineEdit:focus {
    border:1px solid %7;
    background:#ffffff;
}
QComboBox {
    border:1px solid %3;
    border-radius:%4px;
    padding:%5px %6px;
    background:#f9fafb;
}
QComboBox::drop-down {
    border:none;
}
QCheckBox {
    font-size:%8px;
    color:#475569;
}
QCheckBox::indicator {
    width:16px;
    height:16px;
    border-radius:4px;
    border:1px solid #cbd5e1;
}
QCheckBox::indicator:checked {
    background-color:%7;
    border-color:%7;
}
QPushButton {
    border:none;
    border-radius:%4px;
    padding:%9px;
    color:white;
}
QPushButton:pressed {
    padding:9px 8px;
}
#btnOk, #btnLogin {
    background:%7;
}
#btnOk:hover, #btnLogin:hover {
    background:%10;
}
#btnOk:pressed, #btnLogin:pressed {
    background:%11;
}
#btnCancel {
    background:%12;
    color:#333;
}
#btnCancel:hover {
    background:#d1d5db;
}
#btnRegister {
    background:transparent;
    color:%7;
}
#btnRegister:hover {
    text-decoration:underline;
}
#btnAdd { background:%13; }
#btnDelete { background:%14; }
#btnSave { background:%15; }
#btnSearch,#btnChart { background:%16; }
#btnExportCsv,#btnExportExcel { background:%17; }
QHeaderView::section {
    background-color:#34495e;
    color:white;
    padding:6px;
    border:none;
}
QTableView {
    background:white;
    border:1px solid #ddd;
    gridline-color:#e8e8e8;
    color:#222;
}
QTableView::item:alternate {
    background-color:#f6f7f9;
}
)";
    return base.arg(FONT_FAMILY)
        .arg(FONT_NORMAL)
        .arg(GRAY_MID)
        .arg(RADIUS)
        .arg(INPUT_PADDING_V)
        .arg(INPUT_PADDING_H)
        .arg(PRIMARY)
        .arg(FONT_SMALL)
        .arg(BTN_PADDING)
        .arg(PRIMARY_HOVER)
        .arg(PRIMARY_PRESS)
        .arg(GRAY_LIGHT)
        .arg(SUCCESS)
        .arg(DANGER)
        .arg(INFO)
        .arg(PURPLE)
        .arg(ORANGE);
}

QString GlobalStyle::getDarkBaseStyle()
{
    QString dark = R"(
* {
    font-family:%1;
    font-size:%2px;
}
QLabel {
    color:#cbd5e1;
}
QLabel[labelTitle="true"] {
    font-size:%3px;
    font-weight:bold;
    color:#f1f5f9;
}
QLineEdit {
    background:#334155;
    border:1px solid #475569;
    border-radius:%4px;
    padding:%5px %6px;
    color:#f1f5f9;
}
QLineEdit:focus {
    border:1px solid %7;
}
QComboBox {
    background:#334155;
    border:1px solid #475569;
    border-radius:%4px;
    padding:%5px %6px;
    color:#f1f5f9;
}
QComboBox::drop-down {
    border:none;
}
QCheckBox {
    font-size:%8px;
    color:#cbd5e1;
}
QCheckBox::indicator {
    width:16px;
    height:16px;
    border-radius:4px;
    background:#334155;
    border:1px solid #64748b;
}
QCheckBox::indicator:checked {
    background-color:%7;
    border-color:%7;
}
QPushButton {
    border:none;
    border-radius:%4px;
    padding:%9px;
    color:white;
}
QPushButton:pressed {
    padding:9px 8px;
}
#btnOk, #btnLogin {
    background:%7;
}
#btnOk:hover, #btnLogin:hover {
    background:#3b82f6;
}
#btnCancel {
    background:#475569;
    color:#f1f5f9;
}
#btnRegister {
    background:transparent;
    color:#60a5fa;
}
#btnRegister:hover {
    text-decoration:underline;
}
#btnAdd { background:%10; }
#btnDelete { background:%11; }
#btnSave { background:%12; }
#btnSearch,#btnChart { background:%13; }
#btnExportCsv,#btnExportExcel { background:%14; }
QHeaderView::section {
    background-color:#1e293b;
    color:#f1f5f9;
    padding:6px;
    border:none;
}
QTableView {
    background:#3a3f48;
    border:1px solid #444;
    gridline-color:#484c54;
    color:#ffffff;
}
QTableView::item:alternate {
    background-color:#353942;
}
QWidget#RegisterDialog, QWidget#LoginWidget, QMainWindow {
    background:#1e293b;
}
)";
    return dark.arg(FONT_FAMILY)
        .arg(FONT_NORMAL)
        .arg(FONT_TITLE)
        .arg(RADIUS)
        .arg(INPUT_PADDING_V)
        .arg(INPUT_PADDING_H)
        .arg(PRIMARY)
        .arg(FONT_SMALL)
        .arg(BTN_PADDING)
        .arg(SUCCESS)
        .arg(DANGER)
        .arg(INFO)
        .arg(PURPLE)
        .arg(ORANGE);
}

void GlobalStyle::setLightPalette()
{
    QPalette pal = QApplication::style()->standardPalette();
    pal.setColor(QPalette::WindowText, Qt::black);
    pal.setColor(QPalette::Text, Qt::black);
    qApp->setPalette(pal);
}

void GlobalStyle::setDarkPalette()
{
    QPalette darkPal;
    darkPal.setColor(QPalette::Window, QColor(44,49,58));
    darkPal.setColor(QPalette::WindowText, Qt::white);
    darkPal.setColor(QPalette::Base, QColor(58,63,72));
    darkPal.setColor(QPalette::AlternateBase, QColor(53,57,66));
    darkPal.setColor(QPalette::Text, Qt::white);
    darkPal.setColor(QPalette::Highlight, QColor(41,121,185));
    qApp->setPalette(darkPal);
}
