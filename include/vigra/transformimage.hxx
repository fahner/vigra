/************************************************************************/
/*                                                                      */
/*               Copyright 1998-2000 by Ullrich Koethe                  */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the VIGRA computer vision library.           */
/*    You may use, modify, and distribute this software according       */
/*    to the terms stated in the LICENSE file included in               */
/*    the VIGRA distribution.                                           */
/*                                                                      */
/*    The VIGRA Website is                                              */
/*        http://kogs-www.informatik.uni-hamburg.de/~koethe/vigra/      */
/*    Please direct questions, bug reports, and contributions to        */
/*        koethe@informatik.uni-hamburg.de                              */
/*                                                                      */
/*  THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR          */
/*  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. */
/*                                                                      */
/************************************************************************/
 
 
#ifndef VIGRA_TRANSFORMIMAGE_HXX
#define VIGRA_TRANSFORMIMAGE_HXX

#include "vigra/utilities.hxx"
#include "vigra/numerictraits.hxx"

namespace vigra {

/** @heading Algorithms to Transform Images
    @memo apply functor to calculate a pixelwise transformation of one image
*/
//@{

/********************************************************/
/*                                                      */
/*                      transformImage                  */
/*                                                      */
/********************************************************/

/** Transform source image into destination image.
    The transformation given by the functor is applied to every source 
    pixel and the result written into the corresponding destination pixel.
    The function uses accessors to access the pixel data.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor, class Functor>
        void
        transformImage(SrcImageIterator src_upperleft, 
               SrcImageIterator src_lowerright, SrcAccessor sa,
               DestImageIterator dest_upperleft, DestAccessor da,
               Functor f)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor, class Functor>
        void
        transformImage(triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
               pair<DestImageIterator, DestAccessor> dest,
               Functor f)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
        Include-File:
        \URL[vigra/transformimage.hxx]{../include/vigra/transformimage.hxx}\\
        Namespace: vigra
    
    \begin{verbatim}
    
    #include <math.h>         // for sqrt()
    
    vigra::transformImage(srcImageRange(src), 
                          destImage(dest), 
                          &::sqrt );
    
    \end{verbatim}

    {\bf Required Interface:}
    
    \begin{verbatim}
    SrcImageIterator src_upperleft, src_lowerright;
    DestImageIterator      dest_upperleft;
    
    SrcAccessor src_accessor;
    DestAccessor dest_accessor;
    
    Functor functor;

    dest_accessor.set(functor(src_accessor(src_upperleft)), dest_upperleft);

    \end{verbatim}
    
    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor, class Functor>
void
transformImage(SrcImageIterator src_upperleft, 
               SrcImageIterator src_lowerright, SrcAccessor sa,
               DestImageIterator dest_upperleft, DestAccessor da,
           Functor f)
{
    int w = src_lowerright.x - src_upperleft.x;
    int h = src_lowerright.y - src_upperleft.y;
    
    for(int y=0; y<h; ++y, ++src_upperleft.y, ++dest_upperleft.y)
    {
        SrcImageIterator six(src_upperleft);
        DestImageIterator dix(dest_upperleft);

        for(int x=0; x<w; ++x, ++six.x, ++dix.x)
        {
            da.set(static_cast<typename 
                 DestAccessor::value_type>(f(sa(six))), dix);
        }
    }
}
    
template <class SrcImageIterator, class SrcAccessor,
      class DestImageIterator, class DestAccessor, class Functor>
inline
void
transformImage(triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
           pair<DestImageIterator, DestAccessor> dest,
           Functor f)
{
    transformImage(src.first, src.second, src.third, 
                   dest.first, dest.second, f);
}

/********************************************************/
/*                                                      */
/*                   transformImageIf                   */
/*                                                      */
/********************************************************/

/** Transform source ROI into destination ROI.
    The transformation given by the functor is applied to every source 
    pixel in the ROI (i.e. when the return vlaue of the mask's accessor
    is not zero)
    and the result is written into the corresponding destination pixel.
    The function uses accessors to access the pixel data.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class MaskImageIterator, class MaskAccessor,
              class DestImageIterator, clas DestAccessor,
              class Functor>
        void
        transformImageIf(SrcImageIterator src_upperleft, 
            SrcImageIterator src_lowerright, SrcAccessor sa,
            MaskImageIterator mask_upperleft, MaskAccessor ma,
            DestImageIterator dest_upperleft, DestAccessor da,
            Functor f)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class MaskImageIterator, class MaskAccessor,
              class DestImageIterator, clas DestAccessor,
              class Functor>
        void
        transformImageIf(triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
                 pair<MaskImageIterator, MaskAccessor> mask,
                 pair<DestImageIterator, DestAccessor> dest,
                 Functor f)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
        Include-File:
        \URL[vigra/transformimage.hxx]{../include/vigra/transformimage.hxx}\\
        Namespace: vigra
    
    \begin{verbatim}
    #include <math.h>         // for sqrt()
    
    vigra::transformImageIf(srcImageRange(src), 
                            maskImage(mask), 
                            destImage(dest), 
                            &::sqrt );

    \end{verbatim}

    {\bf Required Interface:}
    
    \begin{verbatim}
    SrcImageIterator src_upperleft, src_lowerright;
    DestImageIterator  dest_upperleft;
    MaskImageIterator mask_upperleft;
    
    SrcAccessor src_accessor;
    DestAccessor dest_accessor;
    MaskAccessor mask_accessor;
    Functor functor;
    
    if(mask_accessor(mask_uppeleft))
       dest_accessor.set(functor(src_accessor(src_upperleft)), dest_upperleft);

    \end{verbatim}
    
    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class MaskImageIterator, class MaskAccessor,
          class DestImageIterator, class DestAccessor,
      class Functor>
void
transformImageIf(SrcImageIterator src_upperleft, 
            SrcImageIterator src_lowerright, SrcAccessor sa,
            MaskImageIterator mask_upperleft, MaskAccessor ma,
        DestImageIterator dest_upperleft, DestAccessor da,
        Functor f)
{
    int w = src_lowerright.x - src_upperleft.x;
    int h = src_lowerright.y - src_upperleft.y;
    
    for(int y=0; y<h; ++y, 
             ++src_upperleft.y, ++mask_upperleft.y, ++dest_upperleft.y)
    {
        SrcImageIterator six(src_upperleft);
        MaskImageIterator mx(mask_upperleft);
        DestImageIterator       dix(dest_upperleft);

        for(int x=0; x<w; ++x, ++six.x, ++mx.x, ++dix.x)
        {
            if(ma(mx)) da.set(static_cast<typename 
                DestAccessor::value_type>(f(sa(six))), dix);
        }
    }
}

template <class SrcImageIterator, class SrcAccessor,
          class MaskImageIterator, class MaskAccessor,
          class DestImageIterator, class DestAccessor,
      class Functor>
inline
void
transformImageIf(triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
             pair<MaskImageIterator, MaskAccessor> mask,
             pair<DestImageIterator, DestAccessor> dest,
             Functor f)
{
    transformImageIf(src.first, src.second, src.third, 
                     mask.first, mask.second, 
             dest.first, dest.second, f);
}

//@}

/** @heading Functors to Transform Images

    Note that the unary functors of the STL can also be used.
    
    @memo frequently used pixel transformations
*/
//@{

template <class SrcValueType>
class LinearIntensityTransform
{
   public:
        /* a real type convertible from SrcValueType
            @memo
        */
    typedef typename 
            NumericTraits<SrcValueType>::RealPromote SrcPromote;
    
