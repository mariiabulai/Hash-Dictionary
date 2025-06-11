#pragma once
#include <iostream>
using namespace std;
class cuckoo
{
private:
    //nie potrzebujemy stanu deleted, usunięcie po prostu zwalnia miejsce
    enum class SlotState { EMPTY, OCCUPIED };
    //grupujemy powiązane dane w jednej strukturze (lepsza lokalizacja danych)
    struct Entry {
        int key;
        int value;
        SlotState state;
    };
    Entry* table; //wskaźnik na tablicę kubełków
    int capacity; //aktualna pojemność tablicy
    int size; //liczba zajętych elementów

    //dwie różne funkcje mieszające
    int hash1(int key) const;
    int hash2(int key) const;

    //funkcja do przeprowadzenia rehashu (cykl)
    void rehash();

    //maksymalna liczba prób wyrzucenia elementu przed uznaniem cyklu 
    //(bedzie wartość proporcjonalną do pojemności)
    int max_kick_attempts;

public:
    explicit cuckoo(int initial_capacity = 8);
    ~cuckoo();

    //blokujemy kopiowanie, aby uniknąć problemów z zarządzaniem pamięcią.
    cuckoo(const cuckoo&) = delete;
    cuckoo& operator=(const cuckoo&) = delete;

    void insert(int key, int value);
    bool remove(int key);
    int* get(int key); //zwraca wskaźnik, aby odróżnić nullptr od wartości 0.
    void print() const;
    int get_size() const { return size; }
    int get_capacity() const { return capacity; }
    void clear();
};

