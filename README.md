# toyGround
Graphics toy collections for libreliu

## Current implemented
- `noise`: a library for generating various noises
  - Uniform Noise
  - Perlin Noise
  - Fractual Noise
  - Worley Noise
  - Composite of above
- `editor`: a wxWidgets based GUI editor, for playing with the above

## Install
Build and run under Ubuntu 18.04:
```
sudo apt install build-essential libwxgtk3.0-dev
mkdir build && cd build && cmake ..
make
./editor/editor
```

Build with MSVC 2019:
> vcpkg's wxWidgets support is currently broken.
> Use native packages instead.
TBC