        /* init scale and offset
            @memo
        */
    LinearIntensityTransform(double scale, SrcPromote offset)
    : scale_(scale), offset_(offset)
    {}
    
        /* calculate transform
            @memo
        */
    SrcPromote operator()(SrcValueType s) const
    {
        return (scale_ * (s + offset_));
    }
    
  private:
  
    double scale_;
    SrcPromote offset_;
};

/********************************************************/
/*                                                      */
/*              linearIntensityTransform                */
/*                                                      */
/********************************************************/

/** Factory function for a functor that linearly transforms the 
    source pixel values. The functor applies the transform 
    '#destvalue = scale * (srcvalue + offset)#' to every pixel.
    This can, for example, be used to transform images into the visible 
    range 0...255 or to invert an image.
    
    {\bf Declaration:}
    
    \begin{verbatim}
    namespace vigra {
        template <class SrcValueType>
        LinearIntensityTransform<SrcValueType>
        linearIntensityTransform(double scale, SrcValueType offset)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
        Include-File:
        \URL[vigra/transformimage.hxx]{../include/vigra/transformimage.hxx}\\
        Namespace: vigra
    
    \begin{verbatim}
    vigra::IImage src(width, height);
    vigra::BImage dest(width, height);
    ...
    vigra::FindMinMax<IImage::PixelType> minmax;   // functor to find range
    
    vigra::inspectImage(srcImageRange(src), minmax); // find original range
    
    // transform to range 0...255
    vigra::transformImage(srcImageRange(src), destImage(dest),
                          linearIntensityTransform(
                            255.0 / (minmax.max - minmax.min), // scaling
                          - minmax.min));                    // offset
     
    \end{verbatim}

    {\bf Required Interface:}
    
    The source value type must be a model of \Ref{LinearSpace}.
    
    @memo Apply a linear transform to the source pixel values 
    
*/
template <class SrcValueType>
LinearIntensityTransform<SrcValueType>
linearIntensityTransform(double scale, SrcValueType offset)
{
    return LinearIntensityTransform<SrcValueType>(scale, offset);
}


/********************************************************/
/*                                                      */
/*                      Threshold                       */
/*                                                      */
/********************************************************/

/** Threshold an image.
    If a source pixel is above or equal the lower and below 
    or equal the higher threshold (i.e. within the closed interval
    [lower, heigher]) the destination pixel is set to 'yesresult',
    otherwise to 'noresult'.

    {\bf Usage:}
    
        Include-File:
        \URL[vigra/transformimage.hxx]{../include/vigra/transformimage.hxx}\\
        Namespace: vigra
    
    \begin{verbatim}
    vigra::BImage src(width, height), dest(width, height);
    ...
    vigra::transformImage(src.upperLeft(), src.lowerRight(), src.accessor(),
       dest.upperLeft(), dest.accessor(),
       vigra::Threshold<
          vigra::BImage::PixelType, vigra::BImage::PixelType>(10, 100, 0, 255));
    
    \end{verbatim}

    {\bf Required Interface:}
    
    \begin{verbatim}
    
    SrcValueType   src;
    DestValueType  dest, yesresult, noresult;
    
    dest = ((src < lower) || (higher < src)) ? noresult : yesresult;
    
    \end{verbatim}
    
*/
template <class SrcValueType, class DestValueType>
class Threshold
{
   public:
    /** init thresholds and resturn values
        @memo
    */
    Threshold(SrcValueType lower, SrcValueType higher,
              DestValueType noresult, DestValueType yesresult)
    : lower_(lower), higher_(higher),
      yesresult_(yesresult), noresult_(noresult)
    {}
    
