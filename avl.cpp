#include "avl.h"
#include <iostream>
using namespace std;

avl::avl(int initial_capacity) : capacity(initial_capacity), size(0) {
    if (capacity < 1) {
        capacity = 1;
    }
    //Alokujemy tablicę wskaźników i inicjalizujemy je jako puste (nullptr)
    table = new AVLNode * [capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i] = nullptr;
    }
}

avl::~avl() {
    //Musimy zniszczyć każde drzewo w każdym kubełku
    for (int i = 0; i < capacity; ++i) {
        destroyTree(table[i]);
    }
    delete[] table; //na końcu zwalniamy samą tablicę wskaźników
}

int maxx(int a, int b) {
    return (a > b) ? a : b;
}

void avl::destroyTree(AVLNode* root) {
    if (root != nullptr) {
        destroyTree(root->left);
        destroyTree(root->right);
        delete root;
    }
}

int avl::hash(int key) const {
    return (key % capacity + capacity) % capacity;
}

int avl::height(AVLNode* node) {
    return (node == nullptr) ? 0 : node->height;
}

void avl::updateHeight(AVLNode* node) {
    if (node != nullptr) {
        node->height = 1 + maxx(height(node->left), height(node->right));
    }
}

int avl::getBalance(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

AVLNode* avl::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    //rotacja
    x->right = y;
    y->left = T2;

    //aktualizowanie wysokości (najpierw y, potem x)
    updateHeight(y);
    updateHeight(x);

    //nowy korzeń
    return x;
}

AVLNode* avl::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    //rotacja
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    //nowy korzeń
    return y;
}

void avl::insert(int key, int value) {
    int index = hash(key);
    bool inserted_new = false;
    //Wywoływanie rekurencyjnego wstawiania i aktualizowanie korzenia w danym kubełku
    table[index] = insertNode(table[index], key, value, inserted_new);
    if (inserted_new) {
        size++;
    }
}

AVLNode* avl::insertNode(AVLNode* root, int key, int value, bool& inserted) {
    // 1. Standardowe wstawianie do BST
    if (root == nullptr) {
        inserted = true;
        return new AVLNode(key, value);
    }
    if (key < root->key) {
        root->left = insertNode(root->left, key, value, inserted);
    }
    else if (key > root->key) {
        root->right = insertNode(root->right, key, value, inserted);
    }
    else { // Klucz już istnieje, tylko aktualizować wartość
        root->value = value;
        inserted = false; //nie wstawiono nowego elementu
        return root;
    }
    // 2. Aktualizacja wysokości bieżącego węzła
    updateHeight(root);
    // 3. Obliczenie współczynnika zrównoważenia i ewentualne rotacje
    int balance = getBalance(root);
    //Lewy-Lewy (LL)
    if (balance > 1 && key < root->left->key) {
        return rightRotate(root);
    }
    //Prawy-Prawy (RR)
    if (balance < -1 && key > root->right->key) {
        return leftRotate(root);
    }
    //Lewy-Prawy (LR)
    if (balance > 1 && key > root->left->key) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    //Prawy-Lewy (RL)
    if (balance < -1 && key < root->right->key) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    //(potencjalnie niezmieniony) wskaźnik na korzeń
    return root;
}

bool avl::remove(int key) {
    int index = hash(key);
    bool removed = false;
    table[index] = removeNode(table[index], key, removed);
    if (removed) {
        size--;
    }
    return removed;
}

AVLNode* avl::removeNode(AVLNode* root, int key, bool& removed) {
    // 1. Standardowe usuwanie z BST
    if (root == nullptr) {
        removed = false;
        return root;
    }
    if (key < root->key) {
        root->left = removeNode(root->left, key, removed);
    }
    else if (key > root->key) {
        root->right = removeNode(root->right, key, removed);
    }
    else {
        //Znaleziono węzeł do usunięcia
        removed = true;
        //Przypadek 1 i 2: Węzeł ma jedno dziecko lub nie ma dzieci
        if (root->left == nullptr || root->right == nullptr) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (temp == nullptr) { //Brak dzieci
                temp = root;
                root = nullptr;
            }
            else { //Jedno dziecko
                *root = *temp; //kopijowanie zawartości dziecka
            }
            delete temp;
        }
        else {
            //Przypadek 3: Węzeł ma dwoje dzieci
            //znalezienie następnika w porządku in-order (najmniejszy w prawym poddrzewie)
            AVLNode* temp = findMin(root->right);
            //kopijowanie dane następnika do tego węzła
            root->key = temp->key;
            root->value = temp->value;
            //usuwanie następnika
            root->right = removeNode(root->right, temp->key, removed);
        }
    }

    //Jeśli drzewo stało się puste po usunięciu
    if (root == nullptr) {
        return root;
    }

    //2. Aktualizacja wysokości i równoważenie
    updateHeight(root);
    int balance = getBalance(root);

    //sprawdzamy 4 przypadki
    if (balance > 1 && getBalance(root->left) >= 0) // LL
        return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0) { // LR
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0) // RR
        return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0) { // RL
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

AVLNode* avl::findMin(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

int* avl::get(int key) {
    int index = hash(key);
    AVLNode* node = findNode(table[index], key);
    if (node != nullptr) {
        return &node->value;
    }
    return nullptr;
}

AVLNode* avl::findNode(AVLNode* root, int key) const {
    if (root == nullptr || root->key == key) {
        return root;
    }
    if (key < root->key) {
        return findNode(root->left, key);
    }
    return findNode(root->right, key);
}

void avl::print() const {
    cout << "HashTable (size: " << size << ", capacity: " << capacity << ")\n";
    for (int i = 0; i < capacity; ++i) {
        cout << "Bucket " << i << ":\n";
        if (table[i] == nullptr) {
            cout << "  [EMPTY]\n";
        }
        else {
            printTree(table[i], "  ", true);
        }
    }
}

void avl::printTree(AVLNode* root, string indent, bool last) const {
    if (root != nullptr) {
        cout << indent;
        if (last) {
            cout << "R----";
            indent += "     ";
        }
        else {
            cout << "L----";
            indent += "|    ";
        }
        cout << "{" << root->key << ":" << root->value << "} (h=" << root->height << ")\n";
        printTree(root->left, indent, false);
        printTree(root->right, indent, true);
    }
}

void avl::clear() {
    for (int i = 0; i < capacity; ++i) {
        if (table[i] != nullptr) {
            destroyTree(table[i]);
            table[i] = nullptr;
        }
    }
    size = 0;
}
