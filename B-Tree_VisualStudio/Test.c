#include "btree.h"

void danger(Page* root) {
	Page* child;
	char key[2];
	
	key[0] = 'a';
	key[1] = 'a';

	child = btree_create(Value);
	btree_insert_index(root, key, 2, child);
}

int main(int argc, char* argv[]) {

	Page* root;
	Page* child1;
	root = btree_create(Index);
	child1 = btree_create(Value);
	btree_insert_index(root, "foobarbaz", 9, child1);

	danger(root);

	btree_free(root);
	return 0;
}