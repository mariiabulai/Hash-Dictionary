#include "open.h"
#include <iostream>
#include <stdexcept>
using namespace std;

open::open(int initial_capacity) : capacity(initial_capacity), size(0) {
    if (capacity < 1) {
        capacity = 1;
    }
    table = new Entry[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i].state = SlotState::EMPTY;
    }
}

open::~open() {
    delete[] table;
}

int open::hash(int key) const {
    //funkcja mieszająca (modulo)
    //obsługa ujemnych kluczy, aby wynik był zawsze w zakresie [0, capacity-1]
    int h = key % capacity;
    return (h >= 0) ? h : (h + capacity);
}

void open::resize() {
    int old_capacity = capacity;
    Entry* old_table = table;
    capacity *= 2;
    table = new Entry[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i].state = SlotState::EMPTY;
    }
    size = 0; //rozmiar zostanie zaktualizowany przez ponowne wstawienia
    //przenosimy elementy do nowej tablicy
    for (int i = 0; i < old_capacity; ++i) {
        if (old_table[i].state == SlotState::OCCUPIED) {
            insert(old_table[i].key, old_table[i].value);
        }
    }
    delete[] old_table;
}

//linear probing
//funkcja do znajdowania miejsca na klucz.
//zwraca indeks, gdzie klucz jest lub powinien być.
int open::find_slot(int key) const {
    int idx = hash(key);
    int start_idx = idx;
    int first_deleted = -1;
    do {
        //jeśli miejsce jest puste, to klucza nie ma dalej
        if (table[idx].state == SlotState::EMPTY) {
            //jeśli znaleźliśmy wcześniej usunięte miejsce, użyjmy go
            return (first_deleted != -1) ? first_deleted : idx;
        }
        //jeśli miejsce jest zajęte przez ten sam klucz
        if (table[idx].state == SlotState::OCCUPIED && table[idx].key == key) {
            return idx;
        }
        //jeśli miejsce jest usunięte, zapamiętujemy je jako kandydata
        if (table[idx].state == SlotState::DELETED && first_deleted == -1) {
            first_deleted = idx;
        }
        //przechodzimy do następnego miejsca (próbowanie liniowe)
        idx = (idx + 1) % capacity;
    } while (idx != start_idx);

    //kiedy przeszliśmy całą tablicę
    return (first_deleted != -1) ? first_deleted : -1; //-1 = tablica jest pełna
}


void open::insert(int key, int value) {
    //sprawdzamy współczynnik wypełnienia, w razie potrzeby powiększamy tablicę
    if ((double)size / capacity >= 0.7) {
        resize();
    }
    int idx = find_slot(key);
    if (idx == -1) { //nie powinno się zdarzyć dzięki resize
        cout << "Table is full, cannot insert.";
    }

    //jeśli miejsce jest już zajęte przez ten klucz, aktualizujemy wartość
    if (table[idx].state == SlotState::OCCUPIED) {
        table[idx].value = value;
    }
    else { //wstawiamy nowy element
        table[idx].key = key;
        table[idx].value = value;
        table[idx].state = SlotState::OCCUPIED;
        size++;
    }
}

bool open::remove(int key) {
    int idx = find_slot(key);
    //usuwamy jeśli klucz istnieje i jest zajęty
    if (idx != -1 && table[idx].state == SlotState::OCCUPIED) {
        table[idx].state = SlotState::DELETED;
        size--;
        return true;
    }

    return false;
}

int* open::get(int key) {
    int idx = find_slot(key);
    if (idx != -1 && table[idx].state == SlotState::OCCUPIED) {
        return &table[idx].value;
    }
    return nullptr;
}


void open::print() const {
    cout << "Hash Table: Open Addressing (size: " << size << ", capacity: " << capacity << ")\n";
    for (int i = 0; i < capacity; ++i) {
        cout << "Index " << i << ": ";
        if (table[i].state == SlotState::OCCUPIED) {
            cout << "{" << table[i].key << ": " << table[i].value << "}\n";
        }
        else if (table[i].state == SlotState::DELETED) {
            cout << "[DELETED]\n";
        }
        else {
            cout << "[EMPTY]\n";
        }
    }
    cout << "\n";
}

void open::clear() {
    for (int i = 0; i < capacity; i++) {
        table[i].state = SlotState::EMPTY;
    }
    size = 0;
}
