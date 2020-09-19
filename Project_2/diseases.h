#ifndef DISEASES_H
#define DISEASES_H

#include "avl.h"

struct Bucket_Entry
{
    char* name;
    avl_ptr avl_tree_root;
};

typedef struct Bucket_Entry *bucket_entry;

#endif