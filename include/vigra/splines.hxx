/************************************************************************/
/*                                                                      */
/*               Copyright 1998-2004 by Ullrich Koethe                  */
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

#ifndef VIGRA_SPLINES_HXX
#define VIGRA_SPLINES_HXX

#include <cmath>
#include "vigra/config.hxx"
#include "vigra/mathutil.hxx"
#include "vigra/polynomial.hxx"
#include "vigra/array_vector.hxx"

namespace vigra {

/** \addtogroup MathFunctions Mathematical Functions
*/
//@{
/*! 

    <b>\#include</b> "<a href="splines_8hxx-source.html">vigra/splines.hxx</a>"<br>
    Namespace: vigra
*/
#ifndef NO_PARTIAL_TEMPLATE_SPECIALIZATION

template <int ORDER, class T = double>
class BSplineBase
{
  public:
  
    typedef T            value_type;  
    typedef T            argument_type;  
    typedef T            first_argument_type;  
    typedef unsigned int second_argument_type;  
    typedef T            result_type; 
    enum               { order = ORDER };

    BSplineBase(unsigned int derivativeOrder = 0)
    : s1_(derivativeOrder)
    {}
    
    result_type operator()(argument_type x) const
    {
        return exec(x, derivativeOrder());
    }

    result_type operator()(first_argument_type x, second_argument_type derivative_order) const
    {
         return exec(x, derivativeOrder() + derivative_order);
    }

    value_type operator[](value_type x) const
        { return operator()(x); }
    
    double radius() const
        { return (ORDER + 1) * 0.5; }
        
    unsigned int derivativeOrder() const
        { return s1_.derivativeOrder(); }

    ArrayVector<double> const & prefilterCoefficients() const
    { 
        static ArrayVector<double> const & b = calculatePrefilterCoefficients();
        return b;
    }
    
    static ArrayVector<double> const & calculatePrefilterCoefficients();
    
    typedef T WeightMatrix[ORDER+1][ORDER+1];
    static WeightMatrix & weights()
    {
        static WeightMatrix & b = calculateWeightMatrix();
        return b;
    }
    
    static WeightMatrix & calculateWeightMatrix();
    
  protected:
    result_type exec(first_argument_type x, second_argument_type derivative_order) const;
    
    BSplineBase<ORDER-1, T> s1_;  
};

template <int ORDER, class T>
typename BSplineBase<ORDER, T>::result_type 
BSplineBase<ORDER, T>::exec(first_argument_type x, second_argument_type derivative_order) const
{
    if(derivative_order == 0)
    {
        T n12 = (ORDER + 1.0) / 2.0;
        return ((n12 + x) * s1_(x + 0.5) + (n12 - x) * s1_(x - 0.5)) / ORDER;
    }
    else
    {
        --derivative_order;
        return s1_(x + 0.5, derivative_order) - s1_(x - 0.5, derivative_order);
    }
}

template <int ORDER, class T>
ArrayVector<double> const & BSplineBase<ORDER, T>::calculatePrefilterCoefficients()
{ 
    static ArrayVector<double> b(ORDER / 2 == 0 ? 1 : ORDER / 2, 0.0);
    if(ORDER > 1)
    {
        static const int r = ORDER / 2;
        StaticPolynomial<2*r, double> p(2*r);
        BSplineBase spline;
        for(int i = 0; i <= 2*r; ++i)
            p[i] = spline(T(i-r));
        ArrayVector<double> roots;
        polynomialRealRoots(p, roots);
        int k = 0;
        for(unsigned int i = 0; i < roots.size(); ++i)
            if(std::abs(roots[i]) < 1.0)
                b[k++] = roots[i];
    }
    return b;
}
    
template <int ORDER, class T>
typename BSplineBase<ORDER, T>::WeightMatrix & 
BSplineBase<ORDER, T>::calculateWeightMatrix()
{
    static WeightMatrix b;
    double faculty = 1.0;
    for(int d = 0; d <= ORDER; ++d)
    {
        if(d > 1)
            faculty *= d;
        double x = ORDER / 2;
        BSplineBase spline;
        for(int i = 0; i <= ORDER; ++i, --x)
            b[d][i] = spline(x, d) / faculty;
    }
    return b;
}

template <int ORDER, class T = double>
class BSpline
: public BSplineBase<ORDER, T>
{
  public:
    BSpline(unsigned int derivativeOrder = 0)
    : BSplineBase<ORDER, T>(derivativeOrder)
    {}
};

/********************************************************/
/*                                                      */
/*                     BSpline<0, T>                    */
/*                                                      */
/********************************************************/

template <class T>
class BSplineBase<0, T>
{
  public:
  
