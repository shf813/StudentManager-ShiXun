#include "stylehelper.h"
#include <QApplication>
#include <QStyleFactory>
#include <QRegularExpression>
#include <QFile>
#include <QDebug>
#include<QStyle>
#include<QPushButton>
#include<QMessageBox>

static bool g_isDark = false;
static QString g_cachedStyle;

GlobalStyle::ThemeColor GlobalStyle::getLightColor()
{
    ThemeColor c;
    c.primary       = "#3b82f6";   // 更柔和的蓝
    c.primaryHover  = "#2563eb";
    c.primaryPress  = "#1d4ed8";
    c.success       = "#10b981";   // 翠绿，替换原来的深绿
    c.danger        = "#ef4444";   // 暖红，比正红更温和
    c.info          = "#06b6d4";   // 青色，更有科技感
    c.purple        = "#8b5cf6";   // 紫罗兰
    c.orange        = "#f97316";   // 亮橙，醒目但不刺眼
    c.grayLight     = "#e5e7eb";
    c.grayMid       = "#d1d5db";
    c.grayDark      = "#475569";
    c.bgMain        = "#ffffff";
    c.bgWidget      = "#f9fafb";
    c.textNormal    = "#334155";
    c.textLight     = "#1e293b";
    c.importColor  = "#0d9488";   // teal-600
    c.importHover  = "#0f766e";   // teal-700
    c.importPress  = "#115e59";   // teal-800
    c.successHover = "#34d399";   c.successPress = "#059669";
    c.dangerHover  = "#f87171";   c.dangerPress  = "#dc2626";
    c.infoHover    = "#22d3ee";   c.infoPress    = "#0891b2";
    c.purpleHover  = "#a78bfa";   c.purplePress  = "#7c3aed";
    c.orangeHover  = "#fb923c";   c.orangePress  = "#ea580c";

    return c;
}

GlobalStyle::ThemeColor GlobalStyle::getDarkColor()
{
    ThemeColor c;
    c.primary       = "#60a5fa";   // 天蓝，在暗背景上柔和又清晰
    c.primaryHover  = "#3b82f6";
    c.primaryPress  = "#2563eb";
    c.success       = "#34d399";   // 浅翠绿
    c.danger        = "#f87171";   // 淡红，避免暗色下过于刺眼
    c.info          = "#22d3ee";   // 浅青
    c.purple        = "#a78bfa";   // 浅紫
    c.orange        = "#fb923c";   // 浅橙
    c.grayLight     = "#475569";
    c.grayMid       = "#64748b";
    c.grayDark      = "#334155";
    c.bgMain        = "#1e293b";
    c.bgWidget      = "#334155";
    c.textNormal    = "#cbd5e1";
    c.textLight     = "#f1f5f9";
    c.importColor  = "#2dd4bf";   // teal-400
    c.importHover  = "#5eead4";   // teal-300
    c.importPress  = "#14b8a6";   // teal-500
    c.successHover = "#6ee7b7";   c.successPress = "#34d399";
    c.dangerHover  = "#fca5a5";   c.dangerPress  = "#f87171";
    c.infoHover    = "#67e8f9";   c.infoPress    = "#22d3ee";
    c.purpleHover  = "#c4b5fd";   c.purplePress  = "#a78bfa";
    c.orangeHover  = "#fdba74";   c.orangePress  = "#fb923c";


    return c;
}
QString GlobalStyle::loadStyleSheet(const QString& qssPath, const ThemeColor& color)
{
    // 读取外部qss模板
    QFile file(qssPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "加载样式文件失败:" << qssPath;
        return "";
    }
    QString sheet = file.readAll();
    file.close();

    // 批量替换模板变量
    sheet.replace("{{FONT_FAMILY}}", FONT_FAMILY);
    sheet.replace("{{FONT_NORMAL}}", QString::number(FONT_NORMAL));
    sheet.replace("{{FONT_SMALL}}", QString::number(FONT_SMALL));
    sheet.replace("{{FONT_TITLE}}", QString::number(FONT_TITLE));
    sheet.replace("{{RADIUS}}", QString::number(RADIUS));
    sheet.replace("{{INPUT_PADDING_V}}", QString::number(INPUT_PADDING_V));
    sheet.replace("{{INPUT_PADDING_H}}", QString::number(INPUT_PADDING_H));
    sheet.replace("{{BTN_PADDING}}", QString::number(BTN_PADDING));

    sheet.replace("{{PRIMARY}}", color.primary);
    sheet.replace("{{PRIMARY_HOVER}}", color.primaryHover);
    sheet.replace("{{PRIMARY_PRESS}}", color.primaryPress);
    sheet.replace("{{SUCCESS}}", color.success);
    sheet.replace("{{DANGER}}", color.danger);
    sheet.replace("{{INFO}}", color.info);
    sheet.replace("{{PURPLE}}", color.purple);
    sheet.replace("{{ORANGE}}", color.orange);
    sheet.replace("{{GRAY_LIGHT}}", color.grayLight);
    sheet.replace("{{GRAY_MID}}", color.grayMid);
    sheet.replace("{{GRAY_DARK}}", color.grayDark);
    sheet.replace("{{BG_MAIN}}", color.bgMain);
    sheet.replace("{{BG_WIDGET}}", color.bgWidget);
    sheet.replace("{{TEXT_NORMAL}}", color.textNormal);
    sheet.replace("{{TEXT_LIGHT}}", color.textLight);
    sheet.replace("{{IMPORT}}",       color.importColor);
    sheet.replace("{{IMPORT_HOVER}}", color.importHover);
    sheet.replace("{{IMPORT_PRESS}}", color.importPress);
    sheet.replace("{{SUCCESS_HOVER}}", color.successHover);
    sheet.replace("{{SUCCESS_PRESS}}", color.successPress);
    sheet.replace("{{DANGER_HOVER}}",  color.dangerHover);
    sheet.replace("{{DANGER_PRESS}}",  color.dangerPress);
    sheet.replace("{{INFO_HOVER}}",    color.infoHover);
    sheet.replace("{{INFO_PRESS}}",    color.infoPress);
    sheet.replace("{{PURPLE_HOVER}}",  color.purpleHover);
    sheet.replace("{{PURPLE_PRESS}}",  color.purplePress);
    sheet.replace("{{ORANGE_HOVER}}",  color.orangeHover);
    sheet.replace("{{ORANGE_PRESS}}",  color.orangePress);

    return sheet;
}

