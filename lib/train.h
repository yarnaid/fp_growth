#ifndef TRAIN_H
#define TRAIN_H

#include "../FP-growth/fptree.hpp"
#include "../dclib-code/dlib/svm.h"
#include <set>


typedef dlib::matrix<double, 12, 1> sample_type;
typedef dlib::radial_basis_kernel<sample_type> kernel_type;

void train(std::set<Pattern>& items);


#endif // TRAIN_H
