#include "train.h"
#include <vector>
#include <iostream>
#include <functional>
#include <fstream>

#define DEBUG

#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define BLACK "\e[0;0m"

std::string test_output = "train_test.csv";

sample_type get_sample (const Item& item);

// typdef for decision functions
typedef dlib::normalized_function<dlib::decision_function<Kernels::Linear>> linear_func;
typedef dlib::normalized_function<dlib::decision_function<Kernels::RBF>> rbf_func;
typedef dlib::normalized_function<dlib::decision_function<Kernels::Sigmoid>> sigmoid_func;
typedef dlib::normalized_function<dlib::decision_function<Kernels::Poly>> poly_func;

// kernel evaluators
Kernels::Linear linear(const dlib::matrix<double>& p) {return Kernels::Linear();}
Kernels::RBF rbf(const dlib::matrix<double>& p) {return Kernels::RBF(p(0));}
Kernels::Sigmoid sigmoid(const dlib::matrix<double>& p) {return Kernels::Sigmoid(p(0), p(1));}
Kernels::Poly poly(const dlib::matrix<double>& p) {return Kernels::Poly(p(0), p(1), std::floor(p(2)));}

// builder of kernal linear params from short bound matrix
dlib::matrix<double> get_kernel_params(const dlib::matrix<double>& params, const unsigned& size);

// output parameters of training
template<typename K>
void build_info(dlib::normalized_function<dlib::decision_function<K>>& df,
                const std::vector<sample_type>& test_samples,
                const std::vector<double>& test_labels);

// splits for training and normalizes input samples
dlib::vector_normalizer<sample_type> split_patterns(std::set<Pattern>& patterns,
                                                    std::vector<sample_type>& train_samples,
                                                    std::vector<double>& train_labels,
                                                    std::vector<sample_type>& test_samples,
                                                    std::vector<double>& test_labels,
                                                    const double& prop=0.8
        );

// train with givven kernel and returns decision function
template<typename K>
dlib::normalized_function<dlib::decision_function<K>>
do_train(dlib::svm_nu_trainer<K> trainer,
         const std::vector<sample_type>& train_samples,
         const std::vector<double>& train_labels,
         const dlib::matrix<double>& nus,
         const dlib::matrix<double>& params,
         dlib::vector_normalizer<sample_type>& normalizer,
         K (*pass_kernel)(const dlib::matrix<double>&),
         const std::string& out_file_name
         );

// main function for classification
void build_classifier(std::set<Pattern>& patterns,
                      const std::vector<Item>& external_items,
                      const double& prop,
                      const unsigned& nu_num,
                      const double& nu_min,
                      const unsigned& kernel_params)
{
    // split to vectors for training and testing
    std::vector<sample_type> train_samples;
    std::vector<double> train_labels;
    std::vector<sample_type> test_samples;
    std::vector<double> test_labels;
    dlib::vector_normalizer<sample_type> normalizer = split_patterns(
                patterns,
                train_samples,
                train_labels,
                test_samples,
                test_labels,
                prop
                );

    // do training
    const double max_nu = 0.999 * dlib::maximum_nu(train_labels);  // nu must not be max
    dlib::matrix<double> nus = dlib::linspace(nu_min, max_nu, nu_num);


    // RBF
    std::cerr << "Begin RBF training" << std::endl;
    dlib::matrix<double> rbf_params(2,1);
    rbf_params = 1.e-2,
            1e+1;
    rbf_params = get_kernel_params(rbf_params, kernel_params);

    dlib::svm_nu_trainer<Kernels::RBF> rbf_trainer;
    rbf_func rf =
            do_train(rbf_trainer, train_samples, train_labels, nus, rbf_params, normalizer, &rbf, "rbf_cv.csv");

    // Linear
    std::cerr << "Begin Linear kernel training" << std::endl;
    dlib::matrix<double> linear_params(1, 1);
    dlib::svm_nu_trainer<Kernels::Linear> lin_trainer;
    linear_func lf =
            do_train(lin_trainer, train_samples, train_labels, nus, linear_params, normalizer, &linear, "linear_cv.csv");

    // Sigmoid
    std::cerr << "Begin Sigmoid kernel training" << std::endl;
    dlib::matrix<double> sigmoid_params(2, 2);
    sigmoid_params = 0.e-1, -2.e+0,
            2.e-1, -0.e-1;
    sigmoid_params = get_kernel_params(sigmoid_params, kernel_params);
    dlib::svm_nu_trainer<Kernels::Sigmoid> sigmoid_trainer;

    sigmoid_func sf =
            do_train(sigmoid_trainer, train_samples, train_labels, nus, sigmoid_params, normalizer, &sigmoid, "sigmoid_cv.csv");

    // Polynomial
    std::cerr << "Begin Polynomial kernel training" << std::endl;
    dlib::matrix<double> poly_params(2, 3);
    poly_params =   1.e-2, -1.e+0, 1.,
                    9.e+0,  1.e+0, 4.e+0;
    poly_params = get_kernel_params(poly_params, std::max(int(double(kernel_params)/10.), 4)); // it's slow and not interesting
    dlib::svm_nu_trainer<Kernels::Poly> poly_trainer;

    poly_func pf =
            do_train(poly_trainer, train_samples, train_labels, nus, poly_params, normalizer, &poly, "poly_cv.csv");



    // do test and quality estimations
    std::cout << "RBF SVM RESULTS" << std::endl;
    build_info<Kernels::RBF>(rf, test_samples, test_labels);
    std::cout << "LINEAR KERNEL SVM RESULTS" << std::endl;
    build_info<Kernels::Linear>(lf, test_samples, test_labels);
    std::cout << "SIGMOID KERNEL SVM RESULTS" << std::endl;
    build_info<Kernels::Sigmoid>(sf, test_samples, test_labels);
    std::cout << "POLYNOMIAL KERNEL SVM RESULTS" << std::endl;
    build_info<Kernels::Poly>(pf, test_samples, test_labels);


    // test on input items if any
    if (!external_items.empty())
    {
        std::ofstream out_file;
        out_file.open(test_output);
        if (out_file.is_open())
        {
            std::vector<sample_type> external_vector;
            external_vector.reserve(external_items.size());
            out_file << "LINEAR,RBF,SIGMOID,POLYNOMIAL,ITEM" << std::endl;
            for (const Item& item : external_items)
            {
                const sample_type& sample = get_sample(item);

                out_file << std::signbit(lf(sample)) << ","
                         << std::signbit(rf(sample)) << ","
                         << std::signbit(sf(sample)) << ","
                         << std::signbit(pf(sample)) << ","
                         << item << std::endl;
            }
            out_file.close();
        }
    }

    return;
}

