//
//  ATM.h
//  MOOP_ATM
//
//  Created by Nazar Ivaniuk on 12.11.2020.
//

#ifndef ATM_h
#define ATM_h
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "AccountRepository.h"



class ATM
{
    friend class ATM_admin;
private:
    std::map<std::string, std::string> additionalServices;
    const size_t _id;
    size_t _cash;
    const Account* _acc = nullptr;
    AccountRepository _ar;
public:
//    static const std::string STEAM = "steam";
    ATM(const size_t id, std::string _login, std::string _password);
    ~ATM();
    Response<> login(std::string cardNumber, std::string pin);
    Response<> transfer(std::string dest, int amount);
    Response<> changePIN(std::string newPin);
    Response<std::vector<Transaction>> getHistory();
    Response<> closeAccount(std::string reason);
    Response<Details> getDetails();
    void exit();
    Response<> refill(int amount);
    Response<> withdraw(const int amount);
};

class ATM_admin
{
private:
    ATM* _atm;
    static const std::string _login;
    static const std::string _password;
public:
    ATM_admin();
    Response<> logIntoATM(ATM* atm, std::string login, std::string password);
    Response<> refillATM(const int amount);
    Response<> withdraw(const int amount);
    Response<> showCash();
    Response<> addOrChangeAdditionalServices(std::string name, std::string value);
};



#endif /* ATM_h */
