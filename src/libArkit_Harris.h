/*
 * libArkit_Harris.h
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBARKIT_HARRIS_H
#define LIBARKIT_HARRIS_H

#include "libArkit_Frame.h"
#include "libArkit_FeatureExtractor.h"

namespace ARKIT
{
    class HarrisExtractor: FeatureExtractor
    {
        private:
            unsigned short window_size;
            float sensitivity_factor;
            bool smoothing;
            bool non_max_suppression;

        public:
            HarrisExtractor(bool smoothing, bool non_max_suppression, bool annotate);
            /* Order the FAST keypoints and return the N top points using the
             * Harris corner measure
             */
            std::vector<Keypoint> Extract(const Frame *f);
            Frame* GetAnnotatedFrame();
    };
}

#endif /* !LIBARKIT_HARRIS_H */
