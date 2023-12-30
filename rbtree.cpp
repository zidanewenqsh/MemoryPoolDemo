//
// Created by wen on 2023-12-30.
//

#include "rbtree.h"
#include <iostream>
#include <vector>
#include <cstring>
//using RBTree::NodePtr = std::shared_ptr<Node>;

RBTree::NodePtr RBTree::NIL = std::make_shared<RBTree::Node>(Data{0, 0});
//using namespace std;
//RBTree::NodePtr __RBTree::NIL;
//#define RBTree::NIL (&__RBTree::NIL)
//
//__attribute__((constructor))

//void init_RBTree::NIL() {
//    RBTree::NIL->data.key = 0;
//    RBTree::NIL->data.value = 0;
//    RBTree::NIL->color = 1;
//    RBTree::NIL->lchild = RBTree::NIL->rchild = RBTree::NIL;
//    return;
//}
//
//RBTree::NodePtr getNewRBTree::NodePtr (Data data) {
//    return std::make_shared<Node>(data);
//}

void RBTree::clear(RBTree::NodePtr root) {
    if (root == NIL) return;
    clear(root->lchild);
    clear(root->rchild);
//    free(root);
    root.reset();
    return;
}

void RBTree::clear() {
    clear(root);
    root = NIL;
    return;
}

void RBTree::insert(Data data) {
    root = insert(root, data);
    root->color = 1;
    return;
}
void RBTree::erase(int key) {
    root = erase(root, key);
    root->color = 1;
    return;
}
int has_red_child(RBTree::NodePtr root) {
    return root->lchild->color == 0 || root->rchild->color == 0;
}

RBTree::NodePtr left_rotate(RBTree::NodePtr root) {
    RBTree::NodePtr temp = root->rchild;
    root->rchild = temp->lchild;
    temp->lchild = root;
    return temp;
}

RBTree::NodePtr right_rotate(RBTree::NodePtr root) {
    RBTree::NodePtr temp = root->lchild;
    root->lchild = temp->rchild;
    temp->rchild = root;
    return temp;
}

RBTree::NodePtr insert_maintain(RBTree::NodePtr root) {
    int flag = 0;
    if (!has_red_child(root)) return root;
    if (root->lchild->color == 0 && root->rchild->color == 0) {
        goto insert_end;
//        root->color = 0;
//        root->lchild->color = root->rchild->color = 0;
    }
    if (root->lchild->color == 0 && has_red_child(root->lchild)) flag = 1;
    if (root->rchild->color == 0 && has_red_child(root->rchild)) flag = 2;
    if (flag == 0) return root;
    if (flag == 1) {
        if (root->lchild->rchild->color == 0) {
            root->lchild = left_rotate(root->lchild);
        }
        root = right_rotate(root);
    } else {
        if (root->rchild->lchild->color == 0) {
            root->rchild = right_rotate(root->rchild);
        }
        root = left_rotate(root);
    }
    insert_end:
    root->color = 0;
    root->lchild->color = root->rchild->color = 1;
    return root;
}

RBTree::NodePtr __insert(RBTree::NodePtr root, Data data) {
    if (root == RBTree::NIL) return RBTree::getNewNodePtr(data);
    if (root->data.key == data.key) return root;
    if (data.key < root->data.key) {
        root->lchild = __insert(root->lchild, data);
    } else {
        root->rchild = __insert(root->rchild, data);
    }
    return insert_maintain(root);
}

RBTree::NodePtr RBTree::insert(RBTree::NodePtr root, Data data) {
    root = __insert(root, data);
    root->color = 1;
    return root;
}

RBTree::NodePtr erase_maintain(RBTree::NodePtr root) {
    if (root->lchild->color != 2 && root->rchild->color != 2) return root;
    if (has_red_child(root)) {
        int flag;
        root->color = 0;
        if (root->lchild->color == 0) {
            root = right_rotate(root);
            flag = 1;
        } else {
            root = left_rotate(root);
            flag = 2;
        }
        root->color = 1;
        if (flag == 1) {
            root->rchild = erase_maintain(root->rchild);
        } else {
            root->lchild = erase_maintain(root->lchild);
        }
        return root;
    }
    if ((root->lchild->color == 2 && !has_red_child(root->rchild)) ||
        (root->rchild->color == 2 && !has_red_child(root->lchild))) {
        root->lchild->color -= 1;
        root->rchild->color -= 1;
        root->color += 1;
        return root;
    }
    if (root->lchild->color == 2) {
        if (root->rchild->rchild->color != 0) {
            root->rchild->color = 0;
            root->rchild = right_rotate(root->rchild);
            root->rchild->color = 1;
        }
        root = left_rotate(root);
        root->color = root->lchild->color;
    } else {
        if (root->lchild->lchild->color != 0) {
            root->lchild->color = 0;
            root->lchild = right_rotate(root->lchild);
            root->lchild->color = 1;
        }
        root = right_rotate(root);
        root->color = root->rchild->color;
    }
    root->lchild->color = root->rchild->color = 1;
    return root;
}


RBTree::NodePtr predecessor(RBTree::NodePtr root) {
    RBTree::NodePtr temp = root->lchild;
    while (temp->rchild != RBTree::NIL) temp = temp->rchild;
    return temp;
}

RBTree::NodePtr __erase(RBTree::NodePtr root, int key) {
    if (root == RBTree::NIL) return RBTree::NIL;
    if (key < root->data.key) {
        root->lchild = __erase(root->lchild, key);
    } else if (key > root->data.key) {
        root->rchild = __erase(root->rchild, key);
    } else {
        if (root->lchild == RBTree::NIL || root->rchild == RBTree::NIL) {
//            RBTree::NodePtr *temp = root->lchild != RBTree::NIL ? root->lchild : root->rchild;
            RBTree::NodePtr temp = (root->lchild != RBTree::NIL) ? root->lchild : root->rchild;

            temp->color += root->color;
//            free(root);
            return temp;
        } else {
            RBTree::NodePtr temp = predecessor(root);
            root->data.key = temp->data.key;
            root->lchild = __erase(root->lchild, temp->data.key);
        }
    }
    return erase_maintain(root);
}

//RBTree::NodePtr RBTree::insert(RBTree::NodePtr root, Data data) {
//RBTree::NodePtr RBTree::*erase(RBTree::NodePtr *root, int key) {
RBTree::NodePtr RBTree::erase(RBTree::NodePtr root, int key) {
    root = __erase(root, key);
    root->color = 1;
    return root;
}

void RBTree::print(RBTree::NodePtr root) {
    printf("(%d|%d, %d,-l: %d, %d, -r: %d, %d)\n",
           root->color, root->data.key, root->data.value,
           root->lchild->data.key,root->lchild->data.value,
           root->rchild->data.key,root->rchild->data.value);
}

void RBTree::output(RBTree::NodePtr root) {
    if (root == RBTree::NIL) return;
    print(root);
    output(root->lchild);
    output(root->rchild);
    return;
}

void RBTree::output() {
    RBTree::output(RBTree::root);
}

RBTree::NodePtr find(RBTree::NodePtr root, int key) {
    while (root != RBTree::NIL && root->data.key != key) {
        if (key < root->data.key) root = root->lchild;
        else root = root->rchild;
    }
    return root;
}
#if 0
int main() {

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
    return 0;
}
#endif