#pragma once

// реализация типов по аналогии с GdiPlus
namespace GdiLinux
{
    typedef float   REAL;
#define REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */

    //--------------------------------------------------------------------------
    // Forward declarations of common classes
    //--------------------------------------------------------------------------

//     class Size;
    class SizeF;
//     class Point;
    class PointF;
//     class Rect;
    class RectF;
//     class CharacterRange;


//--------------------------------------------------------------------------
// Represents a dimension in a 2D coordinate system (floating-point coordinates)
//--------------------------------------------------------------------------
    class SizeF
    {
    public:
        SizeF() {
            Width = Height = 0.0f;
        }
        SizeF( const SizeF& size ) {
            Width = size.Width;
            Height = size.Height;
        }
        SizeF( REAL width, REAL height ) {
            Width = width;
            Height = height;
        }
        SizeF operator+( const SizeF& sz ) const {
            return SizeF( Width + sz.Width, Height + sz.Height );
        }
        SizeF operator-( const SizeF& sz ) const {
            return SizeF( Width - sz.Width, Height - sz.Height );
        }
        BOOL Equals( const SizeF& sz ) const {
            return ( Width == sz.Width ) && ( Height == sz.Height );
        }
        BOOL Empty() const {
            return ( Width == 0.0f && Height == 0.0f );
        }

    public:
        REAL Width;
        REAL Height;
    };
    //--------------------------------------------------------------------------
    // Represents a location in a 2D coordinate system (floating-point coordinates)
    //--------------------------------------------------------------------------
    class PointF
    {
    public:
        PointF() {
            X = Y = 0.0f;
        }
        PointF( const PointF& point ) {
            X = point.X;
            Y = point.Y;
        }
        PointF( const SizeF& size ) {
            X = size.Width;
            Y = size.Height;
        }
        PointF( REAL x, REAL y ) {
            X = x;
            Y = y;
        }
        PointF operator+( const PointF& point ) const {
            return PointF( X + point.X,
                           Y + point.Y );
        }
        PointF operator-( const PointF& point ) const {
            return PointF( X - point.X,
                           Y - point.Y );
        }
        BOOL Equals( const PointF& point ) {
            return ( X == point.X ) && ( Y == point.Y );
        }

    public:
        REAL X;
        REAL Y;
    };

    //--------------------------------------------------------------------------
    // Represents a rectangle in a 2D coordinate system (floating-point coordinates)
    //--------------------------------------------------------------------------

    class RectF
    {
    public:
        RectF() {
            X = Y = Width = Height = 0.0f;
        }
        RectF( REAL x, REAL y, REAL width, REAL height ) {
            X = x;
            Y = y;
            Width = width;
            Height = height;
        }
        RectF( const PointF& location, const SizeF& size ) {
            X = location.X;
            Y = location.Y;
            Width = size.Width;
            Height = size.Height;
        }
        RectF* Clone() const {
            return new RectF( X, Y, Width, Height );
        }
        void GetLocation( PointF* point ) const {
            point->X = X;
            point->Y = Y;
        }
        void GetSize( SizeF* size ) const {
            size->Width = Width;
            size->Height = Height;
        }
        void GetBounds( RectF* rect ) const {
            rect->X = X;
            rect->Y = Y;
            rect->Width = Width;
            rect->Height = Height;
        }
        REAL GetLeft() const {
            return X;
        }
        REAL GetTop() const {
            return Y;
        }
        REAL GetRight() const {
            return X + Width;
        }
        REAL GetBottom() const {
            return Y + Height;
        }
        BOOL IsEmptyArea() const {
            return ( Width <= REAL_EPSILON ) || ( Height <= REAL_EPSILON );
        }
        BOOL Equals( const RectF& rect ) const {
            return X == rect.X &&
                Y == rect.Y &&
                Width == rect.Width &&
                Height == rect.Height;
        }
        BOOL Contains( REAL x, REAL y ) const {
            return x >= X && x < X + Width &&
                y >= Y && y < Y + Height;
        }
        BOOL Contains( const PointF& pt ) const {
            return Contains( pt.X, pt.Y );
        }
        BOOL Contains( const RectF& rect ) const {
            return ( X <= rect.X ) && ( rect.GetRight() <= GetRight() ) &&
                ( Y <= rect.Y ) && ( rect.GetBottom() <= GetBottom() );
        }
        void Inflate( REAL dx, REAL dy ) {
            X -= dx;
            Y -= dy;
            Width += 2 * dx;
            Height += 2 * dy;
        }
        void Inflate( const PointF& point ) {
            Inflate( point.X, point.Y );
        }
        BOOL Intersect( const RectF& rect ) {
            return Intersect( *this, *this, rect );
        }
        static BOOL Intersect( RectF& c, const RectF& a, const RectF& b ) {
            REAL right = std::min( a.GetRight(), b.GetRight() );
            REAL bottom = std::min( a.GetBottom(), b.GetBottom() );
            REAL left = std::max( a.GetLeft(), b.GetLeft() );
            REAL top = std::max( a.GetTop(), b.GetTop() );

            c.X = left;
            c.Y = top;
            c.Width = right - left;
            c.Height = bottom - top;
            return !c.IsEmptyArea();
        }
        BOOL IntersectsWith( const RectF& rect ) const {
            return ( GetLeft() < rect.GetRight() &&
                     GetTop() < rect.GetBottom() &&
                     GetRight() > rect.GetLeft() &&
                     GetBottom() > rect.GetTop() );
        }
        static BOOL Union( RectF& c, const RectF& a, const RectF& b ) {
            REAL right = std::max( a.GetRight(), b.GetRight() );
            REAL bottom = std::max( a.GetBottom(), b.GetBottom() );
            REAL left = std::min( a.GetLeft(), b.GetLeft() );
            REAL top = std::min( a.GetTop(), b.GetTop() );

            c.X = left;
            c.Y = top;
            c.Width = right - left;
            c.Height = bottom - top;
            return !c.IsEmptyArea();
        }
        void Offset( const PointF& point ) {
            Offset( point.X, point.Y );
        }
        void Offset( REAL dx, REAL dy ) {
            X += dx;
            Y += dy;
        }

    public:
        REAL X;
        REAL Y;
        REAL Width;
        REAL Height;
    };
};