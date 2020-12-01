#include "AccountRepository.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;

AccountRepository::AccountRepository(std::string login, std::string password): cli("localhost", 8080)
{
}

std::string toJSONtime(const time_t& now)
{
    std::stringstream ss;
    tm *gmtm = localtime(&now);
    std::string day = gmtm->tm_mday < 10 ? "0"+std::to_string(gmtm->tm_mday) : std::to_string(gmtm->tm_mday);
    std::string mon = gmtm->tm_mon < 10 ? "0"+std::to_string(gmtm->tm_mon) : std::to_string(gmtm->tm_mon);
    ss<<""<<"2020"<<"-"<<mon<<"-"<<day<<"T"<<gmtm->tm_hour<<":"<<gmtm->tm_min<<":"<<gmtm->tm_sec<<".0000000Z";
    std::string res;
    ss>>res;
    return res;
}

Response<Account> AccountRepository::login(const std::string cardNum, const std::string pin)
{
    std::string requestBody = "{\"cardNum\":\"" + cardNum + "\", \"pin\":\"" + pin + "\"}";

    if (auto res = cli.Post("/auth/pin", requestBody, "application/json")) {
#ifndef NDEBUG
    std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
#ifndef NDEBUG
            std::cout << res->body << std::endl;
#endif
            std::string token = res->body;
            json j = json::parse(token);
            std::string tokenValue = j["token"].get<std::string>();
            const int id = j["id"].get<int>();
            const int card_id = j["card_id"].get<int>();
            const Account acc{ id, card_id, tokenValue };
            time_t date = std::time(0);
            return Response<Account>{0, date, "Login Successful", "NULL", acc};
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized 401" << std::endl;
#endif
            time_t date = std::time(0);
            return Response<Account>{ 401, date, "Login Failed", "NULL", Account::getDefault() };
        }
        else if (res->status == 400) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            time_t date = std::time(0);
            return Response<Account>{ 400, date, "Bad Input", "NULL", Account::getDefault() };
        }
        else if (res->status == 500) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            time_t date = std::time(0);
            return Response<Account>{ 500, date, "Login Failed. Server is down!", "NULL", Account::getDefault() };
        }
        else {
            time_t date = std::time(0);
            return Response<Account>{ 404, date, "Login Failed. Server is down!", "NULL", Account::getDefault() };
        }
    }
    else {
        auto err = res.error();
        time_t date = std::time(0);
        return Response<Account>{ 404, date, "Login Failed. Server is down!", "NULL", Account::getDefault() };
    }
}

Response<> AccountRepository::transfer(const Account* acc, const std::string destCardNum, const int amount)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    time_t date = std::time(0);
    std::string json_date = toJSONtime(date);
    json jBody = {
        {"card_num", destCardNum},
        {"amount", amount},
        {"card_id", acc->_card_id},
        {"transaction_date", json_date},
        {"comment", "Transfer to card"}
    };
    std::string requestBody = jBody.dump(4);
#ifndef NDEBUG
    std::cout << requestBody << std::endl;
#endif
    if (auto res = cli.Post("/transactions-api/transaction", requestBody, "application/json")) {
#ifndef NDEBUG
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            /*
            * {
                "ID": 4,
                "CreatedAt": "2020-11-17T19:46:37.7556395Z",
                "UpdatedAt": "2020-11-17T19:46:37.7556395Z",
                "DeletedAt": "0001-01-01T00:00:00Z",
                "card_id": 2,
                "card_num": "542234564322222",
                "amount": 55.23,
                "transaction_date": "2020-04-23T18:25:43.511Z"
            }
            */
            return Response<>{ 0, date, "Transfer Successful", "NULL", Nothing{}};
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            return Response<>{ 401, date, "Transfer Failed, Unauthorized", "NULL", Nothing{}};
        }
        else {
            return Response<>{ 1, date, "Transfer Failed", "NULL", Nothing{}};
        }
    }
    else {
        auto err = res.error();
        return Response<>{ 1, date, "Transfer Failed", "NULL", Nothing{}};
    }
}

Response<> AccountRepository::transferPhone(const Account* acc, const std::string comment, const int amount)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    time_t date = std::time(0);
    std::string json_date = toJSONtime(date);
    json jBody = {
        {"card_num", "2221009198076721"},
        {"amount", amount},
        {"card_id", acc->_card_id},
        {"transaction_date", json_date},
        {"comment", comment}
    };
    std::string requestBody = jBody.dump(4);
