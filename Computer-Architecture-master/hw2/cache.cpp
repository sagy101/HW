#include "cache.h"

set::set(uint32_t numOfWays) {
	_numOfWays = numOfWays;
	_tag = new uint32_t[_numOfWays];
	_valid = new uint32_t[_numOfWays];
	_dirty = new uint32_t[_numOfWays];
	_LRUarr = new uint32_t[_numOfWays];
	for (uint32_t i = 0; i < _numOfWays; i++) {
		_LRUarr[i] = numeric_limits<uint32_t>::infinity();
		_valid[i] = 0;
		_dirty[i] = 0;
		_tag[i] = 0;
	}
}

set::~set() {}

int set::update_set(uint32_t tag, bool isDirty) {
	int lruWay = exists(tag);
	if (lruWay == -1) { //tag doesn't exist
		lruWay = get_empty();
		if (lruWay == -1) {//shouldnt happen
			return -1;
		}
	}
	_dirty[lruWay] = isDirty;
	for (uint32_t j = 0; j < _numOfWays; j++) { // update everyones age
		_LRUarr[j]++;
	}
	_LRUarr[lruWay] = 0; //insert our new tat
	_tag[lruWay] = tag;
	_valid[lruWay] = 1;
	return lruWay;
}

int set::exists(uint32_t tag) {
	int found_id = -1;
	for (uint32_t i = 0; i < _numOfWays; i++) {
		if (_valid[i] == 1 && _tag[i] == tag) {
			found_id = i;
		}
	}
	return found_id;
}

bool set::evacuate_set(uint32_t tag) {
	int lruWay = exists(tag);
	if (lruWay == -1) {
		return false;
	}
	_tag[lruWay] = 0;
	_LRUarr[lruWay] = numeric_limits<uint32_t>::infinity();
	_valid[lruWay] = 0;
	bool dirty = _dirty[lruWay];
	_dirty[lruWay] = 0;
	return dirty;
}

bool set::get_dirty(uint32_t tag) {
	int lruWay = exists(tag);
	if (lruWay == -1) {
		return false;
	}
	return _dirty[lruWay];
}

uint32_t set::which_to_evacuate() {
	uint32_t lruWay = 0;
	uint32_t lru = 0;
	for (uint32_t way = 0; way < _numOfWays; way++) {
		if (_LRUarr[way] >= lru) {
			lru = _LRUarr[way];
			lruWay = way;
		}
	}
	return _tag[lruWay];
}

int set::get_empty() {
	for (uint32_t i = 0; i < _numOfWays; i++) {
		if (_valid[i] == 0) {
			return (int)i;
		}
	}
	return -1;
}

//cache functions
cache::cache(uint32_t cacheSize, uint32_t numOfWays, uint32_t numOfCyc, uint32_t blockSize, uint32_t writeAlloc, string name) :
	_cacheSize((uint32_t)pow(2, cacheSize)), _numOfWays((uint32_t)pow(2, numOfWays)), _numOfCyc(numOfCyc), _blockSize(blockSize),
	_writeAlloc(writeAlloc), _hitCount(0), _dad(NULL), _son(NULL), _missCount(0), _name(name) {
	_numOfSets = _cacheSize / (uint32_t)(_numOfWays * pow(2, blockSize));
	_pSetsArr = new set * [_numOfSets];
	for (uint32_t i = 0; i < _numOfSets; i++) {
		_pSetsArr[i] = new set(_numOfWays);
	}

}

cache::~cache() {
	for (uint32_t i = 0; i < _numOfSets; i++) {
		delete _pSetsArr[i];
	}
	delete[] _pSetsArr;
}

uint32_t cache::get_data(uint32_t address) {
	uint32_t timeOfDad = 0;
	uint32_t setId = address_to_set(address);
	if (address_exists(address)) {
		uint32_t tag = address_to_tag(address);
		update(address, _pSetsArr[setId]->get_dirty(tag));
		_hitCount++;
	}
	else {
		_missCount++;
		timeOfDad = _dad->get_data(address);
		if (((_pSetsArr[setId])->get_empty()) == -1) {//if full
			uint32_t tagToEvac = _pSetsArr[setId]->which_to_evacuate();
			uint32_t addressToEvac = ((tagToEvac << (_blockSize + (uint32_t)log2(_numOfSets))) + (setId << _blockSize)); //fake address
			evacuate_cache(addressToEvac); //evacuate in son and in current cache fake address
		}
		update(address, false);
	}
	return _numOfCyc + timeOfDad;
}

void cache::update(uint32_t address, bool isDirty) {
	uint32_t setId = address_to_set(address);
	uint32_t tag = address_to_tag(address);
	set* pCurSet = _pSetsArr[setId];
	pCurSet->update_set(tag, isDirty);
}

