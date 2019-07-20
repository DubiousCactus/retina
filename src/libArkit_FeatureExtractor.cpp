/*
 * libArkit_FeatureExtractor.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include <vector>

namespace ARKIT
{

    struct FeatureDescriptor {
        int x;
        int y;
    };

    struct Frame {
        unsigned char *pixels;
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
            virtual void Extract(Frame f);
    };


    class ORBExtractor: FeatureExtractor
    {
        private:
            unsigned short intensity_threshold;
            unsigned short top_n_keypoints;
            unsigned short n_keypoints;
            unsigned short pog_levels;

            /* Extract N keypoints in the given frame, using the Features from
             * Accelerated Segment Test algorithm with a given circular radius
             * (threshold)
             */
            std::vector<Keypoint> FAST(Frame f, unsigned short threshold,
                    unsigned short n)
            {
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

                return std::vector<Keypoint>();
            }

            /* Order the FAST keypoints and return the N top points using the
             * Harris corner measure
             */
            std::vector<Keypoint> HarrisFilter(std::vector<Keypoint> keypoints,
                    unsigned short n)
            {
                return std::vector<Keypoint>();
            }

            /* Build a scale pyramid of the base image */
            ScalePyramid BuildPyramid(Frame f, unsigned short levels)
            {
                Frame frames[this->pog_levels];
                frames[0] = Frame();
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
                this->pog_levels = 6;
            }
            ~ORBExtractor();

            void Extract(Frame f)
            {
                /*
                 * 1. Build the PoG to produce multiscale-features
                 * 2. Find keypoints with FAST at each level of the pyramid
                 * 3. Apply Harris corner measure to find the top N points
                 */

                // STEP 1: Build the scale pyramid of the current frame
                ScalePyramid pyramid = this->BuildPyramid(f, this->pog_levels);
                // STEP 2: for each level of the PoG
                for (unsigned short i = 0; i < this->pog_levels; i++) {
                    std::vector<Keypoint> keypoints = this->HarrisFilter(
                            this->FAST(pyramid.scales[i], this->intensity_threshold,
                                this->n_keypoints),
                            this->top_n_keypoints); 
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