#ifndef NDEBUG
    std::cout << requestBody << std::endl;
#endif
    if (auto res = cli.Post("/transactions-api/transaction", requestBody, "application/json")) {
#ifndef NDEBUG
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            /*
            * {
                "ID": 4,
                "CreatedAt": "2020-11-17T19:46:37.7556395Z",
                "UpdatedAt": "2020-11-17T19:46:37.7556395Z",
                "DeletedAt": "0001-01-01T00:00:00Z",
                "card_id": 2,
                "card_num": "542234564322222",
                "amount": 55.23,
                "transaction_date": "2020-04-23T18:25:43.511Z"
            }
            */
            return Response<>{ 0, date, "Transfer Successful", "NULL", Nothing{}};
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            return Response<>{ 401, date, "Transfer Failed, Unauthorized", "NULL", Nothing{}};
        }
        else {
            return Response<>{ 1, date, "Transfer Failed", "NULL", Nothing{}};
        }
    }
    else {
        auto err = res.error();
        return Response<>{ 1, date, "Transfer Failed", "NULL", Nothing{}};
    }
}


Response<> AccountRepository::withdraw(const Account* acc, const int amount)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    time_t date = std::time(0);
    std::string json_date = toJSONtime(date);
    json jBody = {
        {"card_num", "2221009198076721"},
        {"comment", "Withdraw"},
        {"amount", amount},
        {"card_id", acc->_card_id},
        {"transaction_date", json_date}
    };
    std::string requestBody = jBody.dump(4);
    if (auto res = cli.Post("/transactions-api/transaction", requestBody, "application/json")) {
#ifndef NDEBUG
        std::cout << "Request Successful" << std::endl;
        std::cout << requestBody << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            return Response<>{ 0, date, "Transfer Successful", "NULL", Nothing{}};
        }
        else if (res->status == 401) {
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            return Response<>{ 401, date, "Transfer Failed", "NULL", Nothing{} };
        }
        else {
            return Response<>{ 1, date, "Transfer Failed", "NULL", Nothing{} };
        }
    }
    else {
        auto err = res.error();
        return Response<>{ 1, date, "Transfer Failed", "NULL", Nothing{} };
    }
}

Response<> AccountRepository::refill(const Account* acc, const int amount)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    time_t date = std::time(0);
    std::string json_date = toJSONtime(date);
    json jBody = {
        {"card_num", "2221009198076721"},
        {"comment", "Refill"},
        {"amount", amount},
        {"card_id", acc->_card_id},
        {"transaction_date", json_date}
    };
    std::string requestBody = jBody.dump(4);
    if (auto res = cli.Post("/transactions-api/refill", requestBody, "application/json")) {
#ifndef NDEBUG
        std::cout << requestBody << std::endl;
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            /*
            * {
                "ID": 4,
                "CreatedAt": "2020-11-17T19:46:37.7556395Z",
                "UpdatedAt": "2020-11-17T19:46:37.7556395Z",
                "DeletedAt": "0001-01-01T00:00:00Z",
                "card_id": 2,
                "card_num": "542234564322222",
                "amount": 55.23,
                "transaction_date": "2020-04-23T18:25:43.511Z"
            }
            */

            return Response<>{ 0, date, "Refill Successful", "NULL", Nothing{}};
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
            return Response<>{ 401, date, "Refill Failed", "NULL", Nothing{}};
#endif
        }
        else {
            return Response<>{ 401, date, "Refill Failed", "NULL", Nothing{}};
        }
    }
    else {
        auto err = res.error();
        return Response<>{ 401, date, "Refill Failed", "NULL", Nothing{}};
    }
}

Response<> AccountRepository::changeDetails(const Account* acc, const Details d)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    json jBody = {
        {"card_num", d.cardnum},
        {"cvv2", d.CCV},
        {"pin", d.PIN},
        {"expireDate", d.expDate}
    };
    std::string requestBody = jBody.dump(4);
    std::string url = "/cards-api/card/" + std::to_string(acc->_card_id);
    if (auto res = cli.Put(url.c_str(), requestBody, "application/json")) {
#ifndef NDEBUG
        std::cout << requestBody << std::endl;
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            time_t date = std::time(0);
            return Response<>{ 0, date, "Change Successful", "NULL", Nothing{}};
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            time_t date = std::time(0);
            return Response<>{ 401, date, "Change Failed", "NULL", Nothing{}};
        }
        else {
            time_t date = std::time(0);
            return Response<>{ 1, date, "Change Failed", "NULL", Nothing{}};
        }
    }
    else {
        auto err = res.error();
        time_t date = std::time(0);
        return Response<>{ 1, date, "Change Failed", "NULL", Nothing{}};
    }
}

