#pragma once
#include <iostream>
using namespace std;
// Struktura węzła dla drzewa AVL
struct AVLNode {
    int key;
    int value;
    AVLNode* left;
    AVLNode* right;
    int height; //Wysokość węzła (dla równoważenia)
    //Konstruktor dla tworzenia węzłów
    AVLNode(int k, int v) : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
};

class avl {
private:
    AVLNode** table; //Tablica wskaźników na korzenie drzew AVL
    int capacity; //Liczba kubełków
    int size; //Całkowita liczba elementów we wszystkich drzewach
    int height(AVLNode* node);
    //Oblicza współczynnik zrównoważenia węzła
    int getBalance(AVLNode* node);
    //Aktualizuje wysokość węzła na podstawie wysokości jego dzieci
    void updateHeight(AVLNode* node);

    //Rotacje do równoważenia drzewa
    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);

    //do wstawiania węzła do drzewa w danym kubełku
    AVLNode* insertNode(AVLNode* root, int key, int value, bool& inserted);
    //do usuwania węzła z drzewa w danym kubełku
    AVLNode* removeNode(AVLNode* root, int key, bool& removed);
    //znajduje węzeł o najmniejszym kluczu w danym poddrzewie (dla usuwania)
    AVLNode* findMin(AVLNode* node);
    //rekurencyjna funkcja do wyszukiwania węzła
    AVLNode* findNode(AVLNode* root, int key) const;

    //funkcja do niszczenia całego drzewa (dla destruktora)
    void destroyTree(AVLNode* root);
    int hash(int key) const;

public:
    explicit avl(int initial_capacity = 8);
    ~avl();
    // Blokujemy kopiowanie
    avl(const avl&) = delete;
    avl& operator=(const avl&) = delete;
    void insert(int key, int value);
    bool remove(int key);
    int* get(int key);
    void print() const;
    void printTree(AVLNode* root, std::string indent, bool last) const;
    int get_size() const { return size; }
    int get_capacity() const { return capacity; }
    void clear();
};
