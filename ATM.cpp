//
//  ATM.cpp
//  MOOP_ATM
//
//  Created by Nazar Ivaniuk on 12.11.2020.
//

#include "ATM.h"

const std::string ATM_admin::_login = "log";
const std::string ATM_admin::_password = "pass";

ATM::ATM(const size_t id, std::string _login, std::string _password): _cash(10000), _id(id), _ar(_login, _password) {}
ATM::~ATM()
{
    exit();
}
Response<> ATM::login(std::string cardNumber, std::string pin)
{
    Response<Account> r = _ar.login(cardNumber, pin);
    if(r.exitCode == 0) _acc = new Account(r.obj);
    return Response<>{r.exitCode, r.date, r.description, r.balance, Nothing()};
}
Response<> ATM::transfer(std::string dest, int amount)
{
    if(!_acc)
        return Response<>{1, std::time(0), "ACCOUNT was not initialized(need to login)", "NULL", Nothing()};
    auto it = additionalServices.find(dest);
    if (it != additionalServices.end()) dest = it->second;
    Response<> r = _ar.transfer(_acc, dest, amount);
    if(r.exitCode == 0)
        _cash += amount;
    return r;
}
Response<> ATM::transferPhone(std::string dest, int amount)
{
    if(!_acc)
        return Response<>{1, std::time(0), "ACCOUNT was not initialized(need to login)", "NULL", Nothing()};
    auto it = additionalServices.find(dest);
    if (it != additionalServices.end()) dest = it->second;
    std::string comment = "PHONE: " + dest;
    Response<> r = _ar.transferPhone(_acc, comment, amount);
    if(r.exitCode == 0)
        _cash += amount;
    return r;
}
Response<> ATM::changePIN(std::string newPin)
{
    if(!_acc)
        return Response<>{1, std::time(0), "ACCOUNT was not initialized(need to login)", "NULL", Nothing()};
    Response<Details> r1(_ar.getAccountDetails(_acc));
    if(r1.exitCode == 0)
    {
        Details newD = r1.obj;
        newD.PIN = newPin;
        Response<> r2 = _ar.changeDetails(_acc, newD);
        return r2;
    }
    return Response<>{r1.exitCode, r1.date, r1.description,r1.balance, Nothing()};
}
Response<std::vector<Transaction>> ATM::getHistory()
{
    if(!_acc)
        return Response<std::vector<Transaction>>{1, std::time(0), "ACCOUNT was not initialized(need to login)", "NULL", std::vector<Transaction>()};
    return _ar.getHistory(_acc);
}
Response<> ATM::closeAccount(std::string reason)
{
    if(!_acc)
        return Response<>{1, std::time(0), "ACCOUNT was not initialized(need to login)", "NULL", Nothing()};
    return _ar.closeAccount(_acc, reason);
}
Response<Details> ATM::getDetails()
{
    if(!_acc)
        return Response<Details>{1, std::time(0), "ACCOUNT was not initialized(need to login)", "NULL", Details()};
    return _ar.getAccountDetails(_acc);
}
void ATM::exit()
{
    if (_acc) delete _acc;
    _acc = nullptr;
}
Response<> ATM::refill(int amount)
{
    if(!_acc)
        return Response<>{1, std::time(0), "ACCOUNT was not initialized(need to login)",  "NULL", Nothing()};
    Response<> r = _ar.refill(_acc, amount);
    if(r.exitCode == 0)
        _cash += amount;
    return r;
}
Response<> ATM::withdraw(const int amount)
{
    if(!_acc)
        return Response<>{1, std::time(0),"ACCOUNT was not initialized(need to login)","NULL", Nothing()};
    if(_cash<amount)
        return Response<>{1, std::time(0), "Not enough money in ATM", _ar.getBalance(_acc).obj, Nothing()};
    Response<> r = _ar.withdraw(_acc, amount);
    if(r.exitCode == 0)
        _cash -= amount;
    return r;
}

ATM_admin::ATM_admin() : _atm(nullptr) {}
Response<> ATM_admin::logIntoATM(ATM* atm, std::string login, std::string password)
{
    if(login==_login && password==_password)
    {
        _atm = atm;
        return Response<>{0, std::time(0), "Admin ATM access granted", showCash().balance, Nothing()};
    }
    _atm = nullptr;
    return Response<>{2, std::time(0), "Admin ATM access denied", "NULL", Nothing()};
}
Response<> ATM_admin::refillATM(const int amount)
{
    if(_atm)
    {
        _atm->_cash+=amount;
        return Response<>{0, std::time(0), "ATM was refilled by amount " + std::to_string(amount),  showCash().balance,  Nothing()};
    }
    return Response<>{1, std::time(0),"ATM was not initialized(need to login)", "NULL", Nothing()};;
}
Response<> ATM_admin::withdraw(const int amount)
{
    if(_atm)
    {
        if(_atm&&amount<_atm->_cash)
        {
            _atm->_cash-=amount;
            return Response<>{ 0,  std::time(0), "Cash was withdrawn by amount " + std::to_string(amount), showCash().balance, Nothing()};
        }
        return Response<>{3,  std::time(0), "Cash wasn`t withdrawn, not enough money ", showCash().balance, Nothing()};
    }
    return Response<>{1,  std::time(0),  "ATM was not initialized(need to login)", "NULL", Nothing()};
}
Response<> ATM_admin::showCash()
{
    if(_atm)
        return Response<>{ 0,  std::time(0),  "balance: " + std::to_string(_atm->_cash),  std::to_string(_atm->_cash),  Nothing()};
    return Response<>{1,  std::time(0),  "ATM was not initialized(need to login)",  "NULL",  Nothing()};
}
Response<> ATM_admin::addOrChangeAdditionalServices(std::string name, std::string value)
{
    if(_atm)
    {
        _atm->additionalServices[name] = value;
        return Response<>{ 0, std::time(0),  "Service value of " + name + " was changed to " + value,  showCash().balance, Nothing()};
    }
    return Response<>{ 1,  std::time(0),  "ATM was not initialized(need to login)",  "NULL",  Nothing()};
}
