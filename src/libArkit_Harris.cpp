/*
 * libArkit_Harris.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "libArkit_Harris.h"


namespace ARKIT
{
    HarrisExtractor::HarrisExtractor(bool smoothing, bool non_max_suppression, bool annotate)
    {
        this->sensitivity_factor = 0.04;
        this->window_size = 3;
        this->smoothing = smoothing;
        this->non_max_suppression = non_max_suppression;
        this->annotate = annotate;
    }

    /* Order the FAST keypoints and return the N top points using the
     * Harris corner measure
     */
    std::vector<Keypoint> HarrisExtractor::Extract(const Frame *frame)
    {
        int offset = this->window_size / 2, nonMaxWindowSize = 6;
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
        std::cout << "Threshold: " << threshold << std::endl;
        threshold = abs(0.1*threshold); // TODO: Set class property
        std::cout << "Damped threshold: " << threshold << std::endl;

        if (non_max_suppression) {
            offset = nonMaxWindowSize/2;
            for (int i = nonMaxWindowSize; i < harrisResponse.Rows() - nonMaxWindowSize; ++i) {
                for (int j = nonMaxWindowSize; j < harrisResponse.Cols() - nonMaxWindowSize; ++j) {
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

        for (int i = offset; i < img.Rows() - offset; ++i) {
            for (int j = offset; j < img.Cols() - offset; ++j) {
                if (*harrisResponse(i, j) > threshold && (i > 5 && i <
                            frame->Height()-5) && (j > 5 && j <
                            frame->Width()-5)) {
                    this->keypoints.push_back(Keypoint(j, i));
                }
            }
        }
        std::cout << "[*]" << this->keypoints.size() << " IP found!" << std::endl;
        return this->keypoints;
    }

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
}
