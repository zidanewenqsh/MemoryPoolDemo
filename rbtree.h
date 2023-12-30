//
// Created by wen on 2023-12-30.
//

#ifndef MAIN_RBTREE_H
#define MAIN_RBTREE_H
// rbtree.h
#include <iostream>
#include <memory>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct Data {
    int key; // 这个是用于红黑树排序的键
    // ... 添加其他你需要的字段 ...
    int value;
    // 构造函数，用于初始化 Data
    Data(int key = 0, int value = 0) : key(key), value(value) {}
} Data;

class RBTree {
private:
    struct Node {
        Data data;
        int color;
        std::shared_ptr<Node> lchild, rchild;
        Node(Data data) : data(data), color(0), lchild(NIL), rchild(NIL) {}
    };
//    using NodePtr = std::shared_ptr<Node>;
    using nodePtr = std::shared_ptr<Node>;
    std::shared_ptr<Node> root;
//    static std::shared_ptr<Node> NIL;
//    std::shared_ptr<Node> NIL = std::make_shared<Node>(Data());

//    void clear(std::shared_ptr<Node> node);
    nodePtr insert(nodePtr root, Data data);
    nodePtr erase(nodePtr root, int key);
//    std::shared_ptr<Node> insert(std::shared_ptr<Node> root, Data data);
//    std::shared_ptr<Node> erase(std::shared_ptr<Node> root, int key);
    // 其他辅助函数
    void output(nodePtr node);
    void print(nodePtr root);
    void clear(nodePtr root);


public:
    using NodePtr = std::shared_ptr<Node>;
    static std::shared_ptr<Node> NIL;
    // 静态初始化函数
    static void initNIL() {
        NIL = std::make_shared<Node>(Data());
        NIL->color = 1; // NIL 节点通常是黑色的
        NIL->lchild = NIL->rchild = NIL;
    }
    RBTree() : root(NIL) {}
    void insert(Data data);
    void erase(int key);
    void output();
    void clear();
    // 静态成员函数来创建新节点
    static NodePtr getNewNodePtr(Data data) {
        return std::make_shared<Node>(data);
    }
    std::shared_ptr<Node> find(int key);
};

// 初始化 NIL 节点
//std::shared_ptr<RBTree::Node> RBTree::NIL = std::make_shared<RBTree::Node>(Data{0, 0});

// RBTree 类成员函数的实现
// ...


#ifdef __cplusplus
}
#endif


#endif //MAIN_RBTREE_H
