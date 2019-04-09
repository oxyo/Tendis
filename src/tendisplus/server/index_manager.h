// Copyright [2017] <eliotwang, deyukong>
#ifndef SRC_TENDISPLUS_REPLICATION_INDEX_MANAGER_H_
#define SRC_TENDISPLUS_REPLICATION_INDEX_MANAGER_H_

#include <unordered_map>

#include "tendisplus/server/server_entry.h"
#include "tendisplus/network/worker_pool.h"

namespace tendisplus {

using JobStatus = std::unordered_map<std::size_t, std::atomic<bool>>;
using JobCnt = std::unordered_map<std::size_t, std::atomic<uint32_t>>;

class IndexManager {
 public:
    IndexManager(std::shared_ptr<ServerEntry> svr,
                 std::shared_ptr<ServerParams> cfg);
    Status startup();
    void stop();
    Status run();
    Status scanExpiredKeysJob(uint32_t storeId);
    int tryDelExpiredKeysJob(uint32_t storeId);
    bool isRunning();
    Status stopStore(uint32_t storeId);

 private:
    std::unique_ptr<WorkerPool> _indexScanner;
    std::unique_ptr<WorkerPool> _keyDeleter;
    std::unordered_map<std::size_t, std::list<TTLIndex>> _expiredKeys;
    std::unordered_map<std::size_t, std::string> _scanPoints;
    JobStatus _scanJobStatus;
    JobStatus _delJobStatus;
    // when destroystore, _disableStatus[storeId] = true
    JobStatus _disableStatus;
    JobCnt _scanJobCnt;
    JobCnt _delJobCnt;

    std::atomic<bool>  _isRunning;
    std::shared_ptr<ServerEntry> _svr;
    std::thread _runner;
    std::mutex _mutex;

    std::shared_ptr<PoolMatrix> _scannerMatrix;
    std::shared_ptr<PoolMatrix> _deleterMatrix;

    uint64_t _totalDequeue;
    uint64_t _totalEnqueue;

    uint32_t _scanBatch;
    uint32_t _scanPoolSize;
    uint32_t _delBatch;
    uint32_t _delPoolSize;
    uint32_t _pauseTime;
};

}  // namespace tendisplus

#endif