    /** calculate transform
        @memo
    */
    DestValueType operator()(SrcValueType s) const
    {
        return ((s < lower_) || (higher_ < s)) ? noresult_ : yesresult_;
    }
    
  private:
  
    SrcValueType lower_, higher_;
    DestValueType yesresult_, noresult_;
};

/********************************************************/
/*                                                      */
/*                BrightnessContrastFunctor             */
/*                                                      */
/********************************************************/

/** Adjust brightness and contrast of an image.
    This functor applies a gamma correction to each pixel in order to 
    modify the brightness of the image. To the result of the gamma correction,
    a similar transform is applied that modifies the contrast. The brightness and 
    contrast parameters must be positive. Values greater than 1 will increase image 
    brightness and contrast, values smaller than 1 decrease them. A value = 1 will
    have no effect.
    For \ref{RGBValue}'s, the transforms are applied component-wise. The pixel
    values are assumed to lie between the given minimum and maximum
    values. In case of RGB values, this is again understood component-wise. In case 
    of #unsigned char#, min and max default to 0 and 255 respectively.
    Precisely, the following transform is applied to each {\em PixelValue}:
    
    \[
    \begin{array}{rcl}
    V_1 & = & \frac{PixelValue - min}{max - min} \\ \\
    V_2 & = & V_1^\frac{1}{brightness} \\ \\
    V_3 & = & 2 V_2 - 1 \\ \\
    V_4 & = & \left\lbrace
        \begin{array}{l}
         V_3^\frac{1}{contrast} \mbox{\rm \quad if  } V_3 \ge 0 \\ \\
         - (-V_3)^\frac{1}{contrast} \mbox{\rm \quad otherwise}
        \end{array} \right. \\ \\
    Result & = & \frac{V_4 + 1}{2} (max - min) + min
    \end{array}
    \]
    
    If the #PixelType# is #unsigned char#, a look-up-table is used 
    for faster computation.

    {\bf Usage:}
    
        Include-File:
        \URL[vigra/transformimage.hxx]{../include/vigra/transformimage.hxx}\\
        Namespace: vigra
    
    \begin{verbatim}
    vigra::BImage bimage(width, height);
    double brightness, contrast;
    ...
    vigra::transformImage(srcImageRange(bimage), destImage(bimage),
       vigra::BrightnessContrastFunctor<unsigned char>(brightness, contrast));
    


    vigra::FImage fimage(width, height);
    ...
    
    vigra::FindMinmax<float> minmax;
    vigra::inspectImage(srcImageRange(fimage), minmax);
    
    vigra::transformImage(srcImageRange(fimage), destImage(fimage),
       vigra::BrightnessContrastFunctor<float>(brightness, contrast, minmax.min, minmax.max));
    
    
    \end{verbatim}

    {\bf Required Interface:}
    
    Scalar types: must be a linear algebra (+, - *, NumericTraits), 
    strict weakly ordered (<), and #pow()# must be defined.
    
    RGB values: the component type must meet the above requirements.
*/
template <class PixelType>
class BrightnessContrastFunctor
{
    typedef typename 
        NumericTraits<PixelType>::RealPromote tmp_type;
    double b_, c_;
    PixelType min_;
    tmp_type diff_;
 
