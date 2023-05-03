#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <sys/time.h>

#include <Kokkos_Core.hpp>

#include <opencv2/imgcodecs.hpp>


using namespace cv;
using namespace std;

#define NOISE_ITER 15

void checkSizes( int &N, int &M, int &S, int &nrepeat );

int main(int argc, char **argv) {
    CommandLineParser parser(argc, argv,
                             "{@input   |img/lena.jpg|input image}");
    //parser.printMessage();

    String imageName = parser.get<String>("@input");
    string image_path = samples::findFile(imageName);
    Mat img = imread(image_path, IMREAD_COLOR);
    if (img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    Mat mColorNoise(img.size(), img.type());
    
    // Gauss classique
    
    // Convertie en nuance gris
    uint8_t *pixelPtr = (uint8_t *)img.data;
    int cn = img.channels();

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            // bgrPixel.val[0] = 255; //B
            uint8_t b = pixelPtr[i * img.cols * cn + j * cn + 0]; // B
            uint8_t g = pixelPtr[i * img.cols * cn + j * cn + 1]; // G
            uint8_t r = pixelPtr[i * img.cols * cn + j * cn + 2]; // R
            uint8_t grey = r * 0.299 + g * 0.587 + b * 0.114;

            pixelPtr[i * img.cols * cn + j * cn + 0] = grey; // B
            pixelPtr[i * img.cols * cn + j * cn + 1] = grey; // G
            pixelPtr[i * img.cols * cn + j * cn + 2] = grey; // R
        }
    }

    fprintf(stdout, "Writting the output image of size %dx%d...\n", img.rows,
            img.cols);

    imwrite("res/grey_res.png", img);
    imwrite("res/noised_res.png", mColorNoise);

    Kokkos::initialize( argc, argv );
    {

    // // Allocate y, x vectors and Matrix A on device.
    // typedef Kokkos::View<double*>   ViewVectorType;
    // typedef Kokkos::View<double**>  ViewMatrixType;
    // ViewVectorType y( "y", N );
    // ViewVectorType x( "x", M );
    // ViewMatrixType A( "A", N, M );

    // // Create host mirrors of device views.
    // ViewVectorType::HostMirror h_y = Kokkos::create_mirror_view( y );
    // ViewVectorType::HostMirror h_x = Kokkos::create_mirror_view( x );
    // ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view( A );

    // // Initialize y vector on host.
    // for ( int i = 0; i < N; ++i ) {
    //     h_y( i ) = 1;
    // }

    // // Initialize x vector on host.
    // for ( int i = 0; i < M; ++i ) {
    //     h_x( i ) = 1;
    // }

    // // Initialize A matrix on host.
    // for ( int j = 0; j < N; ++j ) {
    //     for ( int i = 0; i < M; ++i ) {
    //     h_A( j, i ) = 1;
    //     }
    // }

    // // Deep copy host views to device views.
    // Kokkos::deep_copy( y, h_y );
    // Kokkos::deep_copy( x, h_x );
    // Kokkos::deep_copy( A, h_A );

    // // Timer products.
    Kokkos::Timer timer;

    // for ( int repeat = 0; repeat < nrepeat; repeat++ ) {
    //     // Application: <y,Ax> = y^T*A*x
    //     double result = 0;

    //     Kokkos::parallel_reduce( "yAx", N, KOKKOS_LAMBDA ( int j, double &update ) {
    //     double temp2 = 0;

    //     for ( int i = 0; i < M; ++i ) {
    //         temp2 += A( j, i ) * x( i );
    //     }

    //     update += y( j ) * temp2;
    //     }, result );

    //     // Output result.
    //     if ( repeat == ( nrepeat - 1 ) ) {
    //     printf( "  Computed result for %d x %d is %lf\n", N, M, result );
    //     }

    //     const double solution = (double) N * (double) M;

    //     if ( result != solution ) {
    //     printf( "  Error: result( %lf ) != solution( %lf )\n", result, solution );
    //     }
    // }

    // Calculate time.
    double time = timer.seconds();
    }
    Kokkos::finalize();

    cout << "Time: " << time << endl;

    return 0;
}