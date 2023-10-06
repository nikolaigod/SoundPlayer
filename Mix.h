#pragma once
#include "PeriodicalSound.h"

template<typename T>
class Mix : public Sound<T>
{
public:
    Mix(Sound<T>** data, const size_t size, const T M): Sound<T>(0, M), data(nullptr), size(size) {
        this->setData(data, size);
        this->duration = this->findMax();
    }

    Mix(const Mix<T>& other):Sound<T>(other.duration, other.M), data(nullptr) {
        Sound<T>** tmp = new(std::nothrow)Sound<T> * [other.size];
        if (tmp == nullptr) {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < other.size; i++) {
            tmp[i] = other.data[i]->clone();
        }
        this->size = other.size;
        this->data = tmp;
    }

    virtual Sound<T>* clone()const override { return new Mix<T>(*this); }

    virtual const T& operator[](size_t index) const override{
        if (index > this->duration || index < 0) {
            throw std::out_of_range("the given index is out of range");
        }
        T ret = this->getFromConstruct(index);
        return ret;
    }

    virtual ~Mix() override{
        for (size_t i = 0; i < this->size; i++) {
            delete this->data[i];
        }
        delete[] this->data;
    }

private:

    void setData(Sound<T>** data, size_t size) {
        Sound<T>** tmp = new(std::nothrow)Sound<T> * [size];
        if (tmp == nullptr) {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < size; i++) {
            tmp[i] = data[i]->clone();
        }

        this->data = tmp;
    }
   
    size_t findMax() const{
        size_t max = 0;
        for (size_t i = 0; i < size; i++) {
            if (this->data[i]->getDuration() > max) {
                max = this->data[i]->getDuration();
            }
        }
        return max;
    }

    T getFromConstruct(size_t index) const{
        
        T avrg = 0;
        for (size_t j = 0; j < this->size; j++) {
            if (index >= this->data[j]->getDuration()) {
                avrg += 0;
            }
            else avrg += this->data[j]->operator[](index);
        }
        T elem = avrg / this->size;
        if (elem < this->M * (-1)) {
            return this->M * (-1);
        }
        else if (elem > this->M) {
            return this->M;
        }
        else return elem;
    }

private:
    Mix<T>& operator=(const Mix<T>& other) = delete;
    Sound<T>** data;
    size_t size;
};
