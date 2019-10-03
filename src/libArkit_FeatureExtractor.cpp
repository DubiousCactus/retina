/*
 * libArkit_FeatureExtractor.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "libArkit_FeatureExtractor.h"

namespace ARKIT
{

    /*
     * Bresenham's circle drawing algorithm
     */
    std::vector<Pixel*> ORBExtractor::BresenhamCircle(Pixel center, int radius, Frame* frame)
    {
        std::vector<Pixel*> circlePixels;
        circlePixels.reserve(radius*8);
        Pixel to(0, radius);
        int d = 3 - (2 * radius);

        auto EightWaySymmetricPlot = [] (Pixel c, Pixel t, Frame* f,
                std::vector<Pixel*>& circle) {
            circle.push_back(f->PixelAt(c.x + t.x, c.y - t.y));
            circle.push_back(f->PixelAt(c.x + t.x, c.y + t.y));
            circle.push_back(f->PixelAt(c.x - t.x, c.y + t.y));
            circle.push_back(f->PixelAt(c.x - t.x, c.y - t.y));
            circle.push_back(f->PixelAt(c.x + t.y, c.y + t.x));
            circle.push_back(f->PixelAt(c.x - t.y, c.y + t.x));
            circle.push_back(f->PixelAt(c.x + t.y, c.y - t.x));
            circle.push_back(f->PixelAt(c.x - t.y, c.y - t.x));
        };

        EightWaySymmetricPlot(center, to, frame, circlePixels);
        while (to.y >= to.x) {
            to.x++;
            if (d > 0) {
                to.y--;
                d = d + 4*(to.x-to.y) + 10;
            } else {
                d = d + 4*to.x + 6;
            }

            EightWaySymmetricPlot(center, to, frame, circlePixels);
        }

        /* TODO: Optimize by skipping the duplicates removal and sorting. The whole parsing can be done in the "sorting phase" directly */

        /* Remove duplicates  and reorder */
        std::vector<Pixel*> circle;
        circle.reserve(radius*8);
        for (auto p = circlePixels.begin(); p != circlePixels.end(); p++) {
            if (std::find_if(circle.begin(), circle.end(), *(*p)) == circle.end())
                circle.push_back(*p);
        }

        // TODO: Optimize and refactor
        int q = 0, quadLength = circle.size() / 4;
        long unsigned int circleSize = circle.size();
        bool search;
        std::vector<Pixel*> sortedCircle;
        sortedCircle.reserve(8*radius);
        sortedCircle.push_back(circle.at(0));
        assert(quadLength != 0);
        for (long unsigned int i = 0; i < circleSize; i++) {
            if (i != 0 && i % quadLength == 0) q++;
            search = true;
            for (auto p = circle.begin() + 1; p != circle.end() && search; p++) {
                switch (q) {
                    case 0:
                        if ((((*p)->x - sortedCircle.back()->x) == 1
                                    || (*p)->x == sortedCircle.back()->x)
                                && ((*p)->y == sortedCircle.back()->y
                                    || ((*p)->y - sortedCircle.back()->y) == 1)) {
                            sortedCircle.push_back(*p);
                            circle.erase(p);
                            search = false;
                        }
                        break;
                    case 1:
                        if ((((*p)->x - sortedCircle.back()->x) == -1
                                    || (*p)->x == sortedCircle.back()->x)
                                && ((*p)->y == sortedCircle.back()->y
                                    || ((*p)->y - sortedCircle.back()->y) == 1)) {
                            sortedCircle.push_back(*p);
                            circle.erase(p);
                            search = false;
                        }
                        break;
                    case 2:
                        if ((((*p)->x - sortedCircle.back()->x) == -1
                                    || (*p)->x == sortedCircle.back()->x)
                                && ((*p)->y == sortedCircle.back()->y
                                    || ((*p)->y - sortedCircle.back()->y) == -1)) {
                            sortedCircle.push_back(*p);
                            circle.erase(p);
                            search = false;
                        }
                        break;
                    case 3:
                        if ((((*p)->x - sortedCircle.back()->x) == 1
                                    || (*p)->x == sortedCircle.back()->x)
                                && ((*p)->y == sortedCircle.back()->y
                                    || ((*p)->y - sortedCircle.back()->y) == -1)) {
                            sortedCircle.push_back(*p);
                            circle.erase(p);
                            search = false;
                        }
                        break;
                }
            }
        }

        return sortedCircle;
    }

    /* Extract N keypoints in the given frame, using the Features from
     * Accelerated Segment Test algorithm with a given circular radius
     * (threshold)
     */
    std::vector<Keypoint> ORBExtractor::FAST(Frame* f)
    {
        std::cout << "\t-> Extracting keypoints (FAST)..." << std::endl;
        /*
         * 1. Select a pixel P in the image, with intensity I
         * 2. Select appropriate threshold value t
         * 3. Run a high-speed test if n=12: exiamine pixels 1 and 9,
         * then 5 and 13 if P can still be a corner.
         * 4. Consider a circle of 16 pixels around the pixel under test
         * 5. P is a corner if there exists a set of n contiguous pixels
         *  in the circle which are all brighter than I + t, or all
         *  darker than I - t. (n, or threshold, can be chosen to be 12)
         * 6. Non-maximal suppression (not in the case of ORB though)
         */
        int keypoints = 0;

        for (int y = this->radius; y < (f->Height() - this->radius); y++) {
            for (int x = this->radius; x < (f->Width() - this->radius); x++) {
                int Ip = f->RawAt(x,y);
                Pixel center(x, y);
                center.intensity = Ip;

                /* High-speed non-corner elimination */
                int upperBound = Ip + this->intensity_threshold;
                int lowerBound = Ip - this->intensity_threshold;
                if (this->contiguous_pixels == 12) {
                    if (this->full_high_speed_test) {
                        if ((f->RawAt(x,y-3) <= upperBound
                                    && f->RawAt(x,y-3) >= lowerBound)
                                || (f->RawAt(x, y+3) <= upperBound
                                    && f->RawAt(x, y+3) >= lowerBound)) {
                            //Cannot be a corner
                            continue;
                        } else if ((f->RawAt(x-3,y) <= upperBound
                                    && f->RawAt(x-3,y) >= lowerBound)
                                || (f->RawAt(x+3, y) <= upperBound
                                    && f->RawAt(x+3, y) >= lowerBound)) {
                            //Cannot be a corner
                            continue;
                        }
                    }
                    /* At least 3 of those 4 pixels must be brighter than p */
                    std::vector<uint8_t> testPixels = {
                        f->RawAt(x, y-3), f->RawAt(x, y+3),
                        f->RawAt(x-3, y), f->RawAt(x+3, y)
                    };
                    bool allAbove = true, allBelow = true;
                    for (auto p = testPixels.begin(); p != testPixels.end(); p++) {
                        if (*p <= upperBound)
                            allAbove = false;
                        if (*p >= lowerBound)
                            allBelow = false;
                    }

                    if (!allAbove && !allBelow)
                        continue;
                }

                /* Complete corner test */
                auto circle = this->BresenhamCircle(center, this->radius, f);
                auto GetPixelWithOverflow = [] (std::vector<Pixel*>& v, unsigned int i) {
                    assert(i/2 < v.size());
                    unsigned int circleSize = v.size();
                    if (i >= circleSize)
                        return v.at(i - circleSize);
                    else
                        return v.at(i);
                };
                bool corner = false;
                for (unsigned int i = 0; i < (int)circle.size(); i++) {
                    std::vector<Pixel*> contiguousPixels;
                    contiguousPixels.reserve(this->contiguous_pixels);
                    for (unsigned int j = 0; j < this->contiguous_pixels; j++)
                        contiguousPixels.push_back(GetPixelWithOverflow(circle, i+j));

                    bool brighter = true, darker = true;
                    for (auto p = contiguousPixels.begin(); p !=
                            contiguousPixels.end(); p++) {
                        if ((*p)->intensity <= upperBound) {
                            brighter = false;
                            break;
                        }
                    }

                    if (brighter) {
                        corner = true;
                        break;
                    }

                    for (auto p = contiguousPixels.begin(); p !=
                            contiguousPixels.end(); p++) {
                        if ((*p)->intensity >= lowerBound) {
                            darker = false;
                            break;
                        }
                    }

                    if (darker) {
                        corner = true;
                        break;
                    }
                }

                if (corner) {
                    keypoints++;
                    for (auto p = circle.begin(); p != circle.end(); p++)
                        f->WriteAt((*p)->x, (*p)->y, 0);
                }
            }
        }

        std::cout<< "\t-> Found " << keypoints << " keypoints" << std::endl;

        return std::vector<Keypoint>();
    }

    /* Order the FAST keypoints and return the N top points using the
     * Harris corner measure
     */
    std::vector<Keypoint> ORBExtractor::HarrisFilter(/*std::vector<Keypoint> keypoints*/)
    {
        int offset = this->window_size / 2;
        int Sxx, Syy, Sxy;
        double det, trace, r;

        // 1. Spatial derivative calculation
        // Sobel operator kernel
        int sX[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };
        int sY[3][3] = {
            {1, 2, 1},
            {0, 0, 0},
            {-1, -2, -1}
        };
        Matrix<int> sobelX(sX);
        Matrix<int> sobelY(sY);
        Matrix<int> f_x = Matrix<int>::Convolve(this->frame, sobelX);
        Matrix<int> f_y = Matrix<int>::Convolve(this->frame, sobelY);
        Matrix<int> f_xx = f_x * f_x.Transpose();
        Matrix<int> f_xy = f_x * f_y.Transpose();
        Matrix<int> f_yy = f_y * f_y.Transpose();

        /* TODO: Apply Gaussian filter to f_xx, f_xy, f_yy */

        Matrix<float> harrisResponse(this->frame->Height(), this->frame->Width());
        for (int i = offset; i < this->frame->Height() - offset; ++i) {
            for (int j = offset; j < this->frame->Width() - offset; ++j) {
                Sxx = Matrix<int>::Sum(f_xx, j-offset, i-offset, j+offset, i+offset);
                Syy = Matrix<int>::Sum(f_yy, j-offset, i-offset, j+offset, i+offset);
                Sxy = Matrix<int>::Sum(f_xy, j-offset, i-offset, j+offset, i+offset);
                det = (Sxx * Syy) - (Sxy * Sxy);
                trace = Sxx + Syy;
                r = det - this->sensitivity_factor * (trace * trace);
                *harrisResponse.At(i, j) = r;
            }
        }

        return std::vector<Keypoint>();
    }

    /* Build a scale pyramid of the base image */
    ScalePyramid ORBExtractor::BuildPyramid()
    {
        ScalePyramid pyramid;
        for (unsigned short i = 0; i < this->pog_levels; i++)
            pyramid.scales.push_back(this->frame);

        return pyramid;
    }

    float IntensityCentroid();

    ORBExtractor::ORBExtractor(Frame& f)
    {
        this->full_high_speed_test = false;
        this->sensitivity_factor = 0.004;
        this->intensity_threshold = 9;
        this->contiguous_pixels = 12;
        this->top_n_keypoints = 50;
        this->n_keypoints = 150;
        this->window_size = 3;
        this->pog_levels = 1;
        this->radius = 3; // For a circle of 16 pixels
        this->frame = &f;
    }
    ORBExtractor::~ORBExtractor() {}

    void ORBExtractor::Extract()
    {
        /*
         * 1. Build the PoG to produce multiscale-features
         * 2. Find keypoints with FAST at each level of the pyramid
         * 3. Apply Harris corner measure to find the top N points
         */

        assert(this->frame != nullptr);
        // STEP 1: Build the scale pyramid of the current frame
        std::cout << "\t-> Building the pyramid" << std::endl;
        ScalePyramid pyramid = this->BuildPyramid();

        auto start = std::chrono::steady_clock::now();
        this->FAST(this->frame);
        auto end = std::chrono::steady_clock::now();
        std::cout << "[*] FAST executed in "
            <<
            (float)std::chrono::duration_cast<std::chrono::microseconds>(end
                    - start).count()/1000
            << " milliseconds" << std::endl;
        start = std::chrono::steady_clock::now();
        this->HarrisFilter();
        end = std::chrono::steady_clock::now();
        std::cout << "[*] Harris filter executed in "
            <<
            (float)std::chrono::duration_cast<std::chrono::microseconds>(end
                    - start).count()/1000
            << " milliseconds" << std::endl;
        // STEP 2: for each level of the PoG
        /*for (unsigned short i = 0; i < this->pog_levels; i++) {
          std::vector<Keypoint> keypoints = this->HarrisFilter(
          this->FAST(pyramid.scales.at(i)));
          }*/
    }

    Frame ORBExtractor::GetAnnotatedFrame()
    {
        return *this->frame;
    }

    std::vector<Keypoint> ORBExtractor::GetKeypoints()
    {
        return this->keypoints;
    }

    void ORBExtractor::Describe()
    {
        /*
         * 1. Apply BRIEF on the keypoints
         */
    }
}
