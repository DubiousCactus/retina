/*
 * libArkit_FeatureExtractor.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

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
        unsigned char value;

        Pixel(int x, int y)
        {
            this->x = x;
            this->y = y;
            this->value = 0;
        }

        Pixel(unsigned char value)
        {
            this->value = value;
        }
    };

    struct Frame {
        std::vector<Pixel> pixels;
        unsigned int width;
        unsigned int height;
        int channels;
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
            unsigned short top_n_keypoints;
            unsigned short n_keypoints;
            unsigned short pog_levels;
            unsigned short radius;

            /*
             * Bresenham's circle drawing algorithm
             */
            std::vector<Pixel> BresenhamCircle(Pixel center, int radius)
            {
                std::cout << "\t-> BresenhamCircle([" << center.x << "," <<
                    center.y << "], " << radius << ")" << std::endl;
                std::vector<Pixel> circlePixels;
                Pixel to(0, radius);
                int d = 3 - (2 * radius);

                auto drawQuadrant = [] (Pixel c, Pixel t) { 
                    std::vector<Pixel> quadrant;
                    quadrant.push_back(Pixel(c.x + t.x, c.y + t.y));
                    quadrant.push_back(Pixel(c.x - t.x, c.y + t.y));
                    quadrant.push_back(Pixel(c.x + t.x, c.y - t.y));
                    quadrant.push_back(Pixel(c.x - t.x, c.y - t.y));
                    quadrant.push_back(Pixel(c.x + t.y, c.y + t.x));
                    quadrant.push_back(Pixel(c.x - t.y, c.y + t.x));
                    quadrant.push_back(Pixel(c.x + t.y, c.y - t.x));
                    quadrant.push_back(Pixel(c.x - t.y, c.y - t.x));

                    return quadrant;
                };

                auto quadrant = drawQuadrant(center, to);
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

                    quadrant = drawQuadrant(center, to);
                    circlePixels.insert(std::end(circlePixels),
                            std::begin(quadrant), std::end(quadrant));
                }

                return circlePixels;
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

                std::cout << "FRAME INFO: " << f.width << "x" << f.height << std::endl;
                for (unsigned int y = this->radius; y < (f.height - this->radius); y++) {
                    for (unsigned int x = this->radius; x < (f.width - this->radius); x++) {
                        Pixel center(x, y);
                        center.value = f.pixels[x*(y+1)].value;

                        std::vector<Pixel> circle = this->BresenhamCircle(center, this->radius);
                        std::cout << "\t-> Circle pixels:" << std::endl;
                        for (auto p = circle.begin(); p != circle.end(); p++)
                            std::cout << "[" << p->x << "," << p->y << "],";

                        return std::vector<Keypoint>();
                    }
                }

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
