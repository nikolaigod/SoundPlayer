#pragma once
#include "Sound.h"

template <typename T>
class Silence : public Sound<T> {
public:
    Silence(const size_t duration = 0, const T M = 0): Sound<T>(duration, M){}

    virtual const T& operator[](size_t index) const override {
        return 0;
    }

    virtual Sound<T>* clone() const override { return new Silence(*this); }

    virtual ~Silence() override {}

private:

    Silence<T>& operator=(const Silence<T>* other) = delete;
};