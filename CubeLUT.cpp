// CubeLUT.cpp

#include "CubeLUT.h"
#include <iostream>
#include <sstream>

CubeLUT::LUTState CubeLUT::SaveCubeFile(std::ofstream& outfile)
{
    if(status != LUTState::OK)
    {
        return status; // Write only good Cubes
    }

    //Write keywords
    const char SPACE = ' ';
    const char QUOTE = '"';

    if(title.size() > 0)
    {
        outfile << "TITLE" << SPACE << QUOTE << title << QUOTE << std::endl;
    }

    outfile << "# Converted by HaldCLUT_to_Cube_Converter" << std::endl;
    outfile << "DOMAIN_MIN" << SPACE << domainMin[0] << SPACE << domainMin[1] << SPACE << domainMin[2] << std::endl;
    outfile << "DOMAIN_MAX" << SPACE << domainMax[0] << SPACE << domainMax[1] << SPACE << domainMax[2] << std::endl;

    //Write LUT data
    if(LUT3D.size() > 0)
    {
        int N = LUT3D.size();
        outfile << "LUT_3D_SIZE" << SPACE << N << std::endl;

        //NOTE that r loops fastest
        for(int b = 0; b < N && outfile.good(); b++)
        {
            for(int g = 0; g < N && outfile.good(); g++)
            {
                for(int r = 0; r < N && outfile.good(); r++ )
                {
                    outfile << LUT3D[r][g][b] [0] << SPACE << LUT3D[r][g][b] [1] << SPACE << LUT3D[r][g][b] [2] << std::endl;
                }
            }
            std::cout << "+";
        }
        std::cout << std::endl;
    } // write 3D LUT

    outfile.flush();

    return(outfile.good()?LUTState::OK:LUTState::WRITE_ERROR);
} // SaveCubeFile
