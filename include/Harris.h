/*
 * Harris.h
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#ifndef HARRIS_H
#define HARRIS_H

#include "FeatureExtractor.h"
#include "Frame.h"

namespace retina {
class HarrisExtractor : FeatureExtractor
{
  private:
    unsigned short window_size;
    float sensitivity_factor;
    bool smoothing;
    void NonMaxSuppression(int window_size, Matrix<double>& harrisResponse);

  public:
    HarrisExtractor(bool smoothing, bool non_max_suppression, bool annotate);
    /* Extracts keypoints in image f using the Harris corner measure */
    std::vector<KeyPoint> Extract(const Frame& f) override;
    /* Computes the Harris corner response of pixel <x,y> in image f,
     * within a patch of block_size*block_size
     */
    float MeasureCorner(const Frame& f, int x, int y, int block_size);
    /* Returns an image as a copy of the extracted frame, with keypoint
     * annotations.
     */
    std::shared_ptr<Frame> GetAnnotatedFrame() override;
    std::vector<KeyPoint> GetKeypoints();
};
}

#endif /* !HARRIS_H */
