#pragma once
#include "Effect.h"
#include <cstdio>

template<typename T>
class Playlist {
public:
	Playlist(const char* fileName, const size_t size) {
		this->data = new(std::nothrow)Sound<T>*[size];
		if (this->data == nullptr) {
			throw std::bad_alloc();
		}
		try {
			this->setData(fileName);
		}
		catch (std::bad_alloc()) {
			this->free();
		}
	}

	Playlist(const Playlist<T>& other) {
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

	Sound<T>& operator[](const size_t index) {
		if (index > this->size || index < 0) {
			throw std::out_of_range("the given index is out of range");
		}
		return *this->data[index];
	}

	void play(const char* fileName) {
		Sound<T>* sequence = new(std::nothrow)Sequence<T>(this->data, this->size, 20);
		saveSoundInFile(sequence, fileName);
	}

	~Playlist() {
		this->free();
	}

	void free() {
		for (size_t i = 0; i < this->size; i++) {
			delete this->data[i];
		}

		delete[] this->data;
	}

private:

	void setData(const char* fileName){
		std::ifstream file(fileName);
		if (!file.is_open()) {
			throw std::exception("could not open the file");
		}

		char buffer[100] = {'\0'};
		size_t countSounds = 0;
		while (file.getline(buffer, 100)) {
			bool isFile = false;
			size_t counterForNow = 0;
			while (buffer[counterForNow] != ' ' && buffer[counterForNow] != '\0') {
				if (!std::isdigit(buffer[counterForNow])) {
					if (buffer[counterForNow] != '.') {
						isFile = true;
						break;
					}
				}
				counterForNow++;
			}
			if (isFile) {  
				size_t counter = 0;
				char songFile[100] = { '\0' };

				while (buffer[counter] != ' ') {
					songFile[counter] = buffer[counter];
					counter++;
				}
				counter++;

				size_t digitCounter = counter;
				bool isDouble = false;
				while (buffer[digitCounter] != ' ') {
					if (buffer[digitCounter] == '.') {
						isDouble = true;
						break;
					}
					digitCounter++;
				}

				if (isDouble) { //song
					this->data[countSounds++] = this->getSong(buffer, counter, songFile);
					this->size++;
				}
				else { //fadein
					this->data[countSounds++] = this->getFadeIn(buffer, counter, songFile);
					this->size++;
				}

			}
			else {
				size_t counter = 0;
				bool isPause = false;
				while (buffer[counter] != ' ') {
					if (buffer[counter] == '\0') {
						isPause = true;
						break;
					}
					counter++;
				}
				counter = 0;

				if (isPause) {  //pause
					this->data[countSounds++] = this->getPause(buffer, counter);
					this->size++;
				}
				else { //prelivka

					unsigned A = this->getInt(buffer, counter);
					unsigned B = this->getInt(buffer, counter);
					float C = this->getFloat(buffer, counter);
					unsigned diff = A - B;
					float span = 1 - C;
					float power = span / (diff - 1);
					T* tmp1 = new(std::nothrow)T[diff];
					if (tmp1 == nullptr) {
						throw std::bad_alloc();
					}
					size_t counter1 = this->data[countSounds - 1]->getDuration() - A;
					for (size_t i = 0; i < diff ; i++) {
						tmp1[i] = this->data[countSounds - 1]->operator[](counter1);
						counter1++;
					}
					float increaser = 1;
					for (size_t i = 0; i < diff; i++) {
						tmp1[i] *= increaser;
						increaser -= power;
					}
					T M = 0;
					std::cout << "Enter M\n";
					std::cin >> M;
					Sound<T>* s1 = new(std::nothrow)PeriodicalSound<T>(tmp1, diff, 1, M);
					if (s1 == nullptr) {
						delete[] tmp1;
						throw std::bad_alloc();
					}
					delete[] tmp1;

					float power2 = C / (B - 1);
					T* tmp2 = new(std::nothrow)T[B];
					if (tmp2 == nullptr) {
						delete s1;
						throw std::bad_alloc();
					}
					size_t counter3 = this->data[countSounds - 1]->getDuration() - B;
					for (size_t i = 0; i < B; i++) {
						tmp2[i] = this->data[countSounds - 1]->operator[](counter3);
						counter3++;
					}
					float increaser2 = C;
					for (size_t i = 0; i < B; i ++) {
						tmp2[i] *= increaser2;
						increaser2 -= power2;
					}
					Sound<T>* s2 = new(std::nothrow)PeriodicalSound<T>(tmp2, B, 1, M);
					if (s2 == nullptr) {
						delete s1;
						delete[] tmp2;
						throw std::bad_alloc();
					}
					delete[] tmp2;


					T* tmp3 = new(std::nothrow)T[B];
					if (tmp3 == nullptr) {
						delete s1;
						delete s2;
						throw std::bad_alloc();
					}

					char buffer2[100] = { '\0' };
					file.getline(buffer2, 100);
					size_t counter2 = 0;
					char songFile[100] = { '\0' };

					while (buffer2[counter2] != ' ') {
						songFile[counter2] = buffer2[counter2];
						counter2++;
					}
					counter2++;

					size_t digitCounter = counter2;
					bool isDouble = false;
					while (buffer2[digitCounter] != ' ') {
						if (buffer2[digitCounter] == '.') {
							isDouble = true;
							break;
						}
						digitCounter++;
					}

					if (isDouble) { 
						Sound<T>* song = this->getSong(buffer2, counter2, songFile);
						for (size_t i = 0; i < B; i++) {
							tmp3[i] = song->operator[](i);
						}
						float increaser3 = 0;
						for (size_t i = 0; i < B; i++) {
							tmp3[i] *= increaser3;
							increaser3 += power2;
						}
						Sound<T>* s3 = new(std::nothrow)PeriodicalSound<T>(tmp3, B, 1, M);
						if (s3 == nullptr) {
							delete song;
							delete s1;
							delete s2;
							delete[] tmp3;
							throw std::bad_alloc();
						}
						delete[] tmp3;
						Sound<T>** arr1 = new(std::nothrow)Sound<T>*[2];
						if (arr1 == nullptr) {
							delete song;
							delete s1;
							delete s2;
							delete s3;
							throw std::bad_alloc();
						}
						arr1[0] = s2;
						arr1[1] = s3;
						Sound<T>* mix = new(std::nothrow)Mix<T>(arr1, 2, M);
						if (mix == nullptr) {
							delete song;
							delete s1;
							delete s2;
							delete s3;
							delete[] arr1;
							throw std::bad_alloc();
						}
						Sound<T>** arr2 = new(std::nothrow)Sound<T>*[2];
						if (arr2 == nullptr) {
							delete song;
							delete s1;
							delete s2;
							delete s3;
							delete[] arr1;
							delete mix;
							throw std::bad_alloc();
						}
						arr2[0] = s1;
						arr2[1] = mix;
						Sound<T>* sequence = new(std::nothrow)Sequence<T>(arr2, 2, M);
						if (sequence == nullptr) {
							delete song;
							delete s1;
							delete s2;
							delete s3;
							delete[] arr1;
							delete mix;
							delete[] arr2;
							throw std::bad_alloc();
						}
						this->data[countSounds++] = sequence;
						this->data[countSounds++] = song;
						this->size += 2;
						for (size_t i = 0; i < 2; i++) {
							delete arr1[i];
							delete arr2[i];
						}
						delete[] arr1;
						delete[] arr2;
					}
					else { 
						Sound<T>* fadeIn = this->getFadeIn(buffer2, counter2, songFile);
						for (size_t i = 0; i < B; i++) {
							tmp3[i] = fadeIn->operator[](i);
						}

						float power3 = C / (B - 1);
						float increaser3 = 0;
						for (size_t i = 0; i < B; i++) {
							tmp3[i] *= increaser3;
							increaser3 += power3;
						}
						Sound<T>* s3 = new(std::nothrow)PeriodicalSound<T>(tmp3, B, 1, M);
						if (s3 == nullptr) {
							delete fadeIn;
							delete s1;
							delete s2;
							delete[] tmp3;
							throw std::bad_alloc();
						}
						delete[] tmp3;
						Sound<T>** arr1 = new(std::nothrow)Sound<T>*[2];
						if (arr1 == nullptr) {
							delete fadeIn;
							delete s1;
							delete s2;
							delete s3;
							throw std::bad_alloc();
						}
						arr1[0] = s2;
						arr1[1] = s3;
						Sound<T>* mix = new(std::nothrow)Mix<T>(arr1, 2, M);
						if (mix == nullptr) {
							delete fadeIn;
							delete s1;
							delete s2;
							delete s3;
							delete[] arr1;
							throw std::bad_alloc();
						}
						Sound<T>** arr2 = new(std::nothrow)Sound<T>*[2];
						if (arr2 == nullptr) {
							delete fadeIn;
							delete s1;
							delete s2;
							delete s3;
							delete[] arr1;
							delete mix;
							throw std::bad_alloc();
						}
						arr2[0] = s1;
						arr2[1] = mix;
						Sound<T>* sequence = new(std::nothrow)Sequence<T>(arr2, 2, M);
						if (sequence == nullptr) {
							delete fadeIn;
							delete s1;
							delete s2;
							delete s3;
							delete[] arr1;
							delete mix;
							delete[] arr2;
							throw std::bad_alloc();
						}
						this->data[countSounds++] = sequence;
						this->data[countSounds++] = fadeIn;
						this->size += 2;
						for (size_t i = 0; i < 2; i++) {
							delete arr1[i];
							delete arr2[i];
						}
						delete[] arr1;
						delete[] arr2;
					}
				}

			}
		}
	}

	unsigned getInt(const char* buffer, size_t& counter) {
		char intChar[10] = { '\0' };
		size_t i = 0;
		while (buffer[counter] != ' ' && buffer[counter] != '\0') {
			intChar[i++] = buffer[counter++];
		}
		unsigned samples = std::stoi(intChar);
		counter++;
		return samples;
	}

	float getFloat(const char* buffer, size_t& counter) {
		char floatChar[10] = { '\0' };
		size_t j = 0;
		while (buffer[counter] != ' ' && buffer[counter] != '\0') {
			floatChar[j++] = buffer[counter++];
		}
		float samples = std::stof(floatChar);
		counter++;
		return samples;
	}

	Sound<T>* getSong(const char* buffer, size_t& counter, const char* songFile) {
		float val = this->getFloat(buffer, counter);
		T M = 0;
		std::cout << "Enter M\n";
		std::cin >> M;
		Sound<T>* sound = new(std::nothrow)SoundFromFile<T>(songFile, M);
		if (sound == nullptr) {
			throw std::bad_alloc();
		}
		Transformation<T> transform(val);
		Sound<T>* effect = new Effect<T>(sound, transform, M);
		if (effect == nullptr) {
			delete sound;
			throw std::bad_alloc();
		}
		delete sound;
		return effect;
	}

	Sound<T>* getPause(const char* buffer, size_t& counter) {
		unsigned val = this->getInt(buffer, counter);
		T M = 0;
		std::cout << "Enter M\n";
		std::cin >> M;
		Sound<T>* silence = new(std::nothrow)Silence<T>(val, M);
		if (silence == nullptr) {
			throw std::bad_alloc();
		}
		return silence;
	}

	Sound<T>* getFadeIn(const char* buffer, size_t& counter, const char* songFile) {
		unsigned samples = this->getInt(buffer, counter);
		float boundary = this->getFloat(buffer, counter);
		T M = 0;
		std::cout << "Enter M\n";
		std::cin >> M;
		Sound<T>* sound = new(std::nothrow)SoundFromFile<T>(songFile, M);
		if (sound == nullptr) {
			throw std::bad_alloc();
		}
		float power = boundary / (samples - 1);
		T* tmp = new(std::nothrow)T[sound->getDuration()];
		if (tmp == nullptr) {
			delete sound;
			throw std::bad_alloc();
		}
		float increaser = 0;
		for (size_t i = 0; i < samples; i++) {
			tmp[i] = sound->operator[](i) * increaser;
			increaser += power;
		}
		Sound<T>* periodical = new(std::nothrow)PeriodicalSound<T>(tmp, samples, 1, M);
		if (periodical == nullptr) {
			delete[] tmp;
			delete sound;
			throw std::bad_alloc();
		}
		delete[] tmp;
		delete sound;
		return periodical;
	}

private:
	Sound<T>** data;
	size_t size;
};