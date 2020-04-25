/*
 * main.cpp
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 *
 * This is an example of usage for this library.
 */

#include <iostream>
#include <vector>

#include "CImg.h"
#include <arlite/arlite.h>

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

    arlite::Frame* frame1 = new arlite::Frame(data, image.width(), image.height());
    std::cout << "[*] Extracting features..." << std::endl;
    arlite::ORBExtractor extractor(500);
    extractor.Extract(frame1);
    std::vector<arlite::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    for (auto kp : keypoints) {
        if (kp.x -5 < 0 || kp.x + 5 >= image.width()
                || kp.y - 5 < 0 || kp.y + 5 >= image.height())
            continue;
        image(kp.x-5, kp.y, 0) = 0;
        image(kp.x-5, kp.y, 2) = 0;
        image(kp.x-5, kp.y, 1) = 255;
        image(kp.x-4, kp.y, 0) = 0;
        image(kp.x-4, kp.y, 2) = 0;
        image(kp.x-4, kp.y, 1) = 255;
        image(kp.x-3, kp.y, 0) = 0;
        image(kp.x-3, kp.y, 2) = 0;
        image(kp.x-3, kp.y, 1) = 255;
        image(kp.x-2, kp.y, 0) = 0;
        image(kp.x-2, kp.y, 2) = 0;
        image(kp.x-2, kp.y, 1) = 255;
        image(kp.x-1, kp.y, 0) = 0;
        image(kp.x-1, kp.y, 2) = 0;
        image(kp.x-1, kp.y, 1) = 255;
        image(kp.x, kp.y, 0) = 0;
        image(kp.x, kp.y, 2) = 0;
        image(kp.x, kp.y, 1) = 255;
        image(kp.x+5, kp.y, 0) = 0;
        image(kp.x+5, kp.y, 2) = 0;
        image(kp.x+5, kp.y, 1) = 255;
        image(kp.x+4, kp.y, 0) = 0;
        image(kp.x+4, kp.y, 2) = 0;
        image(kp.x+4, kp.y, 1) = 255;
        image(kp.x+3, kp.y, 0) = 0;
        image(kp.x+3, kp.y, 2) = 0;
        image(kp.x+3, kp.y, 1) = 255;
        image(kp.x+2, kp.y, 0) = 0;
        image(kp.x+2, kp.y, 2) = 0;
        image(kp.x+2, kp.y, 1) = 255;
        image(kp.x+1, kp.y, 0) = 0;
        image(kp.x+1, kp.y, 2) = 0;
        image(kp.x+1, kp.y, 1) = 255;
        image(kp.x, kp.y-5, 0) = 0;
        image(kp.x, kp.y-5, 2) = 0;
        image(kp.x, kp.y-5, 1) = 255;
        image(kp.x, kp.y-4, 0) = 0;
        image(kp.x, kp.y-4, 2) = 0;
        image(kp.x, kp.y-4, 1) = 255;
        image(kp.x, kp.y-3, 0) = 0;
        image(kp.x, kp.y-3, 2) = 0;
        image(kp.x, kp.y-3, 1) = 255;
        image(kp.x, kp.y-2, 0) = 0;
        image(kp.x, kp.y-2, 2) = 0;
        image(kp.x, kp.y-2, 1) = 255;
        image(kp.x, kp.y-1, 0) = 0;
        image(kp.x, kp.y-1, 2) = 0;
        image(kp.x, kp.y-1, 1) = 255;
        image(kp.x, kp.y, 0) = 0;
        image(kp.x, kp.y, 2) = 0;
        image(kp.x, kp.y, 1) = 255;
        image(kp.x, kp.y+5, 0) = 0;
        image(kp.x, kp.y+5, 2) = 0;
        image(kp.x, kp.y+5, 1) = 255;
        image(kp.x, kp.y+4, 0) = 0;
        image(kp.x, kp.y+4, 2) = 0;
        image(kp.x, kp.y+4, 1) = 255;
        image(kp.x, kp.y+3, 0) = 0;
        image(kp.x, kp.y+3, 2) = 0;
        image(kp.x, kp.y+3, 1) = 255;
        image(kp.x, kp.y+2, 0) = 0;
        image(kp.x, kp.y+2, 2) = 0;
        image(kp.x, kp.y+2, 1) = 255;
        image(kp.x, kp.y+1, 0) = 0;
        image(kp.x, kp.y+1, 2) = 0;
        image(kp.x, kp.y+1, 1) = 255;
    }
    CImgDisplay annotatedDisp(image,"ORB keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }
}

