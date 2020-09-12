#include "bank.h"
#include "atm.h"
#include <pthread.h>
#include <malloc.h>


#define BANK_THREADS 2

bool init_atms(int num_of_atms, atm** atm_arr, char** input_files, bank* pBank, ofstream& ofs);
void deconstuct(int num_of_atms, atm** atm_arr, bank* pBank);

int main(int argc, char* argv[])
{
	int num_of_thread = atoi(argv[1]) + BANK_THREADS;
	int num_of_atms = atoi(argv[1]);
	if (num_of_atms != argc - 2) {
		cout << "illegal arguments" << endl;
		return 1;
	}
	pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * num_of_thread);

	//create log file
	ofstream log;
	log.open("log.txt", ofstream::out);
	if (!log) {//shouldnt happen
		cout << "Error: could not create log file" << endl;
	}

	//create bank;
	bank* pBank = new bank(log);

	//create bank threads
	pthread_create(&threads[0], NULL, &bank::commission_helper, pBank);
	pthread_create(&threads[1], NULL, &bank::print_all_helper, pBank);

	//create atm's
	
	atm** atm_arr = new atm* [num_of_atms];
	if (!init_atms(num_of_atms, atm_arr, argv, pBank, log)) {
		delete[] atm_arr;
		delete pBank;
		free(threads);
		pthread_mutex_destroy(&(pBank->print_mutex));
		return 1;
	}

	//create atm threads
	for (int i = 0; i < num_of_atms; i++) 
		pthread_create(&threads[i + BANK_THREADS], NULL, &atm::operate_helper, atm_arr[i]);

	
	for (int i = BANK_THREADS; i < num_of_thread; i++)
		pthread_join(threads[i], NULL);

	//close proccess routine
	pBank->finishBank();
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	
	log.close();
	free(threads);
	deconstuct(num_of_atms, atm_arr, pBank);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//description: initializes atms
//parameters: num_of_atms- number of atms, atm_arr - array of pointers to atms, input_files - array of input file names, pBank - pointer to bank, ofs - output stream
//return: on success - true, on failure - false
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool init_atms(int num_of_atms, atm** atm_arr, char** input_files, bank* pBank, ofstream& ofs) {
	int i = 0;
	for (; i < num_of_atms; i++) {
		try {
			string input_file_name(input_files[i + 2]);
			atm_arr[i] = new atm(i+1, pBank, ofs, input_file_name);
		}
		catch (exception& exp) {//if atm creation failed - delete all previous created atm's
			cout << "illegal arguments" << endl;
			i--;
			for (; i >= 0; i--) {
				delete atm_arr[i];
			}
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//description: destroys atms and bank
//parameters: num_of_atms- number of atms, atm_arr - array of pointers to atms, pBank - bank to be deleted
//return: void
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void deconstuct(int num_of_atms, atm** atm_arr, bank* pBank) {
	if (pBank != NULL) delete pBank;
	if (atm_arr != NULL) {
		for (int i = 0; i < num_of_atms; i++)
			delete atm_arr[i];
		delete[] atm_arr;
	}
}