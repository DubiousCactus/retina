/*
 * FAST.cpp
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#include "FAST.h"

#include <cmath>

namespace retina {
FASTExtractor::FASTExtractor(unsigned int margin,
                             unsigned short radius,
                             unsigned short intensity_threshold,
                             unsigned short contiguous_pixels,
                             unsigned short n_keypoints,
                             bool full_high_speed_test,
                             bool annotate,
                             bool non_max_suppression,
                             bool orientation,
                             int centroidRadius)
{
    this->margin = margin;
    this->orientation = orientation;
    this->centroid_radius = centroidRadius;
    this->full_high_speed_test = full_high_speed_test;
    this->intensity_threshold = intensity_threshold;
    this->contiguous_pixels = contiguous_pixels;
    this->n_keypoints = n_keypoints;
    this->radius = radius; // For a circle of 16 pixels
    this->annotate = annotate;
    this->non_max_suppression = non_max_suppression;
}

/*
 * Bresenham's circle drawing algorithm
 */
// TODO: Improve by removing the Pixel structure: use references to the frame pixels instead!
std::vector<Pixel>
FASTExtractor::BresenhamCircle(const Pixel center, const Frame& frame)
{
    std::vector<Pixel> circlePixels;
    circlePixels.reserve(this->radius * 8);
//    std::pair<int, int> to {this->radius, 0};
    Pixel to(this->radius, 0);
    int xChange = 1 - 2*this->radius, yChange = 1;
    int d = 0;
    int pointsPerOctant = 0;

    auto EightWaySymmetricPlot =
      [](const Pixel c, Pixel t, const Frame& f, std::vector<Pixel>& circle) {
          circle.emplace_back(f.PixelAt(c.x + t.x, c.y + t.y)); // Octant 1
          circle.emplace_back(f.PixelAt(c.x - t.x, c.y + t.y)); // Octant 4
          circle.emplace_back(f.PixelAt(c.x - t.x, c.y - t.y)); // Octant 5
          circle.emplace_back(f.PixelAt(c.x + t.x, c.y - t.y)); // Octant 8
          circle.emplace_back(f.PixelAt(c.x + t.y, c.y + t.x)); // Octant 2
          circle.emplace_back(f.PixelAt(c.x - t.y, c.y + t.x)); // Octant 3
          circle.emplace_back(f.PixelAt(c.x - t.y, c.y - t.x)); // Octant 6
          circle.emplace_back(f.PixelAt(c.x + t.y, c.y - t.x)); // Octant 7
      };

    while (to.x >= to.y) {
        EightWaySymmetricPlot(center, to, frame, circlePixels);
        pointsPerOctant++;
        to.y++;
        d += yChange;
        yChange += 2;
        if ((2 * d + xChange) > 0) {
            to.x--;
            d += xChange;
            xChange += 2;
        }
    }

    std::vector<Pixel> sortedCircle(pointsPerOctant * 8);
    for (int i = 0; i < pointsPerOctant; i++) {
        sortedCircle[i] = circlePixels[i * 8]; // Octant 1: offset=0
        sortedCircle[pointsPerOctant + i] = circlePixels[i * 8 + 4]; // Octant 2: offset=4
        sortedCircle[(2 * pointsPerOctant) + i] = circlePixels[i * 8 + 5]; // Octant 3: offset=5
        sortedCircle[(3 * pointsPerOctant) + i] = circlePixels[i * 8 + 1]; // Octant 4: offset=1
        sortedCircle[(4 * pointsPerOctant) + i] = circlePixels[i * 8 + 2]; // Octant 5: offset=2
        sortedCircle[(5 * pointsPerOctant) + i] = circlePixels[i * 8 + 6]; // Octant 6: offset=6
        sortedCircle[(6 * pointsPerOctant) + i] = circlePixels[i * 8 + 7]; // Octant 7: offset=7
        sortedCircle[(7 * pointsPerOctant) + i] = circlePixels[i * 8 + 3]; // Octant 8: offset=3
    }

    return sortedCircle;
}

    float
    FASTExtractor::PatchOrientation(const Frame& img,
                                    const int cx,
                                    const int cy)
{
    /* Compare with OpenCV's code to see if I made any mistake:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ICAngles(const Mat& img, const std::vector<Rect>& layerinfo,
                     std::vector<KeyPoint>& pts, const std::vector<int> & u_max, int half_k)
{
    int step = (int)img.step1();
    size_t ptidx, ptsize = pts.size();

    for( ptidx = 0; ptidx < ptsize; ptidx++ )
    {
        const Rect& layer = layerinfo[pts[ptidx].octave];
        const uchar* center = &img.at<uchar>(cvRound(pts[ptidx].pt.y) + layer.y, cvRound(pts[ptidx].pt.x) + layer.x);

        int m_01 = 0, m_10 = 0;

        // Treat the center line differently, v=0
        for (int u = -half_k; u <= half_k; ++u)
            m_10 += u * center[u];

        // Go line by line in the circular patch
        for (int v = 1; v <= half_k; ++v)
        {
            // Proceed over the two lines
            int v_sum = 0;
            int d = u_max[v];
            for (int u = -d; u <= d; ++u)
            {
                int val_plus = center[u + v*step], val_minus = center[u - v*step];
                v_sum += (val_plus - val_minus);
                m_10 += u * (val_plus + val_minus);
            }
            m_01 += v * v_sum;
        }

        pts[ptidx].angle = fastAtan2((float)m_01, (float)m_10);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    */
    float theta;
    unsigned int m01, m10;
    m01 = m10 = 0;
    for (int y = cy - radius; y <= cy + radius; ++y) {
        for (int x = cx - radius; x <= cx + radius; ++x) {
            if (sqrt(pow(x - cx, 2) + pow(y - cy, 2)) <= radius) {
                // m01 += pow(x, 0) * pow(y, 1) * *img(y, x);
                // m10 += pow(x, 1) * pow(y, 0) * *img(y, x);
                m01 += img(x, y) * y;
                m10 += img(x, y) * x;
            }
        }
    }
    theta = std::atan2(m01, m10);
    return theta;
}

