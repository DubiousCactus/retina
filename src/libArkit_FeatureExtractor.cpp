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

    struct Frame {
        unsigned char *pixels;
        unsigned int width;
        unsigned int height;
    };

    struct Keypoint {

    };

    struct ScalePyramid {
        Frame *scales;
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
                        unsigned char p = f.pixels[x*(y+1)];

                        // TODO: Generate circle of radius r around P
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
                Frame frames[this->pog_levels];
                for (unsigned short i = 0; i < this->pog_levels; i++) {
                    frames[i] = f; // TODO
                }
                ScalePyramid pyramid;
                pyramid.scales = frames;

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
                this->radius = 3;
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
