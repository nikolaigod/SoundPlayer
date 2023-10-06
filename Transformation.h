#pragma once
#include "Sequence.h"

template<typename T>
class Transformation {
public:
    Transformation(const T ratio): ratio(ratio){}
    T operator()(const T sample, size_t index) const {
        return sample * this->ratio;
    }
private:
    T ratio;
};