/* Extract N keypoints in the given frame, using the Features from
 * Accelerated Segment Test algorithm with a given circular radius
 * (threshold)
 */
// TODO: Work on the image matrix directly
std::vector<KeyPoint>
FASTExtractor::Extract(const Frame& f)
{
    // TODO: Implement the paper "Machine Learning a Corner Detector"
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
    int Ip, upperBound, lowerBound, score;
    bool corner, brighter, darker;
    unsigned short brighterPixels = 4, darkerPixels = 4;
    this->keypoints.clear();
    this->keypoints.reserve(50000);
    this->annotated_frame = annotate ? std::make_shared<Frame>(Frame(f)) : nullptr;
    auto width = f.Width(), height = f.Height();
    unsigned int max_y = height - this->margin;
    unsigned int max_x = width - this->margin;
//    if (this->non_max_suppression) TODO: Optional
    Matrix<int> fastResponse(height, width);
    // Using row-major ordering should take advantage of the CPU cache
    for (unsigned int y = this->margin; y < max_y; y++) {
        for (unsigned int x = this->margin; x < max_x; x++) {
            Ip = f(x, y);
            Pixel center(x, y);
            center.intensity = Ip;

            /* High-speed non-corner elimination */
            upperBound = Ip + this->intensity_threshold;
            lowerBound = Ip - this->intensity_threshold;
            if (this->contiguous_pixels == 12) {
                std::vector<uint8_t> testPixels = {
                    f(x, y - 3), f(x, y + 3), f(x - 3, y), f(x + 3, y)
                };
                if (this->full_high_speed_test) {
                    bool skip = false;
                    for (auto & testPixel : testPixels) {
                        if (testPixel <= upperBound && testPixel >= lowerBound) {
                            // Cannot be a corner
                            skip = true;
                            break;
                        }
                    }
                    if (skip)
                        continue;
                } else {
                    /* At least 3 of those 4 pixels must be ALL brighter or ALL darker than p */
                    brighterPixels = darkerPixels = 4;
                    for (auto p = testPixels.begin(); p != testPixels.end() &&
                            (brighterPixels >=3 or darkerPixels >= 3); p++) {
                        if (*p <= upperBound) {
                            brighterPixels--;
                        }
                        if (*p >= lowerBound) {
                            darkerPixels--;
                        }
                    }

                    if (brighterPixels < 3 && darkerPixels < 3)
                        continue;
                }
            }

            /* Complete corner test */
            auto circle = this->BresenhamCircle(center, f);
            // TODO: Use a circular buffer or something in the std
            auto GetPixelWithOverflow = [](std::vector<Pixel>& v, unsigned int i) {
                assert(i / 2 < v.size());
                unsigned int circleSize = v.size();
                if (i >= circleSize)
                    return v.at(i - circleSize);
                else
                    return v.at(i);
            };
            corner = false;
            for (unsigned int i = 0; i < (int)circle.size(); i++) {
                std::vector<Pixel> contiguousPixels(this->contiguous_pixels, 0);
                // TODO: Unwrap by multiples of 8 (minimum size of a circle)
                for (unsigned int j = 0; j < this->contiguous_pixels; j++)
                    contiguousPixels[j] = GetPixelWithOverflow(circle, i + j);

                brighter = true, darker = true;
                for (auto & contiguousPixel : contiguousPixels) {
                    if (contiguousPixel.intensity <= upperBound) {
                        brighter = false;
                        break;
                    }
                }

                if (brighter) {
                    corner = true;
                    break;
                }

                for (auto & contiguousPixel : contiguousPixels) {
                    if (contiguousPixel.intensity >= lowerBound) {
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
                // TODO: Optimize by including this computation in the above
                // loop
                score = 0;
                for (unsigned int i = 0; i < (int)circle.size(); ++i) {
                    score += abs(Ip - circle.at(i).intensity);
                }
                if (!non_max_suppression) {
                    float patchOrientation = 0;
                    if (orientation) {
                        patchOrientation = this->PatchOrientation(imgMatrix, x, y);
                    }
                     // TODO: Write in a fixed size vector (large enough for the target amount) with indexing
                    this->keypoints.emplace_back(x, y, score, patchOrientation);
                } else {
                    fastResponse.At(y, x) = score;
                }
            } else if (non_max_suppression) {
                fastResponse.At(y, x) = 0;
            }
        }
    }

    if (non_max_suppression) {
        this->NonMaxSuppression(f, fastResponse);
    }

    std::cout << "\t-> Found " << this->keypoints.size() << " keypoints"
              << std::endl;

    return this->keypoints;
}

void
FASTExtractor::NonMaxSuppression(const Frame& image,
                                 Matrix<int>& fastResponse)
{
    std::vector<KeyPoint> suppressed;
    // TODO: Verify that this works as intended (no duplicates)
    // I feel like it removes way too much...
    for (int y = 1; y < fastResponse.Rows(); ++y) {
        for (int x = 1; x < fastResponse.Cols(); ++x) {
            if (fastResponse(y, x) == 0)
                continue;
            if (fastResponse(y - 1, x) != 0) {
                if (fastResponse(y, x) > fastResponse(y - 1, x)) {
                    fastResponse.At(y - 1, x) = 0;
                } else {
                    fastResponse.At(y, x) = 0;
                }
            }
            if (fastResponse(y, x - 1) != 0) {
                if (fastResponse(y, x) > fastResponse(y, x - 1)) {
                    fastResponse.At(y, x - 1) = 0;
                } else {
                    fastResponse.At(y, x) = 0;
                }
            }
        }
    }
    for (int y = 1; y < fastResponse.Rows(); ++y) {
        for (int x = 1; x < fastResponse.Cols(); ++x) {
            if (fastResponse(y, x) > 0) {
                float patchOrientation = 0;
                if (orientation) {
                    patchOrientation = this->PatchOrientation(image, x, y);
                }
                suppressed.emplace_back(x, y, fastResponse(y, x), patchOrientation);
            }
        }
    }
    this->keypoints = suppressed;
}

std::shared_ptr<Frame>
FASTExtractor::GetAnnotatedFrame()
{
    assert(this->annotated_frame);
    for (auto kp : keypoints) {
        this->annotated_frame->WriteAt(kp.x, kp.y, 0);
    }
    return this->annotated_frame;
}
}
