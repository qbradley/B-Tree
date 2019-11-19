#pragma once
#include <stdbool.h>
#define MAX_ENTRIES 3
#define MAX_VALUES 5

typedef struct Page Page;

typedef struct IndexEntry {
	Page* child;
	char* key;
	int keyLen;
} IndexEntry;

typedef struct IndexPage {
	int entries_size;
	IndexEntry entries[0];
} IndexPage;

typedef struct ValueEntry {
	char* key;
	int keyLen;
	char* value;
	int valueLen;
} ValueEntry;

typedef struct ValuePage {
	int values_size;
	ValueEntry values[0];
} ValuePage;

typedef enum PageType {
	Index = 0, Value = 1
} PageType;

struct Page {
	PageType type;
	union {
		IndexPage index;
		ValuePage value;
	};
};

Page* btree_create(PageType type);
void btree_free(Page* page);
//child cannot be freed while page references it
bool btree_insert_index(Page* page, char* key, int keyLen, Page* child);