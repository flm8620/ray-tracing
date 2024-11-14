#pragma once
#include "RTree.h"

typedef RTree<unsigned int, float, 3, float> MyTree;

void exportRTreeToPly(const MyTree& tree, const char *file);