    typedef T            value_type;  
    typedef T            argument_type;  
    typedef T            first_argument_type;  
    typedef unsigned int second_argument_type;  
    typedef T            result_type; 
    enum               { order = 0 };

    BSplineBase(unsigned int derivativeOrder = 0)
    : derivativeOrder_(derivativeOrder)
    {}
    
    result_type operator()(argument_type x) const
    {
         return exec(x, derivativeOrder_);
    }

    result_type operator()(first_argument_type x, second_argument_type derivative_order) const
    {        
         return exec(x, derivativeOrder_ + derivative_order);
    }
    
    value_type operator[](value_type x) const
        { return operator()(x); }
    
    double radius() const
        { return 0.5; }
        
    unsigned int derivativeOrder() const
        { return derivativeOrder_; }

    ArrayVector<double> const & prefilterCoefficients() const
    { 
        static ArrayVector<double> b(1, 0.0);
        return b;
    }
    
    typedef T WeightMatrix[1][1];
    static WeightMatrix & weights()
    {
        static T b[1][1] = {{ 1.0}};
        return b;
    }
    
  protected:
    result_type exec(first_argument_type x, second_argument_type derivative_order) const
    {
        if(derivative_order == 0)
            return x < 0.5 && -0.5 <= x ?
                     1.0
                   : 0.0;
        else
            return 0.0;
    }
    
    unsigned int derivativeOrder_;
};

/********************************************************/
/*                                                      */
/*                     BSpline<1, T>                    */
/*                                                      */
/********************************************************/

template <class T>
class BSpline<1, T>
{
  public:
  
    typedef T            value_type;  
    typedef T            argument_type;  
    typedef T            first_argument_type;  
    typedef unsigned int second_argument_type;  
    typedef T            result_type; 
    enum               { order = 1 };

    BSpline(unsigned int derivativeOrder = 0)
    : derivativeOrder_(derivativeOrder)
    {}
    
    result_type operator()(argument_type x) const
    {
        return exec(x, derivativeOrder_);
    }

    result_type operator()(first_argument_type x, second_argument_type derivative_order) const
    {
         return exec(x, derivativeOrder_ + derivative_order);
    }
    
    value_type operator[](value_type x) const
        { return operator()(x); }
    
    double radius() const
        { return 1.0; }
        
    unsigned int derivativeOrder() const
        { return derivativeOrder_; }

    ArrayVector<double> const & prefilterCoefficients() const
    { 
        static ArrayVector<double> b(1, 0.0);
        return b;
    }
    
    typedef T WeightMatrix[2][2];
    static WeightMatrix & weights()
    {
        static T b[2][2] = {{ 1.0, 0.0}, {-1.0, 1.0}};
        return b;
    }

  protected:
    T exec(T x, unsigned int derivative_order) const;
    
    unsigned int derivativeOrder_;
};

template <class T>
T BSpline<1, T>::exec(T x, unsigned int derivative_order) const
{
    switch(derivative_order)
    {
        case 0:
        {
            x = VIGRA_CSTD::fabs(x);
            return x < 1.0 ? 
                    1.0 - x
                    : 0.0;
        }
        case 1:
        {
            return x < 0.0 ?
                     -1.0 <= x ?
                          1.0 
                     : 0.0 
                   : x < 1.0 ?
                       -1.0 
                     : 0.0;
        }
        default:
            return 0.0;
    }
}

/********************************************************/
/*                                                      */
/*                     BSpline<2, T>                    */
/*                                                      */
/********************************************************/

template <class T>
class BSpline<2, T>
{
  public:
  
    typedef T            value_type;  
    typedef T            argument_type;  
    typedef T            first_argument_type;  
    typedef unsigned int second_argument_type;  
    typedef T            result_type; 
    enum               { order = 2 };

