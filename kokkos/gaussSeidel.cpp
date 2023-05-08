#include "gaussSeidel.h"

bool GaussSeidel(ViewMatrixType &img, ViewMatrixType &dst, int rows, int cols)
{
    if (rows == 0 || cols == 0) {
        cout << "[Error]! Input Image Empty!";
        return false;
    }

    if (cols % 2 != 0) {
        cout << "[Error]! #ofCols must be even!"
             << endl;
        return false;
    }

    Kokkos::MDRangePolicy< Kokkos::Rank<2>, ExecSpace > policy({0,0}, {rows, cols / 2});
        
    // Rouge
    Kokkos::parallel_for(policy, KOKKOS_LAMBDA(const int i, int j) {
        j = j * 2 + i % 2;
        // Parcours des channels
        for (int c = 0; c < 3; c++) {
            unsigned int alpha = 0;

            int count = 1;
            alpha += img(i, j, c);          // Centre
            if (j > 0) {
                alpha += img(i, j - 1, c);  // Ouest
                count++;
            }
            if (j < cols - 1) {
                alpha += img(i, j + 1, c);  // Est
                count++;
            }
            if (i > 0) {
                alpha += img(i - 1, j, c);  // Sud                
                count++;
            }
            if (i < rows - 1) {
                alpha += img(i + 1, j, c);  // Nord
                count++;
            }

            dst(i, j, c) = alpha / count;
        }
    });

    // Noir
    Kokkos::parallel_for(policy, KOKKOS_LAMBDA(const int i, int j) {
        j = j * 2 + (i + 1) % 2;
        // Parcours des channels
        for (int c = 0; c < 3; c++) {
            unsigned int alpha = 0;

            int count = 1;
            alpha += img(i, j, c);          // Centre
            if (j > 0) {
                alpha += dst(i, j - 1, c);  // Ouest
                count++;
            }
            if (j < cols - 1) {
                alpha += dst(i, j + 1, c);  // Est
                count++;
            }
            if (i > 0) {
                alpha += dst(i - 1, j, c);  // Sud                
                count++;
            }
            if (i < rows - 1) {
                alpha += dst(i + 1, j, c);  // Nord
                count++;
            }

            dst(i, j, c) = alpha / count;
        }
    });

    return true;
}