Response<std::vector<Transaction>> AccountRepository::getHistory(const Account* acc)
{
    std::string url = "/transactions-api/transactions/" + std::to_string(acc->_card_id);
    cli.set_bearer_token_auth(acc->_token.c_str());
    if (auto res = cli.Get(url.c_str())) {
#ifndef NDEBUG
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            time_t date = std::time(0);
            std::vector<Transaction> transactionsV;
            auto const transactions = j.find("transactions");
            for (auto const& transaction : *transactions) {
                std::string to = transaction["card_num"].get<std::string>();
                std::string comment = transaction["comment"].get<std::string>();
                const double amount = transaction["amount"].get<double>();
                std::string date = transaction["CreatedAt"].get<std::string>();
                const Transaction tr{ to, date, std::to_string(amount), comment };
                transactionsV.push_back(tr);
            }
            return Response<std::vector<Transaction>>{ 0, date, "Fetch Successful", "Balance yet unknown", transactionsV};
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            time_t date = std::time(0);
            return Response<std::vector<Transaction>>{ 401, date, "Fetch Successful", "Balance yet unknown"};
        }
        else {
            time_t date = std::time(0);
            return Response<std::vector<Transaction>>{ 1, date, "Fetch Successful", "Balance yet unknown"};
        }
    }
    else {
        auto err = res.error();
        time_t date = std::time(0);
        return Response<std::vector<Transaction>>{ 1, date, "Fetch Successful", "Balance yet unknown"};
    }
}

Response<Details> AccountRepository::getAccountDetails(const Account* acc)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    std::string url = "/accounts-api/account/" + std::to_string(acc->_id);
    if (auto res = cli.Get(url.c_str())) {
#ifndef NDEBUG
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            time_t date = std::time(0);
            std::string name = j["name"].get<std::string>();
            std::string surname = j["surname"].get<std::string>();
            std::string cardnum = j["cardNum"].get<std::string>();
            std::string expDate = j["expDate"].get<std::string>();
            double balance = j["balance"].get<double>();
            Details d{cardnum, "", "", name, surname, expDate};
            Response<Details> res{ 0, date, "Fetch Successful", std::to_string(balance), d};
            return res;
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
            Details d{"", "", "", "", "", ""};
            time_t date = std::time(0);
            Response<Details> res{ 0, date, "Fetch Successful", "Balance yet unknown", d};
            return res;
        } else {
            Details d{"", "", "", "", "", ""};
            time_t date = std::time(0);
            Response<Details> res{ 0, date, "Fetch Successful", "Balance yet unknown", d};
            return res;
        }
    }
    else {
        auto err = res.error();
    }
}

Response<> AccountRepository::closeAccount(const Account* acc, std::string reason)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    std::string url = "/accounts-api/account/" + std::to_string(acc->_id);
    if (auto res = cli.Delete(url.c_str())) {
#ifndef NDEBUG
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            time_t date = std::time(0);
            Response<> res{ 0, date, "Close Successful", "Balance yet unknown" , Nothing{} };
            return res;
        }
        else if (res->status == 401) {
            // throw errror
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
#endif
        }
    }
    else {
        auto err = res.error();
    }
}

Response<std::string> AccountRepository::getBalance(const Account* acc)
{
    cli.set_bearer_token_auth(acc->_token.c_str());
    std::string url = "/cards-api/balance/" + std::to_string(acc->_card_id);
    if (auto res = cli.Delete(url.c_str())) {
#ifndef NDEBUG
        std::cout << "Request Successful" << std::endl;
#endif
        if (res->status == 200) {
            std::string body = res->body;
            json j = json::parse(body);
            std::string balance = j["balance"].get<std::string>();
            time_t date = std::time(0);
            Response<std::string> res{ 0, date, "Fetch Successful", "Balance" , balance };
            return res;
        }
        else if (res->status == 401) {
            // throw error
#ifndef NDEBUG
            std::cout << "Unauthorized" << std::endl;
            time_t date = std::time(0);
            Response<std::string> res{ 0, date, "Fetch Successful", "Balance" , "" };
            return res;
#endif
        } else {
            time_t date = std::time(0);
            Response<std::string> res{ 0, date, "Fetch Successful", "Balance" , "" };
            return res;
        }
    }
    else {
        auto err = res.error();
    }
}
