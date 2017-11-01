// CubeLUT.h

#ifndef CubeLUT_H
#define CubeLUT_H

#include <string>
#include <vector>
#include <fstream>

class CubeLUT
{
public:
    typedef std::vector<float> tableRow;
    typedef std::vector<tableRow> table1D;
    typedef std::vector<table1D> table2D;
    typedef std::vector<table2D> table3D;

    enum class LUTState
    {
        OK,
        NOT_INITIALIZEZED,
        WRITE_ERROR
    };

    LUTState status;

    std::string title;
    tableRow domainMin;
    tableRow domainMax;

    table3D LUT3D;

    CubeLUT(void) {status = LUTState::NOT_INITIALIZEZED;};

    LUTState SaveCubeFile(std::ofstream& outfile);
};

#endif //end CubeLUT.h
