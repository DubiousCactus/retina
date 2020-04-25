/*
 * BRIEF.h
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#ifndef BRIEF_H
#define BRIEF_H

#include <algorithm>
#include <random>

#include "FeatureExtractor.h"
#include "Matrix.h"

namespace retina {
enum SamplingGeometry
{
    UNIFORM,
    GAUSSIAN_I,
    GAUSSIAN_II,
    COARSE_POLAR_GRID_I,
    COARSE_POLAR_GRID_II
};

class BRIEFDescriptor
{
  private:
    bool steer;
    int patch_size;
    int size;
    SamplingGeometry sampling_geometry;
    std::mt19937 randr;

  public:
    BRIEFDescriptor(int size, int patchSize, SamplingGeometry samplingGeometry, bool steer);
    std::vector<FeatureDescriptor>
    Run(Matrix<double>& img, std::vector<KeyPoint> keypoints);
};
}

#endif /* !BRIEF_H */
