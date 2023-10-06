#pragma once
#include <iostream>
#include <string>

template <typename T>
class Sound{
public:
    Sound(const size_t duration = 0, const T M = 0): duration(duration), M(M){}

    const virtual T& operator[](size_t index) const = 0;

    const T getDuration() const { return this->duration; }

    virtual Sound<T>* clone() const = 0;

    virtual ~Sound(){}

protected:
    size_t duration;
    T M;
    Sound<T>& operator=(const Sound<T>& other) = delete;
};