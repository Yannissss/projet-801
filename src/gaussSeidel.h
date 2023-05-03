#ifndef __GAUSS_SEIDEL_H__
#define __GAUSS_SEIDEL_H__

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

bool GaussSeidel_Seq(const Mat mSrc, Mat &mDst);
void GaussSeidel_RowsSup(const Mat mSrc, Mat &mDst);
void GaussSeidel_ColsSup(const Mat mSrc, Mat &mDst);
bool GaussSeidel_Task(const Mat mSrc, Mat &mDst);
void GaussSeidel_RowsSup_Task(const Mat mSrc, Mat &mDst);
void GaussSeidel_ColsSup_Task(const Mat mSrc, Mat &mDst);

#endif
