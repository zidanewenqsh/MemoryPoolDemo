//
// Created by wen on 2023-12-29.
//

#ifndef MEMORYPOOLDEMO_MEMORYPOOL_H
#define MEMORYPOOLDEMO_MEMORYPOOL_H

#include <cstddef>
#include <iostream>
#include <vector>
#include <mutex>
#include <memory>
#include <chrono>
#include <atomic>
#include <map>
#include "Logger.h"
// ... 可能还需要包含其他头文件 ...

class MemoryPool : public Logger {
public:
    MemoryPool();
    MemoryPool(size_t small_size, int small_count);
    ~MemoryPool();

    void* allocate(size_t size);
    void deallocate(void* block);
    void deallocate(void* block, size_t size);
    void stopPool();
//    void SetLogLevel(LogLevel level);
private:
//    Logger logger;



    struct LargeMemoryBlock {
        void* memory = nullptr;
        bool inUse = false;
        LargeMemoryBlock() = default;
    };

    struct LargePool {
        std::vector<LargeMemoryBlock> blocks;
        size_t blockSize;
        // 修改构造函数，只接受 blockSize 作为参数
//        explicit LargePool(size_t size) : blockSize(size) {
        LargePool() : blockSize(4096) {
            std::cout << "LargePool::LargePool(size_t size) : blockSize(size) default: " << 4096 << std::endl;
        }
        explicit LargePool(size_t size) : blockSize(size) {
            // 将一个空的 LargeMemoryBlock 对象添加到 blocks 中
//            blocks.emplace_back();
            std::cout << "LargePool::LargePool(size_t size) : blockSize(size) " << size << std::endl;
        }
    };

    struct SmallMemoryBlock {
        void* memory;
        char* nextAvailable;
        size_t freeSize;
        size_t totalSize;
        int usedCount;
        SmallMemoryBlock() : memory(nullptr), nextAvailable(nullptr), freeSize(0), totalSize(0), usedCount(0) {}
    };

    // ... 其他私有成员变量和方法的声明 ...

    // std::atomic<int> stop;
    std::atomic<bool> stop;
    std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
    size_t smallSize;
    size_t smallPoolSize;
#if 0
    std::vector<LargePool> largePools;
#else
    std::map<size_t, LargePool> largePools;
#endif
    std::vector<size_t> largeBlockSizes;
    std::vector<SmallMemoryBlock> smallPools;

    void addSmallBlock(size_t size);
    void* allocateSmall(size_t size);
    void* allocateLarge(size_t size);
    // ... 其他私有成员函数的声明 ...
};

//#define LOG_DEBUG(message) Log(logger, LogLevel::DEBUG, message)
//#define LOG_INFO(message) Log(logger, LogLevel::INFO, message)
//#define LOG_WARNING(message) Log(logger, LogLevel::WARNING, message)
//#define LOG_ERROR(message) Log(logger, LogLevel::ERROR, message)
//#define LOG_FATAL(message) Log(logger, LogLevel::FATAL, message)
//#define LOGDEBUG(format, ...) Log(LogLevel::DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
//#define LOGINFO(format, ...) Log(LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
//#define LOGWARNING(format, ...) Log(LogLevel::WARNING, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
//#define LOGERROR(format, ...) Log(LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
//#define LOGFATAL(format, ...) Log(LogLevel::FATAL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#endif //MEMORYPOOLDEMO_MEMORYPOOL_H
