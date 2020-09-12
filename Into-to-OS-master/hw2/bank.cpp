#include "bank.h"

bank::bank(ofstream& ofs): m_bank_balance(0), m_ofs(ofs), m_read_acc_count(0) {
	pthread_mutex_init(&m_write_acc_mutex, NULL);
	pthread_mutex_init(&m_read_acc_mutex, NULL);
	pthread_mutex_init(&m_write_bank_balance_mutex, NULL);
	pthread_mutex_init(&m_read_bank_balance_mutex, NULL);
	pthread_mutex_init(&print_mutex, NULL);
	isFinished = false;
}

bank::~bank() {
	pthread_mutex_destroy(&m_write_acc_mutex);
	pthread_mutex_destroy(&m_read_acc_mutex);
	pthread_mutex_destroy(&m_write_bank_balance_mutex);
	pthread_mutex_destroy(&m_read_bank_balance_mutex);
	pthread_mutex_destroy(&print_mutex);
	map<int, account*>::iterator curAccount = m_num_account_map.begin();
	while (curAccount != m_num_account_map.end()) {
		delete curAccount->second;
		curAccount++;
	}
 }


int bank::deposit(int acc_num, int acc_pass, int dep_amount) {
	try {
		start_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		account_Exists(acc_num);
		int new_balance = m_num_account_map[acc_num]->deposit(acc_pass, dep_amount, true);
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		return new_balance;
	}
	catch (string exp) {
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		throw;
	}
}

int bank::withdraw(int acc_num, int acc_pass, int with_amount) {
	try{
		start_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		account_Exists(acc_num);
		int new_balance = m_num_account_map[acc_num]->withdraw(acc_pass, with_amount, true);
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		return new_balance;
	}
	catch (string exp) {
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		throw;
	}
}

int bank::create_acc(int acc_num, int acc_pass, int acc_balance) {
	pthread_mutex_lock(&m_write_acc_mutex);
	if (m_num_account_map.find(acc_num) != m_num_account_map.end()) {
		pthread_mutex_unlock(&m_write_acc_mutex);
		throw string("account with the same id exists");
	}
	try {
		m_num_account_map[acc_num] = new account(acc_num, acc_pass, acc_balance);
		pthread_mutex_unlock(&m_write_acc_mutex);
	}
	catch (string exp) {
		pthread_mutex_unlock(&m_write_acc_mutex);
		throw;
	}
	return acc_balance;
}

int bank::get_balance(int acc_num, int acc_pass) {
	try{
		start_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		account_Exists(acc_num);
		int balance = m_num_account_map[acc_num]->get_balance(acc_pass, true);
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		return balance;
	}
	catch (string exp) {
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		throw;
	}
}

void bank::transfer(int acc_num, int pass, int target_acc_num, int amount, int &acc_balance, int &target_balance) {
	try {
		start_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		account_Exists(acc_num);
		account_Exists(target_acc_num);
		acc_balance = m_num_account_map[acc_num]->withdraw(pass, amount, true);
	}
	catch (string exp) {
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		throw;
	}
	try {
		target_balance = m_num_account_map[target_acc_num]->deposit(m_num_account_map[target_acc_num]->get_password(), amount, false);

	}
	catch (string exp) {
		m_num_account_map[acc_num]->deposit(pass, amount, false);
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		throw;
	}
	end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
}

void bank::print_all() {
	stringstream newOut;
	cout << "\033[2J\033[1;1H";
	while (!isFinished) {
		//critical section - needs to wait for all pending balances to update
		start_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		newOut << "\033[2J\033[1;1H";
		newOut << "Current Bank Status" << endl;
		map<int, account*>::iterator curAccount = m_num_account_map.begin();
		int curPass;
		string curPassStr;
		while (curAccount != m_num_account_map.end()) {
			curPass = curAccount->second->get_password();
			newOut << "Account " << curAccount->first << ": Balance - " << curAccount->second->get_balance(curPass, false) << " $ , Account Password - " << passToString(curPass) << endl;
			curAccount++;
		}
		cout << newOut.str();
		start_reader_crit_section(m_write_bank_balance_mutex, m_read_bank_balance_mutex);
		cout << "The Bank has " << m_bank_balance << " $" << endl;
		end_reader_crit_section(m_write_bank_balance_mutex, m_read_bank_balance_mutex);
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		usleep(500000);
	}
}

string bank::passToString(int pass) const {
	string strPass = "000" + to_string(pass);
	int len = (int)strPass.size();
	return strPass.substr(len-4, len);

}

void bank::account_Exists(int acc_num) {
	if (m_num_account_map.find(acc_num) == m_num_account_map.end()) 
		throw string("account id " + to_string(acc_num) + " does not exist");
}

void bank::commission() {
	while (!isFinished) {
		start_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		map<int, account*>::iterator cur_acc = m_num_account_map.begin();
		int percent = rand() % 3 + 2;
		//int percent = 0;//only for test!!!!!!!!!
		int commission = 0;
		while (cur_acc != m_num_account_map.end()) {
			commission = cur_acc->second->commission(percent);
			pthread_mutex_lock(&m_write_bank_balance_mutex);
			m_bank_balance += commission;
			pthread_mutex_unlock(&m_write_bank_balance_mutex);
			pthread_mutex_lock(&print_mutex);
			m_ofs << "Bank: commission of " << percent << " % were charged, the bank gained " << commission << " $ from account " << cur_acc->first << endl;
			pthread_mutex_unlock(&print_mutex);
			cur_acc++;
		}
		end_reader_crit_section(m_write_acc_mutex, m_read_acc_mutex);
		sleep(3);
	}
}

void bank::start_reader_crit_section(pthread_mutex_t& write_mutex, pthread_mutex_t& read_mutex) {
	pthread_mutex_lock(&read_mutex);
	m_read_acc_count++;
	if (m_read_acc_count == 1) 
		pthread_mutex_lock(&write_mutex);
	pthread_mutex_unlock(&read_mutex);
}

void bank::end_reader_crit_section(pthread_mutex_t& write_mutex, pthread_mutex_t& read_mutex) {
	pthread_mutex_lock(&read_mutex);
	m_read_acc_count--;
	if (m_read_acc_count == 0) 
		pthread_mutex_unlock(&write_mutex);
	pthread_mutex_unlock(&read_mutex);
} 