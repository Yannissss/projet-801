#include "gaussSeidel.h"

// #include <opencv2/core/hal/interface.h>
// #include <opencv2/core/matx.hpp>

inline uchar Clamp(int n) {
    n = n > 255 ? 255 : n;
    return n < 0 ? 0 : n;
}

bool GaussSeidel(ViewMatrixType &img, int rows, int cols) {
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
            uchar alpha = 0;

            if (j > 0)
                alpha += img(i, j - 1, c) * 0.2;  // Ouest
            alpha += img(i, j, c) * 0.2;          // Centre
            if (j < cols - 1)
                alpha += img(i, j + 1, c) * 0.2;  // Est
            if (i > 0)
                alpha += img(i, j - 1, c) * 0.2;  // Sud                
            if (i < rows - 1)
                alpha += img(i, j - 1, c) * 0.2;  // Nord

            img(i, j, c) = alpha;
        }
    });

    // Noir
    Kokkos::parallel_for(policy, KOKKOS_LAMBDA(const int i, int j) {
        j = j * 2 + (i + 1) % 2;
        // Parcours des channels
        for (int c = 0; c < 3; c++) {
            uchar alpha = 0;

            if (j > 0)
                alpha += img(i, j - 1, c) * 0.2;  // Ouest
            alpha += img(i, j, c) * 0.2;          // Centre
            if (j < cols - 1)
                alpha += img(i, j + 1, c) * 0.2;  // Est
            if (i > 0)
                alpha += img(i, j - 1, c) * 0.2;  // Sud                
            if (i < rows - 1)
                alpha += img(i, j - 1, c) * 0.2;  // Nord

            img(i, j, c) = alpha;
        }
    });

    return true;
}
