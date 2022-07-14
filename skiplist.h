#pragma once
/* ************************************************************************
> File Name:     main.cpp
> Author:        linfeng_li
> Created Time:  2022.03.08
> Description:   KV存储引擎
 ************************************************************************/

#include <iostream> 
#include <cstdlib>//cstdlib是C++里面的一个常用函数库， 等价于C中的<stdlib.h>
#include <cmath>
#include <ctime>
#include <cstring>//字符串处理库函数
#include <mutex>//互斥量库函数
#include <fstream>//写文件库函数
#include <string>

#define STORE_FILE "store/dumpFile"


std::mutex mtx;     // mutex for critical section / locks access to counter
                    // 类似于一个互斥量的构造函数 这个互斥量对象为 mtx
/*构造函数，std::mutex不允许拷贝构造，也不允许 move 拷贝，最初产生的 mutex 对象是处于 unlocked 状态的。
`lock()，调用线程将锁住该互斥量。线程调用该函数会发生下面 3 种情况：
      (1). 如果该互斥量当前没有被锁住，则调用线程将该互斥量锁住，直到调用 unlock之前，该线程一直拥有该锁。
      (2). 如果当前互斥量被其他线程锁住，则当前的调用线程被阻塞住。
      (3). 如果当前互斥量被当前调用线程锁住，则会产生死锁(deadlock)。
`unlock()， 解锁，释放对互斥量的所有权。
`try_lock()，尝试锁住互斥量，如果互斥量被其他线程占有，则当前线程也不会被阻塞。线程调用该函数也会出现下面 3 种情况，
      (1). 如果当前互斥量没有被其他线程占有，则该线程锁住互斥量，直到该线程调用 unlock 释放互斥量。
      (2). 如果当前互斥量被其他线程锁住，则当前调用线程返回 false，而并不会被阻塞掉。
      (3). 如果当前互斥量被当前调用线程锁住，则会产生死锁(deadlock)。*/
std::string delimiter = ":";

//Class template to implement node
template<typename K, typename V>
class Node {

public:

    Node() {} //跳表节点随机生成level ，没有实现

    Node(int);

    Node(K k, V v, int);

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);

public:

    // Linear array to hold pointers to next node of different level
    Node<K, V>** forward;

    int node_level;

    //Node<K, V>* BW;
    //double score;

private:
    K key; // m_key
    V value; // m_value
};

template<typename K, typename V>
inline Node<K, V>::Node(int level)
{
    this->node_level = level;
    this->forward = new Node<K, V> * [level + 1];
    memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));

    /*this->key = (K)(0);
    this->value = (V)(0);*/
}

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) {
    this->key = k;
    this->value = v;
    this->node_level = level;

    // level + 1, because array index is from 0 - level
    this->forward = new Node<K, V> * [level + 1];

    // Fill forward array with 0(NULL) 
    /*void* __cdecl memset(
    _Out_writes_bytes_all_(_Size) void*  _Dst,
    _In_                          int    _Val,
    _In_                          size_t _Size
    );*/
    memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));
};

template<typename K, typename V>
Node<K, V>::~Node() {
    delete[]forward;
};

template<typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
};

template<typename K, typename V>
V Node<K, V>::get_value() const {
    return value;
};
template<typename K, typename V>
void Node<K, V>::set_value(V value) {
    this->value = value;
};


// Class template for Skip list
template <typename K, typename V>
class SkipList {

public:
    SkipList(int);
    ~SkipList();
    int get_random_level();// rand()这个函数大概率k=2 ？？ turn to 定义
    Node<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    bool search_element(K key,V* val);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

private:
    // Maximum level of the skip list 
    const int _max_level;

    // current level of skip list 
    int _skip_list_level;

    // pointer to header node 
    Node<K, V>* _header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // skiplist current element count
    int _element_count;
};

// create new node 
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V>* n = new Node<K, V>(k, v, level);
    return n;
}