void cache::evacuate_cache(uint32_t address) {
	if (_son != NULL) {
		_son->evacuate_cache(address);
	}
	uint32_t setId = address_to_set(address);
	uint32_t tag = address_to_tag(address);
	if (_pSetsArr[setId]->evacuate_set(tag) == WAS_DIRTY) {
		_dad->update(address, true);
	}
}

bool cache::address_exists(uint32_t address) {
	uint32_t setId = address_to_set(address);
	uint32_t tag = address_to_tag(address);
	if ((_pSetsArr[setId])->exists(tag) < 0) {
		return false;
	}
	else {
		return true;
	}
}

uint32_t cache::address_to_tag(uint32_t address) {
	uint32_t shifted = (address >> (_blockSize + (uint32_t)log2(_numOfSets)));
	return shifted;
}

uint32_t cache::address_to_set(uint32_t address) {
	uint32_t shiftLeftCnt = ((uint32_t)32 - (_blockSize + (uint32_t)log2(_numOfSets)));
	uint32_t shiftedLeft;
	if (shiftLeftCnt == 32) {
		shiftedLeft = (address << 1) << 31;
	}
	else {
		shiftedLeft = (address << shiftLeftCnt);
	}
	uint32_t shiftedRight;
	uint32_t shiftRightCnt = ((uint32_t)32 - (uint32_t)log2(_numOfSets));
	if (shiftRightCnt == 32) {
		shiftedRight = (shiftedLeft >> 1) >> 31;
	}
	else {
		shiftedRight = (shiftedLeft >> shiftRightCnt);
	}
	return shiftedRight;
}

uint32_t cache::write_data(uint32_t address) {
	uint32_t dadCyc = 0;
	if (_writeAlloc) {
		if (address_exists(address)) {//hit
			_hitCount++;
		}
		else {//miss
			_missCount++;
			dadCyc = _dad->write_data(address);
			uint32_t setId = address_to_set(address);
			if (_pSetsArr[setId]->get_empty() == -1) { //the set is full
				uint32_t tagToEvac = _pSetsArr[setId]->which_to_evacuate();
				uint32_t addressToEvac = ((tagToEvac << (_blockSize + (uint32_t)log2(_numOfSets))) + (setId << _blockSize));
				evacuate_cache(addressToEvac);
			}
		}
		if (_son == NULL) {
			update(address, true);
		}
		else {
			update(address, false);
		}
		return dadCyc + _numOfCyc;
	}
	else {//no write allocate
		if (address_exists(address)) {//hit
			_hitCount++;
			update(address, true);
			return _numOfCyc;
		}
		else {//miss
			_missCount++;
			uint32_t dadCyc = _dad->write_data(address);
			return dadCyc + _numOfCyc;
		}
	}
}

double cache::get_miss_rate() const {
	return (double)_missCount / ((double)_missCount + (double)_hitCount);
}

//simulator functions

simulator::simulator(uint32_t cacheSize1, uint32_t numOfWays1, uint32_t numOfCyc1, //L1 config
	uint32_t cacheSize2, int32_t numOfWays2, uint32_t numOfCyc2, //L2 config
	uint32_t blockSize, uint32_t writeAlloc, uint32_t numOfMemCyc) : _totNumOfCyc(0), _numOfReq(0) { // General Config
	_pMEM = new memory((uint32_t)0, (uint32_t)0, numOfMemCyc, (uint32_t)0, (uint32_t)0, "MEM");
	_pL2 = new cache(cacheSize2, numOfWays2, numOfCyc2, blockSize, writeAlloc, "L2");
	_pL1 = new cache(cacheSize1, numOfWays1, numOfCyc1, blockSize, writeAlloc, "L1");
	_pMEM->_son = _pL2;
	_pL2->_dad = _pMEM;
	_pL2->_son = _pL1;
	_pL1->_dad = _pL2;
}

simulator::~simulator() {
	delete _pL1;
	delete _pL2;
	delete _pMEM;
}

void simulator::get_data(uint32_t address) {
	_numOfReq++;
	_totNumOfCyc += _pL1->get_data(address);
}

void simulator::write_data(uint32_t address) {
	_numOfReq++;
	_totNumOfCyc += _pL1->write_data(address);
}

void simulator::get_stat(double* L1MissRate, double* L2MissRate, double* avgAccTime) {
	*avgAccTime = ((double)(_totNumOfCyc) / (double)(_numOfReq));
	*L1MissRate = _pL1->get_miss_rate();
	*L2MissRate = _pL2->get_miss_rate();
}
