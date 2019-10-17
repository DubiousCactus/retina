/*
 * libArkit_FAST.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "libArkit_FAST.h"


namespace ARKIT
{
    FASTExtractor::FASTExtractor(unsigned short radius, unsigned short
                    intensity_threshold, unsigned short contiguous_pixels,
                    unsigned short top_n_keypoints, unsigned short n_keypoints,
                    bool full_high_speed_test, bool annotate)
    {
        this->full_high_speed_test = full_high_speed_test;
        this->intensity_threshold = intensity_threshold;
        this->contiguous_pixels = contiguous_pixels;
        this->top_n_keypoints = top_n_keypoints;
        this->n_keypoints = n_keypoints;
        this->radius = radius; // For a circle of 16 pixels
        this->annotate = annotate;
    }

    /*
     * Bresenham's circle drawing algorithm
     */
    std::vector<Pixel*> FASTExtractor::BresenhamCircle(const Pixel center, int
            radius, const Frame* frame)
    {
        std::vector<Pixel*> circlePixels;
        circlePixels.reserve(radius*8);
        Pixel to(0, radius);
        int d = 3 - (2 * radius);

        auto EightWaySymmetricPlot = [] (const Pixel c, Pixel t, const Frame* f,
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

        /* TODO: Optimize by skipping the duplicates removal and sorting. The
         * whole parsing can be done in the "sorting phase" directly */

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
    std::vector<Keypoint> FASTExtractor::Extract(const Frame *f)
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
        this->annotated_frame = annotate ?  new Frame(*f) : NULL;
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
                    this->keypoints.push_back(Keypoint(x, y));
                }
            }
        }

        std::cout<< "\t-> Found " << this->keypoints.size() << " keypoints" << std::endl;

        return this->keypoints;
    }

    Frame* FASTExtractor::GetAnnotatedFrame()
    {
        assert(this->annotated_frame);
        for (auto kp : keypoints) {
            this->annotated_frame->WriteAt(kp.x, kp.y, 0);
        }
        return this->annotated_frame;
    }
}
