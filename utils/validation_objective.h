#ifndef VALITAION_OBJECTIVE_H
#define VALITAION_OBJECTIVE_H

#include <vector>
#include "../dclib-code/dlib/svm.h"

template <typename K>
struct validation_objective
{
    typedef K kernel_type;
    typedef typename K::sample_type sample_type;

public:
    const std::vector<sample_type>& samples;
    const std::vector<double>& labels;

    validation_objective(
            const std::vector<sample_type>& samples_,
            const std::vector<double>& labels_
            ) : samples(samples_), labels(labels_) {}

    double operator() (const dlib::matrix<double>& params) const
    {
        const double gamma = std::exp(params(0));
        const double nu = std::exp(params(1));

        dlib::svm_nu_trainer<kernel_type> trainer;
        trainer.set_kernel(kernel_type(gamma));
        trainer.set_nu(nu);

        dlib::matrix<double> result = dlib::cross_validate_trainer(trainer, samples, labels, 10);
        std::cout << "gamma: " << std::setw(11) << gamma
                  << "nu: " << std::setw(11) << nu
                  << "CV: " << result;

        // return harmonic mean
        return 2 * dlib::prod(result) / dlib::sum(result);
    }
};

#endif // VALITAION_OBJECTIVE_H
