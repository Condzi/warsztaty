#ifndef HASH_H
#define HASH_H

#define SIZE 20

struct DataItem {
   int data;   
   int key;
};

struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

struct DataItem *search(int key);
void insert(int key,int data);
struct DataItem* delete(struct DataItem* item);
void display();


#endif