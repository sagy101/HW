#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <limits>

using namespace std;
#define WAS_DIRTY 1
#define WAS_CLEAN 0

class set {
public:
	////////////////////////////////////////////////////////////////////////
	// description: constructor
	// parameters: _numOfWays - number of ways in set
	// return: -
	////////////////////////////////////////////////////////////////////////
	set(uint32_t _numOfWays);

	////////////////////////////////////////////////////////////////////////
	// description: destructor
	// parameters: -
	// return: -
	////////////////////////////////////////////////////////////////////////
	~set();

	////////////////////////////////////////////////////////////////////////
	// description: update set data - lru, tag and valid 
	// parameters: tag - tag to be updated, isDirty - status to set dirty to
	// return: - return way of given tag in set or -1 if it doesnt exist.
	////////////////////////////////////////////////////////////////////////
	int update_set(uint32_t tag, bool isDirty);

	////////////////////////////////////////////////////////////////////////
	// description: checks if given tag exists in set
	// parameters: tag - tag to be serached for
	// return: - return way of given tag in set or -1 if it doesnt exist.
	////////////////////////////////////////////////////////////////////////
	int exists(uint32_t tag);

	////////////////////////////////////////////////////////////////////////
	// description: evacuates given tag from set
	// parameters: tag - tag to be evacuated for
	// return: - return dirty status of evacuated set
	////////////////////////////////////////////////////////////////////////
	bool evacuate_set(uint32_t tag);

	////////////////////////////////////////////////////////////////////////
	// description: find an empty spot in set
	// parameters: -
	// return: - return way in set of empty space
	////////////////////////////////////////////////////////////////////////
	int get_empty();

	////////////////////////////////////////////////////////////////////////
	// description: find which way set to evacuate eccording to lru
	// parameters: -
	// return: - return way in set to evacuate
	////////////////////////////////////////////////////////////////////////
	uint32_t which_to_evacuate();

	////////////////////////////////////////////////////////////////////////
	// description: get dirty state of given tag in set
	// parameters: tag - tag to get dirty state of
	// return: return dirty state of tag in set
	////////////////////////////////////////////////////////////////////////
	bool get_dirty(uint32_t tag);

	//fields
	uint32_t* _tag;//array of tags in each way in set
	uint32_t* _valid;//valid status of each way in set
	uint32_t* _dirty;//dirty status of each way in set
	uint32_t* _LRUarr;//lru status of each way in set
	uint32_t _numOfWays;//number of ways in set
};


class cache {
public: // methods
	////////////////////////////////////////////////////////////////////////
	// description: constructor
	// parameters: -
	// return: -
	////////////////////////////////////////////////////////////////////////
	cache(uint32_t cacheSize, uint32_t numOfWays, uint32_t numOfCyc, uint32_t blockSize, uint32_t writeAlloc, string name);
	
	////////////////////////////////////////////////////////////////////////
	// description: destructor
	// parameters: -
	// return: -
	////////////////////////////////////////////////////////////////////////
	virtual ~cache();

	////////////////////////////////////////////////////////////////////////
	// description: read/get data from cache
	// parameters: address - address of data to be read
	// return: time number of cycles it took to get data
	////////////////////////////////////////////////////////////////////////
	virtual uint32_t get_data(uint32_t address);

	////////////////////////////////////////////////////////////////////////
	// description: updates given address in cache with given dirty status (tag, valid, dirty and lru)
	// parameters: address - address of data to be updated, isDirty - dirty status to be set
	// return: -
	////////////////////////////////////////////////////////////////////////
	void update(uint32_t address, bool isDirty);

	////////////////////////////////////////////////////////////////////////
	// description: evacuates data with given address from cache
	// parameters: address - address of data to be updated
	// return: -
	////////////////////////////////////////////////////////////////////////
	void evacuate_cache(uint32_t address);

	////////////////////////////////////////////////////////////////////////
	// description: writes the data into the cache, according to the policy set to it.
	// parameters: address - address of data to be written
	// return: the number of cycles it took to write to this cache (and all the underlying memories)
	////////////////////////////////////////////////////////////////////////
	virtual uint32_t write_data(uint32_t address);

