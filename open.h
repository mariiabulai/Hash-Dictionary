#pragma once
#include <iostream>
using namespace std;
class open {
private:
    //Używamy enum do reprezentowania stanu każdego miejsca w tablicy
    enum class SlotState { EMPTY, OCCUPIED, DELETED };
    //Grupujemy powiązane dane w jednej strukturze (lepsza lokalizacja danych)
    struct Entry {
        int key;
        int value;
        SlotState state;
    };
    Entry* table;
    int capacity; //aktualna pojemność tablicy
    int size; //liczba zajętych elementów
    //Funkcja do obliczania hasha
    int hash(int key) const;
    void resize();
    int find_slot(int key) const;
public:
    explicit open(int initial_capacity = 8); //explicit zapobiega niejawnym konwersjom
    ~open();

    //Reguła Trzech/Pięciu: Blokujemy kopiowanie, aby uniknąć problemów
    //z podwójnym zwalnianiem pamięci
    open(const open&) = delete;
    open& operator=(const open&) = delete;
    void insert(int key, int value);
    bool remove(int key);
    int* get(int key); //Zwraca wskaźnik, by móc odróżnić brak klucza (nullptr) od wartości 0
    void print() const;
    int get_size() const { return size; }
    int get_capacity() const { return capacity; }
    void clear();
};
