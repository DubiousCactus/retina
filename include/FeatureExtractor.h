/*
 * FeatureExtractor.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

#include "Frame.h"

namespace retina {

struct FeatureDescriptor
{
    int x;
    int y;
    std::string binary_descriptor;

    FeatureDescriptor(int x, int y, std::string descriptor)
    {
        this->x = x;
        this->y = y;
        this->binary_descriptor = std::move(descriptor);
    }
};

struct KeyPoint
{
    int x;
    int y;
    float score;
    float orientation;

    KeyPoint()
    {
        x = -1;
        y = -1;
        score = 0;
        orientation = 0;
    }

    KeyPoint(int x, int y, float score)
    {
        this->x = x;
        this->y = y;
        this->score = score;
        this->orientation = 0;
    }

    KeyPoint(int x, int y, float score, float orientation)
    {
        this->x = x;
        this->y = y;
        this->score = score;
        this->orientation = orientation;
    }

    KeyPoint(int x, int y)
    {
        this->x = x;
        this->y = y;
        this->score = 0;
        this->orientation = 0;
    }

    bool operator<(const KeyPoint& kp) const { return score < kp.score; }
};

struct ScalePyramid
{
    std::vector<Frame*> scales;
};

class FeatureExtractor
{
  protected:
    std::vector<FeatureDescriptor> features;
    std::vector<KeyPoint> keypoints;
    Frame* annotated_frame;
    bool non_max_suppression;
    bool annotate;

  public:
    virtual std::vector<KeyPoint> Extract(const Frame* frame) = 0;
    virtual Frame* GetAnnotatedFrame() = 0;
};
}

#endif /* !FEATUREEXTRACTOR_H */
