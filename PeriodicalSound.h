#pragma once
#include "SoundFromFile.h"

template<typename T>
class PeriodicalSound : public Sound<T> {
public:

    PeriodicalSound(const T* data, const size_t size, const size_t repetitions, const T M) :Sound<T>(repetitions * size, M), data(nullptr) {
        this->setData(data, size, repetitions);
    }

    PeriodicalSound(const PeriodicalSound<T>& other) : Sound<T>(other.duration, other.M), data(nullptr) {
        T* temp = new(std::nothrow)T[other.getDuration()];
        if (temp == nullptr) {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < other.getDuration(); i++) {
            temp[i] = other.data[i];
        }
        this->data = temp;
    }

    virtual const T& operator[](size_t index) const override {
        if (index > this->duration || index < 0) {
            throw std::out_of_range("the given index is out of range");
        }

        return this->data[index];
    }

    virtual Sound<T>* clone() const override {
        return new PeriodicalSound(*this);
    }

    ~PeriodicalSound() override{
        delete[] this->data;
    }

private:

    void setData(const T* data, const size_t size, const size_t repetitions) {
        T* dataTmp = new(std::nothrow)T[this->duration];
        if (dataTmp == nullptr) {
            throw std::bad_alloc();
        }

        size_t counter = 0;
        for (size_t i = 0; i < repetitions; i++) {
            for (size_t j = 0; j < size; j++) {
                T tmp = data[j];
                if (tmp > this->M * (-1) && tmp < this->M) {
                    dataTmp[counter++] = tmp;
                }
                else {
                    if (tmp >= this->M) {
                        dataTmp[counter++] = this->M;
                    }
                    else if (tmp <= this->M * (-1)) {
                        dataTmp[counter++] = this->M * (-1);
                    }
                }
            }
        }
        this->data = dataTmp;
    }

private:
    PeriodicalSound<T>& operator=(const PeriodicalSound<T>& other) = delete;
    T* data;
};