#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <opencv2/imgcodecs.hpp>

#include <omp.h>
#include <cstring>

#include "gaussSeidel.h"
#include "gaussianNoise.h"
#include "stopwatch.hpp"

using namespace cv;
using namespace std;

#define NOISE_ITER 25
#define NB_TACHES 10

int main(int argc, char **argv) {
    int nbTasks = NB_TACHES;
    bool isParallel = false;
    bool isVerbose = true;
    CommandLineParser parser(argc, argv,
                             "{@input   |img/lena.jpg|input image}");
    //parser.printMessage();
    for (int j=0;j<argc;j++) {
        if(strcmp(argv[j],"-v") == 0) {
            isVerbose = false;
        } else if (strcmp(argv[j],"-p") == 0) {
            if (isVerbose)
                printf("Programme lancé en parallele\n");
            isParallel = true;
        } else if (strcmp(argv[j],"-h") == 0) {
            if (isVerbose)
                printf("Utilisation : ./gauss-seidel image\n -h Pour afficher ce message\n -p Pour lancer ce programme en parallèle");
            isParallel = true;
        }
        std::string arg = argv[j];
        try {
            int n = std::stoi(arg);
            //std::cout << "Argument is an integer: " << n << std::endl;
            nbTasks = n;
        } catch (const std::invalid_argument&) {
            //std::cout << "Argument is not an integer: " << arg << std::endl;
        } catch (const std::out_of_range&) {
            //std::cout << "Argument is out of range: " << arg << std::endl;
        }
    }

    String imageName = parser.get<String>("@input");
    string image_path = samples::findFile(imageName);
    Mat img = imread(image_path, IMREAD_COLOR);
    if (img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    Mat mColorNoise(img.size(), img.type());
    // for (int i = 0; i < NOISE_ITER; ++i) {
    //     AddGaussianNoise(img, mColorNoise, 0, 30.0);
    //     if (i < (NOISE_ITER - 1)) {
    //         uint8_t *tmp = img.data;
    //         img.data = mColorNoise.data;
    //         mColorNoise.data = tmp;
    //     }
    // }
    AddGaussianNoise_Opencv(img,mColorNoise,10,30.0);//I recommend to use
    // this way!

    // Gauss-Seidel
    Stopwatch sw;
    Mat mColorGaussSeidel(img.size(), img.type());
    img.copyTo(mColorGaussSeidel);
    if (isVerbose)
        printf("%-22s: %d iteration(s) \n", "Gauss-Seidel", NOISE_ITER);
    sw.start();
    for (int i = 0; i < NOISE_ITER; ++i) {
        if (isParallel) {
            GaussSeidel_Task(img, mColorGaussSeidel, nbTasks);
            //printf("test");
        } else {
            GaussSeidel_Seq(img, mColorGaussSeidel);
            //printf("test2");
        }
            
        if (i < (NOISE_ITER - 1)) {
            uint8_t *tmp = img.data;
            img.data = mColorGaussSeidel.data;
            mColorGaussSeidel.data = tmp;
        }
    }
    sw.stop();
    if (isVerbose)
    printf("%-22s: ", "Time");
    sw.print_human_readable();
    cout << endl; 

    // Gauss classique
    
    // Convertie en nuance gris
    // uint8_t *pixelPtr = (uint8_t *)img.data;
    // int cn = img.channels();

    // for (int i = 0; i < img.rows; i++) {
    //     for (int j = 0; j < img.cols; j++) {
    //         // bgrPixel.val[0] = 255; //B
    //         uint8_t b = pixelPtr[i * img.cols * cn + j * cn + 0]; // B
    //         uint8_t g = pixelPtr[i * img.cols * cn + j * cn + 1]; // G
    //         uint8_t r = pixelPtr[i * img.cols * cn + j * cn + 2]; // R
    //         uint8_t grey = r * 0.299 + g * 0.587 + b * 0.114;

    //         pixelPtr[i * img.cols * cn + j * cn + 0] = grey; // B
    //         pixelPtr[i * img.cols * cn + j * cn + 1] = grey; // G
    //         pixelPtr[i * img.cols * cn + j * cn + 2] = grey; // R
    //     }i
    // }
    if (isVerbose)
        fprintf(stdout, "Writting the output image of size %dx%d...\n", img.rows,
            img.cols);

    imwrite("res/grey_res.png", img);
    imwrite("res/noised_res.png", mColorNoise);
    imwrite("res/gauss_seidel.png", mColorGaussSeidel);

    return 0;
}