    BSpline(unsigned int derivativeOrder = 0)
    : derivativeOrder_(derivativeOrder)
    {}
    
    result_type operator()(argument_type x) const
    {
        return exec(x, derivativeOrder_);
    }

    result_type operator()(first_argument_type x, second_argument_type derivative_order) const
    {
         return exec(x, derivativeOrder_ + derivative_order);
    }
    
    value_type operator[](value_type x) const
        { return operator()(x); }
    
    double radius() const
        { return 1.5; }
        
    unsigned int derivativeOrder() const
        { return derivativeOrder_; }

    ArrayVector<double> const & prefilterCoefficients() const
    { 
        static ArrayVector<double> b(1, 2.0*M_SQRT2 - 3.0);
        return b;
    }
    
    typedef T WeightMatrix[3][3];
    static WeightMatrix & weights()
    {
        static T b[3][3] = {{ 0.125, 0.75, 0.125}, 
                            {-0.5, 0.0, 0.5},
                            { 0.5, -1.0, 0.5}};
        return b;
    }

  protected:
    result_type exec(first_argument_type x, second_argument_type derivative_order) const;
    
    unsigned int derivativeOrder_;
};

template <class T>
T BSpline<2, T>::exec(T x, unsigned int derivative_order) const
{
    switch(derivative_order)
    {
        case 0:
        {
            x = VIGRA_CSTD::fabs(x);
            return x < 0.5 ?
                    0.75 - x*x 
                    : x < 1.5 ?
                        0.5 * sq(1.5 - x) 
                    : 0.0;
        }
        case 1:
        {
            return x >= -0.5 ?
                     x <= 0.5 ?
                       -2.0 * x 
                     : x < 1.5 ?
                         x - 1.5 
                       : 0.0 
                   : x > -1.5 ?
                       x + 1.5 
                     : 0.0;
        }
        case 2:
        {
            return x >= -0.5 ?
                     x < 0.5 ?
                         -2.0 
                     : x < 1.5 ?
                         1.0 
                       : 0.0 
                   : x >= -1.5 ?
                       1.0 
                     : 0.0;
        }
        default:
            return 0.0;
    }
}

/********************************************************/
/*                                                      */
/*                     BSpline<3, T>                    */
/*                                                      */
/********************************************************/

template <class T>
class BSpline<3, T>
{
  public:
  
    typedef T            value_type;  
    typedef T            argument_type;  
    typedef T            first_argument_type;  
    typedef unsigned int second_argument_type;  
    typedef T            result_type; 
    enum               { order = 3 };

    BSpline(unsigned int derivativeOrder = 0)
    : derivativeOrder_(derivativeOrder)
    {}
    
    result_type operator()(argument_type x) const
    {
        return exec(x, derivativeOrder_);
    }

    result_type operator()(first_argument_type x, second_argument_type derivative_order) const
    {
         return exec(x, derivativeOrder_ + derivative_order);
    }
   
    result_type dx(argument_type x) const
        { return operator()(x, 1); }
    
    result_type dxx(argument_type x) const
        { return operator()(x, 2); }
    
    value_type operator[](value_type x) const
        { return operator()(x); }
    
    double radius() const
        { return 2.0; }
        
    unsigned int derivativeOrder() const
        { return derivativeOrder_; }

    ArrayVector<double> const & prefilterCoefficients() const
    { 
        static ArrayVector<double> b(1, VIGRA_CSTD::sqrt(3.0) - 2.0);
        return b;
    }
    
    typedef T WeightMatrix[4][4];
    static WeightMatrix & weights()
    {
        static T b[4][4] = {{ 1.0 / 6.0, 2.0 / 3.0, 1.0 / 6.0, 0.0}, 
                            {-0.5, 0.0, 0.5, 0.0},
                            { 0.5, -1.0, 0.5, 0.0},
                            {-1.0 / 6.0, 0.5, -0.5, 1.0 / 6.0}};
        return b;
    }

  protected:
    result_type exec(first_argument_type x, second_argument_type derivative_order) const;
    
