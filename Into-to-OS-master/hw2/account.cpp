#include "account.h"

account::account(int num, int pass, int amount) {
	if (pass >= 10000) 
		throw "password for " + to_string(num) + " too long";
	m_num = num;
	m_balance = amount;
	m_pass = pass;
	m_count = 0;
	pthread_mutex_init(&m_write_mutex, NULL);
	pthread_mutex_init(&m_read_mutex, NULL);

	sleep(1);
};

account::~account() {
	pthread_mutex_destroy(&m_write_mutex);
	pthread_mutex_destroy(&m_read_mutex);
};

int account::deposit(int pass, int amount, bool wait) {
	if (pass != m_pass)
		throw string("password for account id " + to_string(m_num) + " is incorrect");
	pthread_mutex_lock(&m_write_mutex);
	if(wait)
		sleep(1);
	m_balance += amount;
	pthread_mutex_unlock(&m_write_mutex);
	return m_balance;
}

int account::withdraw(int pass, int amount, bool wait) {
	if (pass != m_pass) 
		throw string("password for account id " + to_string(m_num) + " is incorrect");
	pthread_mutex_lock(&m_write_mutex);
	//withdraw
	if (m_balance - amount < 0) {
		pthread_mutex_unlock(&m_write_mutex);
		throw string("account id " + to_string(m_num) + " balance is lower than " + to_string(amount));
	}
	m_balance -= amount;
	if (wait)
		sleep(1);
	pthread_mutex_unlock(&m_write_mutex);
	return m_balance;
}

int account::commission(int percent) {
	pthread_mutex_lock(&m_write_mutex);
	//commission
	int commission = m_balance * percent / 100;
	m_balance -= commission; 
	pthread_mutex_unlock(&m_write_mutex);
	return commission;
}
 
int account::get_balance(int pass, bool wait) {
	int balance = 0;
	if (pass != m_pass)
		throw string("password for account id " + to_string(m_num) + " is incorrect");

	start_reader_crit_section();
	balance = m_balance;
	if(wait)
		sleep(1);
	end_reader_crit_section();

	return balance;
}

int account::get_password() {
	return m_pass;
}

void account::start_reader_crit_section() {
	pthread_mutex_lock(&m_read_mutex);
	m_count++;
	if (m_count == 1)
		pthread_mutex_lock(&m_write_mutex);
	pthread_mutex_unlock(&m_read_mutex);
}

void account::end_reader_crit_section() {
	pthread_mutex_lock(&m_read_mutex);
	m_count--;
	if (m_count == 0)
		pthread_mutex_unlock(&m_write_mutex);	
	pthread_mutex_unlock(&m_read_mutex);
}
