/******************************************************************************
    ImageConverterIPP: Image resizing & color model convertion using Intel IPP
    Copyright (C) 2012-2013 Wang Bin <wbsecg1@gmail.com>

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

#include <QtAV/ImageConverterIPP.h>
#include <private/ImageConverter_p.h>
#include <QtAV/QtAV_Compat.h>

#ifdef IPP_LINK
#include <ipp.h>
#else

#endif

namespace QtAV {

class ImageConverterIPPPrivate : public ImageConverterPrivate
{
public:
    ImageConverterIPPPrivate():need_scale(true) {}
    bool need_scale;
    QByteArray orig_ori_rgb;
};

ImageConverterIPP::ImageConverterIPP()
    :ImageConverter(*new ImageConverterIPPPrivate())
{
}

bool ImageConverterIPP::convert(const quint8 *const srcSlice[], const int srcStride[])
{
    DPTR_D(ImageConverterIPP);
    //color convertion, no scale
#ifdef IPP_LINK
    ippiYUV420ToRGB_8u_P3AC4R(const_cast<const quint8 **>(srcSlice), const_cast<int*>(srcStride), (Ipp8u*)(d.orig_ori_rgb.data())
                           , 4*sizeof(quint8)*d.w_in, (IppiSize){d.w_in, d.h_in});
    d.data_out = d.orig_ori_rgb;
    return true;
    if (d.need_scale) {
        qDebug("rs");
        ippiResize_8u_AC4R((const Ipp8u*)d.orig_ori_rgb.data(), (IppiSize){d.w_in, d.h_in}, 4*sizeof(quint8)*d.w_in, (IppiRect){0, 0, d.w_in, d.h_in}
                  , (Ipp8u*)d.data_out.data(), 4*sizeof(quint8)*d.w_in, (IppiSize){d.w_out, d.h_out}
                  , (double)d.w_out/(double)d.w_in, (double)d.h_out/(double)d.h_in, IPPI_INTER_CUBIC);
    } else {
        d.data_out = d.orig_ori_rgb;
    }
#endif
    return true;
}

//TODO: call it when out format is setted. and avoid too much calls
bool ImageConverterIPP::prepareData()
{
    DPTR_D(ImageConverterIPP);
    //for color convertion
    if (d.w_in > 0 && d.h_in > 0) {
        qDebug("in size=%d x %d", d.w_in, d.h_in);
        int bytes = avpicture_get_size((PixelFormat)d.fmt_out, d.w_in, d.h_in);
        //if(d.orig_ori_rgb.size() < bytes) {
            d.orig_ori_rgb.resize(bytes);
        //}
    }
    //for resize
    d.need_scale = d.w_in != d.w_out || d.h_in != d.h_out;
    if (d.w_out > 0 && d.h_out > 0) {
        qDebug("out size=%d x %d", d.w_out, d.h_out);
        if (d.need_scale) {
            int bytes = avpicture_get_size((PixelFormat)d.fmt_out, d.w_out, d.h_out);
            //if(d.data_out.size() < bytes) {
                d.data_out.resize(bytes);
            //}
        }
    }
    return true;
}

} //namespace QtAV
