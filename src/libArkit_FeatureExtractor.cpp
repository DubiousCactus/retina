/*
 * libArkit_FeatureExtractor.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include <algorithm>
#include <iostream>
#include <vector>

namespace ARKIT
{

    struct FeatureDescriptor {
        int x;
        int y;
    };

    struct Pixel {
        int x;
        int y;
        unsigned char intensity;

        Pixel(int x, int y)
        {
            this->x = x;
            this->y = y;
            this->intensity = 0;
        }

        Pixel(unsigned char intensity)
        {
            this->intensity = intensity;
        }

        bool operator() (const Pixel& p) const
        {
            return (p.x == this->x) && (p.y == this->y);
        }
    };

    struct Frame {
        std::vector<Pixel> pixels;
        unsigned int width;
        unsigned int height;
        int channels;

        Pixel at(int x, int y)
        {
            return this->pixels.at((y*this->width)+x);
        }
    };

    struct Keypoint {

    };

    struct ScalePyramid {
        std::vector<Frame> scales;
    };

    class FeatureExtractor
    {
        protected:
            std::vector<FeatureDescriptor> features;
            std::vector<Keypoint> keypoints;

        public:
            virtual void Extract(Frame f)=0;
    };


    class ORBExtractor: FeatureExtractor
    {
        private:
            unsigned short intensity_threshold;
            unsigned short contiguous_pixels;
            unsigned short top_n_keypoints;
            unsigned short n_keypoints;
            unsigned short pog_levels;
            unsigned short radius;

            /*
             * Bresenham's circle drawing algorithm
             * TODO: Add the intensity of the pixels!!
             */
            std::vector<Pixel> BresenhamCircle(Pixel center, int radius)
            {
                std::vector<Pixel> circlePixels;
                Pixel to(0, radius);
                int d = 3 - (2 * radius);

                auto EightWaySymmetricPlot = [] (Pixel c, Pixel t) { 
                    std::vector<Pixel> quadrant;
                    quadrant.push_back(Pixel(c.x + t.x, c.y - t.y));
                    quadrant.push_back(Pixel(c.x + t.x, c.y + t.y));
                    quadrant.push_back(Pixel(c.x - t.x, c.y + t.y));
                    quadrant.push_back(Pixel(c.x - t.x, c.y - t.y));
                    quadrant.push_back(Pixel(c.x + t.y, c.y + t.x));
                    quadrant.push_back(Pixel(c.x - t.y, c.y + t.x));
                    quadrant.push_back(Pixel(c.x + t.y, c.y - t.x));
                    quadrant.push_back(Pixel(c.x - t.y, c.y - t.x));

                    return quadrant;
                };

                auto quadrant = EightWaySymmetricPlot(center, to);
                circlePixels.insert(std::end(circlePixels),
                        std::begin(quadrant), std::end(quadrant));
                while (to.y >= to.x) {
                    to.x++;
                    if (d > 0) {
                        to.y--;
                        d = d + 4*(to.x-to.y) + 10;
                    } else {
                        d = d + 4*to.x + 6;
                    }

                    quadrant = EightWaySymmetricPlot(center, to);
                    circlePixels.insert(std::end(circlePixels),
                            std::begin(quadrant), std::end(quadrant));
                }

                /* Remove duplicates  and reorder */
                std::vector<Pixel> circle;
                for (auto p = circlePixels.begin(); p != circlePixels.end(); p++) {
                    if (std::find_if(circle.begin(), circle.end(), Pixel(p->x,
                                    p->y)) == circle.end())
                        circle.push_back(*p);
                }

                // TODO: Optimize and refactor
                int q = 0, quadLength = circle.size() / 4;
                long unsigned int circleSize = circle.size();
                bool search;
                std::vector<Pixel> sortedCircle;
                sortedCircle.push_back(circle.at(0));
                for (long unsigned int i = 0; i < circleSize; i++) {
                    if (i != 0 && i % quadLength == 0) q++;
                    search = true;
                    for (auto p = circle.begin() + 1; p != circle.end() && search; p++) {
                        switch (q) {
                            case 0:
                                if (((p->x - sortedCircle.back().x) == 1
                                        || p->x == sortedCircle.back().x)
                                    && (p->y == sortedCircle.back().y
                                        || (p->y - sortedCircle.back().y) == 1)) {
                                    sortedCircle.push_back(*p);
                                    circle.erase(p);
                                    search = false;
                                }
                                break;
                            case 1:
                                if (((p->x - sortedCircle.back().x) == -1
                                        || p->x == sortedCircle.back().x)
                                    && (p->y == sortedCircle.back().y
                                        || (p->y - sortedCircle.back().y) == 1)) {
                                    sortedCircle.push_back(*p);
                                    circle.erase(p);
                                    search = false;
                                }
                                break;
                            case 2:
                                if (((p->x - sortedCircle.back().x) == -1
                                        || p->x == sortedCircle.back().x)
                                    && (p->y == sortedCircle.back().y
                                        || (p->y - sortedCircle.back().y) == -1)) {
                                    sortedCircle.push_back(*p);
                                    circle.erase(p);
                                    search = false;
                                }
                                break;
                            case 3:
                                if (((p->x - sortedCircle.back().x) == 1
                                        || p->x == sortedCircle.back().x)
                                    && (p->y == sortedCircle.back().y
                                        || (p->y - sortedCircle.back().y) == -1)) {
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
            std::vector<Keypoint> FAST(Frame f)
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
                 * 6. Non-maximal suppression
                 */
                int keypoints = 0;

                for (unsigned int y = this->radius; y < (f.height - this->radius); y++) {
                    for (unsigned int x = this->radius; x < (f.width - this->radius); x++) {
                        int Ip = f.at(x,y).intensity;
                        Pixel center(x, y);
                        center.intensity = Ip;

                        /* High-speed non-corner elimination */
                        if (this->contiguous_pixels == 12) {
                            if (f.at(x,y-3).intensity < (Ip +
                                        this->intensity_threshold)
                                    || f.at(x, y+3).intensity
                                        > (Ip - this->intensity_threshold)) {
                                // Cannot be a corner
                                break;
                            } else if (f.at(x-3, y).intensity
                                    < (Ip + this->intensity_threshold)
                                    || f.at(x+3, y).intensity
                                        > (Ip - this->intensity_threshold)) {
                                // Cannot be a corner
                                break;
                            } else {
                                /* At least 3 of those 4 pixels must be brighter
                                 * than p
                                 */
                                std::vector<Pixel> testPixels = {
                                    f.at(x, y-3), f.at(x, y+3),
                                    f.at(x-3, y), f.at(x+3, y)};
                                int passed = 0;
                                for (auto p = testPixels.begin(); p <
                                        testPixels.end() && passed < 3; p++) {
                                    if (p->intensity > (Ip + this->intensity_threshold)
                                        || p->intensity < (Ip - this->intensity_threshold))
                                        passed++;
                                }
                                if (passed < 3) {
                                    // Cannot be a corner
                                    break;
                                }
                            }
                        }

                        /* Complete corner test */
                        std::vector<Pixel> circle = this->BresenhamCircle(center, this->radius);
                        auto GetPixelWithOverflow = [] (std::vector<Pixel> v, int i) {
                            int circleSize = v.size();
                            if (i >= circleSize)
                                return v.at(circleSize - i);
                            else
                                return v.at(i);
                        };
                        bool corner = false;
                        for (int i = 0; i < (int)circle.size(); i++) {
                            std::vector<Pixel> contiguousPixels = {};
                            for (int j = 0; j < this->contiguous_pixels; j++)
                                contiguousPixels.push_back(GetPixelWithOverflow(circle, i+j));

                            bool brighter = true, darker = true;
                            for (auto p = contiguousPixels.begin(); p !=
                                    contiguousPixels.end(); p++) {
                                if (p->intensity <= (Ip + this->intensity_threshold)) {
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
                                if (p->intensity >= (Ip - this->intensity_threshold)) {
                                    darker = false;
                                    break;
                                }
                            }

                            if (darker) {
                                corner = true;
                                break;
                            }
                        }

                        if (corner)
                            keypoints++;
                    }
                }

                std::cout<< "\t-> Found " << keypoints << " keypoints" << std::endl;

                return std::vector<Keypoint>();
            }

            /* Order the FAST keypoints and return the N top points using the
             * Harris corner measure
             */
            std::vector<Keypoint> HarrisFilter(std::vector<Keypoint> keypoints)
            {
                return std::vector<Keypoint>();
            }

            /* Build a scale pyramid of the base image */
            ScalePyramid BuildPyramid(Frame f)
            {
                ScalePyramid pyramid;
                for (unsigned short i = 0; i < this->pog_levels; i++)
                    pyramid.scales.push_back(f);

                return pyramid;
            }

            float IntensityCentroid();

        public:
            ORBExtractor()
            {
                this->intensity_threshold = 9;
                this->contiguous_pixels = 12;
                this->top_n_keypoints = 50;
                this->n_keypoints = 150;
                this->pog_levels = 1;
                this->radius = 3; // For a circle of 16 pixels
            }
            ~ORBExtractor() {}

            void Extract(Frame f)
            {
                /*
                 * 1. Build the PoG to produce multiscale-features
                 * 2. Find keypoints with FAST at each level of the pyramid
                 * 3. Apply Harris corner measure to find the top N points
                 */

                // STEP 1: Build the scale pyramid of the current frame
                std::cout << "\t-> Building the pyramid" << std::endl;
                ScalePyramid pyramid = this->BuildPyramid(f);
                // STEP 2: for each level of the PoG
                for (unsigned short i = 0; i < this->pog_levels; i++) {
                    std::vector<Keypoint> keypoints = this->HarrisFilter(
                            this->FAST(pyramid.scales[i]));
                }
            }

            std::vector<Keypoint> GetKeypoints()
            {
                return this->keypoints;
            }

            void Describe()
            {
                /*
                 * 1. Apply BRIEF on the keypoints
                 */
            }
    };
}
