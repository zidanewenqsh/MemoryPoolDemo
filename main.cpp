//
// Created by wen on 2023-12-29.
//
#include <iostream>
#include <vector>
#include <memory>
#include "MemoryPool.h"
int main() {
    MemoryPool pool(1024, 10);
    auto ptr = pool.allocate(1025);
    std::cout << ptr << std::endl;
    return 0;
}