/********************************************************************************
** Form generated from reading UI file 'loginwidget1.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWIDGET1_H
#define UI_LOGINWIDGET1_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginWidget
{
public:
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBoxDark;
    QLabel *labelTitle;
    QSplitter *splitter;
    QLabel *label;
    QLineEdit *edtUser;
    QSplitter *splitter_2;
    QLabel *label_2;
    QLineEdit *edtPassword;
    QPushButton *btnLogin;

    void setupUi(QDialog *LoginWidget)
    {
        if (LoginWidget->objectName().isEmpty())
            LoginWidget->setObjectName("LoginWidget");
        LoginWidget->resize(490, 400);
        verticalLayout = new QVBoxLayout(LoginWidget);
        verticalLayout->setObjectName("verticalLayout");
        checkBoxDark = new QCheckBox(LoginWidget);
        checkBoxDark->setObjectName("checkBoxDark");

        verticalLayout->addWidget(checkBoxDark);

        labelTitle = new QLabel(LoginWidget);
        labelTitle->setObjectName("labelTitle");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelTitle->sizePolicy().hasHeightForWidth());
        labelTitle->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(16);
        labelTitle->setFont(font);
        labelTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(labelTitle);

        splitter = new QSplitter(LoginWidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        label = new QLabel(splitter);
        label->setObjectName("label");
        splitter->addWidget(label);
        edtUser = new QLineEdit(splitter);
        edtUser->setObjectName("edtUser");
        splitter->addWidget(edtUser);

        verticalLayout->addWidget(splitter);

        splitter_2 = new QSplitter(LoginWidget);
        splitter_2->setObjectName("splitter_2");
        splitter_2->setOrientation(Qt::Orientation::Horizontal);
        label_2 = new QLabel(splitter_2);
        label_2->setObjectName("label_2");
        splitter_2->addWidget(label_2);
        edtPassword = new QLineEdit(splitter_2);
        edtPassword->setObjectName("edtPassword");
        edtPassword->setEchoMode(QLineEdit::EchoMode::Password);
        splitter_2->addWidget(edtPassword);

        verticalLayout->addWidget(splitter_2);

        btnLogin = new QPushButton(LoginWidget);
        btnLogin->setObjectName("btnLogin");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnLogin->sizePolicy().hasHeightForWidth());
        btnLogin->setSizePolicy(sizePolicy1);
        btnLogin->setSizeIncrement(QSize(0, 0));

        verticalLayout->addWidget(btnLogin);


        retranslateUi(LoginWidget);

        QMetaObject::connectSlotsByName(LoginWidget);
    } // setupUi

    void retranslateUi(QDialog *LoginWidget)
    {
        LoginWidget->setWindowTitle(QCoreApplication::translate("LoginWidget", "Dialog", nullptr));
        checkBoxDark->setText(QCoreApplication::translate("LoginWidget", "\346\232\227\350\211\262\344\270\273\351\242\230", nullptr));
        labelTitle->setText(QCoreApplication::translate("LoginWidget", "\347\231\273\345\275\225\347\225\214\351\235\242", nullptr));
        label->setText(QCoreApplication::translate("LoginWidget", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("LoginWidget", "\345\257\206\347\240\201\357\274\232", nullptr));
        btnLogin->setText(QCoreApplication::translate("LoginWidget", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWidget: public Ui_LoginWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWIDGET1_H