void desk()
{
    CImg<uint8_t> image("desk_2.jpg");

    uint8_t **data = new uint8_t *[image.height()];
    for (int y = 0; y < image.height(); y++) {
        data[y] = new uint8_t [image.width()];
        for (int x = 0; x < image.width(); x++) {
            data[y][x] = (uint8_t) (0.33 * image(x, y, 0, 0) + 0.33 * image(x, y, 0, 1) +
                                            0.33 * image(x, y, 0, 2));
        }
    }

    arlite::Frame* frame1 = new arlite::Frame(data, image.width(), image.height());
    std::cout << "[*] Extracting features..." << std::endl;
    arlite::ORBExtractor extractor(500);
    extractor.Extract(frame1);
    std::vector<arlite::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    for (auto kp : keypoints) {
        if (kp.x -5 < 0 || kp.x + 5 >= image.width()
                || kp.y - 5 < 0 || kp.y + 5 >= image.height())
            continue;
        image(kp.x-5, kp.y, 0) = 0;
        image(kp.x-5, kp.y, 2) = 0;
        image(kp.x-5, kp.y, 1) = 255;
        image(kp.x-4, kp.y, 0) = 0;
        image(kp.x-4, kp.y, 2) = 0;
        image(kp.x-4, kp.y, 1) = 255;
        image(kp.x-3, kp.y, 0) = 0;
        image(kp.x-3, kp.y, 2) = 0;
        image(kp.x-3, kp.y, 1) = 255;
        image(kp.x-2, kp.y, 0) = 0;
        image(kp.x-2, kp.y, 2) = 0;
        image(kp.x-2, kp.y, 1) = 255;
        image(kp.x-1, kp.y, 0) = 0;
        image(kp.x-1, kp.y, 2) = 0;
        image(kp.x-1, kp.y, 1) = 255;
        image(kp.x, kp.y, 0) = 0;
        image(kp.x, kp.y, 2) = 0;
        image(kp.x, kp.y, 1) = 255;
        image(kp.x+5, kp.y, 0) = 0;
        image(kp.x+5, kp.y, 2) = 0;
        image(kp.x+5, kp.y, 1) = 255;
        image(kp.x+4, kp.y, 0) = 0;
        image(kp.x+4, kp.y, 2) = 0;
        image(kp.x+4, kp.y, 1) = 255;
        image(kp.x+3, kp.y, 0) = 0;
        image(kp.x+3, kp.y, 2) = 0;
        image(kp.x+3, kp.y, 1) = 255;
        image(kp.x+2, kp.y, 0) = 0;
        image(kp.x+2, kp.y, 2) = 0;
        image(kp.x+2, kp.y, 1) = 255;
        image(kp.x+1, kp.y, 0) = 0;
        image(kp.x+1, kp.y, 2) = 0;
        image(kp.x+1, kp.y, 1) = 255;
        image(kp.x, kp.y-5, 0) = 0;
        image(kp.x, kp.y-5, 2) = 0;
        image(kp.x, kp.y-5, 1) = 255;
        image(kp.x, kp.y-4, 0) = 0;
        image(kp.x, kp.y-4, 2) = 0;
        image(kp.x, kp.y-4, 1) = 255;
        image(kp.x, kp.y-3, 0) = 0;
        image(kp.x, kp.y-3, 2) = 0;
        image(kp.x, kp.y-3, 1) = 255;
        image(kp.x, kp.y-2, 0) = 0;
        image(kp.x, kp.y-2, 2) = 0;
        image(kp.x, kp.y-2, 1) = 255;
        image(kp.x, kp.y-1, 0) = 0;
        image(kp.x, kp.y-1, 2) = 0;
        image(kp.x, kp.y-1, 1) = 255;
        image(kp.x, kp.y, 0) = 0;
        image(kp.x, kp.y, 2) = 0;
        image(kp.x, kp.y, 1) = 255;
        image(kp.x, kp.y+5, 0) = 0;
        image(kp.x, kp.y+5, 2) = 0;
        image(kp.x, kp.y+5, 1) = 255;
        image(kp.x, kp.y+4, 0) = 0;
        image(kp.x, kp.y+4, 2) = 0;
        image(kp.x, kp.y+4, 1) = 255;
        image(kp.x, kp.y+3, 0) = 0;
        image(kp.x, kp.y+3, 2) = 0;
        image(kp.x, kp.y+3, 1) = 255;
        image(kp.x, kp.y+2, 0) = 0;
        image(kp.x, kp.y+2, 2) = 0;
        image(kp.x, kp.y+2, 1) = 255;
        image(kp.x, kp.y+1, 0) = 0;
        image(kp.x, kp.y+1, 2) = 0;
        image(kp.x, kp.y+1, 1) = 255;
    }

    CImgDisplay annotatedDisp(image,"ORB keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }
}

void mini_desk()
{
    CImg<uint8_t> image("desk_2.jpg");
    CImg<uint8_t> resizedImage(256, 256);

    uint8_t **data = new uint8_t *[image.height()];
    for (int y = 0; y < image.height(); y++) {
        data[y] = new uint8_t [image.width()];
        for (int x = 0; x < image.width(); x++) {
            data[y][x] = (uint8_t) (0.33 * image(x, y, 0, 0) + 0.33 * image(x, y, 0, 1) +
                                            0.33 * image(x, y, 0, 2));
        }
    }

    arlite::Frame* frame = new arlite::Frame(data, image.width(), image.height());
    std::cout << "[*] Downscaling..." << std::endl;
    auto frame_small = arlite::Matrix<int>::Resize(
            frame->GetIntMatrix(), 256, 256, arlite::InterpolationMethod::INTER_AREA);
    std::cout << "[*] Done!" << std::endl;

    for (int y = 0; y < resizedImage.height(); y++) {
        for (int x = 0; x < resizedImage.width(); x++) {
            resizedImage(x, y, 0, 0) = *frame_small(y, x);
        }
    }
    CImgDisplay originalFrame(image, "Original image"), resizedFrame(resizedImage, "Downscaled image");

    while (!resizedFrame.is_closed()) {
        resizedFrame.wait();
        originalFrame.wait();
    }
}

void squares()
{
    CImg<uint8_t> image("squares.jpg");

    uint8_t **data = new uint8_t *[image.height()];
    for (int y = 0; y < image.height(); y++) {
        data[y] = new uint8_t [image.width()];
        for (int x = 0; x < image.width(); x++) {
            data[y][x] = (uint8_t) (0.33 * image(x, y, 0, 0) + 0.33 * image(x, y, 0, 1) +
                                            0.33 * image(x, y, 0, 2));
        }
    }

    arlite::Frame* frame1 = new arlite::Frame(data, image.width(), image.height());
    std::cout << "[*] Extracting features..." << std::endl;
    arlite::ORBExtractor extractor(500);
    extractor.Extract(frame1);
    std::vector<arlite::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    arlite::Frame* annotated = extractor.GetAnnotatedFrame();
    CImg<uint8_t> annotatedImage(annotated->Width(), annotated->Height(), 1, 1, 0);

    for (int y = 0; y < annotated->Height(); y++)
        for (int x = 0; x < annotated->Width(); x++)
            annotatedImage(x, y, 0, 0) = annotated->RawAt(x, y);

    CImgDisplay annotatedDisp(annotatedImage,"FAST keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }
}


void video()
{
    arlite::StreamParser streamParser("desk.mpeg");
    std::cout << "[*] Extracting features..." << std::endl;
    arlite::Frame *f = NULL;
    int i = 0;
    while(i++ < 10)
        f = streamParser.NextFrame();
    arlite::ORBExtractor extractor(500);
    extractor.Extract(f);
    /*do {
        std::cout << "Frame no " << i++ << std::endl;
        f = streamParser.NextFrame();
        if (!f)
            break;
        retina::ORBExtractor extractor(*f);
        extractor.Extract();
    } while (f);
    //retina::Frame *f = streamParser.NextFrame();
    if (!f) {
        std::cerr << "Empty frame!" << std::endl;
        exit(1);
    }*/
    std::vector<arlite::Keypoint> keypoints = extractor.GetKeypoints();
    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;

    arlite::Frame* annotated = extractor.GetAnnotatedFrame();
    CImg<uint8_t> annotatedImage(annotated->Width(), annotated->Height(), 1, 1, 0);

    for (int y = 0; y < annotated->Height(); y++)
        for (int x = 0; x < annotated->Width(); x++)
            annotatedImage(x, y, 0, 0) = annotated->RawAt(x, y);

    CImgDisplay annotatedDisp(annotatedImage,"FAST keypoints");

    while (!annotatedDisp.is_closed()) {
        annotatedDisp.wait();
    }
}

int main() {
    //desk();
    mini_desk();
    //lena();

    return 0;
}

