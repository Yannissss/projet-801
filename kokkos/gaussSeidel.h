#ifndef __GAUSS_SEIDEL_H__
#define __GAUSS_SEIDEL_H__

#include <iostream>

#include <opencv2/core.hpp>

#include <Kokkos_Core.hpp>

using namespace std;

using ExecSpace = Kokkos::Cuda;
using ViewMatrixType = Kokkos::View<uchar**[3], ExecSpace>;

bool GaussSeidel(ViewMatrixType &img, int rows, int cols);

#endif
