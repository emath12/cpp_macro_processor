#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define CAPACITY 50000
#define INVALID "@10NULL01@"


class ht_entry {
    public:
        std::string key;
        std::string value; 

        ht_entry(string key, string value) {
            this->key = key;
            this->value = value;
        }

        ~ht_entry() {
            delete this;   
        }
};

class node {
    public:
        ht_entry *item;
        node *next; 

        node(ht_entry *item) {
            this->item = item; 
        }  
        ~node() {
            delete this;
        }
};

class linked_list {
    node *head;

    public:
        linked_list() {
            this->head = NULL;
        }

        ~linked_list() {
            delete this;
        }

        void insert_node(ht_entry *item) {
            
            // new node object holding the item
            node *new_node = new node(item);

            // special case where the head is not ini't
            if (this->head == NULL) {
                this->head = new_node;
                return;
            }

            node *trav = this->head;

            if (trav == NULL) {
                trav = new_node;
                return;
            }

            while (trav->next != NULL) {
                trav = trav->next;
            }

            trav->next = new_node; 
        }

        node * find_node(string key) {
            node *trav = this->head;
            

            // if its just in the head, since the head's next will be NULL
            if (trav->item->key == key) {
                return trav;
            }

            // normal search for val
            while (trav->next != NULL) {
                if (trav->item->key == key) {
                    return trav;
                }
                
                trav = trav->next;
            }

            return NULL;
        }
};

class hashmap {
    int total_entries;
    int total_capacity;
    std::vector<ht_entry *> items;
    std::vector<linked_list *> overflow_buckets; 

    public:

        hashmap(int total_capacity) {
            this->total_capacity = total_capacity;
        }

        ~hashmap() {
            delete this;  
        }

        unsigned long hash_function(std::string input) { 
            /*
            Computes the index that this string should be stored in.
            */
            unsigned long total = 0;
            for (int j = 0; j < input.length(); j++) {
                total += input[j];
            }
            return total % this->total_capacity;
        }   

        void handle_collisions(int index, ht_entry *item) {
            if (this->overflow_buckets.size() == 0) {
                this->overflow_buckets.resize(1);
            }

            while (index >= this->overflow_buckets.size()) {
                this->overflow_buckets.resize(this->overflow_buckets.size() * 2);
            }

            if (this->overflow_buckets[index] == NULL) {
                this->overflow_buckets[index] = new linked_list();
            }

            this->overflow_buckets[index]->insert_node(item); 
        }

        // we want to insert value @ key
        void add_mapping(string key, string value) {
            ht_entry *ht = new ht_entry(key, value); 

            // the index of the place where we want to store the values of key.
            int index = hash_function(key);

            if (this->total_entries + 1 == this->total_capacity) {
                delete ht; 
                return;
            }

            // hashed pairs list is not large enough to take the index
            if (index > this->items.size() - 1 || this->items.empty()) {
                this->total_entries += 1;

                // resizing the vector to fit the new index
                if (this->items.size() == 0) {
                    this->items.resize(1);
                }

                while (index >= this->items.size()) {
                    this->items.resize(this->items.size() * 2);
                }

                // adding the pair 
                this->items[index] = ht;
            } else {  
                // if the index is within the bounds of the items vector
                // (no resizing required + we need to check things)
                if (this->items[index] == NULL) { // index has been init'd because the size is big enough, but nothing is there.
                    this->items[index] = ht;
                } else if (key == this->items[index]->key) {
                    // if the keys are the same, then we need only update the vlue.
                    this->items[index]->value = value; 
                } else {
                    // there is a collision, same index computed but different key.
                    handle_collisions(index, ht);
                }
            } 
        }

        string get_mapping(string key) {
            int index = hash_function(key);

            ht_entry *item = this->items[index];

            if (item == NULL) {
                return INVALID;
            }

            // if the keys match, just return the corresponding value since there is no collision
            if (item->key == key) {
                return item->value;
            }

            // otherwise, there is a collision. 
            if (this->overflow_buckets[index] != NULL) {
                node *overflow_search = this->overflow_buckets[index]->find_node(key);
                
                if ((overflow_search != NULL) && (overflow_search->item != NULL)) {
                    return overflow_search->item->value;
                }
            }

            return INVALID;
        }      

        void remove_mapping(string key) {
            int index = hash_function(key);
            
            ht_entry *item = this->items[index];
            
            if (item->key == key) {
                this->items[index] = NULL;
            } else {
                node *overflow_search = this->overflow_buckets[index]->find_node(key);
                overflow_search->item = NULL;
            }
        }  

        bool is_mapped(string key) {
            if (this->get_mapping(key) == INVALID) {
                return false;
            }

            return true;
        }
};

// testing ground
// int main() {
//     string a = "hi";
//     string b = "bobby";
//     string c = "kaity";
//     string d = "boo";

//     hashmap *table = new hashmap(CAPACITY);
//     table->add_mapping(a, b);
//     table->add_mapping(c, d);

//     std::cout << "map a " << table->get_mapping(a) << endl;
//     std::cout << "map b " << table->get_mapping(c) << endl;
//     table->remove_mapping(a);
//     if (table->get_mapping(a) == INVALID) {
//         cout << "good" << endl;
//     }

//     return 0;
// }