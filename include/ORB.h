/*
 * ORB.h
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#ifndef ORB_H
#define ORB_H

#include "BRIEF.h"
#include "FAST.h"
#include "FeatureExtractor.h"
#include "Harris.h"

namespace retina {
class ORBExtractor : FeatureExtractor
{
  private:
    size_t n_keypoints;
    unsigned short pog_levels;
    float scaling_factor;
    /* Build a scale pyramid of the base image */
    ScalePyramid BuildPyramid();
    FASTExtractor fast_extractor;
    HarrisExtractor harris_extractor;
    BRIEFDescriptor brief_descriptor;

  public:
    explicit ORBExtractor(size_t n_keypoints);
    std::vector<KeyPoint> GetKeypoints();
    std::vector<KeyPoint> Extract(const Frame& frame) override;
    std::shared_ptr<Frame> GetAnnotatedFrame() override;
};
}

#endif /* !ORB_H */
