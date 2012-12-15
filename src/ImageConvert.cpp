/******************************************************************************
    ImageConvert.cpp: description
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


#include "ImageConvert.h"
#include <QtAV/QtAV_Compat.h>

namespace QtAV {

ImageConvert::ImageConvert()
    :img_convert_ctx(0),Wsrc(0), Hsrc(0)
    , Wdst(0), Hdst(0)
    ,src_fmt(PIX_FMT_YUV420P),dst_fmt(PIX_FMT_RGB32)
{
}

ImageConvert::~ImageConvert()
{
    if (img_convert_ctx)
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = 0;
    }
}

void ImageConvert::setSourceSize(int width, int height)
{
    Wsrc = width;
    Hsrc = height;
}

void ImageConvert::setTargetSize(int width, int height)
{
    Wdst = width;
    Hdst = height;
}

void ImageConvert::setSourceFormat(int format)
{
    src_fmt = format;
}

void ImageConvert::setTargetFormat(int format)
{
    dst_fmt = format;
}

bool ImageConvert::scale(const void *const src, void *dst)
{
    if (Wdst ==0|| Hsrc ==0|| Wdst ==0|| Hdst == 0)
        return false;

    img_convert_ctx = sws_getCachedContext(img_convert_ctx
            , Wsrc, Hsrc, (PixelFormat)src_fmt
            , Wdst, Hdst, (PixelFormat)dst_fmt
            , (Wsrc == Wdst && Hsrc == Hdst) ? SWS_POINT : SWS_FAST_BILINEAR
            , NULL, NULL, NULL
           );

    if (!img_convert_ctx)
        return false;

    struct
    {
        uint8_t *data[4];
        int linesize[4];
    } pictSrc, pictDst;

    if ((PixelFormat)src_fmt == PIX_FMT_YUV420P) {
        pictSrc.data[0] = (uint8_t*)src;
        pictSrc.data[2] = (uint8_t*)pictSrc.data[0] + (Wsrc * Hsrc);
        pictSrc.data[1] = (uint8_t*)pictSrc.data[2] + (Wsrc * Hsrc) / 4;
        //pictSrc.data[3] = (uint8_t*)pictSrc.data[0] - 1;
        pictSrc.linesize[0] = Wsrc;
        pictSrc.linesize[1] = Wsrc / 2;
        pictSrc.linesize[2] = Wsrc / 2;
        //pictSrc.linesize[3] = 0; //not used
    } else {
        pictSrc.data[0] = (uint8_t*)src;
        //pictSrc.data[2] = (uint8_t*)pictSrc.data[0] - 1;
        //pictSrc.data[1] = (uint8_t*)pictSrc.data[2] - 1;
        //pictSrc.data[3] = (uint8_t*)pictSrc.data[0] - 1;
        pictSrc.linesize[0] = Wsrc * 4; //TODO: not 0
        //pictSrc.linesize[1] = 0;
        //pictSrc.linesize[2] = 0;
        //pictSrc.linesize[3] = 0; //not used
    }
    if ((PixelFormat)dst_fmt == PIX_FMT_YUV420P) {
        pictDst.data[0] = (uint8_t*)dst;
        pictDst.data[2] = (uint8_t*)pictDst.data[0] + (Wdst * Hsrc);
        pictDst.data[1] = (uint8_t*)pictDst.data[2] + (Wdst * Hsrc) / 4;
        //pictDst.data[3] = (uint8_t*)pictDst.data[0] - 1;
        pictDst.linesize[0] = Wdst;
        pictDst.linesize[1] = Wdst / 2;
        pictDst.linesize[2] = Wdst / 2;
        //pictDst.linesize[3] = 0; //not used
    } else {
        pictDst.data[0] = (uint8_t*)dst;
        pictDst.linesize[0] = Wdst * 4;
    }




    sws_scale(img_convert_ctx, pictSrc.data, pictSrc.linesize, 0, Hsrc, pictDst.data, pictDst.linesize);
    return true;
}

} //namespace QtAV
