//
//  MOOP_ATM
//
//  Created by Shchebyna Serhii 10.2020-12.2020.
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QLineEdit"
#include "middleware/ATM.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *e);
private slots:

    //Global
    void update_clock();

    void on_card_pin_text_textChanged(const QString &arg1);

    void on_numButton_1_clicked();

    void on_numButton_2_clicked();

    void on_numButton_3_clicked();

    void on_numButton_4_clicked();

    void on_numButton_5_clicked();

    void on_numButton_6_clicked();

    void on_numButton_7_clicked();

    void on_numButton_8_clicked();

    void on_numButton_9_clicked();

    void on_numButton_0_clicked();

    void on_pushButton_correction_clicked();

    void on_pushButton_take_cash_clicked();
    //Login
    void show_login_widget();

    void remove_card();

    void validate_card_pin();

    void on_pushButton_insert_card_clicked();
    //Menu

    void show_menu_widget();

    //Transfer

    void show_transfer_widget();

    void set_tranfser_card();

    void set_transfer_amount();

    void accept_transfer();

    //Balance
    void show_balance_widget();

    //Withdrawal
    void show_withdrawal_widget();

    void set_amount_50();

    void set_amount_100();

    void set_amount_200();

    void set_amount_500();

    void set_amount_1000();

    void set_any_amount_withdrawal();

    void withdraw();

    void on_lineEdit_amount_withdraw_textChanged(const QString &arg1);

    //Deposit
    void show_deposit_widget();

    void on_pushButton_insert_cash_clicked();

    void accept_deposit();

    //Phone
    void show_phone_widget();

    void set_phone_number();

    void set_phone_amount();

    void accept_phone_refill();

    //Show account details

    void show_details_widget();

    //Show settings

    void show_settings_widget();


    void on_lineEdit_transfer_amount_textChanged(const QString &arg1);

    void on_lineEdit_phone_amount_textChanged(const QString &arg1);

    void on_lineEdit_transfer_card_textChanged(const QString &arg1);

    void accept_settings_change();

private:

    ATM atm;
    ATM_admin atm_admin;
    void clear_bindings();
    size_t parse_amount(const QString&);
    Ui::MainWindow *ui;
    QLineEdit* focus_line_;
    QTimer* timer_;
    QString card_name_;
    size_t amount_;
    bool menu_avaible_;
    size_t attempts_login_;
    const static size_t min_banknote_;
};
unsigned int stoui(const std::string& str, size_t *idx = nullptr, int base = 10);
QString format_amount(const QString&);
#endif // MAINWINDOW_H
