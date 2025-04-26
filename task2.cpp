#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>



template <typename T>
class Treap {
private:
    //для реализации используем занакомую по семинарам структуру
    struct Node {
        T key;
        int priority;
        int size;
        int count; //вместо хранения цепи одинаковых по ключу(хранимое значение) элементов, будем хранить доп переменную - кол-во вхождений элемента.
        Node* left;
        Node* right;
        
        Node(const T& k) : key(k), priority(rand()), size(1), count(1), left(nullptr), right(nullptr) {}
    };

    Node* root;

    void updateSize(Node* node) {
        if (node) {
            if(node->left) {node->size+=node->left->size;}
            if(node->right) {node->size+=node->right->size;}
        }
    }

    void split(Node* node, const T& key, Node*& left, Node*& right) {

        /*ключевая идея реализации split такова:
        смотрим на узел - если значение меньше контрольного - node и его правое поддерево уже попадает в left
        надо обработать значения из rigth - нужные после рекурсии также окажутся в left
        аналогично когда значение больше контрольного*/

        if (!node) {//выход из рекурсии
            left = right = nullptr;
            return;
        }
        if (node->key <= key) {
            split(node->right, key, node->right, right);
            left = node;
        } else {
            split(node->left, key, left, node->left);
            right = node;
        }
        //после всех действий размеры попортились - обновляем
        updateSize(node);
    }

    Node* merge(Node* left, Node* right) {

        if (!left) return right;
        //выход из рекурсии
        if (!right) return left;

        /*merge сливает деревья, сохраняя смысл приоритетов (сравнивает приоритет сливаемых узлов)
        сама оперция проводится рекурсивно - сначала дети, потом родители*/

        if (left->priority > right->priority) {
            left->right = merge(left->right, right);
            updateSize(left);
            return left;
        } else {
            right->left = merge(left, right->left);
            updateSize(right);
            return right;
        }
    }

public:
    Treap() : root(nullptr) {
        srand(time(0));
    }

    ~Treap() {
        clear_sub_tree(root);
    }

    void insert(const T& key) {

        /*чтобы вставить проверяем, нет ли уже такого элемента*/
        Node* left, * right;

        /*разбиваем дерево - если есть наш элемент он будет в правом листочке левого поддерева (<=)*/
        split(root, key, left, right);

        Node* found = nullptr;
        if (left) {
            Node* current = left;
            while (current->right) {
                current = current->right;
            }
            if (current->key == key) {
                found = current;
            }
        }

        if (found) {
            found->count++;
            updateSize(found);
            root = merge(left, right);
        } else {

             //просто вставляем в начало, раз не нашлось

            Node* newNode = new Node(key);
            root = merge(merge(left, newNode), right);
        }
    }

    void remove(const T& key) {

        /*тут пользуемся split-ом чтобы в mid получился нужный нам элемент (если он есть)*/

        Node* left, * mid, * right;

        split(root, key - 1 /*тут key-1 не будет работать для всех данных. Подробнее ниже*/, left, mid); 
        //теперь в mid>=key

        split(mid, key, mid, right); 
        //теперь в mid == key

        if (mid) {
            if (mid->count > 1) {
                mid->count--;
                updateSize(mid);
                root = merge(merge(left, mid), right);
            } else {
                delete mid;
                root = merge(left, right);
            }
        } else {

            //eсли нет такого ключа в дереве

            root = merge(left, right);
        }
    }

    int size() const {
        return root ? root->size : 0;
    }

    void clear_sub_tree(Node* node) {
        if (node) {
            clear_sub_tree(node->left);
            clear_sub_tree(node->right);
            delete node;
        }
    }
};


/*Про key - 1
всё же код ориентирован на типы, которые представимы в виде чисел
для наиболее универсального шаблона, нужно бы обрабатывать и другие, более сложные типы данных, сравнение их между собой
эту поддержку в данном случае посчитаю избыточной*/