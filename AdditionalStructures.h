//
//  AdditionalStructures.h
//  MOOP_ATM
//
//  Created by Nazar Ivaniuk on 13.11.2020.
//

#ifndef AdditionalStructures_h
#define AdditionalStructures_h

#include <string>

struct Nothing {};

template <typename T = Nothing>
struct Response
{
    int exitCode;
    time_t date;
    std::string description;
    std::string balance;
    const T obj;
};

struct Account
{
    const int _id;
    const int _card_id;
    mutable std::string _token;
    static Account getDefault() { return Account{ -1, -1, "" }; }
};

struct Details
{
    const std::string cardnum;
    std::string PIN;
    std::string CCV;
    std::string name;
    std::string surname;
    std::string expDate;
};
struct Transaction
{
    std::string to;
    std::string date;
    std::string amount;
};

#endif /* AdditionalStructures_h */
