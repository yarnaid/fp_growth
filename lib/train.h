#ifndef TRAIN_H
#define TRAIN_H

#include "../models/item.h"
#include "../FP-growth/fptree.hpp"
#include "../dclib-code/dlib/svm.h"
#include "../models/pattern.h"
#include <set>


typedef dlib::matrix<double, 16, 1>  sample_type;

void classificate(std::set<Item>& item, const std::string& file_name);
void build_classifier(std::set<Pattern>& patterns, const std::vector<Item>& external_items, const double& prop=0.8, const unsigned &nu_num=30, const double &nu_min=1.e-2, const unsigned& kernel_params=11);

namespace Kernels {
    typedef dlib::linear_kernel<sample_type> Linear;
    typedef dlib::polynomial_kernel<sample_type> Poly;
    typedef dlib::sigmoid_kernel<sample_type> Sigmoid;
    typedef dlib::radial_basis_kernel<sample_type> RBF;
}

#endif // TRAIN_H
