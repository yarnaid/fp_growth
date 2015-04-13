#ifndef TRAIN_H
#define TRAIN_H

#include "../models/item.h"
#include "../FP-growth/fptree.hpp"
#include "../dclib-code/dlib/svm.h"
#include "../models/pattern.h"
#include <set>


typedef dlib::matrix<double, 16, 1>  sample_type;

void classificate(std::set<Item>& item, const std::string& file_name);
void build_classifier(std::set<Pattern>& patterns, const double& prop=0.8, const unsigned &nu_num=10, const double &nu_min=1.e-2);

namespace Kernels {
    typedef dlib::linear_kernel<sample_type> Linear;
    typedef dlib::polynomial_kernel<sample_type> Poly;
    typedef dlib::sigmoid_kernel<sample_type> Sigmoid;
    typedef dlib::radial_basis_kernel<sample_type> RBF;
}

template<typename K>
void train(std::set<Pattern>& patterns);

template<typename K>
void train(std::set<Pattern>& patterns, K& kernel, const unsigned& nu_num=100, const double& nu_min=1.e-2);

#endif // TRAIN_H
