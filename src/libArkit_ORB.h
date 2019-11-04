/*
 * libArkit_ORB.h
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBARKIT_ORB_H
#define LIBARKIT_ORB_H

#include "libArkit_FeatureExtractor.h"
#include "libArkit_Harris.h"
#include "libArkit_FAST.h"

namespace ARKIT
{
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

        public:
            ORBExtractor(size_t n_keypoints);
            std::vector<Keypoint> GetKeypoints();
            void Describe();
            std::vector<Keypoint> Extract(const Frame* frame);
            Frame* GetAnnotatedFrame();
    };
}

#endif /* !LIBARKIT_ORB_H */
