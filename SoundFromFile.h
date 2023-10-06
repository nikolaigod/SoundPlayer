#pragma once
#include "Silence.h"
#include <fstream>

template<typename T>
class SoundFromFile :public Sound<T> {
public:
    SoundFromFile(const char* fileName, const T& M) : Sound<T>(0, M), data(nullptr) {
        this->readFile(fileName);
    }

    SoundFromFile(const SoundFromFile<T>& other) : Sound<T>(other.duration, other.M), data(nullptr) {
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
        return new SoundFromFile(*this);
    }

    ~SoundFromFile() override{
        delete[] this->data;
    }

private:

    size_t getFileSize(std::ifstream& file) {
        unsigned int currentPosition = file.tellg();
        file.seekg(0, std::ios::end);

        size_t fileSize = file.tellg();
        file.seekg(currentPosition);

        return fileSize;
    }

    void readFile(const char* fileName)
    {
        std::ifstream file(fileName, std::ios::binary);
        if (!file.is_open()) {
            throw std::exception("could not open the file");
        }

        size_t durationTmp = this->getFileSize(file) / sizeof(T);
        T* dataTmp = new(std::nothrow)T[durationTmp];
        if (dataTmp == nullptr) {
            throw std::bad_alloc();
        }

        T tmp;
        size_t i = 0;
        while (file.read(reinterpret_cast<char*>(&tmp), sizeof(tmp))) {
            if (tmp > this->M * (-1) && tmp < this->M) {
                dataTmp[i++] = tmp;
            }
            else {
                if (tmp >= this->M) {
                    dataTmp[i++] = this->M;
                }
                else if (tmp <= this->M * (-1)) {
                    dataTmp[i++] = this->M * (-1);
                }
            }
        }

        this->data = dataTmp;
        this->duration = durationTmp;

        file.close();

    }

    SoundFromFile<T>& operator=(const SoundFromFile<T>& other) = delete;

private:
    T* data;
};


template <typename T>
void saveSoundInFile(const Sound<T>* sound, const char* fileName)
{
    std::ofstream file(fileName, std::ios::binary);
    if (!file)
        throw std::exception("could not open the file");

    for (size_t i = 0; i < sound->getDuration(); i++)
    {
        T sample = sound->operator[](i);
        file.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
    }

    file.close();
}
