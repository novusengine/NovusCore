#include "DataStore.h"

SharedPool<DataStore> DataStore::_dataStores128;
SharedPool<DataStore> DataStore::_dataStores512;
SharedPool<DataStore> DataStore::_dataStores1024;
SharedPool<DataStore> DataStore::_dataStores4096;
SharedPool<DataStore> DataStore::_dataStores8192;