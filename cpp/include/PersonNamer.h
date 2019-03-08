//
// Created by Nathan  Pahucki on 10/31/18.
//

#ifndef GESTURES_PERSONNAMER_H
#define GESTURES_PERSONNAMER_H


#include <string>
#include <vector>

class PersonNamer {

private:
    std::vector<std::string> names_;

public:
    PersonNamer();

    std::string getNextName();
};


#endif //GESTURES_PERSONNAMER_H
