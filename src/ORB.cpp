/*
 * ORB.cpp
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#include "ORB.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace retina {
/* Build a scale pyramid of the base image */
ScalePyramid
ORBExtractor::BuildPyramid()
{
  ScalePyramid pyramid;
  /*for (unsigned short i = 0; i < this->pog_levels; i++)
      pyramid.scales.push_back(this->frame);*/

  return pyramid;
}

ORBExtractor::ORBExtractor(size_t n_keypoints)
  : fast_extractor(31, 3, 20, 12, 150, false, true, true, true, 31)
  , harris_extractor(true, true, true)
  , brief_descriptor(256, 31, GAUSSIAN_I, true)
{
  this->n_keypoints = n_keypoints;
  this->pog_levels = 5;
  this->scaling_factor = std::sqrt(2);
}

std::vector<KeyPoint>
ORBExtractor::Extract(const Frame& frame)
{
  /*
   * 1. Build the PoG to produce multiscale-features
   * 2. Find keypoints with FAST at each level of the pyramid
   * 3. Apply Harris corner measure to find the top N points
   */

  // STEP 1: Build the scale pyramid of the current frame
  std::cout << "\t-> Building the pyramid" << std::endl;
//  ScalePyramid pyramid = this->BuildPyramid();

  auto start = std::chrono::steady_clock::now();

  this->keypoints = this->fast_extractor.Extract(frame);

  auto end = std::chrono::steady_clock::now();
  std::cout << "\t-> oFAST executed in "
            << (float)std::chrono::duration_cast<std::chrono::microseconds>(
                 end - start)
                   .count() /
                 1000
            << " milliseconds" << std::endl;
  start = std::chrono::steady_clock::now();

  for (auto kp : this->keypoints) {
    if (kp.x - 3 < 0 || kp.x + 3 >= frame.Width() || kp.y - 3 < 0 ||
        kp.y + 3 >= frame.Height()) {
      continue;
    }
    kp.score = this->harris_extractor.MeasureCorner(frame, kp.x, kp.y, 7);
  }

  end = std::chrono::steady_clock::now();
  std::cout << "\t-> Harris corner measure (for all keypoints) executed in "
            << (float)std::chrono::duration_cast<std::chrono::microseconds>(
                 end - start)
                   .count() /
                 1000
            << " milliseconds" << std::endl;
  std::cout << "\t-> Keeping the top " << this->n_keypoints << " keypoints..."
            << std::endl;

  std::sort(this->keypoints.begin(), this->keypoints.end());
  if (this->n_keypoints <= this->keypoints.size()) {
    this->keypoints.resize(this->n_keypoints);
  }

  start = std::chrono::steady_clock::now();
  Matrix<double> gaussianKernel = Matrix<double>::MakeGaussianKernel(2, 9);
  Matrix<double> img = Matrix<double>::Convolve(frame.GetDoubleMatrix(), gaussianKernel);
  end = std::chrono::steady_clock::now();
  std::cout << "\t-> Gaussian smoothing executed in "
            << (float)std::chrono::duration_cast<std::chrono::microseconds>(
                 end - start)
                   .count() /
                 1000
            << " milliseconds" << std::endl;

  start = std::chrono::steady_clock::now();
  this->features = this->brief_descriptor.Run(img, this->keypoints);
  end = std::chrono::steady_clock::now();
  std::cout << "\t-> sBRIEF descriptors computed in "
            << (float)std::chrono::duration_cast<std::chrono::microseconds>(
                 end - start)
                   .count() /
                 1000
            << " milliseconds" << std::endl;

  this->annotated_frame = std::make_shared<Frame>(Frame(frame));
  // STEP 2: for each level of the PoG
  /*for (unsigned short i = 0; i < this->pog_levels; i++) {
    std::vector<KeyPoint> keypoints = this->HarrisFilter(
    this->FAST(pyramid.scales.at(i)));
    }*/

  return this->keypoints;
}

std::shared_ptr<Frame>
ORBExtractor::GetAnnotatedFrame()
{
    if (annotated_frame == nullptr)
        return nullptr;
  std::cout << "[*] Annotating..." << std::endl;
  for (auto kp : this->keypoints) {
    if (kp.x - 5 < 0 || kp.x + 5 >= annotated_frame->Width() || kp.y - 5 < 0 ||
        kp.y + 5 >= annotated_frame->Height())
      continue;
    annotated_frame->WriteAt(kp.x - 5, kp.y, 0);
    annotated_frame->WriteAt(kp.x - 4, kp.y, 0);
    annotated_frame->WriteAt(kp.x - 3, kp.y, 0);
    annotated_frame->WriteAt(kp.x - 2, kp.y, 0);
    annotated_frame->WriteAt(kp.x - 1, kp.y, 0);
    annotated_frame->WriteAt(kp.x, kp.y, 0);
    annotated_frame->WriteAt(kp.x + 5, kp.y, 0);
    annotated_frame->WriteAt(kp.x + 4, kp.y, 0);
    annotated_frame->WriteAt(kp.x + 3, kp.y, 0);
    annotated_frame->WriteAt(kp.x + 2, kp.y, 0);
    annotated_frame->WriteAt(kp.x + 1, kp.y, 0);
    annotated_frame->WriteAt(kp.x, kp.y - 5, 0);
    annotated_frame->WriteAt(kp.x, kp.y - 4, 0);
    annotated_frame->WriteAt(kp.x, kp.y - 3, 0);
    annotated_frame->WriteAt(kp.x, kp.y - 2, 0);
    annotated_frame->WriteAt(kp.x, kp.y - 1, 0);
    annotated_frame->WriteAt(kp.x, kp.y, 0);
    annotated_frame->WriteAt(kp.x, kp.y + 5, 0);
    annotated_frame->WriteAt(kp.x, kp.y + 4, 0);
    annotated_frame->WriteAt(kp.x, kp.y + 3, 0);
    annotated_frame->WriteAt(kp.x, kp.y + 2, 0);
    annotated_frame->WriteAt(kp.x, kp.y + 1, 0);
  }
  return annotated_frame;
}

std::vector<KeyPoint>
ORBExtractor::GetKeypoints()
{
  return this->keypoints;
}
}
