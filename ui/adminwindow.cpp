//
//  MOOP_ATM
//
//  Created by Shchebyna Serhii 10.2020-12.2020.
//
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
            QMessageBox::warning(this,QString("DANGER"),QString::fromStdString(resp.description));
            static_cast<QMainWindow*>(parent())->close();
        } else {
            QMessageBox::warning(this,QString("Error"),QString::fromStdString(resp.description));
            ui->lineEdit_admin_login->setText("");
            ui->lineEdit_admin_password->setText("");
        }
    }
}

void AdminWindow::on_pushButton_balance_clicked()
{
    //set amount here
    auto resp = atm_admin.showCash();
    QString amount = QString::fromStdString(resp.balance);
    QMessageBox::information (this, "Balance in uah", amount);
}

void AdminWindow::on_pushButton_refill_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Refill", "Amount:", QLineEdit::Normal, "", &ok);
    std::regex num_regex ("[\\d]+");
    if (ok){
        if (std::regex_match(text.toStdString(),num_regex)){
            try{
                size_t amount = stoui(text.toStdString());
                //Top up here
                auto response = atm_admin.refillATM(amount);
                if (response.exitCode == 0){
                    QMessageBox::information (this, "Refill", QString::fromStdString(response.description));
                } else{
                    QMessageBox::warning (this, "Refill", QString::fromStdString(response.description));
                }
            } catch(...){
                QMessageBox::warning(this,QString("Error"),QString("Some error occured during refilling"));
            }

        } else {
            QMessageBox::warning(this,QString("Error"),QString("Incorrect amount"));
        }

    }
}

void AdminWindow::on_pushButton_withdraw_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Withdraw", "Amount:", QLineEdit::Normal, "", &ok);
    std::regex num_regex ("[\\d]+");
    if (ok){
        if (std::regex_match(text.toStdString(),num_regex)){
            try{
                size_t amount = stoui(text.toStdString());
                //widthdraw here
                auto response = atm_admin.withdraw(amount);
                if (response.exitCode == 0){
                    QMessageBox::information (this, "Withdraw", QString::fromStdString(response.description));
                } else{
                    QMessageBox::warning (this, "Withdraw", QString::fromStdString(response.description));
                }
            } catch(...){
                QMessageBox::warning(this,QString("Error"),QString("Some error occured during withrawal"));
            }
        }else {
            QMessageBox::warning(this,QString("Error"),QString("Incorrect amount"));
        }

    }

}
