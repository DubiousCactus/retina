/*
 * BRIEF.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "BRIEF.h"

namespace arlite {
BRIEFDescriptor::BRIEFDescriptor(int size,
                                 int patchSize,
                                 SamplingGeometry samplingGeometry,
                                 bool steer)
  : randr((std::random_device())())
{
  this->steer = steer;
  this->size = size;
  this->patch_size = patchSize;
  this->sampling_geometry = samplingGeometry;
}

std::vector<FeatureDescriptor>
BRIEFDescriptor::Run(Matrix<double>& img, std::vector<Keypoint> keypoints)
{
  std::vector<FeatureDescriptor> descriptors;
  Pixel x1(0), x2(0);

  // TODO: Is it faster to convolve each patch individually? (I checked,
  // doesn't seem so..)
  // TODO: Generate all needed samples in a separate thread, and finally add
  // them all to each keypoint center
  for (auto kp : keypoints) {
    std::string featureVector(this->size, '0');
    for (auto& b : featureVector) {
      // TODO: Use a if-constexpr ?
      switch (this->sampling_geometry) {
        case UNIFORM: {
          const int spread = patch_size / 2;
          std::uniform_int_distribution<int> distribution_x(-spread, spread);
          std::uniform_int_distribution<int> distribution_y(-spread, spread);
          x1.x = distribution_x(randr);
          x1.y = distribution_y(randr);
          x2.x = distribution_x(randr);
          x2.y = distribution_y(randr);
        } break;
        case GAUSSIAN_I: {
          const double spread = 0.04 * (patch_size / 2 * patch_size / 2);
          std::normal_distribution<double> distribution_x(0, spread);
          std::normal_distribution<double> distribution_y(0, spread);
          x1.x = std::clamp(
            (int)round(distribution_x(randr)), -patch_size / 2, patch_size / 2);
          x1.y = std::clamp(
            (int)round(distribution_y(randr)), -patch_size / 2, patch_size / 2);
          x2.x = std::clamp(
            (int)round(distribution_x(randr)), -patch_size / 2, patch_size / 2);
          x2.y = std::clamp(
            (int)round(distribution_y(randr)), -patch_size / 2, patch_size / 2);
        } break;
        case GAUSSIAN_II: {
          const double spread = 0.04 * (patch_size / 2 * patch_size / 2);
          std::normal_distribution<double> distribution_x1_x(0, spread);
          std::normal_distribution<double> distribution_x1_y(0, spread);
          x1.x = std::clamp(
            (int)distribution_x1_x(randr), -patch_size / 2, patch_size / 2);
          x1.y = std::clamp(
            (int)distribution_x1_y(randr), -patch_size / 2, patch_size / 2);
          std::normal_distribution<double> distribution_x2_x(0, spread);
          std::normal_distribution<double> distribution_x2_y(0, spread);
          x2.x = std::clamp(
            (int)distribution_x2_x(randr), -patch_size / 2, patch_size / 2);
          x2.y = std::clamp(
            (int)distribution_x2_y(randr), -patch_size / 2, patch_size / 2);
        } break;
        case COARSE_POLAR_GRID_I: // TODO
          // Both x and y pixels in the random pair are sampled
          // from discrete locations of a coarse polar grid
          // introducing a spatial quantization.
          break;
        case COARSE_POLAR_GRID_II: // TODO
          // Pick x at (0, 0) (which is kp) and y from discrete
          // locations of a coarse polar grid.
          break;
        default:
          x1.x = x1.y = x2.x = x2.y = 0;
      }
      if (this->steer) {
        // Rotate the sampled points by R_theta, obtained from
        // PatchOrientation()
        // TODO: Compute cos(theta) and sin(theta) only once
        float theta = kp.orientation;
        // theta *= (float)(M_PI/180.0); // Convert from deg to rad (but fucking
        // why???)
        x1.x = round(cos(theta) * x1.x - sin(theta) * x1.y);
        x1.y = round(sin(theta) * x1.x + cos(theta) * x1.y);
        x2.x = round(cos(theta) * x2.x - sin(theta) * x2.y);
        x2.y = round(sin(theta) * x2.x + cos(theta) * x2.y);
      }
      b = img(kp.y + x1.y, kp.x + x1.x) < img(kp.y + x2.y, kp.x + x2.x) ? '1'
                                                                        : '0';
    }
    descriptors.push_back(FeatureDescriptor(kp.x, kp.y, featureVector));
  }

  return descriptors;
}
}
