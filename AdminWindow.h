#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include "ATM.h"

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event);
public:
    explicit AdminWindow(QWidget *parent = nullptr, ATM* atm = nullptr, size_t attempts_login = 3);
    ~AdminWindow();

private slots:
    void on_pushButton_admin_login_clicked();

    void on_pushButton_balance_clicked();

    void on_pushButton_refill_clicked();

    void on_pushButton_withdraw_clicked();

private:
    ATM* atm;
    ATM_admin atm_admin;
    Ui::AdminWindow *ui;
    size_t attempts_login_;
};

#endif // ADMINWINDOW_H
