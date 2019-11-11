/*
 * FeatureExtractor.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>

#include "Frame.h"

namespace arlib
{

    struct FeatureDescriptor {
        int x;
        int y;
    };

    struct Keypoint {
        int x;
        int y;
        float score;

        Keypoint()
        {
            x = -1;
            y = -1;
            score = 0;
        }

        Keypoint(int x, int y, float score)
        {
            this->x = x;
            this->y = y;
            this->score = score;
        }

        Keypoint(int x, int y)
        {
            this->x = x;
            this->y = y;
            this->score = 0;
        }

        bool operator<(const Keypoint& kp) const
        {
            return score < kp.score;
        }
    };

    struct ScalePyramid {
        std::vector<Frame*> scales;
    };

    class FeatureExtractor
    {
        protected:
            std::vector<FeatureDescriptor> features;
            std::vector<Keypoint> keypoints;
            Frame *annotated_frame;
            bool non_max_suppression;
            bool annotate;

        public:
            virtual std::vector<Keypoint> Extract(const Frame *frame)=0;
            virtual Frame* GetAnnotatedFrame()=0;
    };
}

#endif /* !FEATUREEXTRACTOR_H */
