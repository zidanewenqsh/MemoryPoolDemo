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
// ... 可能还需要包含其他头文件 ...

class MemoryPool {
public:
    MemoryPool();
    MemoryPool(size_t small_size, int small_count);
    ~MemoryPool();

    void* allocate(size_t size);
    void deallocate(void* block);
    void stopPool();

private:
    struct LargeMemoryBlock {
        void* memory = nullptr;
        bool inUse = false;
        LargeMemoryBlock() = default;
    };

    struct LargePool {
        std::vector<LargeMemoryBlock> blocks;
        size_t blockSize;
        // 修改构造函数，只接受 blockSize 作为参数
        explicit LargePool(size_t size) : blockSize(size) {
            // 将一个空的 LargeMemoryBlock 对象添加到 blocks 中
//            blocks.emplace_back();
            std::cout << "LargePool::LargePool(size_t size) : blockSize(size) " << blocks.size() << std::endl;
        }
    };

//    std::map<size_t, LargePool> largePools;
    struct SmallMemoryBlock {
        void* memory;
        char* nextAvailable;
        size_t freeSize;
        size_t totalSize;
        SmallMemoryBlock() : memory(nullptr), nextAvailable(nullptr), freeSize(0), totalSize(0) {}
    };

    // ... 其他私有成员变量和方法的声明 ...

    // std::atomic<int> stop;
    std::atomic<bool> stop;
    std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
    size_t smallSize;
    size_t smallPoolSize;
    std::vector<LargePool> largePools;
    std::vector<size_t> largeBlockSizes;
    std::vector<SmallMemoryBlock> smallPools;

    void addSmallBlock(size_t size);
    void* allocateSmall(size_t size);
    void* allocateLarge(size_t size);
    // ... 其他私有成员函数的声明 ...
};


#endif //MEMORYPOOLDEMO_MEMORYPOOL_H
