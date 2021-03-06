#ifndef BANK_H_
#define BANK_H_
#include "account.h"
#include <vector>
#include <map>
#include <fstream> 
#include <stdlib.h>
#include <bits/stdc++.h> 
#include <sstream>
 

class bank
{
public:
	//constructor
	bank(ofstream& ofs);

	//deconstruct
	~bank();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: deposit into given account
	//parameters: acc_num- account number, acc_pass- account password, dep_amount - desposit amount
	//return: on success - new account balance after deposit, on failure - throw's an error massage (string)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	int deposit(int acc_num, int acc_pass, int dep_amount);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: withdraw from given account
	//parameters: acc_num- account number, acc_pass- account password, dep_amount - withdraw amount
	//return: on success - new account balance after deposit, on failure - throw's an error massage (string)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	int withdraw(int acc_num, int acc_pass, int with_amount);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: opens new bank account with given info
	//parameters: acc_num- account number, acc_pass- account password, acc_balance - account balance
	//return: on success - new account balance, on failure - throw's an error massage (string)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	int create_acc(int acc_num, int acc_pass, int acc_balance);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: gets given account balance
	//parameters: acc_num- account number, acc_pass- account password
	//return: on success - given account balance, on failure - throw's an error massage (string)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	int get_balance(int acc_num, int acc_pass);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: transfers given amount from given account to given target account
	//parameters: acc_num- account number, acc_pass- account password, target_acc_num - target account number, amount - amount to be transferred,
	//			  acc_balance - new account balance after transfer, target_balance - new target account balance after transfer
	//return: on success - void, on failure - throw's an error massage (string)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void transfer(int acc_num, int pass, int target_acc_num, int amount, int &acc_balance, int &target_balance);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: take random commission from all accounts, prints how much the bank took from each account
	//parameters: none
	//return: none
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void commission();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: prints all the accounts, sorted ny account number
	//parameters: none
	//return: none
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void print_all();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: commission thread utility function
	//parameters: pBank
	//return: none
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void* commission_helper(void* pBank) {
		((bank*)pBank)->commission();
		pthread_exit(NULL);
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: print_all thread utility function
	//parameters: pBank
	//return: none
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void* print_all_helper(void* pBank) {
		((bank*)pBank)->print_all();
		pthread_exit(NULL);
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: finish all bank functions
	//parameters: none
	//return: none
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void finishBank() {
		isFinished = true;
	}

	pthread_mutex_t print_mutex;//print to log file mutex for bank and atm's (hence public)

protected:
	map <int, account*> m_num_account_map;//map of accounts: first - account number, second - account type
	int m_bank_balance;

	ofstream& m_ofs;//output log file

	//write/read mechanism for account map
	pthread_mutex_t m_write_acc_mutex;
	pthread_mutex_t m_read_acc_mutex;
	int m_read_acc_count;

	//write/read mechanism for bank balance
	pthread_mutex_t m_write_bank_balance_mutex;
	pthread_mutex_t m_read_bank_balance_mutex;
	int m_read_bank_balance_count;

	bool isFinished;

	//internal use member functions

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: finds if account exists or not 
	//parameters: acc_num - account number
	//return: throw an error massage, if account does not exist
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void account_Exists(int acc_num);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: transforms password from int to string
	//parameters: pass - password to be transformed to string
	//return: password as string
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	string passToString(int pass) const;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: mutex reader mechanism start - for given mutex's
	//parameters: none
	//return: none
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void start_reader_crit_section(pthread_mutex_t& write_acc_mutex, pthread_mutex_t& read_acc_mutex);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//description: mutex reader mechanism end - for given mutex's
	//parameters: none
	//return: none
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void end_reader_crit_section(pthread_mutex_t& write_acc_mutex, pthread_mutex_t& read_acc_mutex);
};

#endif