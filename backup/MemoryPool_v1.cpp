//
// Created by wen on 2023-12-29.
//

#include "../MemoryPool.h"
MemoryPool::MemoryPool():stop(false) {
    smallSize = 0x100;
    smallPoolSize = smallSize * 0x10;
    addSmallBlock(smallPoolSize);
    int i = 1, small_size = smallSize;
    while (small_size >>=1) i++;
    // printf("i=%d\n", i);
    size_t blockSize = 1 << i;
    largeBlockSizes.push_back(blockSize);
//    largePools.push_back({{}, blockSize}); // 为每个大小创建一个空内存池
    largePools.emplace_back(blockSize);
}
MemoryPool::MemoryPool(size_t small_size, int small_count)
        : stop(false), smallSize(small_size), smallPoolSize(smallSize * small_count) {
    // ... 构造函数的实现 ...
    // 初始化小块内存
    addSmallBlock(smallPoolSize);

    // 定义大块内存的潜在大小
//    int i = 1;
//    while (small_size >>=1) i++;
    // printf("i=%d\n", i);
//    size_t blockSize = 1 << i;
    size_t blockSize = 0x1000;
    largeBlockSizes.push_back(blockSize);
//    largePools.push_back({{}, blockSize}); // 为每个大小创建一个空内存池
//    largePools.emplace_back(LargePool{{}, blockSize});
    largePools.emplace_back(blockSize);

}

MemoryPool::~MemoryPool() {
    // ... 析构函数的实现 ...
    // 释放所有小块内存的大块
    for (auto& block : smallPools) {
//        checkRuntime(cudaFreeHost(block.memory));
        if (block.memory != nullptr) {
//            delete[] block.memory;
            delete[] reinterpret_cast<char*>(block.memory);
        }
    }
    // 释放所有大块内存
    for (auto& pool : largePools) {
        for (auto& block : pool.blocks) {
            if (block.memory != nullptr) {
//                checkRuntime(cudaFreeHost(block.memory));
                if (block.memory != nullptr) {
//                    delete[] block.memory;
                    delete[] reinterpret_cast<char*>(block.memory);
                }
            }
        }
    }
}

void* MemoryPool::allocate(size_t size) {
    // ... allocate 的实现 ...
    if (stop.load()) {
        return nullptr; // 如果内存池已停止，直接返回
    }
    if (size <= smallSize) {
        // 在小块内存中分配
        return allocateSmall(size);
    } else {
        // 在大块内存中分配
        return allocateLarge(size);
    }
}

void MemoryPool::deallocate(void* block) {
    // ... deallocate 的实现 ...
    if (stop.load()) {
        return ; // 如果内存池已停止，直接返回
    }
    while (spinlock.test_and_set(std::memory_order_acquire)); // 获取锁
    // 检查是否为大块内存
    for (auto& pool : largePools) {
        for (auto& blockInPool : pool.blocks) {
            if (blockInPool.memory == block) {
                blockInPool.inUse = false; // 标记为未使用
                std::cout << "deallocate " << block << std::endl;
                spinlock.clear(std::memory_order_release); // 释放锁
                return;
            }
        }
    }
    spinlock.clear(std::memory_order_release); // 释放锁
}

void MemoryPool::stopPool() {
    // ... stopPool 的实现 ...
    // stop.store(1); // 设置停止标志
    stop.store(true);
}

void MemoryPool::addSmallBlock(size_t size) {
    SmallMemoryBlock newBlock;
    newBlock.totalSize = size;
    newBlock.freeSize = size;
    newBlock.memory = new char[size]; // 使用 new 分配内存
    newBlock.nextAvailable = reinterpret_cast<char*>(newBlock.memory);
    smallPools.push_back(newBlock);
}
void* MemoryPool::allocateSmall(size_t size) {
    std::cout << "allocateSmall " << size << std::endl;
    // ... allocateSmall 的实现 ...
    while (spinlock.test_and_set(std::memory_order_acquire)); // 获取锁

    // 尝试在现有的小块内存中分配
    for (auto& block : smallPools) {
        if (block.freeSize >= size) {
            void* allocatedMemory = block.nextAvailable;
            block.nextAvailable += size;
            block.freeSize -= size;
            spinlock.clear(std::memory_order_release); // 释放锁
            return allocatedMemory;
        }
    }

    // 所有现有的小块内存都已满，分配一个新的大块
    addSmallBlock(smallPools[0].totalSize);
    SmallMemoryBlock& newBlock = smallPools.back(); // 获取刚刚添加的新块

    // 直接在新块中分配内存
    void* allocatedMemory = newBlock.nextAvailable;
    newBlock.nextAvailable += size;
    newBlock.freeSize -= size;

    spinlock.clear(std::memory_order_release); // 释放锁
    return allocatedMemory;
}

void* MemoryPool::allocateLarge(size_t size) {
    std::cout << "allocateLarge " << size << std::endl;
    // ... allocateLarge 的实现 ...
    while (spinlock.test_and_set(std::memory_order_acquire)); // 获取锁
    // 如果预设的largeBlockSize不满足要求，扩容
    while (size > largeBlockSizes.back()) {
        largeBlockSizes.push_back(largeBlockSizes.back() + 0x1000);
        largePools.emplace_back(largeBlockSizes.back());
//        largePools.push_back({{}, largeBlockSizes.back()}); // 为每个大小创建一个空内存池
    }
    for (auto& pool : largePools) {
        if (pool.blockSize >= size) {
            for (auto& block : pool.blocks) {
                if (!block.inUse) {
                    block.inUse = true;
                    spinlock.clear(std::memory_order_release); // 释放锁
                    return block.memory;
                }
            }

            LargeMemoryBlock newBlock;
//            checkRuntime(cudaMallocHost(&newBlock.memory, pool.blockSize));
            char *newmemory = new char[pool.blockSize]; // 使用 new 分配内存
//            if (newmemory == nullptr) {
//                std::cout << "allocateLarge failed" << std::endl;
//                exit(1);
//            }
            newBlock.memory = newmemory; // 使用 new 分配内存

            newBlock.inUse = true;
            pool.blocks.push_back(newBlock);
            spinlock.clear(std::memory_order_release); // 释放锁
            return newBlock.memory;
        }
    }
    spinlock.clear(std::memory_order_release); // 释放锁
    throw std::bad_alloc();
}