/*
 * FAST.h
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#ifndef FAST_H
#define FAST_H

#include "FeatureExtractor.h"

namespace retina {
class FASTExtractor : FeatureExtractor
{
  private:
    bool full_high_speed_test;
    bool orientation;
    int centroid_radius;
    unsigned int margin;
    unsigned short radius;
    unsigned short intensity_threshold;
    unsigned short contiguous_pixels;
    unsigned short n_keypoints;

    /*
     * Bresenham's circle drawing algorithm
     */
    std::vector<Pixel>
    BresenhamCircle(Pixel center, int radius, const Frame& frame);
    void NonMaxSuppression(const Matrix<double>& imgMatrix,
                           Matrix<int>& fastResponse);
    /* Computes the given patch's orientation using the intensity
     * centroid and the keypoint as center of patch.
     */
    float
    PatchOrientation(const Matrix<double>& img, int cx, int cy);

  public:
    FASTExtractor(unsigned int margin,
                  unsigned short radius,
                  unsigned short intensity_threshold,
                  unsigned short contiguous_pixels,
                  unsigned short n_keypoints,
                  bool full_high_speed_test,
                  bool annotate,
                  bool non_max_suppression,
                  bool orientation,
                  int centroidRadius);

    /* Extract N keypoints in the given frame, using the Features from
     * Accelerated Segment Test algorithm with a given circular radius
     * (threshold)
     */
    std::vector<KeyPoint> Extract(const Frame& f) override;
    std::shared_ptr<Frame> GetAnnotatedFrame() override;
};
}

#endif /* !FAST_H */
