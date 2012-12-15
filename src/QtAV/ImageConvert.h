/******************************************************************************
    ImageConvert.h: description
    Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#ifndef IMAGECONVERT_H
#define IMAGECONVERT_H

struct SwsContext;
namespace QtAV {

class ImageConvert
{
public:
    ImageConvert();
    ~ImageConvert();
    void setSourceSize(int width, int height);
    void setTargetSize(int width, int height);
    //use FFmpeg's enum
    void setSourceFormat(int format);
    void setTargetFormat(int format);
    bool scale(const void *const src, void *dst);
private:
    SwsContext *img_convert_ctx;
    int Wsrc, Hsrc, Wdst, Hdst;
    int src_fmt, dst_fmt;
};

} //namespace QtAV
#endif // IMAGECONVERT_H
