#include "gaussSeidel.h"
#include <omp.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/matx.hpp>


inline uchar Clamp(int n) {
    n = n > 255 ? 255 : n;
    return n < 0 ? 0 : n;
}

inline Vec3b SampleAt(Mat const &mSrc, int Row, int Col) {
    Row = Row >= 0 ? (Row >= mSrc.rows ? mSrc.rows - 1 : Row) : 0;
    Col = Col >= 0 ? (Col >= mSrc.cols ? mSrc.cols - 1 : Col) : 0;
    return mSrc.at<Vec3b>(Row, Col);
}

inline uchar ToGrey(Vec3b const &c) {
    uchar b = c.val[0];
    uchar g = c.val[1];
    uchar r = c.val[2];
    return r * 0.299 + g * 0.587 + b * 0.114;
}

inline void Blur(Mat const &mSrc, Mat const &mDst, Vec3b &Des_Pixel, int Row,
                 int Col) {
    // Memorisation
    Vec3b const &w = SampleAt(mDst, Row - 1, Col); // West
    Vec3b const &n = SampleAt(mDst, Row, Col - 1); // North
    Vec3b const &c = SampleAt(mSrc, Row, Col);     // Center
    Vec3b const &e = SampleAt(mSrc, Row + 1, Col); // East
    Vec3b const &s = SampleAt(mSrc, Row, Col + 1); // South

    for (int i = 0; i < 3; i++) {
        // Initialisation
        unsigned int alpha = 0;

        // Itération courante
        alpha += w.val[i];
        alpha += n.val[i];

        // Itération précedente
        alpha += c.val[i];
        alpha += e.val[i];
        alpha += s.val[i];

        // Normalisation
        alpha /= 5;

        // Affectation
        Des_Pixel.val[i] = alpha;
    }
}
bool GaussSeidel_Seq(const Mat mSrc, Mat &mDst) {
    if (mSrc.empty()) {
        cout << "[Error]! Input Image Empty!";
        return false;
    }

    if (mSrc.rows > mSrc.cols) {
        GaussSeidel_RowsSup(mSrc, mDst);
    } else {
        GaussSeidel_ColsSup(mSrc, mDst);
    }
    return true;
    
}

void GaussSeidel_RowsSup(const Mat mSrc, Mat &mDst) {
    // Ascendance par ligne (Partie I)
    for (int Diag = 0; Diag < mSrc.cols; Diag++) {
        for (int Row = 0; Row <= Diag; Row++) {
            
            int Col = Diag - Row;
            Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

            // Debug
            // Des_Pixel.val[0] = 255;
            // Des_Pixel.val[1] = 0;
            // Des_Pixel.val[2] = 0;
            
                Blur(mSrc, mDst, Des_Pixel, Row, Col);
        }
    }

    // Translation du milieu (Partie II)
    for (int Diag = mSrc.rows; Diag < mSrc.cols; Diag++) {
        for (int Row = 0; Row < mSrc.rows; Row++) {
            int Col = Diag - Row;
            Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

            // Debug
            // Des_Pixel.val[0] = 0;
            // Des_Pixel.val[1] = 255;
            // Des_Pixel.val[2] = 0;
            Blur(mSrc, mDst, Des_Pixel, Row, Col);
        }
    }

    // Descendance par colonne (Partie III)
    // Le 1 est pour éviter la coupure avec le milieu
    for (int t = 1; t < mSrc.rows; t++) {
        int Diag = mSrc.cols + t - 1;
        for (int Row = t; Row < mSrc.rows; Row++) {
            int Col = Diag - Row;
            Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

            // Debug
            // Des_Pixel.val[0] = 0;
            // Des_Pixel.val[1] = 0;
            // Des_Pixel.val[2] = 255;
            Blur(mSrc, mDst, Des_Pixel, Row, Col);
        }
    }

}
void GaussSeidel_ColsSup(const Mat mSrc, Mat &mDst) {

    // Ascendance par ligne (Partie I)
    for (int Diag = 0; Diag < mSrc.rows; Diag++) {
        for (int Row = 0; Row <= Diag; Row++) {
            int Col = Diag - Row;
            Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

            // Debug
            // Des_Pixel.val[0] = 255;
            // Des_Pixel.val[1] = 0;
            // Des_Pixel.val[2] = 0;

            Blur(mSrc, mDst, Des_Pixel, Row, Col);
        }
    }

    // Translation du milieu (Partie II)
    for (int Diag = mSrc.rows; Diag < mSrc.cols; Diag++) {
        for (int Row = 0; Row < mSrc.rows; Row++) {
            int Col = Diag - Row;
            Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

            // Debug
            // Des_Pixel.val[0] = 0;
            // Des_Pixel.val[1] = 255;
            // Des_Pixel.val[2] = 0;

            Blur(mSrc, mDst, Des_Pixel, Row, Col);
        }
    }

    // Descendance par colonne (Partie III)
    // Le 1 est pour éviter la coupure avec le milieu
    for (int t = 1; t < mSrc.rows; t++) {
        int Diag = mSrc.cols + t - 1;
        for (int Row = t; Row < mSrc.rows; Row++) {
            int Col = Diag - Row;
            Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

            // Debug
            // Des_Pixel.val[0] = 0;
            // Des_Pixel.val[1] = 0;
            // Des_Pixel.val[2] = 255;

            Blur(mSrc, mDst, Des_Pixel, Row, Col);
        }
    }

}
bool GaussSeidel_Task(const Mat mSrc, Mat &mDst, int nbTasks) {
    if (mSrc.empty()) {
        cout << "[Error]! Input Image Empty!";
        return false;
    }

    if (mSrc.rows > mSrc.cols) {
        GaussSeidel_RowsSup_Task(mSrc, mDst, nbTasks);
    } else {
        GaussSeidel_ColsSup_Task(mSrc, mDst, nbTasks);
    }
    return true;
    
}

