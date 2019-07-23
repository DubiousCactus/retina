/*
 * main.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 *
 * This is an example of usage for this library.
 */

#include <iostream>
#include <vector>

#include "../external/CImg.h"
#include "libArkit_FeatureExtractor.cpp"

using namespace cimg_library;

int main() {
    CImg<unsigned char> image("shapes.jpg");

    ARKIT::Frame frame1(image.width(), image.height());
    for (int y = 0; y < image.height(); y++)
        for (int x = 0; x < image.width(); x++)
            frame1.pixels.push_back(ARKIT::Pixel(x, y, 0.33*image(x, y, 0, 0)
                        + 0.33*image(x, y, 0, 1) + 0.33*image(x, y, 0, 2)));

    std::cout << "[*] Extracting features..." << std::endl;
    ARKIT::ORBExtractor extractor(frame1);
    extractor.Extract();
    std::vector<ARKIT::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    ARKIT::Frame annotated = extractor.GetAnnotatedFrame();
    CImg<unsigned char> annotatedImage(frame1.width, frame1.height, 1, 1, 0);

    for (unsigned int y = 0; y < annotated.height; y++)
        for (unsigned int x = 0; x < annotated.width; x++)
            annotatedImage(x, y, 0, 0) = annotated.at(x,y).intensity;

    CImgDisplay annotatedDisp(annotatedImage,"FAST keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }

    return 0;
}