// Insert given key and value in skip list 
// return 1 means element exists  
// return 0 means insert successfully
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {

    mtx.lock();
    Node<K, V>* current = this->_header;

    // create update array and initialize it 
    // update is array which put node that the node->forward[i] should be operated later
    Node<K, V>* update[this->_max_level + 1];
    //Node<K, V>* update[32];
    memset(update, 0, sizeof(Node<K, V>*) * (this->_max_level + 1));

    // start form highest level of skip list 
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // reached level 0 and forward pointer to right node, which is desired to insert key.
    current = current->forward[0];

    // if current node have key equal to searched key, we get it
    if (current != NULL && current->get_key() == key) {
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1; // return 1 means element exists 
    }

    // if current is NULL that means we have reached to end of the level 
    // if current's key is not equal to key that means we have to insert node between update[0] and current node 
    if (current == NULL || current->get_key() != key) {

        // Generate a random level for node
        int random_level = get_random_level();

        // If random level is greater than skip list's current level, initialize update value with pointer to header
        //这里更新update数组有什么用？？？
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level + 1; i < random_level + 1; i++) {
                // 如果节点level高于现有的_skip_list_level，则头结点在_skip_list_level+1 至 random_level 的层数直接指向这个节点
                // update[i] 表示新建节点的上一跳节点
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        // create new node with random level generated 
        Node<K, V>* inserted_node = create_node(key, value, random_level);

        // insert node 
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count++;
    }
    mtx.unlock();
    return 0;
}

// Display skip list 
template<typename K, typename V>
void SkipList<K, V>::display_list() {

    std::cout << "\n*****Skip List*****" << "\n";
    for (int i = 0; i <= _skip_list_level; i++) {
        Node<K, V>* node = this->_header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// Dump data in memory to file 
template<typename K, typename V>
void SkipList<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K, V>* node = this->_header->forward[0];

    while (node != NULL) {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return;
}

// Load data from disk
template<typename K, typename V>
void SkipList<K, V>::load_file() {

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(stoi(*key), *value);
        //std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    _file_reader.close();
}

// Get current SkipList size 
template<typename K, typename V>
int SkipList<K, V>::size() {
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {

    if (!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter) + 1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        // str.find(delimiter) == str.end() ???
        //string中find()返回值是字母在母串中的位置（下标记录），如果没有找到，那么会返回一个特别的标记npos。（返回值可以看成是一个int型的数）
        return false;
    }
    return true;
}

// Delete element from skip list 
template<typename K, typename V>
void SkipList<K, V>::delete_element(K key) {

    mtx.lock();
    Node<K, V>* current = this->_header;
    Node<K, V>* update[_max_level + 1]; // Xcode可以运行，MSVC(Visual C++运行库)不能运行
    //Node<K, V>* update[6];
    memset(update, 0, sizeof(Node<K, V>*) * (_max_level + 1));

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if (current != NULL && current->get_key() == key) {

        // start for lowest level and delete the current node of each level
        for (int i = 0; i <= _skip_list_level; i++) {

            // if at level i, next node is not target node, break the loop.
            if (update[i]->forward[i] != current)
                break;

            update[i]->forward[i] = current->forward[i];
        }

        // Remove levels which have no elements
        // 防止删除的元素level为 skiplist 中最高的
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {
            _skip_list_level--;
        }

        std::cout << "Successfully deleted key " << key << std::endl;
        _element_count--;
    }
    mtx.unlock();
    return;
}

// Search for element in skip list 
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V>
bool SkipList<K, V>::search_element(K key) {

    std::cout << "search_element-----------------" << std::endl;
    Node<K, V>* current = _header;

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    //reached level 0 and advance pointer to right node, which we search
    current = current->forward[0];

    // if current node have key equal to searched key, we get it
    if (current and current->get_key() == key) {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

//服务端search_element
template<typename K, typename V>                                                                                        
bool SkipList<K, V>::search_element(K key,V* val) {                                                                            
                                                                                                                        
    Node<K, V>* current = _header; 
    for (int i = _skip_list_level; i >= 0; i--) {                                                                       
        while (current->forward[i] && current->forward[i]->get_key() < key) {                                           
            current = current->forward[i];                                                                              
        }                                                                                                               
    }  
    current = current->forward[0];
    if (current and current->get_key() == key) {                                                                        
        *val = current->get_value();
        return true;                                                                                                    
    }                                                                                                                   
                                                                                                                        
    return false;                                                                                                       
}                 





// construct skip list
template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level) :_max_level(max_level) {

    //this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    // create header node and initialize key and value to null
    /*K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);*/
    this->_header = new Node<K, V>(this->_max_level);
};

template<typename K, typename V>
SkipList<K, V>::~SkipList() {

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
    //rand()出来的数字为固定值，所以会导致所有的k输出确定
    //应该使用srand()
    srand((unsigned int)time(NULL));
    int k = rand() % this->_max_level;
    /*while (rand() % 2) {
        k++;
    }*/
    //<ctime>

    k = (k < _max_level) ? k : _max_level;
    return k;
};
// vim: et tw=100 ts=4 sw=4 cc=120