    unsigned int derivativeOrder_;
};

template <class T>
T BSpline<3, T>::exec(T x, unsigned int derivative_order) const
{
    switch(derivative_order)
    {
        case 0:
        {
            x = VIGRA_CSTD::fabs(x);
            if(x < 1.0)
            {
                return 2.0/3.0 + x*x*(-1.0 + 0.5*x);
            }
            else if(x < 2.0)
            {
                x = 2.0 - x;
                return x*x*x/6.0;
            }
            else
                return 0.0;
        }
        case 1:
        {
            double s = x < 0.0 ?
                         -1.0 
                       :  1.0;
            x = VIGRA_CSTD::fabs(x);
            return x < 1.0 ?
                     s*x*(-2.0 + 1.5*x) 
                   : x < 2.0 ?
                       -0.5*s*sq(2.0 - x)
                     : 0.0;
        }
        case 2:
        {
            x = VIGRA_CSTD::fabs(x);
            return x < 1.0 ?
                     3.0*x - 2.0 
                   : x < 2.0 ?
                       2.0 - x 
                     : 0.0;
        }
        case 3:
        {
            return x < 0.0 ?
                     x < -1.0 ?
                       x < -2.0 ?
                         0.0 
                       : 1.0 
                     : -3.0 
                   : x < 1.0 ?
                       3.0 
                     : x < 2.0 ?
                         -1.0 
                       : 0.0;
        }
        default:
            return 0.0;
    }
}

typedef BSpline<3, double> CubicBSplineKernel;

/********************************************************/
/*                                                      */
/*                     BSpline<5, T>                    */
/*                                                      */
/********************************************************/

template <class T>
class BSpline<5, T>
{
  public:
  
    typedef T            value_type;  
    typedef T            argument_type;  
    typedef T            first_argument_type;  
    typedef unsigned int second_argument_type;  
    typedef T            result_type; 
    enum               { order = 5 };

    BSpline(unsigned int derivativeOrder = 0)
    : derivativeOrder_(derivativeOrder)
    {}
    
    result_type operator()(argument_type x) const
    {
        return exec(x, derivativeOrder_);
    }

    result_type operator()(first_argument_type x, second_argument_type derivative_order) const
    {
         return exec(x, derivativeOrder_ + derivative_order);
    }
    
    result_type dx(argument_type x) const
        { return operator()(x, 1); }
    
    result_type dxx(argument_type x) const
        { return operator()(x, 2); }
    
    result_type dx3(argument_type x) const
        { return operator()(x, 3); }
    
    result_type dx4(argument_type x) const
        { return operator()(x, 4); }
    
    value_type operator[](value_type x) const
        { return operator()(x); }
    
    double radius() const
        { return 3.0; }
        
    unsigned int derivativeOrder() const
        { return derivativeOrder_; }

    ArrayVector<double> const & prefilterCoefficients() const
    { 
        static ArrayVector<double> const & b = initPrefilterCoefficients();
        return b;
    }
    
    static ArrayVector<double> const & initPrefilterCoefficients()
    { 
        static ArrayVector<double> b(2);
        b[0] = -0.43057534709997114;
        b[1] = -0.043096288203264652;
        return b;
    }
    
    typedef T WeightMatrix[6][6];
    static WeightMatrix & weights()
    {
        static T b[6][6] = {{ 1.0/120.0, 13.0/60.0, 11.0/20.0, 13.0/60.0, 1.0/120.0}, 
                            {-1.0/24.0, -5.0/12.0, 0.0, 5.0/12.0, 1.0/24.0},
                            { 1.0/12.0, 1.0/6.0, -0.5, 1.0/6.0, 1.0/12.0},
                            {-1.0/12.0, 1.0/6.0, 0.0, -1.0/6.0, 1.0/12.0},
                            { 1.0/24.0, -1.0/6.0, 0.25, -1.0/6.0, 1.0/24.0},
                            {-1.0/120.0, 1.0/24.0, -1.0/12.0, 1.0/12.0, -1.0/24.0, 1.0/120.0}};
        return b;
    }

  protected:
    result_type exec(first_argument_type x, second_argument_type derivative_order) const;
    
