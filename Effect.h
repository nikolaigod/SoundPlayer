#pragma once
#include "Transformation.h"

template<typename T>
class Effect : public Sound<T> {
public:
	Effect(Sound<T>* sound, const Transformation<T> transform, const T M):Sound<T>(sound->getDuration(), M), transform(transform){
		this->sound = sound->clone();
	}

	Effect(const Effect& other):Sound<T>(other.getDuration(), other.M), transform(other.transform) {
		this->sound = other.sound->clone();
	}

	virtual const T& operator[](size_t index) const override {
		if (index > this->duration || index < 0) {
			throw std::out_of_range("the given index is out of range");
		}
		T val = this->transform(this->sound->operator[](index), index);
		if (val > this->M) {
			return this->M;
		}
		else if (val < this->M * -1) {
			return this->M * -1;
		}
		else return val;
	}

	virtual Sound<T>* clone()const override { return new Effect<T>(*this); }

	virtual ~Effect() override {
		delete this->sound;
	}

private:
	Effect<T>& operator=(const Effect<T>& other) = delete;
	Transformation<T> transform;
	Sound<T>* sound;
};

template<typename T>
Sound<T>* normalize(Sound<T>* sound, const T M) {
	T max = 0;
	for (size_t i = 0; i < sound->getDuration(); i++)
	{
		T elem = sound->operator[](i);
		if (elem < 0) {
			if (max < (elem * -1)) {
				max = (elem * -1);
			}
		}
		else {
			if (max < elem) {
				max = elem;
			}
		}
	}

	T power = M / max;
	Transformation<T> transformNew(power);
	Sound<T>* effect = new(std::nothrow)Effect<T>(sound, transformNew, M);
	if (effect == nullptr) {
		throw std::bad_alloc();
	}
	return effect;
}

