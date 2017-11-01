/**
Copyright (c) 2017 Martin Busse

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>

#include "IL/il.h"

#include "CubeLUT.h"

float *temp_cube_array;

void ReadConvertAndSave(char* filename, bool full_res);

int main(int argc, char* argv[])
{
    std::cout << "HaldCLUT to Cube Converter" << std::endl;

    bool high_res = false;

    if(argc < 2)
    {
        std::cout << std::endl << "Missing argument(s)" << std::endl;
        std::cout << "Usage: HaldCLUT_to_Cube [-high_res] 1.png [2.png ...]" << std::endl;
        std::cout << "HaldCLUT PNG format: 1728px * 1728px RGB" << std::endl;

        std::cout << "Press enter";
        std::cin.ignore();

        exit(EXIT_FAILURE);
        return 0;
    }

    ilInit();

    int first_file = 1;

    if(std::string(argv[1]).compare(std::string("-high_res")) == 0)
    {
        first_file = 2;
        high_res = true;
    }

    temp_cube_array = new float[144 * 144 * 144 * 3]; //36 MB
    if(temp_cube_array == nullptr)
    {
        std::cout << "Memory error!";
        std::cin.ignore();
    }

    std::cout << "Number of arguments/files = " << (argc - 1) << std::endl;
    for(int i = first_file; i < argc; i++)
    {
        std::cout << std::endl << std::endl << "argument/file[" << i << "] = " << argv[i] << std::endl;
        ReadConvertAndSave(argv[i], high_res);
    }

    delete[] temp_cube_array;

    std::cout << "Press enter";
    std::cin.ignore();

    return 0;
}

void ReadConvertAndSave(char* filename, bool high_res = false)
{
    //ILboolean result = ilLoadImage(filename);
    //ILboolean result = ilLoad(IL_PNG, filename);

    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if(in == nullptr)
    {
        std::cout << "file open failed" << std::endl;
        return;
    }

    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();

    ILuint imgId = 0;
    ilGenImages(1, &imgId);
    ilBindImage(imgId);

    ILboolean result = ilLoadL(IL_PNG, contents.str().data(), contents.str().length());

    if(result)
    {
        std::cout << "the image loaded successfully" << std::endl;
    }
    else
    {
        std::cout << "The image failed to load" << std::endl;

        ILenum err = ilGetError() ;
        std::cout << "the error " << err << std::endl;
        std::cout << "string is " << ilGetString(err) << std::endl;
    }

    int size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
    std::cout << "Data size: " << size << std::endl;

    CubeLUT cube;

    if((ilGetInteger(IL_IMAGE_WIDTH) != 1728) || (ilGetInteger(IL_IMAGE_HEIGHT) != 1728))
    {
        std::cout << "Error: Wrong image size (must be 1728 x 1728 px)" << std::endl;

        ilBindImage(0);
        ilDeleteImage(imgId);

        return;
    }

    // Set defaults
    cube.status = CubeLUT::LUTState::OK;
    cube.title.clear();
    cube.domainMin = CubeLUT::tableRow(3, 0.0);
    cube.domainMax = CubeLUT::tableRow(3, 1.0);
    cube.LUT3D.clear();

    if(high_res)
    {
        cube.LUT3D = CubeLUT::table3D(72, CubeLUT::table2D(72, CubeLUT::table1D(72, CubeLUT::tableRow(3))));
    }
    else
    {
        cube.LUT3D = CubeLUT::table3D(36, CubeLUT::table2D(36, CubeLUT::table1D(36, CubeLUT::tableRow(3))));
    }

    //!!!temp cube index...
    int temp_cube_index = 0;

    ILubyte* pixeldata_8 = nullptr;
    ILushort* pixeldata_16 = nullptr;
    float temp_value;

    switch(ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL))
    {
    case 1: //L8
        pixeldata_8 = ilGetData();
        //Warning: only 8 bit per colour-channel
        std::cout << "Warning: Only 8 bit per colour channel" << std::endl;

        for(int i = 0; i < (144 * 144 * 144); i++)
        {
            temp_value = (*pixeldata_8++) / 255.f;

            temp_cube_array[temp_cube_index++] = temp_value;
            temp_cube_array[temp_cube_index++] = temp_value;
            temp_cube_array[temp_cube_index++] = temp_value;

            if((i % (144 * 144)) == 0)
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
        break;

    case 2: //L16
        pixeldata_16 = (ILushort*)ilGetData();

        for(int i = 0; i < (144 * 144 * 144); i++)
        {
            temp_value = (*pixeldata_16++) / 65535.f;

            temp_cube_array[temp_cube_index++] = temp_value;
            temp_cube_array[temp_cube_index++] = temp_value;
            temp_cube_array[temp_cube_index++] = temp_value;

            if((i % (144 * 144)) == 0)
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
        break;

    case 3: //RGB8
        pixeldata_8 = ilGetData();
        //Warning: only 8 bit per colour-channel
        std::cout << "Warning: Only 8 bit per colour channel" << std::endl;

        for(int i = 0; i < (144 * 144 * 144); i++)
        {
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_8++) / 255.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_8++) / 255.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_8++) / 255.f;

            if((i % (144 * 144)) == 0)
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
        break;

    case 4: //RGBA8
        pixeldata_8 = ilGetData();
        //Warning: only 8 bit per colour-channel
        std::cout << "Warning: Only 8 bit per colour channel" << std::endl;

        for(int i = 0; i < (144 * 144 * 144); i++)
        {
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_8++) / 255.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_8++) / 255.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_8++) / 255.f;
            ++pixeldata_8;

            if((i % (144 * 144)) == 0)
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
        break;

    case 6: //RGB16
        pixeldata_16 = (ILushort*)ilGetData();

        for(int i = 0; i < (144 * 144 * 144); i++)
        {
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_16++) / 65535.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_16++) / 65535.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_16++) / 65535.f;

            if((i % (144 * 144)) == 0)
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
        break;

    case 8: //RGBA16
        pixeldata_16 = (ILushort*)ilGetData();

        for(int i = 0; i < (144 * 144 * 144); i++)
        {
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_16++) / 65535.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_16++) / 65535.f;
            temp_cube_array[temp_cube_index++] = (float)(*pixeldata_16++) / 65535.f;
            ++pixeldata_16;

            if((i % (144 * 144)) == 0)
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
        break;

    default:
        std::cout << "Error: Pixel not RGB(A)/BW, 8/16 bit per colour channel" << std::endl;

        ilBindImage(0);
        ilDeleteImage(imgId);

        return;
    }


    temp_cube_index = 0;

    if(high_res) // 72 ^ 3
    {
        int base_index;
        int nr = 1 * 3;
        int ng = 144 * 3;
        int nb = 144 * 144 * 3;

        for(int b = 0; b < 72; b++)
        {
            for(int g = 0; g < 72; g++)
            {
                for(int r = 0; r < 72; r++)
                {
                    for(int i = 0; i < 3; i++)
                    {
                        base_index = ((r * 2) + (g * 2) * 144 + (b * 2) * 144 * 144) * 3;

                        cube.LUT3D[r][g][b][i] = (temp_cube_array[base_index                + i] +
                                                  temp_cube_array[base_index + nr           + i] +
                                                  temp_cube_array[base_index      + ng      + i] +
                                                  temp_cube_array[base_index + nr + ng      + i] +
                                                  temp_cube_array[base_index           + nb + i] +
                                                  temp_cube_array[base_index + nr      + nb + i] +
                                                  temp_cube_array[base_index      + ng + nb + i] +
                                                  temp_cube_array[base_index + nr + ng + nb + i]) * 0.125f;
                    }
                }
            }
            std::cout << "#";
        }
        std::cout << std::endl;
    }
    else // 36 ^ 3
    {
        float *temp_cube_array_low_res = new float[72 * 72 * 72 * 3]; //560 KB
        if(temp_cube_array_low_res == nullptr)
        {
            std::cout << "Memory error!";
            std::cin.ignore();
        }

        int base_index;
        int low_res_cube_index = 0;
        int nr = 1 * 3;
        int ng = 144 * 3;
        int nb = 144 * 144 * 3;

        for(int b = 0; b < 72; b++)
        {
            for(int g = 0; g < 72; g++)
            {
                for(int r = 0; r < 72; r++)
                {
                    for(int i = 0; i < 3; i++)
                    {
                        base_index = ((r * 2) + (g * 2) * 144 + (b * 2) * 144 * 144) * 3;

                        temp_cube_array_low_res[low_res_cube_index++] = (temp_cube_array[base_index                + i] +
                                                                         temp_cube_array[base_index + nr           + i] +
                                                                         temp_cube_array[base_index      + ng      + i] +
                                                                         temp_cube_array[base_index + nr + ng      + i] +
                                                                         temp_cube_array[base_index           + nb + i] +
                                                                         temp_cube_array[base_index + nr      + nb + i] +
                                                                         temp_cube_array[base_index      + ng + nb + i] +
                                                                         temp_cube_array[base_index + nr + ng + nb + i]) * 0.125f;
                    }
                }
            }
            std::cout << "#";
        }
        std::cout << std::endl;


        nr = 1 * 3;
        ng = 72 * 3;
        nb = 72 * 72 * 3;

        for(int b = 0; b < 36; b++)
        {
            for(int g = 0; g < 36; g++)
            {
                for(int r = 0; r < 36; r++)
                {
                    for(int i = 0; i < 3; i++)
                    {
                        base_index = ((r * 2) + (g * 2) * 72 + (b * 2) * 72 * 72) * 3;

                        cube.LUT3D[r][g][b][i] = (temp_cube_array_low_res[base_index                + i] +
                                                  temp_cube_array_low_res[base_index + nr           + i] +
                                                  temp_cube_array_low_res[base_index      + ng      + i] +
                                                  temp_cube_array_low_res[base_index + nr + ng      + i] +
                                                  temp_cube_array_low_res[base_index           + nb + i] +
                                                  temp_cube_array_low_res[base_index + nr      + nb + i] +
                                                  temp_cube_array_low_res[base_index      + ng + nb + i] +
                                                  temp_cube_array_low_res[base_index + nr + ng + nb + i]) * 0.125f;
                    }
                }
            }
            std::cout << "+";
        }
        std::cout << std::endl;

        delete[] temp_cube_array_low_res;
    }

    std::string new_file_name(filename);
    new_file_name = new_file_name.substr(0, new_file_name.length() - 4); //remove ".png"
    new_file_name += ".cube";

    std::ofstream outfile(new_file_name, std::fstream::trunc);
    if(!outfile.good())
    {
        std::cout << "Could not open output file " << new_file_name << std::endl;

        ilBindImage(0);
        ilDeleteImage(imgId);

        return;
    }

    CubeLUT::LUTState ret = cube.SaveCubeFile(outfile);
    outfile.close();
    if(ret != CubeLUT::LUTState::OK)
    {
        std::cout << "Could not write the cube to the output file." << std::endl;

        ilBindImage(0);
        ilDeleteImage(imgId);

        return;
    }

    std::cout << "File saved: = " << new_file_name << std::endl;

    //clean up devIL image/memory???
    ilBindImage(0);
    ilDeleteImage(imgId);
}
