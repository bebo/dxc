#ifndef FRAME_H_
#define FRAME_H_

enum PIXEL_FORMAT {
    ARGB32 = 0
};

class Frame {
  protected:
    UINT width_;
    UINT height_;
    PIXEL_FORMAT pixel_format_;

  public:
    UINT GetWidth() { return width_; }
    UINT GetHeight() { return height_; }
};

#endif
