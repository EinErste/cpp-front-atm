//
//  MOOP_ATM
//
//  Created by Shchebyna Serhii 10.2020-12.2020.
//
#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "QInputDialog"
#include "QDebug"
#include "QMessageBox"
#include <string>
#include <regex>
#include <iostream>
#include "QFile"
#include "QTimer"
#include "QTime"
#include "QKeyEvent"
#include "AdminWindow.h"
#include <vector>
const size_t MainWindow::min_banknote_ = 50;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    atm(1, "log", "pass"),
    ui(new Ui::MainWindow),
    focus_line_(nullptr),
    timer_(new QTimer()),
    card_name_(""),
    amount_(0),
    menu_avaible_(true),
    attempts_login_(3)
{
    ui->setupUi(this);
    setWindowTitle(tr("ATM LarinBets"));
    connect(timer_, SIGNAL(timeout()), this, SLOT(update_clock()));
    timer_->start(1000);

    setFixedSize(size());
    show_login_widget();
}

MainWindow::~MainWindow()
{
    delete timer_;
    delete ui;
}

void MainWindow::update_clock()
{
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss   ");
    ui->label_time->setText(time_text);
    ui;
}


//Login
void MainWindow::show_login_widget()
{
    attempts_login_ = 3;
    clear_bindings();
    ui->stackedWidget->setCurrentIndex(0);

    ui->lineEdit_insert_card->setDisabled(false);
    ui->pushButton_insert_card->setDisabled(false);

    ui->card_pin_text->setDisabled(true);
    ui->card_pin_text->hide();
    ui->labe_pin->hide();
    ui->label_insert_card_invitation->show();
    ui->card_pin_text->setText("");

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(remove_card()));
}

void MainWindow::on_pushButton_insert_card_clicked()
{
    QString number = ui->lineEdit_insert_card->text();
    std::regex card_regex ("[\\d]{16}");
    if (std::regex_match(number.toStdString(),card_regex)){
        card_name_ = QString(ui->lineEdit_insert_card->text());
        ui->lineEdit_insert_card->setText("");
        ui->lineEdit_insert_card->setDisabled(true);
        ui->pushButton_insert_card->setDisabled(true);
        connect(ui->pushButton_accept, SIGNAL(clicked()), this, SLOT(validate_card_pin()));
        ui->label_insert_card_invitation->hide();
        ui->card_pin_text->show();
        ui->labe_pin->show();
        focus_line_ = ui->card_pin_text;
    } else{
        QMessageBox::warning(this,QString("Error"),QString("Something wrong with your card. Try again"));

    }
}


void MainWindow::validate_card_pin()
{
    QString pin = ui->card_pin_text->text();
    std::regex card_regex ("[\\d]{4}");

    if (std::regex_match(pin.toStdString(),card_regex)){
        auto resp = atm.login(card_name_.toStdString(), pin.toStdString());
        if (resp.exitCode == 0) {
            ui->card_pin_text->setText("");
            show_menu_widget();
        } else{
            if (--attempts_login_ == 0) {
                ui->lineEdit_insert_card->setText("");
                QMessageBox::information(this,QString("Card"),QString("Your card has been blocked. Please, contact your bank."));
                atm.closeAccount("Too many attempts too login");
                atm.exit();
                show_login_widget();
            } else {
                ui->card_pin_text->setText("");
                QMessageBox::warning(this,QString("Error"),QString("Incorrect PIN. Attempts left: ") + QString::fromStdString(std::to_string(attempts_login_)));
            }

        }

    } else{
         ui->card_pin_text->setText("");
        QMessageBox::warning(this,QString("Error"),QString("Incorrect PIN format"));
    }
}

void MainWindow::on_card_pin_text_textChanged(const QString& str)
{
    std::string s = str.toStdString();
    if (s.length()>4) s.pop_back();
    ui->card_pin_text->setText(tr(s.c_str()));
}

void MainWindow::remove_card()
{
    if (card_name_!=""){
        atm.exit();
        ui->lineEdit_insert_card->setText(card_name_);
        card_name_ = "";
        show_login_widget();
        QMessageBox::information(this,QString("Information"),QString("Card was ejected"));
    }

}