void GlobalStyle::switchLightTheme()
{
    g_isDark = false;
    QString style = loadStyleSheet(":/style/base.qss", getLightColor());
    qApp->setStyleSheet(style);
    g_cachedStyle = style;
    // 清空应用调色板缓存，强制所有控件重读QSS
    qApp->setPalette(QApplication::style()->standardPalette());

    // 新增：保存主题状态
    QSettings settings("YourCompany", "StudentManager");
    settings.setValue("theme", "light");

}

void GlobalStyle::switchDarkTheme()
{
    g_isDark = true;
    QString style = loadStyleSheet(":/style/base.qss", getDarkColor());
    qApp->setStyleSheet(style);
    g_cachedStyle = style;
    qApp->setPalette(QApplication::style()->standardPalette());

    // 新增：保存主题状态
    QSettings settings("YourCompany", "StudentManager");
    settings.setValue("theme", "dark");
}

// 新增：初始化主题函数
void GlobalStyle::initTheme() {
    QSettings settings("YourCompany", "StudentManager");
    QString theme = settings.value("theme", "light").toString(); // 默认是亮色

    if (theme == "dark") {
        switchDarkTheme();
    } else {
        switchLightTheme();
    }
}

bool GlobalStyle::isDarkMode()
{
    return g_isDark;
}


void GlobalStyle::setupMessageBoxButton(QMessageBox* msgBox, QMessageBox::StandardButton which, const QString& btnType)
{
    if (!msgBox) return;
    QPushButton* btn = qobject_cast<QPushButton*>(msgBox->button(which));
    if (!btn) {
        qWarning() << "setupMessageBoxButton: button not found for" << which;
        return;
    }

    // 设置属性
    btn->setProperty("btnType", btnType);

    // 强制样式重新计算
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
    btn->update();

    // 同时刷新对话框，确保内部布局更新
    msgBox->update();

    // 调试：打印属性值，确认设置成功
    qDebug() << "Button property set:" << btn->property("btnType").toString();
}