    unsigned int derivativeOrder_;
};

template <class T>
T BSpline<5, T>::exec(T x, unsigned int derivative_order) const
{
    switch(derivative_order)
    {
        case 0:
        {
            x = VIGRA_CSTD::fabs(x);
            if(x <= 1.0)
            {
                return 0.55 + x*x*(-0.5 + x*x*(0.25 - x/12.0));
            }
            else if(x < 2.0)
            {
                return 17.0/40.0 + x*(0.625 + x*(-1.75 + x*(1.25 + x*(-0.375 + x/24.0))));
            }
            else if(x < 3.0)
            {
                x = 3.0 - x;
                return x*sq(x*x) / 120.0;
            }
            else
                return 0.0;
        }
        case 1:
        {
            double s = x < 0.0 ?
                          -1.0 :
                           1.0;
            x = VIGRA_CSTD::fabs(x);
            if(x <= 1.0)
            {
                return s*x*(-1.0 + x*x*(1.0 - 5.0/12.0*x));
            }
            else if(x < 2.0)
            {
                return s*(0.625 + x*(-3.5 + x*(3.75 + x*(-1.5 + 5.0/24.0*x))));
            }
            else if(x < 3.0)
            {
                x = 3.0 - x;
                return s*sq(x*x) / -24.0;
            }
            else
                return 0.0;
        }
        case 2:
        {
            x = VIGRA_CSTD::fabs(x);
            if(x <= 1.0)
            {
                return -1.0 + x*x*(3.0 -5.0/3.0*x);
            }
            else if(x < 2.0)
            {
                return -3.5 + x*(7.5 + x*(-4.5 + 5.0/6.0*x));
            }
            else if(x < 3.0)
            {
                x = 3.0 - x;
                return x*x*x / 6.0;
            }
            else
                return 0.0;
        }
        case 3:
        {
            double s = x < 0.0 ?
                          -1.0 :
                           1.0;
            x = VIGRA_CSTD::fabs(x);
            if(x <= 1.0)
            {
                return s*x*(6.0 - 5.0*x);
            }
            else if(x < 2.0)
            {
                return s*(7.5 + x*(-9.0 + 2.5*x));
            }
            else if(x < 3.0)
            {
                x = 3.0 - x;
                return -0.5*s*x*x;
            }
            else
                return 0.0;
        }
        case 4:
        {
            x = VIGRA_CSTD::fabs(x);
            if(x <= 1.0)
            {
                return 6.0 - 10.0*x;
            }
            else if(x < 2.0)
            {
                return -9.0 + 5.0*x;
            }
            else if(x < 3.0)
            {
                return 3.0 - x;
            }
            else
                return 0.0;
        }
        case 5:
        {
            return x < 0.0 ?
                     x < -2.0 ?
                       x < -3.0 ?
                         0.0 
                       : 1.0 
                     : x < -1.0 ?
                         -5.0    
                       : 10.0 
                   : x < 2.0 ?
                       x < 1.0 ?
                         -10.0 
                       : 5.0 
                     : x < 3.0 ?
                         -1.0 
                       : 0.0; 
        }
        default:
            return 0.0;
    }
}

typedef BSpline<5, double> QuinticBSplineKernel;

#endif // NO_PARTIAL_TEMPLATE_SPECIALIZATION

/********************************************************/
/*                                                      */
/*                      CatmullRomSpline                */
/*                                                      */
/********************************************************/

template <class T = double>
class CatmullRomSpline
{
public:
    typedef T value_type;
    typedef T argument_type;
    typedef T result_type;
    enum { order = 3 };

    result_type operator()(argument_type x) const;
    
    T operator[] (T x) const
        { return operator()(x); }

    int radius() const
        {return 2;}
        
    unsigned int derivativeOrder() const
        { return 0; }

    ArrayVector<double> const & prefilterCoefficients() const
    { 
        static ArrayVector<double> b(1, 0.0);
        return b;
    }
};


template <class T>
T CatmullRomSpline<T>::operator()(T x) const
{
    x = VIGRA_CSTD::fabs(x);
    if (x <= 1.0)
    {
        return 1.0 + x * x * (-2.5 + 1.5 * x);
    }
    else if (x >= 2.0)
    {
        return 0.0;
    }
    else
    {
        return 2.0 + x * (-4.0 + x * (2.5 - 0.5 * x));
    }
}


//@}

} // namespace vigra


#endif /* VIGRA_SPLINES_HXX */
