#ifndef STYLEHELPER_H
#define STYLEHELPER_H
#include <QString>
#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QMap>
#include<QSettings>

namespace GlobalStyle
{
// 尺寸、字体常量（不变）
const int RADIUS = 7;
const int INPUT_PADDING_V = 7;
const int INPUT_PADDING_H = 10;
const int BTN_PADDING = 7;
const QString FONT_FAMILY = "\"Microsoft YaHei\"";
const int FONT_NORMAL = 14;
const int FONT_SMALL = 13;
const int FONT_TITLE = 22;

// 主题色彩上下文结构体，统一管理亮色/暗色两套色值
struct ThemeColor
{
    QString primary;
    QString primaryHover;
    QString primaryPress;
    QString success;
    QString danger;
    QString info;
    QString purple;
    QString orange;
    QString grayLight;
    QString grayMid;
    QString grayDark;
    QString bgMain;
    QString bgWidget;
    QString textNormal;
    QString textLight;
    QString importColor;
    QString importHover;
    QString importPress;
    QString successHover, successPress;
    QString dangerHover, dangerPress;
    QString infoHover, infoPress;
    QString purpleHover, purplePress;
    QString orangeHover, orangePress;
};

// 获取亮色/暗色色彩集
ThemeColor getLightColor();
ThemeColor getDarkColor();

// 加载外部QSS模板并填充色彩变量
QString loadStyleSheet(const QString& qssPath, const ThemeColor& color);

// 全局切换主题
void switchLightTheme();
void switchDarkTheme();

// 新增：初始化主题，在程序启动时调用
void initTheme();

// 缓存当前主题
bool isDarkMode();
}



#endif
