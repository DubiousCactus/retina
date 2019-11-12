/*
 * BRIEF.h
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef BRIEF_H
#define BRIEF_H

#include <random>
#include <algorithm>

#include "Matrix.h"
#include "FeatureExtractor.h"

namespace arlite
{
    enum SamplingGeometry {
        UNIFORM,
        GAUSSIAN_I,
        GAUSSIAN_II,
        COARSE_POLAR_GRID_I,
        COARSE_POLAR_GRID_II
    };

    class BRIEFDescriptor
    {
        private:
            int patch_size;
            int size;
            SamplingGeometry sampling_geometry;
            std::mt19937 randr;

        public:
            BRIEFDescriptor(int size, int patchSize, SamplingGeometry samplingGeometry);
            std::vector<FeatureDescriptor> Run(Matrix<double> &img,
                    std::vector<Keypoint> keypoints);
    };
}

#endif /* !BRIEF_H */
