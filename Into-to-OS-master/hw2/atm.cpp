#include "atm.h"

using namespace std;

atm::atm(int atm_num, bank* bank, ofstream& ofs, string file_name) : m_atm_num(atm_num), m_bank(bank), m_ofs(ofs), m_input_file_name(file_name) {
	try {
		m_input_file_stream.open(file_name.c_str(), ifstream::in);
	}
	catch (exception & exp) {
		throw;
	}
}

atm::~atm() {
	m_input_file_stream.close();
}

void atm::operate() {
	string line;
	string op, account, password, initial_amount, amount, target_account;
	while(getline(m_input_file_stream, line)){
		stringstream lineStream(line);
		lineStream >> op >> account >> password;
		int acc_num = stoi(account);
		int pass_num = stoi(password);
		if (op == "O") {
			lineStream >> initial_amount;
			int init_balance = stoi(initial_amount);
			creat_account(acc_num, pass_num, init_balance);
		}
		else if (op == "D") {
			lineStream >> amount;
			int amnt = stoi(amount);
			deposit(acc_num, pass_num, amnt);
		}
		else if (op == "W") {
			lineStream >> amount;
			int amnt = stoi(amount);
			withdraw(acc_num, pass_num, amnt);
		}
		else if (op == "B") {
			get_balance(acc_num, pass_num);
		}
		else if (op == "T") {
			lineStream >> target_account >> amount;
			int tag_acc_num = stoi(target_account);
			int amnt = stoi(amount);
			transfer(acc_num, pass_num, tag_acc_num, amnt);
		}
		else//shouldnt happen
			cout << "Error: invalid command: " << line << endl;
		usleep(100000);
	}
}

void atm::creat_account(int acc_num, int pass, int init_balance){
	int balance = 0;
	try {
		balance = m_bank->create_acc(acc_num, pass, init_balance);
		pthread_mutex_lock(&(m_bank->print_mutex));
		m_ofs << m_atm_num << ": New account id is " << acc_num << " with password " << pass << " and initial balance " << balance << endl;
		pthread_mutex_unlock(&(m_bank->print_mutex));
	}
	catch (string exp){
		print_error(exp);
	}
}

void atm::deposit(int acc_num, int pass, int dep_amount) {
	int balance = 0;
	try {
		balance = m_bank->deposit(acc_num, pass, dep_amount);
		pthread_mutex_lock(&(m_bank->print_mutex));
		m_ofs << m_atm_num << ": Account " << acc_num << " new balance is " << balance << " after " << dep_amount << " $ was deposited" << endl;
		pthread_mutex_unlock(&(m_bank->print_mutex));
	}
	catch (string exp){
		print_error(exp);
	}
}

void atm::withdraw(int acc_num, int pass, int with_amount) {
	int balance = 0;
	try {
		balance = m_bank->withdraw(acc_num, pass, with_amount);
		pthread_mutex_lock(&(m_bank->print_mutex));
		m_ofs << m_atm_num << ": Account " << acc_num << " new balance is " << balance << " after " << with_amount << " $ was withdrew" << endl;
		pthread_mutex_unlock(&(m_bank->print_mutex));
	} 
	catch (string exp){
		print_error(exp);
	}
}

void atm::get_balance(int acc_num, int pass){
	int balance = 0;
	try {
		balance = m_bank->get_balance(acc_num, pass);
		pthread_mutex_lock(&(m_bank->print_mutex));
		m_ofs << m_atm_num << ": Account " << acc_num << " balance is " << balance << endl;
		pthread_mutex_unlock(&(m_bank->print_mutex));
	}
	catch (string exp){
		print_error(exp);
	}
}

void atm::transfer(int acc_num, int pass, int target_acc_num, int amount) {
	int acc_balance = 0, target_balance = 0;
	try {
		m_bank->transfer(acc_num, pass, target_acc_num, amount, acc_balance, target_balance);
		pthread_mutex_lock(&(m_bank->print_mutex));
		m_ofs << m_atm_num << ": Transfer " << amount << " from " << acc_num << " to account " << target_acc_num << " new account balance is " << acc_balance << " new target account balance is " << target_balance << endl;
		pthread_mutex_unlock(&(m_bank->print_mutex));
	}
	catch (string exp) {
		print_error(exp);
	}
}

void atm::print_error(string exp){
	pthread_mutex_lock(&(m_bank->print_mutex));
	m_ofs << "Error " << m_atm_num << ": Your transaction failed - " << exp << endl;
	pthread_mutex_unlock(&(m_bank->print_mutex));
}
