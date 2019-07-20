/*
 * main.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 *
 * This is an example of usage for this library.
 */

#include <vector>
#include "../external/CImg.h"
#include "libArkit_FeatureExtractor.cpp"

using namespace cimg_library;

int main() {
    CImg<unsigned char> image("lena.jpg");

    ARKIT::ORBExtractor extractor;
    // Build the frame
    ARKIT::Frame frame1;
    frame1.pixels = image.data();
    extractor.Extract(frame1);
    std::vector<ARKIT::Keypoint> keypoints = extractor.GetKeypoints();

    return 0;
}

