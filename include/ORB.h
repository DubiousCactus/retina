/*
 * ORB.h
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef ORB_H
#define ORB_H

#include "FeatureExtractor.h"
#include "Harris.h"
#include "FAST.h"

namespace arlib
{
    enum SamplingGeometries {
        UNIFORM,
        GAUSSIAN_I,
        GAUSSIAN_II,
        COARSE_POLAR_GRID_I,
        COARSE_POLAR_GRID_II
    };

    class ORBExtractor: FeatureExtractor
    {
        private:
            size_t n_keypoints;
            unsigned short pog_levels;
            /* Build a scale pyramid of the base image */
            ScalePyramid BuildPyramid();
            float IntensityCentroid();
            FASTExtractor fast_extractor;
            HarrisExtractor harris_extractor;
            void Describe(const Frame *frame);

        public:
            ORBExtractor(size_t n_keypoints);
            std::vector<Keypoint> GetKeypoints();
            std::vector<Keypoint> Extract(const Frame* frame);
            Frame* GetAnnotatedFrame();
    };
}

#endif /* !ORB_H */
