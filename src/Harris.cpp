/*
 * Harris.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "Harris.h"

namespace arlib
{
    HarrisExtractor::HarrisExtractor(bool smoothing, bool non_max_suppression, bool annotate)
    {
        this->sensitivity_factor = 0.04;
        this->window_size = 3;
        this->smoothing = smoothing;
        this->non_max_suppression = non_max_suppression;
        this->annotate = annotate;
    }

    /* Extracts keypoints in image f using the Harris corner measure */
    std::vector<Keypoint> HarrisExtractor::Extract(const Frame *frame)
    {
        int offset = this->window_size / 2;
        double Sxx, Syy, Sxy;
        double det, trace, r, threshold;
        double sX[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };
        double sY[3][3] = {
            {-1, -2, -1},
            {0, 0, 0},
            {1, 2, 1}
        };
        this->annotated_frame = annotate ?  new Frame(*frame) : NULL;
        Matrix<double> sobelX(sX);
        Matrix<double> sobelY(sY);
        Matrix<double> gaussianKernel = Matrix<double>::MakeGaussianKernel(3);
        Matrix<double> img = this->smoothing
            ? Matrix<double>::Convolve(frame->GetDoubleMatrix(), gaussianKernel)
            : frame->GetDoubleMatrix();
        Matrix<double> f_x = Matrix<double>::Convolve(img, sobelX);
        Matrix<double> f_y = Matrix<double>::Convolve(img, sobelY);

        Matrix<double> f_xx = Matrix<double>::ElementWiseProduct(f_x, f_x);
        Matrix<double> f_xy = Matrix<double>::ElementWiseProduct(f_y, f_x);
        Matrix<double> f_yy = Matrix<double>::ElementWiseProduct(f_y, f_y);

        threshold = 0;
        Matrix<double> harrisResponse(img.Rows(), img.Cols());
        for (int i = offset; i < img.Rows() - offset; ++i) {
            for (int j = offset; j < img.Cols() - offset; ++j) {
                Sxx = Matrix<double>::Sum(f_xx, i, j, this->window_size);
                Syy = Matrix<double>::Sum(f_yy, i, j, this->window_size);
                Sxy = Matrix<double>::Sum(f_xy, i, j, this->window_size);
                det = (Sxx * Syy) - pow(Sxy, 2);
                trace = Sxx + Syy;
                r = det - this->sensitivity_factor * pow(trace, 2);
                threshold += r;
                *harrisResponse(i, j) = r;
            }
        }
        threshold /= img.Rows() * img.Cols();
        threshold = abs(0.1*threshold); // TODO: Set class property

        if (non_max_suppression) {
            this->NonMaxSuppression(6, harrisResponse);
        }

        for (int i = offset; i < img.Rows() - offset; ++i) {
            for (int j = offset; j < img.Cols() - offset; ++j) {
                if (*harrisResponse(i, j) > threshold && (i > 5 && i <
                            frame->Height()-5) && (j > 5 && j <
                            frame->Width()-5)) {
                    this->keypoints.push_back(Keypoint(j, i, *harrisResponse(i, j)));
                }
            }
        }
        std::cout << "[*] " << this->keypoints.size() << " keypoints found!" << std::endl;
        return this->keypoints;
    }

    void HarrisExtractor::NonMaxSuppression(int window_size, Matrix<double>& harrisResponse)
    {
        int offset = window_size/2;
        for (int i = window_size; i < harrisResponse.Rows() - window_size; ++i) {
            for (int j = window_size; j < harrisResponse.Cols() - window_size; ++j) {
                for (int k = -offset; k <= offset; ++k) {
                    for (int l = -offset; l <= offset; ++l) {
                        if (*harrisResponse(i+k, j+l) > *harrisResponse(i, j)) {
                            *harrisResponse(i, j) = 0;
                            break;
                        }
                    }
                }
            }
        }
    }

    /* Computes the Harris corner response of pixel <x,y> in image f,
     * within a patch of block_size*block_size
     */
    float HarrisExtractor::MeasureCorner(const Frame* f, int x, int y, int block_size)
    {
        // TODO: Optimize (don't use matrices you fool!)
        //1. Extract a patch of blocksize*blocksize from the image f
        Matrix<double> patch = f->GetDoubleMatrix(x, y, block_size);
        // TODO: Maybe smoothen the patch??

        //3. Compute the derivatives of the pixel within the patch
        double Sxx, Syy, Sxy;
        double det, trace, r, threshold;
        double sX[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };
        double sY[3][3] = {
            {-1, -2, -1},
            {0, 0, 0},
            {1, 2, 1}
        };
        const Matrix<double> sobelX(sX);
        const Matrix<double> sobelY(sY);
        Matrix<double> f_x = Matrix<double>::Convolve(patch, sobelX);
        Matrix<double> f_y = Matrix<double>::Convolve(patch, sobelY);

        Matrix<double> f_xx = Matrix<double>::ElementWiseProduct(f_x, f_x);
        Matrix<double> f_xy = Matrix<double>::ElementWiseProduct(f_y, f_x);
        Matrix<double> f_yy = Matrix<double>::ElementWiseProduct(f_y, f_y);

        //4. Compute the Harris corner response
        Sxx = Matrix<double>::Sum(f_xx, f_xx.Rows()/2, f_xx.Cols()/2, this->window_size);
        Syy = Matrix<double>::Sum(f_yy, f_yy.Rows()/2, f_yy.Cols()/2, this->window_size);
        Sxy = Matrix<double>::Sum(f_xy, f_xy.Rows()/2, f_xy.Cols()/2, this->window_size);
        det = (Sxx * Syy) - pow(Sxy, 2);
        trace = Sxx + Syy;

        return det - this->sensitivity_factor * pow(trace, 2);
    }

    /* Returns an image as a copy of the extracted frame, with keypoint
     * annotations.
     */
    Frame* HarrisExtractor::GetAnnotatedFrame()
    {
        assert(this->annotated_frame);
        for (auto kp : this->keypoints) {
            annotated_frame->WriteAt(kp.x-5, kp.y, 0);
            annotated_frame->WriteAt(kp.x-4, kp.y, 0);
            annotated_frame->WriteAt(kp.x-3, kp.y, 0);
            annotated_frame->WriteAt(kp.x-2, kp.y, 0);
            annotated_frame->WriteAt(kp.x-1, kp.y, 0);
            annotated_frame->WriteAt(kp.x, kp.y, 0);
            annotated_frame->WriteAt(kp.x+5, kp.y, 0);
            annotated_frame->WriteAt(kp.x+4, kp.y, 0);
            annotated_frame->WriteAt(kp.x+3, kp.y, 0);
            annotated_frame->WriteAt(kp.x+2, kp.y, 0);
            annotated_frame->WriteAt(kp.x+1, kp.y, 0);
            annotated_frame->WriteAt(kp.x, kp.y-5, 0);
            annotated_frame->WriteAt(kp.x, kp.y-4, 0);
            annotated_frame->WriteAt(kp.x, kp.y-3, 0);
            annotated_frame->WriteAt(kp.x, kp.y-2, 0);
            annotated_frame->WriteAt(kp.x, kp.y-1, 0);
            annotated_frame->WriteAt(kp.x, kp.y, 0);
            annotated_frame->WriteAt(kp.x, kp.y+5, 0);
            annotated_frame->WriteAt(kp.x, kp.y+4, 0);
            annotated_frame->WriteAt(kp.x, kp.y+3, 0);
            annotated_frame->WriteAt(kp.x, kp.y+2, 0);
            annotated_frame->WriteAt(kp.x, kp.y+1, 0);
        }
        return this->annotated_frame;
    }

    std::vector<Keypoint> HarrisExtractor::GetKeypoints()
    {
        return this->keypoints;
    }
}
