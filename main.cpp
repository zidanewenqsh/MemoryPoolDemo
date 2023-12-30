//
// Created by wen on 2023-12-29.
//
#include <iostream>
#include <vector>
#include <memory>
#include "MemoryPool.h"
#include "rbtree.h"
//Node __NIL;
//#define NIL (&__NIL)
typedef struct Info {
    size_t size;
    char *ptr;
    Info(size_t size, char *ptr) : size(size), ptr(ptr) {}
} Info;

int main() {
    MemoryPool pool(1024, 10);
    std::vector<void *> ptrs;
    std::vector<Info> infos;
    for (int i = 0; i < 10; i++) {
        int size = (i % 3 + 1)*0x1000 + 1;
        auto ptr = pool.allocate(size);
        std::cout << ptr << std::endl;
        infos.emplace_back(size, reinterpret_cast<char*>(ptr));
        if (i % 2 == 0) {
//            pool.deallocate(ptr);
            pool.deallocate(infos[i].ptr, infos[i].size);
            infos.pop_back();
        } else {
            ptrs.push_back(ptr);
        }
    }
    for (int i = 0; i < infos.size(); i++) {
        pool.deallocate(infos[i].ptr, infos[i].size);
//        pool.deallocate(ptrs[i]);
    }
//    for (int i = 0; i < ptrs.size(); i++) {
//        pool.deallocate(ptrs[i]);
//    }
#if 0
////    RBTree::NodePtr RBTree::NIL = std::make_shared<RBTree::Node>(Data{0, 0});
//    RBTree::initNIL();
    RBTree myTree; // 创建红黑树实例，此时根节点是 NIL

//    Data rootData{0,};
//    myTree.root = myTree.insert(myTree.root, rootData); // 创建根节点


//    Node *root = NIL;
    int keys[] = {5,1,9,4,7,3,2,6,8,10};
    for (int i = 0; i < 10; i++) {
        Data data;
        data.key = keys[i];
        data.value = i;
        myTree.insert(data);
    }
    myTree.output();
    std::cout << "------------------" << std::endl;
    int del_keys[] = {1,4,3,8,10, 100};
    for (int i = 0; i < 6; i++) {
        myTree.erase(del_keys[i]);
    }
//    output(root);
    myTree.output();
    std::cout << "------------------" << std::endl;
    myTree.clear();
    myTree.output();
#endif
    return 0;
}