//Menu
void MainWindow::show_menu_widget()
{
    if (menu_avaible_){
        clear_bindings();
        ui->stackedWidget->setCurrentIndex(1);
        ui->lineEdit_insert_card->setDisabled(true);
        ui->pushButton_insert_card->setDisabled(true);
        ui->lineEdit_insert_cash->setDisabled(true);
        ui->pushButton_insert_cash->setDisabled(true);
        connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(remove_card()));
        connect(ui->pushButton_1, SIGNAL(clicked()), this, SLOT(show_transfer_widget()));
        connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(show_balance_widget()));
        connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(show_withdrawal_widget()));
        connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(show_deposit_widget()));
        connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(show_phone_widget()));
        connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(show_details_widget()));
        connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(show_settings_widget()));
        //connect(ui->pushButton_8, SIGNAL(clicked()), this, nullptr);


    } else{
        QMessageBox::information(this,QString("Information"),QString("Please, finish current operation"));
    }

}

//Tranfser
void MainWindow::show_transfer_widget()
{
    clear_bindings();
    ui->stackedWidget->setCurrentIndex(2);
    amount_ = 0;
    ui->lineEdit_transfer_card->setText("");
    ui->lineEdit_transfer_amount->setText("0");
    ui->pushButton_3->setCheckable(true);
    ui->pushButton_4->setCheckable(true);
    ui->pushButton_5->setCheckable(true);
    ui->pushButton_6->setCheckable(true);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(show_menu_widget()));
    connect(ui->pushButton_accept, SIGNAL(clicked()), this, SLOT(accept_transfer()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(set_tranfser_card()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(set_tranfser_card()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(set_transfer_amount()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(set_transfer_amount()));
}

void MainWindow::set_tranfser_card()
{
    focus_line_ = ui->lineEdit_transfer_card;
    ui->pushButton_3->setChecked(true);
    ui->pushButton_4->setChecked(true);
}

void MainWindow::set_transfer_amount()
{
    focus_line_ = ui->lineEdit_transfer_amount;
    ui->pushButton_5->setChecked(true);
    ui->pushButton_6->setChecked(true);
}

void MainWindow::accept_transfer()
{
    amount_ = parse_amount(ui->lineEdit_transfer_amount->text());

    if (amount_!=0){
        QString card = ui->lineEdit_transfer_card->text();
        std::regex card_regex ("[\\d]{16}");
        if (std::regex_match(card.toStdString(),card_regex)){
            if (QMessageBox::Yes == QMessageBox::question(this, tr("Transfer"),tr(
                                                              (std::string("Are you sure you want to transfer to card ")+
                                                               ui->lineEdit_transfer_card->text().toStdString() +
                                                               "?").c_str())))
            {
                auto resp = atm.transfer(card.toStdString(), amount_);
                if (resp.exitCode == 0) {
                    QMessageBox::information(this,QString("Information"),QString("Transfer was successful"));
                    ui->lineEdit_transfer_card->setText("");
                    ui->lineEdit_transfer_amount->setText("");
                    amount_ = 0;
                    show_menu_widget();
                } else{
                    QMessageBox::warning(this,QString("Error"),QString::fromStdString(resp.description));
                }
            }
        } else{
            QMessageBox::information(this,QString("Information"),QString("Check your transfer card number"));

        }

    } else {

        QMessageBox::information(this,QString("Information"),QString("Amount must be greater than zero"));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Escape){
        this->hide();
        QMainWindow* admin = new AdminWindow(this,&atm);
        admin->show();
    }

}

//Balance
void MainWindow::show_balance_widget()
{

    QAbstractItemModel* const mdl = ui->tableWidget_history->model();
    mdl->removeRows(0,mdl->rowCount());
    auto resp = atm.getDetails();
    std::string balance = resp.balance;
    clear_bindings();
    ui->stackedWidget->setCurrentIndex(3);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(show_menu_widget()));
    ui->label_balance_number->setText(QString::fromStdString("Balance: " + balance.substr(0, balance.find(".")+3)) +"₴");

    auto h = atm.getHistory();
    size_t index = 0;
    for(auto it = h.obj.begin() ;it!=h.obj.end();++it, ++index){
        ui->tableWidget_history->insertRow(index);
        std::regex regex{R"(T)"};
        std::sregex_token_iterator str_iter{it->date.begin(), it->date.end(), regex, -1};
        std::vector<std::string> time_date{str_iter, {}};
        ui->tableWidget_history->setItem(index,0, new QTableWidgetItem(QString::fromStdString(time_date[0])));
        ui->tableWidget_history->setItem(index,1, new QTableWidgetItem(QString::fromStdString(time_date[1].substr(0,8))));
        ui->tableWidget_history->setItem(index,2, new QTableWidgetItem(QString::fromStdString(it->comment)));
        ui->tableWidget_history->setItem(index,3, new QTableWidgetItem(QString::fromStdString(it->amount.substr(0, it->amount.find(".")+3)) +"₴"));
        ui->tableWidget_history->setItem(index,4, new QTableWidgetItem(QString::fromStdString(it->to)));
    }

    ui;
}


//Withdraw
void MainWindow::show_withdrawal_widget()
{
    clear_bindings();
    ui->stackedWidget->setCurrentIndex(4);
    ui->pushButton_3->setCheckable(true);
    ui->pushButton_4->setCheckable(true);
    ui->pushButton_5->setCheckable(true);
    ui->pushButton_6->setCheckable(true);
    ui->pushButton_7->setCheckable(true);
    ui->pushButton_8->setCheckable(true);
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(set_amount_50()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(set_amount_100()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(set_amount_200()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(set_amount_500()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(set_amount_1000()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(set_any_amount_withdrawal()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(show_menu_widget()));
    connect(ui->pushButton_accept, SIGNAL(clicked()), this, SLOT(withdraw()));
}

void MainWindow::set_amount_50()
{
    focus_line_ = nullptr;
    amount_=50;
    ui->pushButton_3->setChecked(true);

}

void MainWindow::set_amount_100()
{
    focus_line_ = nullptr;
    amount_=100;
    ui->pushButton_4->setChecked(true);

}

void MainWindow::set_amount_200()
{
    focus_line_ = nullptr;
    amount_=200;
    ui->pushButton_5->setChecked(true);
}

void MainWindow::set_amount_500()
{
    focus_line_ = nullptr;
    amount_=500;
    ui->pushButton_6->setChecked(true);
}

void MainWindow::set_amount_1000()
{
    focus_line_ = nullptr;
    amount_=1000;
    ui->pushButton_7->setChecked(true);
}

void MainWindow::set_any_amount_withdrawal()
{
    focus_line_ = ui->lineEdit_amount_withdraw;
    amount_ = parse_amount(ui->lineEdit_amount_withdraw->text());
    ui->pushButton_8->setChecked(true);
}
void MainWindow::on_lineEdit_amount_withdraw_textChanged(const QString & str)
{
    ui->lineEdit_amount_withdraw->setText(format_amount(str));
    set_any_amount_withdrawal();
}

void MainWindow::withdraw()
{
    if (amount_!=0){
        if (QMessageBox::Yes == QMessageBox::question(this,
                                                      tr("Withdrawal"),
                                                      (std::string("Are you sure you want to withdraw ") + std::to_string(amount_) + "?").c_str()))
        {
            if (amount_ % min_banknote_ == 0){
                //balance ok?
                qDebug() << amount_;
                auto resp = atm.withdraw(amount_);
                if (resp.exitCode==0){
                    size_t old_amount = 0;
                    try {
                        old_amount = stoui(ui->label_take_cash->text().toStdString());
                    } catch (...) {}
                    ui->label_take_cash->setText(std::to_string(old_amount+amount_).c_str());
                    QMessageBox::information(this,QString("Information"),(std::string("You have widthdrawn ") + std::to_string(amount_)).c_str());
                    show_menu_widget();
                } else{
                    QMessageBox::warning(this,QString("Error"),QString::fromStdString(resp.description));
                }
            } else {
                 QMessageBox::warning(this,QString("Error"),QString("Amount must be %50 == 0"));
            }
        }
    } else {
        QMessageBox::information(this,QString("Information"),QString("Select amount"));
    }

}
//Deposit
void MainWindow::show_deposit_widget()
{
    clear_bindings();
    amount_ = 0;
    ui->stackedWidget->setCurrentIndex(5);
    ui->lineEdit_insert_cash->setDisabled(false);
    ui->pushButton_insert_cash->setDisabled(false);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(show_menu_widget()));
    connect(ui->pushButton_accept, SIGNAL(clicked()), this, SLOT(accept_deposit()));
}

void MainWindow::on_pushButton_insert_cash_clicked()
{
    if ( ui->lineEdit_insert_cash->text()=="") return;
    try {
        size_t amount = stoui(ui->lineEdit_insert_cash->text().toStdString());
        if (amount % min_banknote_ != 0) throw "";

        menu_avaible_ = false;
        ui->lineEdit_insert_cash->setText("");
        amount_ += amount;
        ui->label_deposited_cash->setText(std::to_string(amount_).c_str());
    } catch (...) {
        QMessageBox::warning(this,QString("Error"),QString("Banknote was rejected or there was none. Check and try again."));
    }

}


void MainWindow::accept_deposit()
{
    if (amount_!=0){
        if (QMessageBox::Yes == QMessageBox::question(this,
                                                      tr("Depositing"),
                                                      tr("Are you sure you want to finish depositing?")))
        {
            QMessageBox::information(this,QString("Information"), QString::fromStdString(std::to_string(amount_)) + QString("uah was deposited"));
            menu_avaible_ = true;
            ui->label_deposited_cash->setText("0");
            atm.refill(amount_);
            amount_ = 0;
            show_menu_widget();
        }
    } else {
        QMessageBox::information(this,QString("Information"),QString("Deposit your cash"));
    }

}

//Phone
void MainWindow::show_phone_widget()
{
    clear_bindings();
    amount_ = 0;
    ui->lineEdit_phone_number->setText("");
    ui->lineEdit_phone_amount->setText("0");
    ui->stackedWidget->setCurrentIndex(6);
    ui->pushButton_5->setCheckable(true);
    ui->pushButton_6->setCheckable(true);
    ui->pushButton_7->setCheckable(true);
    ui->pushButton_8->setCheckable(true);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(show_menu_widget()));
    connect(ui->pushButton_accept, SIGNAL(clicked()), this, SLOT(accept_phone_refill()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(set_phone_number()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(set_phone_number()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(set_phone_amount()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(set_phone_amount()));

}

void MainWindow::set_phone_number()
{
    focus_line_ = ui->lineEdit_phone_number;
    ui->pushButton_5->setChecked(true);
    ui->pushButton_6->setChecked(true);
}

void MainWindow::set_phone_amount()
{
    focus_line_ = ui->lineEdit_phone_amount;
    ui->pushButton_7->setChecked(true);
    ui->pushButton_8->setChecked(true);
}

void MainWindow::accept_phone_refill()
{
    amount_ = parse_amount(ui->lineEdit_phone_amount->text());

    if (amount_!=0){
        if (QMessageBox::Yes == QMessageBox::question(this, tr("Phone refilling"),tr(
                                                          (std::string("Are you sure you want to refill phone ")+
                                                           ui->lineEdit_phone_number->text().toStdString() +
                                                           "?").c_str())))
        {
            auto resp = atm.transfer(ui->lineEdit_phone_number->text().toStdString(), amount_);
            if (resp.exitCode == 0){
                QMessageBox::information(this,QString("Information"),QString("Phone was refilled"));
                ui->lineEdit_phone_number->setText("");
                ui->lineEdit_phone_amount->setText("");
                amount_ = 0;
                show_menu_widget();
            } else{
                QMessageBox::warning(this,QString("Error"),QString::fromStdString(resp.description));
            }

        }
    } else {
        QMessageBox::information(this,QString("Information"),QString("Amount must be greater than zero"));
    }
}
//Details
void MainWindow::show_details_widget()
{
    clear_bindings();
    ui->stackedWidget->setCurrentIndex(7);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(show_menu_widget()));
    //set data
    Details d = atm.getDetails().obj;
    ui->label_details_cardnumber->setText(QString::fromStdString(d.cardnum));
    ui->label_details_name->setText(QString::fromStdString(d.name));
    ui->label_details_surname->setText(QString::fromStdString(d.surname));
    ui->label_details_expdate->setText(QString::fromStdString(d.expDate));
}
//Settings
void MainWindow::show_settings_widget()
{
    clear_bindings();
    ui->stackedWidget->setCurrentIndex(8);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(show_menu_widget()));
    connect(ui->pushButton_accept, SIGNAL(clicked()), this, SLOT(accept_settings_change()));
    ui->lineEdit_change_pin->setText("");
    focus_line_ = ui->lineEdit_change_pin;
}

void MainWindow::accept_settings_change()
{
    QString pin = ui->lineEdit_change_pin->text();
    std::regex card_regex ("[\\d]{4}");

    if (std::regex_match(pin.toStdString(),card_regex)){
        if (QMessageBox::Yes == QMessageBox::question(this,
                                                      tr("Settings"),
                                                      tr("Are you sure you want to change your pin?")))
        {
            QMessageBox::information(this,QString("Information"),QString("Pin was changed"));

            atm.changePIN(pin.toStdString());
            show_menu_widget();
        }
    } else {
        QMessageBox::warning(this,QString("Information"),QString("Pin consists of 4 numbers"));
    }
}


//General
void MainWindow::clear_bindings()
{
    focus_line_ = nullptr;
    foreach(QAbstractButton  *button, ui->buttonGroup->buttons()) {
        disconnect(button, SIGNAL(clicked()), nullptr, nullptr);
        button->setChecked(false);
        button->setCheckable(false);
        button->repaint();
        button->update();
    }
    disconnect(ui->pushButton_accept, SIGNAL(clicked()), nullptr, nullptr);
    disconnect(ui->pushButton_cancel, SIGNAL(clicked()), nullptr, nullptr);

    amount_ = 0;
    ui->lineEdit_amount_withdraw->setText("2500");

    ui->lineEdit_phone_amount->setText("");
    ui->lineEdit_phone_number->setText("");

}

unsigned int stoui(const std::string& str, size_t *idx, int base) {

    unsigned long u = std::stoul(str, idx, base);
    if (u > UINT_MAX) throw std::out_of_range(str);
    return u;
}

size_t MainWindow::parse_amount(const QString & str)
{
    size_t amount = 0;
    try {
        amount = stoui(str.toStdString());
    } catch (const std::exception& d) {
        QMessageBox::warning(this,QString("Error"),QString("Incorrect amount"));
    }

    return amount;
}

void MainWindow::on_numButton_1_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'1');
    }
}

void MainWindow::on_numButton_2_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'2');
    }
}

void MainWindow::on_numButton_3_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'3');
    }
}

