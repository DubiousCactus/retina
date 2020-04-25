//
// Created by transpalette on 3/22/20.
//

#include "debug.h"
#include <iostream>
#include <vector>
#include "CImg.h"
#include "retina.h"

using namespace cimg_library;

void debug()
{
    CImg<uint8_t> image("../desk_2.jpg");
    CImg<uint8_t> resizedImage(300, 225);

    auto **data = new uint8_t *[image.height()];
    for (int y = 0; y < image.height(); y++) {
        data[y] = new uint8_t [image.width()];
        for (int x = 0; x < image.width(); x++) {
            data[y][x] = (uint8_t) (0.33 * image(x, y, 0, 0) + 0.33 * image(x, y, 0, 1) +
                                    0.33 * image(x, y, 0, 2));
        }
    }

    auto* frame = new retina::Frame(data, image.width(), image.height());
    std::cout << "[*] Downscaling..." << std::endl;
    auto frame_small = retina::Matrix<int>::Resize(
            frame->GetIntMatrix(), 225, 300, retina::InterpolationMethod::INTER_AREA);
    std::cout << "[*] Done!" << std::endl;

    for (int y = 0; y < resizedImage.height(); y++) {
        for (int x = 0; x < resizedImage.width(); x++) {
            resizedImage(x, y, 0, 0) = *frame_small(y, x);
            resizedImage(x, y, 0, 1) = *frame_small(y, x);
            resizedImage(x, y, 0, 2) = *frame_small(y, x);
        }
    }
    CImgDisplay originalFrame(image, "Original image"), resizedFrame(resizedImage, "Downscaled image");

    while (!resizedFrame.is_closed()) {
        resizedFrame.wait();
        originalFrame.wait();
    }
}

int main(int argc, char **argv)
{
    std::cout << "[*] Debugging ArLite" << std::endl;
    debug();
    return 0;
}