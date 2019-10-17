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
            /* Extracts keypoints in image f using the Harris corner measure */
            std::vector<Keypoint> Extract(const Frame *f);
            /* Computes the Harris corner response of pixel <x,y> in image f,
             * within a patch of block_size*block_size
             */
            float MeasureCorner(const Frame *f, int x, int y, int block_size);
            /* Returns an image as a copy of the extracted frame, with keypoint
             * annotations.
             */
            Frame* GetAnnotatedFrame();
            std::vector<Keypoint> GetKeypoints();
    };
}

#endif /* !LIBARKIT_HARRIS_H */
