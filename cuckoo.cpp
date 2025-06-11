#include "cuckoo.h"
#include <iostream>
using namespace std;
cuckoo::cuckoo(int initial_capacity) : capacity(initial_capacity), size(0) {
    if (capacity < 2) { //co najmniej 2 miejsca
        capacity = 2;
    }
    table = new Entry[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i].state = SlotState::EMPTY;
    }
    max_kick_attempts = capacity;
}

cuckoo::~cuckoo() {
    delete[] table;
}

int cuckoo::hash1(int key) const {
    //funkcja modulo
    long long h = key; //użycie long long, aby uniknąć przepełnienia przy mnożeniu
    h = (h * 31 + 17); //"mieszanie" bitów
    return (h % capacity + capacity) % capacity; //żeby wynik był w [0, capacity-1]
}

int cuckoo::hash2(int key) const {
    //funkcja, używająca dzielenia, aby zapewnić różnorodność
    long long h = (key / capacity);
    h = (h * 29 + 13);
    return (h % capacity + capacity) % capacity;
}

int* cuckoo::get(int key) {
    //wyszukiwanie jest bardzo szybkie: sprawdzamy tylko dwa miejsca.
    int idx1 = hash1(key);
    if (table[idx1].state == SlotState::OCCUPIED && table[idx1].key == key) {
        return &table[idx1].value;
    }

    int idx2 = hash2(key);
    if (table[idx2].state == SlotState::OCCUPIED && table[idx2].key == key) {
        return &table[idx2].value;
    }

    return nullptr;
}

bool cuckoo::remove(int key) {
    //też 2 miejsca
    int idx1 = hash1(key);
    if (table[idx1].state == SlotState::OCCUPIED && table[idx1].key == key) {
        table[idx1].state = SlotState::EMPTY;
        size--;
        return true;
    }

    int idx2 = hash2(key);
    if (table[idx2].state == SlotState::OCCUPIED && table[idx2].key == key) {
        table[idx2].state = SlotState::EMPTY;
        size--;
        return true;
    }
    return false;
}

void cuckoo::insert(int key, int value) {
    //sprawdzanie, czy klucz już istnieje - jeśli tak, to tylko aktualizujemy wartość
    if (int* existing_val = get(key)) {
        *existing_val = value;
        return;
    }
    //sprawdzanie, czy jest miejsce, zanim zaczniemy pętlę. jeśli współczynnik wypełnienia > 50% = ryzyk
    //prewencyjny rehash może być wydajniejszy niż czekanie na cykl
    if ((double)size / capacity >= 0.5) {
        rehash();
    }

    Entry current_entry = { key, value, SlotState::OCCUPIED };

    //pętla próbująca znaleźć miejsce dla elementu
    for (int i = 0; i < max_kick_attempts; ++i) {
        //próba umieścić w pierwszym gnieździe
        int idx1 = hash1(current_entry.key);
        if (table[idx1].state == SlotState::EMPTY) {
            table[idx1] = current_entry;
            size++;
            return;
        }
        //gniazdo jest zajęte - wyrzucamy stary element.
        //`current_entry` staje się elementem, który został wyrzucony.
        Entry temp = table[idx1];
        table[idx1] = current_entry;
        current_entry = temp;
        //teraz próbujemy umieścić wyrzucony element w jego drugim gnieździe.
        int idx2 = hash2(current_entry.key);
        if (table[idx2].state == SlotState::EMPTY) {
            table[idx2] = current_entry;
            size++;
            return;
        }
        //drugie gniazdo też jest zajęte - wyrzucamy i kontynuujemy pętlę.
        Entry temp2 = table[idx2];
        table[idx2] = current_entry;
        current_entry = temp2;
    }

    //jeśli pętla się zakończyła, oznacza to, że przekroczyliśmy limit prób.
    cout << "Cycle detected or too many kicks. Rehashing...\n";
    rehash();
    insert(current_entry.key, current_entry.value);
}

void cuckoo::rehash() {
    int old_capacity = capacity;
    Entry* old_table = table;
    //podwajamy pojemność i resetujemy tablicę
    capacity *= 2;
    table = new Entry[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i].state = SlotState::EMPTY;
    }
    //aktualizujemy limit prób dla nowej, większej tablicy
    max_kick_attempts = capacity;
    size = 0;
    //wstawiamy wszystkie stare elementy do nowej tablicy
    //`capacity` się zmieniło, funkcje `hash1` i `hash2` będą dawać inne wyniki, co "rozbije" cykl.
    for (int i = 0; i < old_capacity; ++i) {
        if (old_table[i].state == SlotState::OCCUPIED) {
            insert(old_table[i].key, old_table[i].value);
        }
    }

    delete[] old_table; //zwolnienie starej pamięci
}

void cuckoo::print() const {
    cout << "Cuckoo Hash Table: (size: " << size << ", capacity: " << capacity << ")\n";
    for (int i = 0; i < capacity; ++i) {
        cout << "Index " << i << ": ";
        if (table[i].state == SlotState::OCCUPIED) {
            cout << "{" << table[i].key << ": " << table[i].value << "} (h1=" 
                << hash1(table[i].key) << ", h2=" << hash2(table[i].key) << ")\n";
        }
        else {
            cout << "[EMPTY]\n";
        }
    }
    cout << "\n";
}

void cuckoo::clear() {
    for (int i = 0; i < capacity; ++i) {
        table[i].state = SlotState::EMPTY;
    }
    size = 0;
}
