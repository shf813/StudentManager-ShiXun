/********************************************************************************
** Form generated from reading UI file 'loginwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWIDGET_H
#define UI_LOGINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWidget
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter_4;
    QSplitter *splitter;
    QCheckBox *ckDark;
    QLabel *labelTitle;
    QLineEdit *editUser;
    QLineEdit *editPwd;
    QSplitter *splitter_2;
    QCheckBox *ckRemember;
    QSplitter *splitter_3;
    QPushButton *btnLogin;
    QPushButton *btnRegister;

    void setupUi(QWidget *LoginWidget)
    {
        if (LoginWidget->objectName().isEmpty())
            LoginWidget->setObjectName("LoginWidget");
        LoginWidget->resize(600, 600);
        verticalLayout = new QVBoxLayout(LoginWidget);
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        splitter_4 = new QSplitter(LoginWidget);
        splitter_4->setObjectName("splitter_4");
        splitter_4->setOrientation(Qt::Orientation::Vertical);
        splitter = new QSplitter(splitter_4);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        ckDark = new QCheckBox(splitter);
        ckDark->setObjectName("ckDark");
        splitter->addWidget(ckDark);
        splitter_4->addWidget(splitter);
        labelTitle = new QLabel(splitter_4);
        labelTitle->setObjectName("labelTitle");
        splitter_4->addWidget(labelTitle);
        editUser = new QLineEdit(splitter_4);
        editUser->setObjectName("editUser");
        splitter_4->addWidget(editUser);
        editPwd = new QLineEdit(splitter_4);
        editPwd->setObjectName("editPwd");
        editPwd->setEchoMode(QLineEdit::EchoMode::Password);
        splitter_4->addWidget(editPwd);
        splitter_2 = new QSplitter(splitter_4);
        splitter_2->setObjectName("splitter_2");
        splitter_2->setOrientation(Qt::Orientation::Horizontal);
        ckRemember = new QCheckBox(splitter_2);
        ckRemember->setObjectName("ckRemember");
        splitter_2->addWidget(ckRemember);
        splitter_4->addWidget(splitter_2);
        splitter_3 = new QSplitter(splitter_4);
        splitter_3->setObjectName("splitter_3");
        splitter_3->setOrientation(Qt::Orientation::Horizontal);
        btnLogin = new QPushButton(splitter_3);
        btnLogin->setObjectName("btnLogin");
        splitter_3->addWidget(btnLogin);
        btnRegister = new QPushButton(splitter_3);
        btnRegister->setObjectName("btnRegister");
        splitter_3->addWidget(btnRegister);
        splitter_4->addWidget(splitter_3);

        verticalLayout->addWidget(splitter_4);


        retranslateUi(LoginWidget);

        QMetaObject::connectSlotsByName(LoginWidget);
    } // setupUi

    void retranslateUi(QWidget *LoginWidget)
    {
        LoginWidget->setWindowTitle(QCoreApplication::translate("LoginWidget", "Form", nullptr));
        ckDark->setText(QCoreApplication::translate("LoginWidget", "\346\232\227\350\211\262\344\270\273\351\242\230", nullptr));
        labelTitle->setText(QCoreApplication::translate("LoginWidget", "\346\225\231\345\212\241\345\255\246\347\224\237\347\256\241\347\220\206\347\263\273\347\273\237\347\231\273\345\275\225", nullptr));
        ckRemember->setText(QCoreApplication::translate("LoginWidget", "\350\256\260\344\275\217\345\257\206\347\240\201", nullptr));
        btnLogin->setText(QCoreApplication::translate("LoginWidget", "\347\231\273\345\275\225", nullptr));
        btnRegister->setText(QCoreApplication::translate("LoginWidget", "\346\263\250\345\206\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWidget: public Ui_LoginWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWIDGET_H
