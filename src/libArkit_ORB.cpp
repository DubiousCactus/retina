/*
 * libArkit_ORB.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "libArkit_ORB.h"

#include <cmath>
#include "../external/CImg.h"

namespace ARKIT
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

    ORBExtractor::ORBExtractor()
    {
        this->pog_levels = 1;
        this->fast_extractor = new FASTExtractor(3, 9, 12, 50, 150, false, true);
        this->harris_extractor = new HarrisExtractor(true, true, true);
    }

    ORBExtractor::~ORBExtractor()
    {
        delete this->fast_extractor;
        delete this->harris_extractor;
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
        std::cout << "\t-> Building the pyramid" << std::endl;
        ScalePyramid pyramid = this->BuildPyramid();

        auto start = std::chrono::steady_clock::now();
        this->fast_extractor->Extract(frame);
        auto end = std::chrono::steady_clock::now();
        std::cout << "[*] FAST executed in "
            <<
            (float)std::chrono::duration_cast<std::chrono::microseconds>(end
                    - start).count()/1000
            << " milliseconds" << std::endl;
        start = std::chrono::steady_clock::now();
        this->harris_extractor->Extract(frame);
        end = std::chrono::steady_clock::now();
        std::cout << "[*] Harris corner executed in "
            <<
            (float)std::chrono::duration_cast<std::chrono::microseconds>(end
                    - start).count()/1000
            << " milliseconds" << std::endl;
        // STEP 2: for each level of the PoG
        /*for (unsigned short i = 0; i < this->pog_levels; i++) {
          std::vector<Keypoint> keypoints = this->HarrisFilter(
          this->FAST(pyramid.scales.at(i)));
          }*/

        return this->keypoints;
    }

    Frame* ORBExtractor::GetAnnotatedFrame()
    {
        return this->annotated_frame;
    }

    std::vector<Keypoint> ORBExtractor::GetKeypoints()
    {
        return this->keypoints;
    }

    void ORBExtractor::Describe()
    {
        /*
         * 1. Apply BRIEF on the keypoints
         */
    }
}