template<typename K>
dlib::normalized_function<dlib::decision_function<K>>
do_train(dlib::svm_nu_trainer<K> trainer,
         const std::vector<sample_type>& train_samples,
         const std::vector<double>& train_labels,
         const dlib::matrix<double>& nus,
         const dlib::matrix<double>& params,
         dlib::vector_normalizer<sample_type>& normalizer,
         K (*pass_kernel)(const dlib::matrix<double> &),
         const std::string& out_file_name
         )
{
    dlib::matrix<double> best_pp(1, params.nc());
    double best_nu = -1;
    double best_p = -1;
    for (long i = 0; i < params.nr(); ++i)
    {
        dlib::matrix<double> p(1, params.nc());
        p = dlib::rowm(params, i);
#ifdef DEBUG
        std::cout << dlib::csv << p;
#endif //DEBUG
        K k = pass_kernel(p);
        trainer.set_kernel(k);
        for(const double& nu : nus)
        {
            trainer.set_nu(nu);
            auto cvt = dlib::cross_validate_trainer(trainer, train_samples, train_labels, 5);
            double res = dlib::sum(cvt) / 2. * 100;
#ifdef DEBUG
            std::cout << "nu: " << std::setw(11) << nu
                      << (res > 50? GREEN : RED)
                      << " CV accuarcy: " << std::setw(2) << res << "%" << BLACK << std::endl;
#endif //DEBUG
            if (res > best_p)
            {
                best_pp = p;
                best_nu = nu;
                best_p = res;
#ifdef DEBUG
                std::cout << RED << "NEW NU " << best_nu << " NEW P " << best_p << BLACK << std::endl;
#endif //DEBUG
            }
        }
    }
    trainer.set_kernel(pass_kernel(best_pp));
    trainer.set_nu(best_nu);
    typedef dlib::decision_function<K> dec_funct_type;
    typedef dlib::normalized_function<dec_funct_type> funct_type;
    funct_type learned_function;
    learned_function.normalizer = normalizer;
    learned_function.function = trainer.train(train_samples, train_labels);
#ifdef DEBUG
    std::cout << "basis vectors: " << learned_function.function.basis_vectors.size()
              << std::endl;
    std::cout << "best nu: " << best_nu << std::endl;
    std::cout << "best p: " << best_p << std::endl;
#endif //DEBUG


    std::ofstream out_file;
    out_file.open(out_file_name);
    if (out_file.is_open())
    {
        out_file << "trained,labeled,item" << std::endl;
    for (int i = 0; i < train_samples.size(); ++i)
    {
        const sample_type& sample = train_samples[i];
        out_file << std::signbit(learned_function(sample)) << ","
                 << train_labels[i] << ",";
        for (int j = 0; j < sample.size() - 1; ++j)
        {
            out_file << sample(j) << ",";
        }
        out_file << sample(sample.size() - 1) << std::endl;
    }
    out_file.close();
    }

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
              << dlib::csv << confusion_matrix;

    std::cout << "Accuracy: "
              << dlib::sum(dlib::diag(confusion_matrix)) / dlib::sum(confusion_matrix) << std::endl;
    std::cout << "Sensivity: "
              << confusion_matrix(1, 1) / (confusion_matrix(1, 0) + confusion_matrix(1, 1)) << std::endl;
    std::cout << "Specifity: "
              << confusion_matrix(0, 0) / (confusion_matrix(0, 0) + confusion_matrix(0, 1)) << std::endl;
    std::cout << "Precision: "
              << confusion_matrix(1, 1) / (confusion_matrix(0, 1) + confusion_matrix(1, 1)) << std::endl;

    std::cout << std::endl;
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

    dlib::randomize_samples(samples, labels);

    int len = samples.size();
    int pos = double(len) * prop;
    int npos = len - pos;
    train_samples.insert(train_samples.end(), samples.begin(), samples.end() - npos);
    train_labels.insert(train_labels.end(), labels.begin(), labels.end() - npos);

    test_samples.insert(test_samples.end(), samples.begin() + pos, samples.end());
    test_labels.insert(test_labels.end(), labels.begin() + pos, labels.end());

    for (unsigned long i = 0; i < train_samples.size(); ++i)
        train_samples[i] = normalizer(train_samples[i]);


    return normalizer;
}


sample_type get_sample (const Item& item)
{
    sample_type res;
    res = 0;
    res(0) = item.time;
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
    //    res(11) = item.svm_class;  // little testing hack
    res(12) = double(item.q_number);
    res(13) = double(item.q_length);
    res(14) = double(item.url_length);
    res(15) = double(item.domain_length);
    return res;
}