void MainWindow::on_numButton_4_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'4');
    }
}

void MainWindow::on_numButton_5_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'5');
    }
}

void MainWindow::on_numButton_6_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'6');
    }
}

void MainWindow::on_numButton_7_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'7');
    }
}

void MainWindow::on_numButton_8_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'8');
    }
}

void MainWindow::on_numButton_9_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'9');
    }
}

void MainWindow::on_numButton_0_clicked()
{
    if (focus_line_!=nullptr){
        focus_line_->setText(focus_line_->text()+'0');
    }
}

void MainWindow::on_pushButton_correction_clicked()
{
    if (focus_line_!=nullptr){
        std::string str = focus_line_->text().toStdString();
        if (str.length()>0) str.pop_back();
        focus_line_->setText(QString(str.c_str()));
    }
}

void MainWindow::on_pushButton_take_cash_clicked()
{
    ui->label_take_cash->setText("");
}

QString format_amount(const QString& str)
{
    std::string amount_string = str.toStdString();
    amount_string.erase(0, std::min(amount_string.find_first_not_of('0'), amount_string.size()-1));
    if (amount_string.size()==0) amount_string = "0";
    return QString(amount_string.c_str());
}

void MainWindow::on_lineEdit_transfer_amount_textChanged(const QString &str)
{
    ui->lineEdit_transfer_amount->setText(format_amount(str));
}

void MainWindow::on_lineEdit_phone_amount_textChanged(const QString &str)
{
    ui->lineEdit_phone_amount->setText(format_amount(str));

}

void MainWindow::on_lineEdit_transfer_card_textChanged(const QString &str)
{
    std::string s = str.toStdString();
    if (s.length()>16) s.pop_back();
    ui->lineEdit_transfer_card->setText(tr(s.c_str()));
}


