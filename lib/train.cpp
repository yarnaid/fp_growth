#include "train.h"
#include <vector>
#include <iostream>

#define DEBUG

sample_type get_sample (const Item& item);

template void train<Kernels::RBF>(std::set<Pattern>& patterns);
template void train<Kernels::Linear>(std::set<Pattern>& patterns);
template void train<Kernels::Poly>(std::set<Pattern>& patterns);
template void train<Kernels::Sigmoid>(std::set<Pattern>& patterns);

template<typename K>
void train(std::set<Pattern>& patterns)
{
    K k;
    train(patterns, k);
    return;
}

template<typename K>
void train(std::set<Pattern>& patterns, K& kernel, const unsigned& nu_num, const double& nu_min)
{
    std::vector<sample_type> samples;
    std::vector<double> labels;
    std::set<Item> items;
    typedef K kernel_type;

    for (const Pattern& p : patterns)
        for (const Item& i : p.first)
            items.insert(i);

    for (const Item& i : items) {
        samples.push_back(get_sample(i));
        labels.push_back(i.svm_class);
    }

    dlib::vector_normalizer<sample_type> normalizer;
    normalizer.train(samples);

    for (unsigned long i = 0; i < samples.size(); ++i)
        samples[i] = normalizer(samples[i]);

    dlib::randomize_samples(samples, labels);

    const double max_nu = dlib::maximum_nu(labels);
    dlib::svm_nu_trainer<kernel_type> trainer;
    dlib::matrix<double> nus = dlib::linspace(nu_min, max_nu, nu_num);

    dlib::matrix<double> res;
    for(const double& nu : nus)
    {
        trainer.set_kernel(kernel);
        trainer.set_nu(nu);
        res = dlib::cross_validate_trainer(trainer, samples, labels, 3);
#ifdef DEBUG
        std::cout << "nu: " << std::setw(11) << nu;
        std::cout << " CV accuarncy: " << std::setw(2) << dlib::sum(res) / 2. * 100 << "%" << std::endl;
#endif
    }

    return;
}

sample_type get_sample (const Item& item)
{
    sample_type res;
    res(0) = dlib::hash(static_cast<dlib::uint32>(item.time));
    res(1) = dlib::hash(item.id);
    res(2) = dlib::hash(item.source);
    res(3) = dlib::hash(item.category);
    res(4) = dlib::hash(item.name);
    res(5) = dlib::hash(item.md5);
    res(6) = dlib::hash(item.ip);
    res(7) = dlib::hash(item.url);
    res(8) = dlib::hash(item.fqdn);
    res(9) = dlib::hash(item.asn);
    res(10) = dlib::hash(item.cc);
    res(11) = dlib::hash(item.details);
    res(12) = dlib::hash(static_cast<dlib::uint32>(item.q_number));
    res(13) = dlib::hash(static_cast<dlib::uint32>(item.q_length));
    res(14) = dlib::hash(static_cast<dlib::uint32>(item.url_length));
    res(15) = dlib::hash(static_cast<dlib::uint32>(item.domain_length));
    return res;
}
