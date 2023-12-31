//
// Created by wen on 2023-12-29.
//

#include "MemoryPool_v2.h"
#include "Logger.h"
//static Logger logger;
void MemoryPool::SetLogLevel(LogLevel level) {
    logger.SetLevel(level);
}
//logger.SetLevel(LogLevel::DEBUG);
//Logger.setLevel(LogLevel::ERROR);
//logger.SetLevel(LogLevel::DEBUG);
MemoryPool::MemoryPool():stop(false) {
    smallSize = 0x100;
    smallPoolSize = smallSize * 0x10;
    addSmallBlock(smallPoolSize);
    int i = 1, small_size = smallSize;
    while (small_size >>=1) i++;
    // printf("i=%d\n", i);
#if 0
    size_t blockSize = 1 << i;
    largeBlockSizes.push_back(blockSize);
//    largePools.push_back({{}, blockSize}); // 为每个大小创建一个空内存池
    largePools.emplace_back(blockSize);
#endif
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
#if 0
    size_t blockSize = 0x1000;
    largeBlockSizes.push_back(blockSize);
#else
#endif
//    largePools.push_back({{}, blockSize}); // 为每个大小创建一个空内存池
//    largePools.emplace_back(LargePool{{}, blockSize});
//    largePools.emplace_back(blockSize);

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
#if 0
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
#else
    for (auto& pool : largePools) {
        for (auto& block : pool.second.blocks) {
            if (block.memory != nullptr) {
                delete[] reinterpret_cast<char*>(block.memory);
            }
        }
    }

#endif
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
#if 0
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
#else
    for (auto& pool : largePools) {
        for (auto& blockInPool : pool.second.blocks) {
            if (blockInPool.memory == block) {
                blockInPool.inUse = false; // 标记为未使用
//                std::cout << "deallocate " << block << std::endl;
                LOGINFO("deallocate %p", block);
                spinlock.clear(std::memory_order_release); // 释放锁
                return;
            }
        }
    }
#endif
    spinlock.clear(std::memory_order_release); // 释放锁
}

void MemoryPool::deallocate(void* block, size_t size) {
    // ... deallocate 的实现 ...
    if (stop.load()) {
        return ; // 如果内存池已停止，直接返回
    }
    while (spinlock.test_and_set(std::memory_order_acquire)); // 获取锁
    if (size <= smallSize) {
        // 在小块内存中分配
        for (auto& blockInPool : smallPools) {
            if (blockInPool.memory == block) {
                blockInPool.usedCount--;
                if (blockInPool.usedCount == 0) {
                    blockInPool.nextAvailable = reinterpret_cast<char*>(blockInPool.memory);
                    blockInPool.freeSize = blockInPool.totalSize;
                }
//                std::cout << "deallocate small " << block << " size: " << size << std::endl;
                LOGINFO("deallocate small %p size: %d", block, size);
                spinlock.clear(std::memory_order_release); // 释放锁
                return;
            }
        }
    } else {
        int sizeindex = (static_cast<int>(size) + 0x1000 - 1) / 0x1000;
        auto it = largePools.find(sizeindex);
        if (it == largePools.end()) {
            spinlock.clear(std::memory_order_release); // 释放锁
            return;
        }
        for (auto &blockInPool: it->second.blocks) {
            if (blockInPool.memory == block) {
                blockInPool.inUse = false; // 标记为未使用
//                std::cout << "deallocate large " << block << " size: " << size << std::endl;
                LOGINFO("deallocate large %p size: %d", block, size);
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
    // ... addSmallBlock 的实现 ...
//    std::cout << "addSmallBlock " << size << std::endl;
    LOGINFO("addSmallBlock %d", size);
    SmallMemoryBlock newBlock;
    newBlock.totalSize = size;
    newBlock.freeSize = size;
    newBlock.memory = new char[size]; // 使用 new 分配内存
    newBlock.nextAvailable = reinterpret_cast<char*>(newBlock.memory);
    smallPools.push_back(newBlock);
}
void* MemoryPool::allocateSmall(size_t size) {
//    std::cout << "allocateSmall " << size << std::endl;
    LOGINFO("allocateSmall %d", size);
    // ... allocateSmall 的实现 ...
    while (spinlock.test_and_set(std::memory_order_acquire)); // 获取锁

    // 尝试在现有的小块内存中分配
    for (auto& block : smallPools) {
        if (block.freeSize >= size) {
            void* allocatedMemory = block.nextAvailable;
            block.nextAvailable += size;
            block.freeSize -= size;
            block.usedCount++;
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
//    std::cout << "allocateLarge " << size << std::endl;
    LOGINFO("allocateLarge %d", size);
    // ... allocateLarge 的实现 ...
#if 0
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
#else
    while (spinlock.test_and_set(std::memory_order_acquire)); // 获取锁
//    size_t mapsize = (size + 4096 - 1) / 4096;
    int sizeindex = (static_cast<int>(size) + 0x1000 - 1) / 0x1000;
    auto it = largePools.find(sizeindex);
    if (it == largePools.end()) {
//        largePools.emplace(sizeindex, LargePool(sizeindex * 4096));
        largePools.emplace(sizeindex, static_cast<size_t>(sizeindex * 0x1000));
    }
//    largePools[sizeindex];
    for (auto& block : largePools[sizeindex].blocks) {
        if (!block.inUse) {
            block.inUse = true;
            spinlock.clear(std::memory_order_release); // 释放锁
            return block.memory;
        }
    }
    LargeMemoryBlock newBlock;
    char *newmemory = new char[sizeindex * 0x1000]; // 使用 new 分配内存
    newBlock.memory = newmemory; // 使用 new 分配内存
    newBlock.inUse = true;
    largePools[sizeindex].blocks.push_back(newBlock);
    spinlock.clear(std::memory_order_release); // 释放锁
    return newBlock.memory;
#endif
//    return nullptr;
}