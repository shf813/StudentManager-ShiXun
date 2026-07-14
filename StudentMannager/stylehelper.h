#ifndef STYLEHELPER_H
#define STYLEHELPER_H
#include <QString>
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>

// 全局配色常量定义
namespace GlobalStyle
{
// 主蓝色
const QString PRIMARY = "#2563eb";
const QString PRIMARY_HOVER = "#1d4ed8";
const QString PRIMARY_PRESS = "#1e40af";
// 功能色
const QString SUCCESS = "#27ae60";
const QString DANGER = "#e74c3c";
const QString INFO = "#2980b9";
const QString PURPLE = "#8e44ad";
const QString ORANGE = "#d35400";
const QString GRAY_LIGHT = "#e5e7eb";
const QString GRAY_MID = "#d1d5db";
const QString GRAY_DARK = "#475569";
// 尺寸规范
const int RADIUS = 7;
const int INPUT_PADDING_V = 7;
const int INPUT_PADDING_H = 10;
const int BTN_PADDING = 7;
// 字体
const QString FONT_FAMILY = "\"Microsoft YaHei\"";
const int FONT_NORMAL = 14;
const int FONT_SMALL = 13;
const int FONT_TITLE = 22;

// 获取亮色全局基础样式
QString getLightBaseStyle();
// 获取暗色全局基础样式
QString getDarkBaseStyle();
// 全局切换调色板
void setLightPalette();
void setDarkPalette();
}

#endif // STYLEHELPER_H
