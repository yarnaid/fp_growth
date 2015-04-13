#include "train.h"
#include <vector>
#include <iostream>
#include <functional>

#define DEBUG

#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define BLACK "\e[0;0m"

sample_type get_sample (const Item& item);

template void train<Kernels::RBF>(std::set<Pattern>& patterns);
template void train<Kernels::Linear>(std::set<Pattern>& patterns);
template void train<Kernels::Poly>(std::set<Pattern>& patterns);
template void train<Kernels::Sigmoid>(std::set<Pattern>& patterns);

template<typename ...P> Kernels::Linear get_kernel(P...);
template<typename ...P> Kernels::Poly get_kernel(P...);
template<typename ...P> Kernels::RBF get_kernel(P...);
template<typename ...P> Kernels::Sigmoid get_kernel(P...);

typedef dlib::normalized_function<dlib::decision_function<Kernels::Linear>> linear_func;
typedef dlib::normalized_function<dlib::decision_function<Kernels::RBF>> rbf_func;
typedef dlib::normalized_function<dlib::decision_function<Kernels::Sigmoid>> sigmoid_func;
typedef dlib::normalized_function<dlib::decision_function<Kernels::Poly>> poly_func;

template<typename K>K evaluate_kernel(const dlib::matrix<double>& params);
template<typename K, typename ...P> K get_kernel(P... args);
dlib::matrix<double> get_kernel_params(const dlib::matrix<double>& params, const unsigned& size);

template<typename K>
void build_info(dlib::normalized_function<dlib::decision_function<K>>& df,
                const std::vector<sample_type>& test_samples,
                const std::vector<double>& test_labels);

dlib::vector_normalizer<sample_type> split_patterns(std::set<Pattern>& patterns,
                    std::vector<sample_type>& train_samples,
                    std::vector<double>& train_labels,
                    std::vector<sample_type>& test_samples,
                    std::vector<double>& test_labels,
                    const double& prop=0.8
                    );

Kernels::Linear linear(const dlib::matrix<double>& p) {return Kernels::Linear();}
Kernels::RBF rbf(const dlib::matrix<double>& p) {return Kernels::RBF(p(0));}
Kernels::Sigmoid sigmoid(const dlib::matrix<double>& p) {return Kernels::Sigmoid(p(0), p(1));}
Kernels::Poly poly(const dlib::matrix<double>& p) {return Kernels::Poly(p(0), p(1), std::floor(p(2)));}

template<typename K>
dlib::normalized_function<dlib::decision_function<K>>
    do_train(dlib::svm_nu_trainer<K> trainer,
              const std::vector<sample_type>& train_samples,
              const std::vector<double>& train_labels,
              const dlib::matrix<double>& nus,
              const dlib::matrix<double>& params,
              dlib::vector_normalizer<sample_type> normalizer,
              K (*pass_kernel)(const dlib::matrix<double>&)
              );

template<typename K, typename ...P>
K get_kernel(P... args)
{
    return K(args...);
}


void build_classifier(std::set<Pattern>& patterns, const double& prop, const unsigned& nu_num, const double& nu_min)
{
    // slit to 2 input vectors
    std::vector<sample_type> train_samples;
    std::vector<double> train_labels;
    std::vector<sample_type> test_samples;
    std::vector<double> test_labels;
    dlib::vector_normalizer<sample_type> normalizer = split_patterns(
                patterns,
                train_samples,
                train_labels,
                test_samples,
                test_labels
                );
    // do training
    const double max_nu = dlib::maximum_nu(train_labels);
    dlib::matrix<double> nus = dlib::linspace(nu_min, max_nu, nu_num);


    // RBF
    dlib::matrix<double> rbf_params(2,1);
    rbf_params = 1.e-2,
                 1e+1;
    rbf_params = get_kernel_params(rbf_params, 200);

    dlib::svm_nu_trainer<Kernels::RBF> rbf_trainer;
//    do_train(rbf_trainer, train_samples, train_labels, nus, rbf_params, normalizer, &rbf);

    // Linear
    dlib::matrix<double> linear_params(1, 1);
    dlib::svm_nu_trainer<Kernels::Linear> lin_trainer;
//    do_train(lin_trainer, train_samples, train_labels, nus, linear_params, normalizer, &linear);

    // Sigmoid
    dlib::matrix<double> sigmoid_params(2, 2);
    sigmoid_params = 1.e-2, -1.e+1,
                    1.e+1, -1.e-1;
    sigmoid_params = get_kernel_params(sigmoid_params, 200);
    dlib::svm_nu_trainer<Kernels::Sigmoid> sigmoid_trainer;
//    do_train(sigmoid_trainer, train_samples, train_labels, nus, sigmoid_params, normalizer, &sigmoid);

    // Sigmoid
    dlib::matrix<double> poly_params(2, 3);
    poly_params =   1.e-2, -1.e+0, 1.,
                    9.e+0,  1.e+0, 5.e+0;
    poly_params = get_kernel_params(poly_params, 30);
    dlib::svm_nu_trainer<Kernels::Poly> poly_trainer;

    poly_func pf =
    do_train(poly_trainer, train_samples, train_labels, nus, poly_params, normalizer, &poly);
    build_info<Kernels::Poly>(pf, test_samples, test_labels);


    // do test
    // dump test
    // return classifier
    return;
}

