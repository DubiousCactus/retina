/*
 * ORB.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "ORB.h"

#include <cmath>
#include <random>
#include <algorithm>

namespace arlib
{
    /* Build a scale pyramid of the base image */
    ScalePyramid ORBExtractor::BuildPyramid()
    {
        ScalePyramid pyramid;
        /*for (unsigned short i = 0; i < this->pog_levels; i++)
            pyramid.scales.push_back(this->frame);*/

        return pyramid;
    }

    float IntensityCentroid();

    ORBExtractor::ORBExtractor(size_t n_keypoints)
        : fast_extractor(3, 20, 12, 150, false, true, false),
        harris_extractor(true, true, true)
    {
        this->n_keypoints = n_keypoints;
        this->pog_levels = 1;
    }

    std::vector<Keypoint> ORBExtractor::Extract(const Frame *frame)
    {
        /*
         * 1. Build the PoG to produce multiscale-features
         * 2. Find keypoints with FAST at each level of the pyramid
         * 3. Apply Harris corner measure to find the top N points
         */

        assert(frame != nullptr);
        // STEP 1: Build the scale pyramid of the current frame
        //std::cout << "\t-> Building the pyramid" << std::endl;
        //ScalePyramid pyramid = this->BuildPyramid();

        auto start = std::chrono::steady_clock::now();
        this->keypoints = this->fast_extractor.Extract(frame);
        auto end = std::chrono::steady_clock::now();
        std::cout << "\t-> FAST executed in "
            <<
            (float)std::chrono::duration_cast<std::chrono::microseconds>(end
                    - start).count()/1000
            << " milliseconds" << std::endl;
        start = std::chrono::steady_clock::now();
        for (auto kp : this->keypoints) {
            if (kp.x - 3 < 0 || kp.x + 3 >= frame->Width() || kp.y - 3 < 0 ||
                    kp.y + 3 >= frame->Height()) {
                continue;
            }
            kp.score = this->harris_extractor.MeasureCorner(frame, kp.x, kp.y, 7);
        }
        end = std::chrono::steady_clock::now();
        std::cout << "\t-> Harris corner measure (for all keypoints) executed in "
            <<
            (float)std::chrono::duration_cast<std::chrono::microseconds>(end
                    - start).count()/1000
            << " milliseconds" << std::endl;
        std::cout << "\t-> Keeping the top " << this->n_keypoints << " keypoints..." << std::endl;
        std::sort(this->keypoints.begin(), this->keypoints.end());
        if (this->n_keypoints <= this->keypoints.size()) {
            this->keypoints.resize(this->n_keypoints);
        }
        this->Describe(frame);
        this->annotated_frame = new Frame(*frame);
        // STEP 2: for each level of the PoG
        /*for (unsigned short i = 0; i < this->pog_levels; i++) {
          std::vector<Keypoint> keypoints = this->HarrisFilter(
          this->FAST(pyramid.scales.at(i)));
          }*/

        return this->keypoints;
    }

    Frame* ORBExtractor::GetAnnotatedFrame()
    {
        std::cout << "[*] Annotating..." << std::endl;
        for (auto kp : this->keypoints) {
            if (kp.x -5 < 0 || kp.x + 5 >= annotated_frame->Width()
                    || kp.y - 5 < 0 || kp.y + 5 >= annotated_frame->Height())
                continue;
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
        return annotated_frame;
    }

    std::vector<Keypoint> ORBExtractor::GetKeypoints()
    {
        return this->keypoints;
    }

    void ORBExtractor::Describe(const Frame *frame)
    {
        /*
         * 1. Apply BRIEF on the keypoints
         * See: https://medium.com/software-incubator/introduction-to-brief-binary-robust-independent-elementary-features-436f4a31a0e6
         * TODO: Maybe move to a BRIEF class?
         */
        const int patch_size = 15;
        const int length = 128;
        const auto samplingGeometry = GAUSSIAN_I;
        std::default_random_engine generator;
        Pixel x1(0), x2(0);
        // TODO: Is it faster to convolve each patch individually?
        Matrix<double> gaussianKernel = Matrix<double>::MakeGaussianKernel(5);
        Matrix<double> img = Matrix<double>::Convolve(frame->GetDoubleMatrix(),
                gaussianKernel);
        //Matrix<double> img = Matrix<double>::Convolve(frame->GetDoubleMatrix(), gaussianKernel);

        for (auto kp : this->keypoints) {
            if (((kp.x - patch_size) < 0) || ((kp.y - patch_size) < 0)
                    || ((kp.x + patch_size) >= frame->Width())
                    || ((kp.y + patch_size) >= frame->Height())) {
                continue;
            }
            std::string featureVector(length, '0');
            for (auto &b : featureVector) {
                // TODO: Use a if-constexpr ?
                switch (samplingGeometry) {
                    case UNIFORM:
                        {
                            const int spread = patch_size / 2;
                            std::uniform_int_distribution<int>
                                distribution_x(kp.x-spread, kp.x+spread);
                            std::uniform_int_distribution<int>
                                distribution_y(kp.y-spread, kp.y+spread);
                            x1.x = distribution_x(generator);
                            x1.y = distribution_y(generator);
                            x2.x = distribution_x(generator);
                            x2.y = distribution_y(generator);
                        }
                        break;
                    case GAUSSIAN_I:
                        {
                            const double spread = 0.04 * (patch_size * patch_size);
                            std::normal_distribution<double> distribution_x(kp.x, spread);
                            std::normal_distribution<double> distribution_y(kp.y, spread);
                            x1.x = (int)distribution_x(generator);
                            x1.y = (int)distribution_y(generator);
                            x2.x = (int)distribution_x(generator);
                            x2.y = (int)distribution_y(generator);
                        }
                        break;
                    case GAUSSIAN_II:
                        {
                            const double spread = 0.04 * (patch_size * patch_size);
                            std::normal_distribution<double> distribution_x1_x(kp.x, spread);
                            std::normal_distribution<double> distribution_x1_y(kp.y, spread);
                            x1.x = (int)distribution_x1_x(generator);
                            x1.y = (int)distribution_x1_y(generator);
                            std::normal_distribution<double> distribution_x2_x(kp.x, spread);
                            std::normal_distribution<double> distribution_x2_y(kp.y, spread);
                            x2.x = std::clamp((int)distribution_x2_x(generator),
                                    kp.x-patch_size/2, kp.x+patch_size/2);
                            x2.y = std::clamp((int)distribution_x2_y(generator),
                                    kp.y-patch_size/2, kp.y+patch_size/2);
                        }
                        break;
                    case COARSE_POLAR_GRID_I:
                        // Both x and y pixels in the random pair are sampled
                        // from discrete locations of a coarse polar grid
                        // introducing a spatial quantization.
                        break;
                    case COARSE_POLAR_GRID_II:
                        // Pick x at (0, 0) (which is kp) and y from discrete
                        // locations of a coarse polar grid.
                        break;
                    default:
                        x1.intensity = x2.intensity = 0;
                }
                b = img(x1.x, x1.y) < img(x2.x, x2.y) ? '1' : '0';
            }
            std::cout << featureVector << std::endl;
        }
    }
}
