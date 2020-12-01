//
//  AccountRepository.h
//  MOOP_ATM
//
//  Created by Nazar Ivaniuk on 12.11.2020.
//

#ifndef AccountRepository_h
#define AccountRepository_h
#include "AdditionalStructures.h"
#include "httplib.h"


class AccountRepository
{
private:
    const std::string HOST = "http://localhost:8000";
    httplib::Client cli;
public:
    AccountRepository(std::string login, std::string password);
    Response<Account> login(const std::string cardNum, const std::string pin);
    Response<> transfer(const Account* acc, const std::string destCardNum, const int amount);
    Response<> withdraw(const Account* acc, const int amount);
    Response<> refill(const Account* acc, const int amount);
    Response<> changeDetails(const Account* acc, const Details d);
    Response<std::vector<Transaction>> getHistory(const Account* acc);
    Response<Details> getAccountDetails(const Account* acc);
    Response<> closeAccount(const Account* acc, std::string reason);
    Response<std::string> getBalance(const Account* acc);
};


#endif /* AccountRepository_h */
