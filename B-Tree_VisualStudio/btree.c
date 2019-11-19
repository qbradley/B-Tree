#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <assert.h>
#include "btree.h"


//Chose to align with disk page sizes
#define PAGE_SIZE 4096


//Returns NULL on failure
Page* btree_create(PageType type) {
	Page* result;
	
	//Make sure we don't underallocate
	assert(sizeof(Page) <= PAGE_SIZE);

	result = malloc(PAGE_SIZE);
	if (result != NULL) {
		result->type = type;
		if (type == Index) {
			result->index.entries_size = 0;
		}
		else {
			result->value.values_size = 0;
		}
	}

	return result;
}

void btree_free(Page* page) {
	free(page);
}

static int IndexEntry_Compare(const void* a, const void* b) {
	const IndexEntry* pA = a;
	const IndexEntry* pB = b;
	int shortestLength = (pA->keyLen < pB->keyLen) ? pA->keyLen : pB->keyLen;
	int result = memcmp(pA->key, pB->key, shortestLength);
	if (result == 0) {
		result = (pA->keyLen - pB->keyLen);
	}

	return result;
}

static int get_keys_used(Page* page) {
	int keys_used = 0;
	for (int i = 0; i < page->index.entries_size; i++) {
		keys_used += page->index.entries[i].keyLen;
	}
	return keys_used;
}

static int get_index_available(Page* page) {
	assert(page->type == Index);
	int page_used = sizeof(Page);
	int entries_used = page->index.entries_size * sizeof(IndexEntry);
	// to do: consider get keys used only once per insert
	int keys_used = get_keys_used(page);

	int total_used = page_used + entries_used + keys_used;
	return PAGE_SIZE - total_used;
}

static char* copy_to_page(Page* page, char* key, int keyLen) {
	// byte math
	char* ptr = (char*)page;

	int keys_used = get_keys_used(page) + keyLen;

	// got to end of page
	ptr += PAGE_SIZE;
	ptr -= keys_used;
	memcpy(ptr, key, keyLen);
	return ptr;
}

//bool returns true if insertion succeeded false otherwise
bool btree_insert_index(Page* page, char* key, int keyLen, Page* child) {
	//Figure out the right one
	int entryIndex = 0;
	int need = sizeof(IndexEntry) + keyLen;
	int available = get_index_available(page);

	if (need <= available) {
		entryIndex = page->index.entries_size;
		page->index.entries[entryIndex].key = copy_to_page(page, key, keyLen);
		page->index.entries[entryIndex].keyLen = keyLen;
		page->index.entries[entryIndex].child = child;
		page->index.entries_size++;
		qsort(page->index.entries, page->index.entries_size, sizeof(IndexEntry), IndexEntry_Compare);
		return true;
	}
	
	return false;
}