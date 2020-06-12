# Minimal dependency Computer Vision & Image Processing library for learning purposes

**Retina is still a work in progress! (and it might never be finished)**

Retina is a zero-dependency (almost) computer vision library built from scratch
in C++17 (or trying to be). The only required dependency is ffmpeg (libavutil,
libavformat, libavcodec, libswscale) for the video decoding bit.
It was written to be easily readable (yes I'm looking at you OpenCV) and
understandable, while trying to achieve decent performance.
Technically, it could be built for embedded platforms as well if you don't need
video decoding.

You won't ever be using this if you are mentally stable, but if you're looking
for simple code to understand basic CV algorithms, this is the right place.

## Usage
Build it with cmake. Use it as in the example project in `example/`.

## Literature and documentation

The following papers and articles were used to understand all the computer
vision methods and algorithms used in the building of this project:

### Papers

- ORB: an efficient alternative to SIFT or SURF
- Fusing Points and Lines for High Performance Tracking
- Machine learning for high-speed corner detection
- BRIEF: Binary Robust IndependentElementary Features
- Measuring Corner Properties
- A COMBINED CORNER AND EDGE DETECTOR


### Articles & websites

- https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_orb/py_orb.html
- https://github.com/opencv/opencv


## TODO

### Feature extraction / description library 

- [x] Harris
- [x] FAST
    - [x] oFAST
- [x] BRIEF
    - [x] steered BRIEF
    - [x] rBRIEF
- [ ] ORB
    - [ ] Scale pyramid
    - [ ] Learning Good Binary Features

### Feature matching
- [ ] Brute-force matcher
- [ ] FLANN Matcher

### Image processing
- [ ] Image rescaling
    - [x] Downscaling
    - [ ] Upscaling
- [ ] Simple convolution filters
    - [x] Gaussian filter
    - [ ] Sobel operator
- [ ] Color conversion

### Performance improvements

- [ ] Multi-threading
- [ ] Algorithmic optimizations (Matrix class mainly)
- [ ] Harris algorithm (oh god it's terribly slow and dumb)

