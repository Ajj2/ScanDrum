/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_1595683_INCLUDED
#define BINARYDATA_H_1595683_INCLUDED

namespace BinaryData
{
    extern const char*   libopencv_core_dylib;
    const int            libopencv_core_dylibSize = 2170020;

    extern const char*   libopencv_highgui_dylib;
    const int            libopencv_highgui_dylibSize = 65888;

    extern const char*   libopencv_imgproc_dylib;
    const int            libopencv_imgproc_dylibSize = 2447092;

    extern const char*   libLeap_dylib;
    const int            libLeap_dylibSize = 2164592;

    extern const char*   libaubio_5_2_6_dylib;
    const int            libaubio_5_2_6_dylibSize = 124200;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 5;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif
