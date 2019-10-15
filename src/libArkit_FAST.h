/*
 * libArkit_FAST.h
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBARKIT_FAST_H
#define LIBARKIT_FAST_H

#include "libArkit_FeatureExtractor.h"

namespace ARKIT
{
    class FASTExtractor: FeatureExtractor
    {
        private:
            bool full_high_speed_test;
            unsigned short radius;
            unsigned short intensity_threshold;
            unsigned short contiguous_pixels;
            unsigned short top_n_keypoints;
            unsigned short n_keypoints;

            /*
             * Bresenham's circle drawing algorithm
             */
            std::vector<Pixel*> BresenhamCircle(const Pixel center, int radius,
                    const Frame* frame);

        public:
            FASTExtractor(unsigned short radius, unsigned short
                    intensity_threshold, unsigned short contiguous_pixels,
                    unsigned short top_n_keypoints, unsigned short n_keypoints,
                    bool full_high_speed_test, bool annotate);

            /* Extract N keypoints in the given frame, using the Features from
             * Accelerated Segment Test algorithm with a given circular radius
             * (threshold)
             */
            std::vector<Keypoint> Extract(const Frame* f);
            Frame* GetAnnotatedFrame();
    };
}

#endif /* !LIBARKIT_FAST_H */
