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
#include "libArkit_FeatureExtractor.h"
#include "libArkit_StreamParser.h"

using namespace cimg_library;

void lena()
{
    CImg<uint8_t> image("lena.jpg");

    uint8_t **data = new uint8_t *[image.height()];
    for (int y = 0; y < image.height(); y++) {
        data[y] = new uint8_t [image.width()];
        for (int x = 0; x < image.width(); x++) {
            data[y][x] = (uint8_t) (0.33 * image(x, y, 0, 0) + 0.33 * image(x, y, 0, 1) +
                                            0.33 * image(x, y, 0, 2));
        }
    }

    ARKIT::Frame frame1(data, image.width(), image.height());
    std::cout << "[*] Extracting features..." << std::endl;
    ARKIT::ORBExtractor extractor(frame1);
    extractor.Extract();
    std::vector<ARKIT::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    ARKIT::Frame annotated = extractor.GetAnnotatedFrame();
    CImg<uint8_t> annotatedImage(annotated.Width(), annotated.Height(), 1, 1, 0);

    for (int y = 0; y < annotated.Height(); y++)
        for (int x = 0; x < annotated.Width(); x++)
            annotatedImage(x, y, 0, 0) = annotated.RawAt(x, y);

    CImgDisplay annotatedDisp(annotatedImage,"FAST keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }
}

void squares()
{
    CImg<uint8_t> image("squares2.jpg");

    uint8_t **data = new uint8_t *[image.height()];
    for (int y = 0; y < image.height(); y++) {
        data[y] = new uint8_t [image.width()];
        for (int x = 0; x < image.width(); x++) {
            data[y][x] = (uint8_t) (0.33 * image(x, y, 0, 0) + 0.33 * image(x, y, 0, 1) +
                                            0.33 * image(x, y, 0, 2));
        }
    }

    ARKIT::Frame frame1(data, image.width(), image.height());
    std::cout << "[*] Extracting features..." << std::endl;
    ARKIT::ORBExtractor extractor(frame1);
    extractor.Extract();
    std::vector<ARKIT::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    ARKIT::Frame annotated = extractor.GetAnnotatedFrame();
    CImg<uint8_t> annotatedImage(annotated.Width(), annotated.Height(), 1, 1, 0);

    for (int y = 0; y < annotated.Height(); y++)
        for (int x = 0; x < annotated.Width(); x++)
            annotatedImage(x, y, 0, 0) = annotated.RawAt(x, y);

    CImgDisplay annotatedDisp(annotatedImage,"FAST keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }
}


void video()
{
    ARKIT::StreamParser streamParser("../sample.mpg");
    std::cout << "[*] Extracting features..." << std::endl;
    ARKIT::Frame *f = NULL;
    int i = 0;
    while(i++ < 15)
        f = streamParser.NextFrame();
    ARKIT::ORBExtractor extractor(*f);
    extractor.Extract();
    /*do {
        std::cout << "Frame no " << i++ << std::endl;
        f = streamParser.NextFrame();
        if (!f)
            break;
        ARKIT::ORBExtractor extractor(*f);
        extractor.Extract();
    } while (f);
    //ARKIT::Frame *f = streamParser.NextFrame();
    if (!f) {
        std::cerr << "Empty frame!" << std::endl;
        exit(1);
    }*/
    std::vector<ARKIT::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    ARKIT::Frame annotated = extractor.GetAnnotatedFrame();
    CImg<uint8_t> annotatedImage(annotated.Width(), annotated.Height(), 1, 1, 0);

    for (int y = 0; y < annotated.Height(); y++)
        for (int x = 0; x < annotated.Width(); x++)
            annotatedImage(x, y, 0, 0) = annotated.RawAt(x, y);

    CImgDisplay annotatedDisp(annotatedImage,"FAST keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }
}

int main() {
    lena();

    return 0;
}

