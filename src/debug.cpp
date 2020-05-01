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
//    auto frame_small = retina::Matrix<int>::Resize(
//            frame, 225, 300, retina::InterpolationMethod::INTER_AREA);
//    std::cout << "[*] Done!" << std::endl;
//
//    for (int y = 0; y < resizedImage.height(); y++) {
//        for (int x = 0; x < resizedImage.width(); x++) {
//            resizedImage(x, y, 0, 0) = frame_small(y, x);
//            resizedImage(x, y, 0, 1) = frame_small(y, x);
//            resizedImage(x, y, 0, 2) = frame_small(y, x);
//        }
//    }
//    CImgDisplay originalFrame(image, "Original image"), resizedFrame(resizedImage, "Downscaled image");

//    while (!resizedFrame.is_closed()) {
//        resizedFrame.wait();
//        originalFrame.wait();
//    }
}

void annotate(CImg<uint8_t>& image, std::vector<retina::KeyPoint> keypoints)
{

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
}

void video()
{
    retina::StreamParser streamParser("flip.mp4");
    std::optional<retina::Frame> f;
    int i = 0;
    f = streamParser.NextFrame();
    retina::ORBExtractor extractor(5000);

    CImgDisplay annotatedDisp(f->Width(), f->Height());
    CImg<uint8_t> annotatedImage(f->Width(), f->Height(), 1, 1, 0);

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
//    std::vector<retina::KeyPoint> keypoints = extractor.GetKeypoints();
//    std::cout << "[*] Keypoints extracted: " << keypoints.size() << std::endl;
//

//    while (!annotatedDisp.is_closed()) {
//        annotatedDisp.wait();
//    }
}

int main(int argc, char **argv)
{
    std::cout << "[*] Debugging Retina" << std::endl;
    video();
    return 0;
}