	////////////////////////////////////////////////////////////////////////
	// description: gets the miss rate of the cache.
	// parameters: none
	// return: miss rate.
	////////////////////////////////////////////////////////////////////////
	double get_miss_rate() const;

	//internal use
	uint32_t address_to_tag(uint32_t address);//transforms address to tag
	uint32_t address_to_set(uint32_t address); // transforms the adress to a setId according to the caches parameters.
	bool address_exists(uint32_t address);//returns true if address exists in cache

	//fields
	uint32_t _cacheSize;
	uint32_t _numOfWays;
	uint32_t _numOfCyc;
	uint32_t _blockSize;
	uint32_t _writeAlloc;
	uint32_t _hitCount;//number of hits in chache
	uint32_t _missCount;//number of misses in chache
	set** _pSetsArr;//array of sets
	uint32_t _numOfSets;
	cache* _son;//upper chache (if this is L2 then son is L1)
	cache* _dad;//lower chache (if this is L2 then dad is MEM)
	string _name;//name of cache (ie L1/L2/MEM)

};

class memory : public cache {
public:

	////////////////////////////////////////////////////////////////////////
	// description: constructor
	// parameters: all parameters needed.
	// return: none
	////////////////////////////////////////////////////////////////////////
	memory(uint32_t cacheSize, uint32_t numOfWays, uint32_t numOfCyc, uint32_t blockSize, uint32_t writeAlloc, string name)
		: cache(cacheSize, numOfWays, numOfCyc, blockSize, writeAlloc, name) {};
	////////////////////////////////////////////////////////////////////////
	// description: destructor
	// parameters: none
	// return: none
	////////////////////////////////////////////////////////////////////////
	~memory() {};
	////////////////////////////////////////////////////////////////////////
	// description: this is the memory so no real reading, just returning the number of cycles it takes for the mem.
	// parameters: address (compatability with overloading of other caches)
	// return: number of cycles of the memory access.
	////////////////////////////////////////////////////////////////////////
	uint32_t get_data(uint32_t address) {
		return _numOfCyc;
	}
	////////////////////////////////////////////////////////////////////////
	// description: this is the memory so no real writing, just returning the number of cycles it takes for the mem.
	// parameters: address (compatability with overloading of other caches)
	// return: number of cycles of the memory access.
	////////////////////////////////////////////////////////////////////////
	uint32_t write_data(uint32_t address) {
		return _numOfCyc;
	}
};

class simulator {
public:
	////////////////////////////////////////////////////////////////////////
	// description: constructor ,initializes all the inner caches according to the parameters.
	// parameters: parameters for each cache and general parameters.
	// return: none
	////////////////////////////////////////////////////////////////////////
	simulator(uint32_t cacheSize1, uint32_t numOfWays1, uint32_t numOfCyc1, //L1 config
		uint32_t cacheSize2, int32_t numOfWays2, uint32_t numOfCyc2, //L2 config
		uint32_t blockSize, uint32_t writeAlloc, uint32_t numOfMemCyc);
	////////////////////////////////////////////////////////////////////////
	// description: destructor
	// parameters: none
	// return: none
	////////////////////////////////////////////////////////////////////////
	~simulator();
	////////////////////////////////////////////////////////////////////////
	// description: tries to get data from the caches.
	// parameters: address - 32 bit address we wish to get the data for.
	// return: none.
	////////////////////////////////////////////////////////////////////////
	void get_data(uint32_t address);
	////////////////////////////////////////////////////////////////////////
	// description: write the data into the caches
	// parameters: address
	// return: none.
	////////////////////////////////////////////////////////////////////////
	void write_data(uint32_t address);


	////////////////////////////////////////////////////////////////////////
	// description: gets the miss ratio for the L1 and L2 caches, aswell as the average access time.
	// parameters: pointers to the variables in which we store the stats
	// return: none
	////////////////////////////////////////////////////////////////////////
	void get_stat(double* L1MissRate, double* L2MissRate, double* avgAccTime);

	//fields
	cache* _pMEM;
	cache* _pL2;
	cache* _pL1;
	uint32_t _totNumOfCyc;
	uint32_t _numOfReq;
};

#endif