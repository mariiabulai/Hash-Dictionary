#include "open.h"
#include "cuckoo.h"
#include "avl.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <string>
#include <limits>

using namespace std;

void run_all_benchmarks();
void manual_interaction();

template<typename HashTable>
void perform_test_suite(const string& table_name, int size, int repeats, const int* seeds, int num_seeds);

template<typename HashTable>
void manual_test_loop();

int main() {
    int choice;
    while (true) {
        cout << "\n--- MENU GlOWNE ---\n";
        cout << "1. Uruchom zautomatyzowane testy wydajnosci\n";
        cout << "2. Wejdz w tryb interakcji recznej\n";
        cout << "3. Zakoncz program\n";
        cout << "Wybierz opcje: ";

        cin >> choice;
        if (cin.fail()) {
            cout << "Bledne dane. Prosze podac liczbe.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1:
            run_all_benchmarks();
            break;
        case 2:
            manual_interaction();
            break;
        case 3:
            cout << "Zakonczono program.\n";
            return 0;
        default:
            cout << "Nieznana opcja. Sprobuj ponownie.\n";
        }
    }
    return 0;
}

//testy wydajnosciowe
template<typename HashTable>
void perform_test_suite(const string& table_name, int size, int repeats, const int* seeds, int num_seeds) {
    ofstream output_file(table_name + "_results.csv", ios_base::app);

    long long total_insert_time = 0;
    long long total_remove_time = 0;

    //alokujemy pamiec dynamicznie na stercie za pomoca 'new'
    int* keys = new int[size];
    int* values = new int[size];

    cout << "  Testowanie " << table_name << " dla rozmiaru " << size << "...\n";

    //Petla po tablicy 
    for (int s_idx = 0; s_idx < num_seeds; ++s_idx) {
        int seed = seeds[s_idx];
        mt19937 rng(seed);
        uniform_int_distribution<int> key_dist(1, size * 10);
        uniform_int_distribution<int> val_dist(1, size);

        for (int i = 0; i < size; ++i) {
            keys[i] = key_dist(rng);
            values[i] = val_dist(rng);
        }

        HashTable table(size);

        for (int r = 0; r < repeats; ++r) {
            table.clear();

            auto t1 = chrono::high_resolution_clock::now();
            for (int i = 0; i < size; ++i) {
                table.insert(keys[i], values[i]);
            }
            auto t2 = chrono::high_resolution_clock::now();
            total_insert_time += chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();

            auto t3 = chrono::high_resolution_clock::now();
            for (int i = 0; i < size; ++i) {
                table.remove(keys[i]);
            }
            auto t4 = chrono::high_resolution_clock::now();
            total_remove_time += chrono::duration_cast<chrono::nanoseconds>(t4 - t3).count();
        }
    }

    //Zwalniamy pamiec, aby uniknac wyciekow
    delete[] keys;
    delete[] values;

    long long total_operations = (long long)size * repeats * num_seeds;
    long long avg_insert_ns = total_insert_time / total_operations;
    long long avg_remove_ns = total_remove_time / total_operations;

    output_file << size << "," << avg_insert_ns << "," << avg_remove_ns << "\n";
    output_file.close();
}

void run_all_benchmarks() {
    const int num_sizes = 10;
    int sizes[num_sizes] = { 10000, 20000, 30000, 40000, 50000,
                            60000, 70000, 80000, 90000, 100000 };

    int repeats = 10;

    const int num_seeds = 8;
    int seeds[num_seeds] = { 42, 1337, 2024, 77, 100, 256, 999, 888 };

    ofstream("open_addressing_results.csv") << "Size,Insert(ns),Remove(ns)\n";
    ofstream("cuckoo_hashing_results.csv") << "Size,Insert(ns),Remove(ns)\n";
    ofstream("chaining_avl_results.csv") << "Size,Insert(ns),Remove(ns)\n";

    cout << "\nRozpoczynam testy wydajnosci\n";
    for (int i = 0; i < num_sizes; ++i) {
        int s = sizes[i];
        //przekazujemy tablice seeds jako wskaźnik i rozmiar
        perform_test_suite<open>("open_addressing", s, repeats, seeds, num_seeds);
        perform_test_suite<cuckoo>("cuckoo_hashing", s, repeats, seeds, num_seeds);
        perform_test_suite<avl>("chaining_avl", s, repeats, seeds, num_seeds);
    }
    cout << "Testy zakonczone. Wyniki zapisano do plikow CSV.\n";
}

//interakcja reczna

template<typename HashTable>
void manual_test_loop() {
    HashTable table;
    int choice;
    int key, value;

    while (true) {
        cout << "\n--- Tryb Reczny ---\n";
        cout << "1. Wstaw element (insert)\n";
        cout << "2. Usun element (remove)\n";
        cout << "3. Pobierz wartosc (get)\n";
        cout << "4. Wydrukuj tablice (print)\n";
        cout << "5. Wroc do menu glownego\n";
        cout << "Wybierz opcje: ";

        cin >> choice;
        if (cin.fail()) {
            cout << "Bledne dane. Prosze podac liczbe.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Podaj klucz (liczba calkowita): ";
            cin >> key;
            cout << "Podaj wartosc (liczba calkowita): ";
            cin >> value;
            table.insert(key, value);
            cout << "Wstawiono {" << key << ": " << value << "}\n";
            break;
        case 2:
            cout << "Podaj klucz do usuniecia: ";
            cin >> key;
            if (table.remove(key)) {
                cout << "Usunieto element o kluczu " << key << ".\n";
            }
            else {
                cout << "Nie znaleziono elementu o kluczu " << key << ".\n";
            }
            break;
        case 3:
            cout << "Podaj klucz do znalezienia: ";
            cin >> key;
            {
                int* found_value = table.get(key);
                if (found_value) {
                    cout << "Znaleziono wartosc: " << *found_value << "\n";
                }
                else {
                    cout << "Nie znaleziono elementu o kluczu " << key << ".\n";
                }
            }
            break;
        case 4:
            table.print();
            break;
        case 5:
            return;
        default:
            cout << "Nieznana opcja.\n";
        }
    }
}

void manual_interaction() {
    int choice;
    cout << "\n--- Wybierz strukture do testow recznych ---\n";
    cout << "1. Tablica mieszajaca z adresowaniem otwartym\n";
    cout << "2. Tablica mieszajaca z Cuckoo Hashing\n";
    cout << "3. Tablica mieszajaca z adresowaniem zamknietym (Drzewa AVL)\n";
    cout << "4. Wroc do menu glownego\n";
    cout << "Wybierz opcje: ";

    cin >> choice;
    if (cin.fail()) {
        cout << "Bledne dane. Prosze podac liczbe.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    switch (choice) {
    case 1:
        manual_test_loop<open>();
        break;
    case 2:
        manual_test_loop<cuckoo>();
        break;
    case 3:
        manual_test_loop<avl>();
        break;
    case 4:
        return;
    default:
        cout << "Nieznana opcja.\n";
    }
}
