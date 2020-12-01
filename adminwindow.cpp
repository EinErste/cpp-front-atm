#include "AdminWindow.h"
#include "ui_adminwindow.h"
#include "QInputDialog"
#include "QDebug"
#include "QMessageBox"
#include <string>
#include <regex>
#include "MainWindow.h"
AdminWindow::AdminWindow(QWidget *parent, ATM* _atm, size_t attempts_login) :
    QMainWindow(parent),
    atm(_atm),
    ui(new Ui::AdminWindow),
    attempts_login_(attempts_login)
{
    ui->setupUi(this);
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::closeEvent(QCloseEvent*)
{
    static_cast<QMainWindow*>(parent())->show();
}

void AdminWindow::on_pushButton_admin_login_clicked()
{
    QString login = ui->lineEdit_admin_login->text();
    QString password = ui->lineEdit_admin_password->text();
    auto resp = atm_admin.logIntoATM(atm, login.toStdString(), password.toStdString());
    if (resp.exitCode == 0){
        ui->stackedWidget->setCurrentIndex(1);
    } else{
        if (--attempts_login_ == 0) {
            QMessageBox::warning(this,QString("DANGER"),QString("Too many attempts to login. The police was informed"));
            static_cast<QMainWindow*>(parent())->close();
        } else {
            QMessageBox::warning(this,QString("Error"),QString("Incorrect login or password. Try again"));
            ui->lineEdit_admin_login->setText("");
            ui->lineEdit_admin_password->setText("");
        }
    }
}

void AdminWindow::on_pushButton_balance_clicked()
{
    //set amount here
    QString amount = "";
    auto resp = atm_admin.showCash();
    amount = QString::fromStdString(resp.balance);
    QMessageBox::information (this, "Balance in uah", amount);
}

void AdminWindow::on_pushButton_refill_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Refill", "Amount:", QLineEdit::Normal, "", &ok);
    std::regex num_regex ("[\\d]+");
    if (ok && std::regex_match(text.toStdString(),num_regex)){
        size_t amount = stoui(text.toStdString());
        //Top up here
        atm_admin.refillATM(amount);

        QMessageBox::information (this, "Refill", QString("ATM was refilled"));
    } else {
        QMessageBox::warning(this,QString("Error"),QString("Incorrect amount"));
    }
}

void AdminWindow::on_pushButton_withdraw_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Withdraw", "Amount:", QLineEdit::Normal, "", &ok);
    std::regex num_regex ("[\\d]+");
    if (ok && std::regex_match(text.toStdString(),num_regex)){
        size_t amount = stoui(text.toStdString());
        //widthdraw here
        atm_admin.withdraw(amount);
        QMessageBox::information (this, "Refill", QString("Cash was withdrawn"));
    } else {
        QMessageBox::warning(this,QString("Error"),QString("Incorrect amount"));
    }
}
