/*
 * main.cpp
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 *
 * This is an example usage for this library.
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


void video()
{
    arlite::StreamParser streamParser("flip.mp4");
    std::optional<retina::Frame> f;
    int i = 0;
    f = streamParser.NextFrame();
    retina::ORBExtractor extractor(5000);

    CImgDisplay annotatedDisp(f->Width(), f->Height());
    CImg<uint8_t> annotatedImage(f->Width(), f->Height(), 1, 1, 0);
    sleep(4);

    while(f.has_value()) {
        std::cout << "Frame " << i++ << std::endl;
        extractor.Extract(*f);
        std::shared_ptr<retina::Frame> annotated = extractor.GetAnnotatedFrame();
//
        for (int y = 0; y < f->Height(); y++)
            for (int x = 0; x < f->Width(); x++)
                annotatedImage(x, y, 0, 0) = (*annotated)(x, y);

//        annotate(annotatedImage, extractor.GetKeypoints());

        annotatedDisp.render(annotatedImage);
        annotatedDisp.paint();
        f = streamParser.NextFrame();
    }
    std::cout << "End of video stream" << std::endl;
}

int main() {
    //lena();
    video();

    return 0;
}