template<typename K>
dlib::normalized_function<dlib::decision_function<K>>
do_train(dlib::svm_nu_trainer<K> trainer,
              const std::vector<sample_type>& train_samples,
              const std::vector<double>& train_labels,
              const dlib::matrix<double>& nus,
              const dlib::matrix<double>& params,
              dlib::vector_normalizer<sample_type> normalizer,
              K (*pass_kernel)(const dlib::matrix<double> &)
              )
{
    K best_k;
    double best_nu;
    double best_p = -1;
    for (long i = 0; i < params.nr(); ++i)
    {
        dlib::matrix<double> p(1, params.nc());
        p = dlib::rowm(params, i);
        std::cout << dlib::csv << p << std::endl;
        K k = pass_kernel(p);
        for(const double& nu : nus)
        {
            trainer.set_kernel(k);
            trainer.set_nu(nu);
            auto cvt = dlib::cross_validate_trainer(trainer, train_samples, train_labels, 3);
            double res = dlib::sum(cvt) / 2. * 100;
    #ifdef DEBUG
            std::cout << "nu: " << std::setw(11) << nu
                      << (res > 50? GREEN : RED)
                      << " CV accuarncy: " << std::setw(2) << res << "%" << BLACK << std::endl;
    #endif
            if (res > best_p)
            {
                best_k = k;
                best_nu = nu;
                best_p = res;
            }
        }
    }
    trainer.set_kernel(best_k);
    trainer.set_nu(best_nu);
    typedef dlib::decision_function<K> dec_funct_type;
    typedef dlib::normalized_function<dec_funct_type> funct_type;
    funct_type learned_function;
    learned_function.normalizer = normalizer;
    learned_function.function = trainer.train(train_samples, train_labels);
    std::cout << "basis vectors: " << learned_function.function.basis_vectors.size()
              << std::endl;

    return learned_function;
}

template<typename K>
void build_info(dlib::normalized_function<dlib::decision_function<K>>& df,
                const std::vector<sample_type>& test_samples,
                const std::vector<double>& test_labels)
{
    dlib::matrix<double, 2, 2> confusion_matrix;
    confusion_matrix = 0;
    for (int i = 0; i < test_samples.size(); ++i)
    {
        double label = test_labels[i];
        double predicted_label = df(test_samples[i]);
//        cls = cls/cls * signbit(cls);
        if (predicted_label < 0)
        {
            if (label < 0)
            {
                confusion_matrix(0, 0) += 1;
            } else {
                confusion_matrix(1, 0) += 1;
            }
        } else {
            if (label < 0)
            {
                confusion_matrix(0, 1) += 1;
            } else {
                confusion_matrix(1, 1) += 1;
            }
        }
    }
    std::cout << "Confusion Matrix" << std::endl
              << dlib::csv << confusion_matrix
              << std::endl;

    std::cout << "Accuracy: " << dlib::sum(dlib::diag(confusion_matrix)) / dlib::sum(confusion_matrix) << std::endl;
    std::cout << "Sensivity: " << confusion_matrix(1, 1) / (confusion_matrix(1, 0) + confusion_matrix(1, 1)) << std::endl;
    std::cout << "Specifity: " << confusion_matrix(0, 0) / (confusion_matrix(0, 0) + confusion_matrix(0, 1)) << std::endl;
    std::cout << "Precision: " << confusion_matrix(1, 1) / (confusion_matrix(0, 1) + confusion_matrix(1, 1)) << std::endl;
    return;
}

dlib::matrix<double> get_kernel_params(const dlib::matrix<double>& params, const unsigned& size)
{
    long nc = params.nc();
    dlib::matrix<double> res(size, nc);
    res.set_size(size, nc);
    for (long i = 0; i < nc; ++i)
    {
        dlib::set_colm(res, i) = dlib::trans(dlib::linspace(params(0, i), params(1, i), size));
    }
    return res;
}

dlib::vector_normalizer<sample_type> split_patterns(std::set<Pattern>& patterns,
                    std::vector<sample_type>& train_samples,
                    std::vector<double>& train_labels,
                    std::vector<sample_type>& test_samples,
                    std::vector<double>& test_labels,
                    const double& prop
                    )
{
    std::vector<sample_type> samples;
    std::vector<double> labels;
    std::set<Item> items;

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

    int len = samples.size();
    int pos = double(len) * prop;
    int npos = len - pos;
    train_samples.insert(train_samples.end(), samples.begin(), samples.end() - npos);
    train_labels.insert(train_labels.end(), labels.begin(), labels.end() - npos);

    test_samples.insert(test_samples.end(), samples.begin() + pos, samples.end());
    test_labels.insert(test_labels.end(), labels.begin() + pos, labels.end());

    return normalizer;
}

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
        auto cvt = dlib::cross_validate_trainer(trainer, samples, labels, 3);
        double res = dlib::sum(cvt) / 2. * 100;
#ifdef DEBUG
        std::cout << "nu: " << std::setw(11) << nu
                  << (res > 50? GREEN : RED)
                  << " CV accuarncy: " << std::setw(2) << res << "%" << BLACK << std::endl;
#endif
    }

//    dlib::typedef decision_function<kernel_type> dec_funct_type;
//    dlib::typedef normalized_function<dec_funct_type> funct_type;

//    funct_type learned_func;
//    learned_func.normalizer = normalizer;
//    learned_func.function = trainer.train(samples, labels);

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