void GaussSeidel_RowsSup_Task(const Mat mSrc, Mat &mDst, int nbTasks) {
    // Ascendance par ligne (Partie I)
    //int nbTasks=16; //nombre de taches OMP
    #pragma omp parallel
    #pragma omp single 
    {
        printf("%d\n",omp_get_thread_num());
        for (int Diag = 0; Diag < mSrc.cols; Diag++) {
            for (int Row = 0; Row <= Diag; Row++) {
                
                int Col = Diag - Row;
                Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);
                
                // Debug
                // Des_Pixel.val[0] = 255;
                // Des_Pixel.val[1] = 0;
                // Des_Pixel.val[2] = 0;
                #pragma omp task shared(Des_Pixel) firstprivate(mSrc,mDst,Row,Col) if(Diag>=100000)
                {
                    Blur(mSrc, mDst, Des_Pixel, Row, Col);
                }
            }
            #pragma omp taskwait
        }

        // Translation du milieu (Partie II)
        for (int Diag = mSrc.rows; Diag < mSrc.cols; Diag++) {
            for (int Row = 0; Row < mSrc.rows; Row++) {
                int Col = Diag - Row;
                Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);
                // Debug
                // Des_Pixel.val[0] = 0;
                // Des_Pixel.val[1] = 255;
                // Des_Pixel.val[2] = 0;
                #pragma omp task shared(Des_Pixel) firstprivate(mSrc,mDst,Row,Col) if(mSrc.rows>=100000)
                {
                    Blur(mSrc, mDst, Des_Pixel, Row, Col);
                }
            }
            #pragma omp taskwait
        }

        // Descendance par colonne (Partie III)
        // Le 1 est pour éviter la coupure avec le milieu
        for (int t = 1; t < mSrc.rows; t++) {
            int Diag = mSrc.cols + t - 1;
            for (int Row = t; Row < mSrc.rows; Row++) {
                int Col = Diag - Row;
                Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

                // Debug
                // Des_Pixel.val[0] = 0;
                // Des_Pixel.val[1] = 0;
                // Des_Pixel.val[2] = 255;
                #pragma omp task shared(Des_Pixel) firstprivate(mSrc,mDst,Row,Col) if(mSrc.rows>=100000)
                {
                    Blur(mSrc, mDst, Des_Pixel, Row, Col);
                }
            }
            #pragma omp taskwait
        }
    }
}
void GaussSeidel_ColsSup_Task(const Mat mSrc, Mat &mDst, int nbTasks) {
    //int nbTasks=10;
    #pragma omp parallel 
    #pragma omp single
    {   
        int start,end;
        // Ascendance par ligne (Partie I)
        for (int Diag = 0; Diag < mSrc.rows; Diag++) {
            //#pragma omp parallel for shared(Diag)
            for (int i=0;i<nbTasks;i++) {
                start=i*Diag/nbTasks;
                end=(i+1)*Diag/nbTasks;
                if (end>=Diag)
                    end=Diag;
                #pragma omp task if(end-start>=100)
                {
                    for (int Row = start; Row <= end; Row++) {
                        int Col = Diag - Row;
                        Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);
                        Blur(mSrc, mDst, Des_Pixel, Row, Col);
                        // Des_Pixel.val[0] = 255;
                        // Des_Pixel.val[1] = 0;
                        // Des_Pixel.val[2] = 0;
                    }
                }                
            }
            #pragma omp taskwait  

            // for (int Row = 0; Row <= Diag; Row++) {
            //     int Col = Diag - Row;
            //     Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);
            //     //printf("Hello from thread %d On row :%d and Col : %d\n",omp_get_thread_num(),Row,Col);
            //     // Debug
            //     // Des_Pixel.val[0] = 255;
            //     // Des_Pixel.val[1] = 0;
            //     // Des_Pixel.val[2] = 0;
            //     #pragma omp task shared(Des_Pixel) firstprivate(mSrc,mDst,Row,Col) if(Diag>=1000)
            //     {
            //         Blur(mSrc, mDst, Des_Pixel, Row, Col);
            //     }
            // }
            
        }
        
        // Translation du milieu (Partie II)
        for (int Diag = mSrc.rows; Diag < mSrc.cols; Diag++) {
            for (int i=0;i<nbTasks;i++) {
                start=i*(mSrc.rows/nbTasks);
                end=(i+1)*(mSrc.rows/nbTasks);
                if (end>=mSrc.rows)
                    end=mSrc.rows;
                #pragma omp task if(end-start>=100) 
                {
                    for (int Row = start; Row <= end; Row++) {
                        int Col = Diag - Row;
                        Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);
                        // Des_Pixel.val[0] = 0;
                        // Des_Pixel.val[1] = 255;
                        // Des_Pixel.val[2] = 0;
                        Blur(mSrc, mDst, Des_Pixel, Row, Col);
                    }
                }
                      
            }
            #pragma omp taskwait  
            /*
            for (int Row = 0; Row < mSrc.rows; Row++) {
                int Col = Diag - Row;
                Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

                // Debug
                // Des_Pixel.val[0] = 0;
                // Des_Pixel.val[1] = 255;
                // Des_Pixel.val[2] = 0;
                
                    Blur(mSrc, mDst, Des_Pixel, Row, Col);
                
            }*/
        }
        
        // Descendance par colonne (Partie III)
        // Le 1 est pour éviter la coupure avec le milieu
        for (int t = 1; t < mSrc.rows; t++) {
            // int Diag = mSrc.cols + t - 1;
            // for (int i=0;i<nbTasks;i++) {
            //     start=i*(mSrc.rows/nbTasks);
            //     end=(i+1)*(mSrc.rows/nbTasks);
            //     if (end>=mSrc.rows)
            //         end=mSrc.rows;
            //     #pragma omp task if(end-start>=100) 
            //     {
            //         for (int Row = start; Row <= end; Row++) {
            //             int Col = Diag - Row;
            //             Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);
            //             //Blur(mSrc, mDst, Des_Pixel, Row, Col);
            //             // Debug
            //             Des_Pixel.val[0] = 0;
            //             Des_Pixel.val[1] = 0;
            //             Des_Pixel.val[2] = 255;
            //         }
            //     }                
            // }
            
            int Diag = mSrc.cols + t - 1;
            for (int i=0;i<nbTasks;i++) {
                start = i*(mSrc.rows/nbTasks)+(t-1);
                end = (i+1)*(mSrc.rows/nbTasks)+(t-1);
                if (end >= mSrc.rows)
                    end=mSrc.rows;
                #pragma omp task if(end-start>=100)
                {
                    for (int Row = start; Row < end; Row++) {
                        int Col = Diag - Row;
                        Vec3b &Des_Pixel = mDst.at<Vec3b>(Row, Col);

                        // Debug
                        // Des_Pixel.val[0] = 0;
                        // Des_Pixel.val[1] = 0;
                        // Des_Pixel.val[2] = 255;
                        
                        Blur(mSrc, mDst, Des_Pixel, Row, Col);
                    }   
                }             
            }
			#pragma omp taskwait 

        }
    }
}