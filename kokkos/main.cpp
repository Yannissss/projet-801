#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <sys/time.h>

#include <Kokkos_Core.hpp>

#include <opencv2/imgcodecs.hpp>

#include "gaussSeidel.h"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
    CommandLineParser parser(argc, argv,
        "{help h usage ? |      | print this message   }"
        "{@image         |      | input image          }"
        "{@N             |  20  | noise iterations     }");
    parser.printMessage();

    String imageName = parser.get<String>("@image");
    const int NOISE_ITER = parser.get<int>("@N");

    string image_path = samples::findFile(imageName);
    Mat img = imread(image_path, IMREAD_COLOR);
    if (img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    Mat mColorGaussSeidel(img.size(), img.type());

    Kokkos::initialize( argc, argv );
    {
        // Préparation
        const int rows = img.rows;
        const int cols = img.cols;

        // Mémoire device
        ViewMatrixType A("A", rows, cols);
        ViewMatrixType B("B", rows, cols);

        // Mirroir host
        ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view( A );
        ViewMatrixType::HostMirror h_B = Kokkos::create_mirror_view( B );

        // Copie de l'image sur GPU
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Vec3b pixel = img.at<Vec3b>(i, j);
                for (int c = 0; c < 3; c++) {
                    h_A(i, j, c) = pixel.val[c];
                }
            }
        }
        Kokkos::deep_copy( A, h_A );

        // // Traitement
        double elapsed = 0.0;
        {
            Kokkos::Timer timer;
            for (int i = 0; i < NOISE_ITER; ++i) {
                GaussSeidel(A, B, rows, cols);
                if (i < (NOISE_ITER - 1)) { // Swap buffers
                    swap(A, B);
                }
            }
            elapsed = timer.seconds();
        }

        // Récupération de l'image GPU
        Kokkos::deep_copy( h_B, B );
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Vec3b &pixel = mColorGaussSeidel.at<Vec3b>(i, j);
                for (int c = 0; c < 3; c++) {
                    pixel.val[c] = h_B(i, j, c);
                }
            }
        }
        cout << "Time: " << elapsed << "s" << endl;
        fprintf(stderr, "%d, %d, %d, %f \n", rows, cols, NOISE_ITER, elapsed);
    }
    Kokkos::finalize();


    printf("Writting the output image of size %dx%d...\n", img.rows, img.cols);
    imwrite("res/gauss_seidel.png", mColorGaussSeidel);

    return 0;
}