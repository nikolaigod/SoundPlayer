#include <iostream>
#include <fstream>
#include "SoundFromFile.h"
#include "PeriodicalSound.h"
#include "Mix.h"
#include "Sequence.h"
#include "Effect.h"
#include "Playlist.h"

int main()
{
    try{
        float arr0[10] = { 1,2,3,4,50,6,7,8,9,10 };
        float arr1[10] = { 2,3,4,5,6,1,2,3,4,5 };
        float arr2[3] = { 1,2,3 };
        Sound<float>* tmp0 = new PeriodicalSound<float>(arr0, 10, 1, 20);
        Sound<float>* tmp1 = new PeriodicalSound<float>(arr1, 10, 1, 20);
        Sound<float>* tmp2 = new PeriodicalSound<float>(arr2, 3, 1, 20);
        Sound<float>** arr3 = new Sound<float>*[3];
        arr3[0] = tmp0;
        arr3[1] = tmp1;
        arr3[2] = tmp2;
        Sound<float>* tmp3 = new Mix<float>(arr3, 3, 20);
        std::cout << tmp3->operator[](3) << std::endl;
        Sound<float>* tmp4 = new Sequence<float>(arr3, 3, 20);
        std::cout << tmp4->operator[](12) << std::endl;
        Transformation<float> transfrom(2);
        Sound<float>* tmp5 = new Effect<float>(tmp0, transfrom, 20);
        std::cout << tmp5->operator[](2) << std::endl;
        Sound<float>* norm = normalize<float>(tmp0, 20);
        saveSoundInFile(tmp0, "nikishef.bin");
        Playlist<float> playlist("shef.txt", 4);

        std::cout << playlist[0].operator[](0);
        std::cout << playlist[1].operator[](0);
        std::cout << playlist[2].operator[](0);
        std::cout << playlist[3].operator[](0);
        playlist.play("finala.bin");
        Sound<float>* tmp6 = new SoundFromFile<float>("finala.bin", 20);
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "playlist:";
        std::cout << std::endl;
        for (size_t i = 0; i < tmp6->getDuration(); i++)
        {
            std::cout << tmp6->operator[](i) << " ";
        }

        for (size_t i = 0; i < 3; i++)
        {
            delete arr3[i];
        }
        delete[] arr3;
        delete tmp3;
        delete tmp4;
        delete tmp5;
        delete tmp6;
        delete norm;
    }
    catch(std::bad_alloc& e){
        std::cout << "Couldnt allocate memory";
    }
    catch(std::out_of_range& e){
        std::cout << e.what();
    }
    catch(std::exception& e){
        std::cout << e.what();
    }
}


