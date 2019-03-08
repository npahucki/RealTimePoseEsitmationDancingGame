//
// Created by Nathan  Pahucki on 10/31/18.
//

#include "PersonNamer.h"
#include <fstream>
#include <string>
#include <list>

PersonNamer::PersonNamer() {
    std::ifstream file("./cpp/assets/names.txt");
    std::string s;
    while (std::getline(file, s))
        names_.push_back(s);
}

std::string PersonNamer::getNextName() {
    auto begin = names_.begin();
    const unsigned long n = static_cast<const unsigned long>(std::distance(begin, names_.end()));
    const unsigned long divisor = names_.size() / n;

    unsigned long k;
    do { k = std::rand() / divisor; } while (k >= n);
    std::advance(begin, k);
    return *begin;
}