 public:
    
    typedef PixelType value_type;
    
    BrightnessContrastFunctor(double brightness, double contrast,
                              PixelType const & min, PixelType const & max)
    : b_(1.0/brightness), c_(1.0/contrast), min_(min), diff_(max - min)
    {}
    
    value_type operator()(value_type const & v) const
    {
        tmp_type v1 = (v - min_) / diff_;
        tmp_type brighter = pow(v1, b_);
        tmp_type v2 = 2.0 * brighter - NumericTraits<tmp_type>::one();
        tmp_type contrasted = (v2 < NumericTraits<tmp_type>::zero()) ?
                                -pow(-v2, c_) :
                                pow(v2, c_);
        return value_type(0.5 * diff_ * 
             (contrasted + NumericTraits<tmp_type>::one()) + min_);
    }
};

template <>
class BrightnessContrastFunctor<unsigned char>
{
    unsigned char lut[256];

 public:
    
    typedef unsigned char value_type;
    
    BrightnessContrastFunctor(double brightness, double contrast,
                              value_type const & min = 0, value_type const & max = 255)
    {
        BrightnessContrastFunctor<double> f(brightness, contrast, min, max);
        
        for(int i = min; i <= max; ++i)
        {
            lut[i] = static_cast<unsigned char>(f(i)+0.5);
        }
    }
    
    value_type operator()(value_type const & v) const
    {
        
        return lut[v];
    }
};

#ifndef NO_PARTIAL_TEMPLATE_SPECIALIZATION

template <class ComponentType>
class BrightnessContrastFunctor<RGBValue<ComponentType> >
{
    BrightnessContrastFunctor<ComponentType> red, green, blue;

 public:
    
    typedef RGBValue<ComponentType> value_type;
    
    BrightnessContrastFunctor(double brightness, double contrast,
                              value_type const & min, value_type const & max)
    : red(brightness, contrast, min.red(), max.red()),
      green(brightness, contrast, min.green(), max.green()),
      blue(brightness, contrast, min.blue(), max.blue())
    {}
    
    value_type operator()(value_type const & v) const
    {
        
        return value_type(red(v.red()), green(v.green()), blue(v.blue()));
    }
};

#else // NO_PARTIAL_TEMPLATE_SPECIALIZATION

template <>
class BrightnessContrastFunctor<RGBValue<int> >
{
    BrightnessContrastFunctor<int> red, green, blue;

 public:
    
    typedef RGBValue<int> value_type;
    
    BrightnessContrastFunctor(double brightness, double contrast,
                              value_type const & min, value_type const & max)
    : red(brightness, contrast, min.red(), max.red()),
      green(brightness, contrast, min.green(), max.green()),
      blue(brightness, contrast, min.blue(), max.blue())
    {}
    
    value_type operator()(value_type const & v) const
    {
        
        return value_type(red(v.red()), green(v.green()), blue(v.blue()));
    }
};

template <>
class BrightnessContrastFunctor<RGBValue<float> >
{
    BrightnessContrastFunctor<float> red, green, blue;

 public:
    
    typedef RGBValue<float> value_type;
    
    BrightnessContrastFunctor(double brightness, double contrast,
                              value_type const & min, value_type const & max)
    : red(brightness, contrast, min.red(), max.red()),
      green(brightness, contrast, min.green(), max.green()),
      blue(brightness, contrast, min.blue(), max.blue())
    {}
    
    value_type operator()(value_type const & v) const
    {
        
        return value_type(red(v.red()), green(v.green()), blue(v.blue()));
    }
};

#endif // NO_PARTIAL_TEMPLATE_SPECIALIZATION

template <>
class BrightnessContrastFunctor<RGBValue<unsigned char> >
{
    BrightnessContrastFunctor<unsigned char> red, green, blue;
    
 public:
    
    typedef RGBValue<unsigned char> value_type;
    
    BrightnessContrastFunctor(double brightness, double contrast,
       value_type const & min = value_type(0,0,0), 
       value_type const & max = value_type(255, 255, 255))
    : red(brightness, contrast, min.red(), max.red()),
      green(brightness, contrast, min.green(), max.green()),
      blue(brightness, contrast, min.blue(), max.blue())
    {}
    
    value_type operator()(value_type const & v) const
    {
        
        return value_type(red(v.red()), green(v.green()), blue(v.blue()));
    }
};

//@}

} // namespace vigra

#endif // VIGRA_TRANSFORMIMAGE_HXX
