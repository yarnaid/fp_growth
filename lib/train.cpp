#include "train.h"
#include <vector>

sample_type get_sample (const Item& item);
std::set<Item> append_svm_class(std::set<Pattern>& patterns);


void train(std::set<Pattern>& patterns) {
    std::set<Item> items = append_svm_class(patterns);

    std::vector<sample_type> samples(items.size());
    std::vector<double> labels(items.size());

    for (const Item& i : items) {
        samples.push_back(get_sample(i));
        labels.push_back(i.svm_class);
    }

    dlib::vector_normalizer<sample_type> normalizer;
    normalizer.train(samples);

    for (unsigned long i = 0; i < samples.size(); ++i)
        samples[i] = normalizer(samples[i]);

    dlib::randomize_samples(samples, labels);

    dlib::svm_c_trainer<kernel_type> trainer;

    for (double gamma = 1.e-2; gamma <= 1e+1; gamma *= 2) {
        for (double C = 1; C < 1e+7; C *= 5) {
            // tell the trainer the parameters we want to use
            trainer.set_kernel(kernel_type(gamma));
            trainer.set_c(C);

            std::cout << "gamma: " << gamma << "    C: " << C;
            std::cout << "     cross validation accuracy: " << cross_validate_trainer(trainer, samples, labels, 3);
        }
    }

    return;
}

sample_type get_sample (const Item& item) {
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
    return res;
}


std::set<Item> append_svm_class(std::set<Pattern>& patterns) {
    std::set<Item> res;
    for (auto it = patterns.cbegin(); it != patterns.cend(); ++it) {
        const unsigned frequency = it->second;
        std::set<Item> items = it->first;
        for (auto it_items = items.cbegin(); it_items != items.cend(); ++it_items) {
            Item i(*it_items);
//            i.token = i.fqdn;
            int svm_class = 1;
            if (frequency > 3) {
                svm_class = -1;
            }
            i.svm_class = svm_class;
            res.insert(i);
        }
    }
    return res;
}
