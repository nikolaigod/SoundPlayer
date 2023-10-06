#pragma once
#include "Mix.h"

template<typename T>
class Sequence : public Sound<T>
{
public:
    Sequence(Sound<T>** data, const size_t size, const T M) : Sound<T>(0, M), data(nullptr), size(size) {
        this->setData(data, size);
        this->duration = this->findSum();
    }

    Sequence(const Sequence<T>& other):Sound<T>(other.duration, other.M), data(nullptr) {
        Sound<T>** tmp = new(std::nothrow)Sound<T> *[other.size];
        if (tmp == nullptr) {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < other.size; i++) {
            tmp[i] = other.data[i]->clone();
        }
        this->size = other.size;
        this->data = tmp;
    }

    virtual Sound<T>* clone()const override { return new Sequence<T>(*this); }

    virtual const T& operator[](size_t index) const override {
        if (index > this->duration || index < 0) {
            throw std::out_of_range("the given index is out of range");
        }
        T ret = this->getFromConstruct(index);
        return ret;
    }

    ~Sequence() override{
        for (size_t i = 0; i < this->size; i++) {
            delete this->data[i];
        }
        delete[] this->data;
    }


private:

    void setData(Sound<T>** data, size_t size) {
        Sound<T>** tmp = new(std::nothrow)Sound<T> *[size];
        if (tmp == nullptr) {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < size; i++) {
            tmp[i] = data[i]->clone();
        }

        this->data = tmp;
    }

    size_t findSum() const {
        size_t sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += this->data[i]->getDuration();
        }
        return sum;
    }

    T getFromConstruct(size_t index) const{

        size_t counter = 0;
        for (size_t i = 0; i < this->size; i++) {
            for (size_t j = 0; j < this->data[i]->getDuration(); j++) {
                if (counter == index) {
                    T elem = this->data[i]->operator[](j);
                    if (elem < this->M * (-1)) {
                        return this->M * (-1);
                    }
                    else if (elem > this->M) {
                        return this->M;
                    }
                    else return elem;
                }
                counter++;
            }
        }
    }

private:
    Sequence<T>& operator=(const Sequence<T>& other) = delete;
    Sound<T>** data;
    size_t size;
};