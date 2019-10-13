/*
 * libArkit_FeatureExtractor.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBARKIT_FEATUREEXTRACTOR_H
#define LIBARKIT_FEATUREEXTRACTOR_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>

#include "libArkit_Matrix.h"
#include "libArkit_Frame.h"

namespace ARKIT
{

    struct FeatureDescriptor {
        int x;
        int y;
    };

    struct Keypoint {

    };

    struct ScalePyramid {
        std::vector<Frame*> scales;
    };

    class FeatureExtractor
    {
        protected:
            std::vector<FeatureDescriptor> features;
            std::vector<Keypoint> keypoints;
            Frame* frame;

        public:
            virtual void Extract()=0;
    };


    class ORBExtractor: FeatureExtractor
    {
        private:
            bool full_high_speed_test;
            unsigned short intensity_threshold;
            unsigned short contiguous_pixels;
            unsigned short top_n_keypoints;
            unsigned short n_keypoints;
            unsigned short window_size; // Harris corner detector
            unsigned short pog_levels;
            unsigned short radius;
            float sensitivity_factor; // Harris

            /*
             * Bresenham's circle drawing algorithm
             */
            std::vector<Pixel*> BresenhamCircle(Pixel center, int radius, Frame* frame);

            /* Extract N keypoints in the given frame, using the Features from
             * Accelerated Segment Test algorithm with a given circular radius
             * (threshold)
             */
            std::vector<Keypoint> FAST(Frame* f);

            /* Order the FAST keypoints and return the N top points using the
             * Harris corner measure
             */
            std::vector<Keypoint> HarrisFilter(bool smoothing/*std::vector<Keypoint> keypoints*/);

            /* Build a scale pyramid of the base image */
            ScalePyramid BuildPyramid();

            float IntensityCentroid();

        public:
            ORBExtractor(Frame& f);
            ~ORBExtractor();
            void Extract();
            Frame GetAnnotatedFrame();
            std::vector<Keypoint> GetKeypoints();
            void Describe();
    };
}

#endif /* !LIBARKIT_FEATUREEXTRACTOR_H */
