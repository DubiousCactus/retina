/*
 * FAST.h
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FAST_H
#define FAST_H

#include "FeatureExtractor.h"

namespace arlib
{
    class FASTExtractor: FeatureExtractor
    {
        private:
            bool full_high_speed_test;
            unsigned int margin;
            unsigned short radius;
            unsigned short intensity_threshold;
            unsigned short contiguous_pixels;
            unsigned short n_keypoints;

            /*
             * Bresenham's circle drawing algorithm
             */
            std::vector<Pixel*> BresenhamCircle(const Pixel center, int radius,
                    const Frame* frame);
            void NonMaxSuppression(Matrix<int>& fastResponse);

        public:
            FASTExtractor(unsigned int margin, unsigned short radius, unsigned
                    short intensity_threshold, unsigned short contiguous_pixels,
                    unsigned short n_keypoints, bool full_high_speed_test, bool
                    annotate, bool non_max_suppression);

            /* Extract N keypoints in the given frame, using the Features from
             * Accelerated Segment Test algorithm with a given circular radius
             * (threshold)
             */
            std::vector<Keypoint> Extract(const Frame* f);
            Frame* GetAnnotatedFrame();
    };
}

#endif /* !FAST_H */
