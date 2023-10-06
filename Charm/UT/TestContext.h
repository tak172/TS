#include "../StatBuilder/GraphicContext.h"

class TestContext : public GraphicContext
{
public:
	//сглаживание
	virtual Gdiplus::SmoothingMode GetSmoothingMode() const { return Gdiplus::SmoothingModeDefault; }
	virtual Gdiplus::Status SetSmoothingMode( Gdiplus::SmoothingMode ) { return Gdiplus::Ok; }
	virtual Gdiplus::Status SetTextRenderingHint( Gdiplus::TextRenderingHint ) { return Gdiplus::Ok; }

    //прозрачность
    virtual Gdiplus::CompositingMode GetCompositingMode() const { return Gdiplus::CompositingModeSourceCopy; };
    virtual Gdiplus::Status SetCompositingMode( Gdiplus::CompositingMode ) { return Gdiplus::Ok; };

    //рисование
	virtual Gdiplus::Status DrawRectangle( const Gdiplus::Pen *, const Gdiplus::RectF & )
	{ 
		return Gdiplus::Ok;
	}
    virtual Gdiplus::Status DrawString( const std::wstring& /*str*/, const Gdiplus::Font * /*font*/, 
		const Gdiplus::RectF & /*layoutRect*/, const Gdiplus::StringFormat * /*stringFormat*/, const Gdiplus::Brush * /*brush*/ )
	{
		return Gdiplus::Ok;
	}
    virtual Gdiplus::Status DrawString( const std::wstring& /*string*/, const Gdiplus::Font *, 
        const Gdiplus::PointF&, const Gdiplus::Brush * )
    {
        return Gdiplus::Ok;
    }
    virtual Gdiplus::Status DrawString( const std::wstring&, const Gdiplus::Font * font, 
        const Gdiplus::PointF& , const Gdiplus::StringFormat * , const Gdiplus::Brush *  )
    {
        return Gdiplus::Ok;
    }

	virtual Gdiplus::Status DrawLine( const Gdiplus::Pen * /*pen*/, const Gdiplus::PointF & pt1, const Gdiplus::PointF & pt2 )
	{
		painted_lines.push_back( std::make_pair( pt1, pt2 ) );
		return Gdiplus::Ok;
	}
    virtual Gdiplus::Status DrawLine( const Gdiplus::Pen * /*pen*/, int x1, int y1, int x2, int y2)
    {
		painted_lines.push_back( std::make_pair( Gdiplus::PointF( ( Gdiplus::REAL ) x1, ( Gdiplus::REAL ) y1 ), 
			Gdiplus::PointF( ( Gdiplus::REAL ) x2, ( Gdiplus::REAL ) y2 ) ) );
        return Gdiplus::Ok;
    }

    virtual Gdiplus::Status DrawLines( const Gdiplus::Pen * /*pen*/, const Gdiplus::PointF * points, INT count )
	{
		for ( INT i = 0; i < count; ++i )
		{
			if ( i != count - 1 )
			{
				INT i_next = i + 1;
				painted_lines.push_back( std::make_pair( points[i], points[i_next] ) );
			}
		}
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status DrawPath( const Gdiplus::Pen * /*pen*/, const Gdiplus::GraphicsPath * /*path*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status FillEllipse( const Gdiplus::Brush * /*brush*/, const Gdiplus::RectF & /*rectf*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status DrawBezier( const Gdiplus::Pen * /*pen*/, const Gdiplus::PointF & /*pt1*/, const Gdiplus::PointF & /*pt2*/,
		const Gdiplus::PointF & /*pt3*/, const Gdiplus::PointF & /*pt4*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status FillRectangle( const Gdiplus::Brush * /*brush*/, const Gdiplus::RectF & /*rect*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status FillRectangle( const Gdiplus::Brush * /*brush*/, Gdiplus::REAL /*x*/, Gdiplus::REAL /*y*/,
		Gdiplus::REAL /*width*/, Gdiplus::REAL /*height*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status FillPath( const Gdiplus::Brush * /*brush*/, const Gdiplus::GraphicsPath * /*path*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status DrawImage( Gdiplus::Image * /*image*/, const Gdiplus::RectF & /*destRect*/, Gdiplus::REAL /*srcx*/, 
		Gdiplus::REAL /*srcy*/, Gdiplus::REAL /*srcwidth*/, Gdiplus::REAL /*srcheight*/, Gdiplus::Unit /*srcUnit*/, 
		const Gdiplus::ImageAttributes * /*imageAttributes*/, Gdiplus::DrawImageAbort /*callback*/, VOID * /*callbackData*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status DrawPolygon(const Gdiplus::Pen* /*pen*/, const Gdiplus::Point* /*points*/, INT /*count*/)
	{
		return Gdiplus::Ok;
	}

	virtual Gdiplus::Status FillPolygon( const Gdiplus::Brush * /*brush*/, const Gdiplus::PointF * /*points*/, INT /*count*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status FillPolygon( const Gdiplus::Brush * /*brush*/, const Gdiplus::Point * /*points*/, INT /*count*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status DrawImage( Gdiplus::Image * /*image*/, const Gdiplus::RectF & /*rect*/)
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status DrawEllipse( const Gdiplus::Pen * /*pen*/, const Gdiplus::RectF & /*rect*/)
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status FillRegion( const Gdiplus::Brush * /*brush*/, const Gdiplus::Region * /*region*/ )
	{
		return Gdiplus::Ok;
	}

	////строки
	virtual Gdiplus::Status MeasureString( const std::wstring& str, const Gdiplus::Font * /*font*/, 
		const Gdiplus::PointF & origin, Gdiplus::RectF * boundingBox ) const
	{
		//условно исходим из расчета: ширина одного символа == 1 клетка
		*boundingBox = Gdiplus::RectF( origin, Gdiplus::SizeF( ( Gdiplus::REAL ) ((double)str.size()), 
			GetSymHeight() ) );
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status MeasureString( const std::wstring& /*str*/, const Gdiplus::Font * /*font*/, 
		const Gdiplus::PointF & /*origin*/, const Gdiplus::StringFormat * /*string_format*/, Gdiplus::RectF * /*boundingBox*/ ) const
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status MeasureString( const std::wstring& /*str*/, const Gdiplus::Font * /*font*/, 
		const Gdiplus::RectF & /*layoutRect*/, const Gdiplus::StringFormat * /*string_format*/, Gdiplus::RectF * /*boundingBox*/, 
		INT * /*codepointsFitted*/, INT * /*linesFilled*/ ) const
	{
		return Gdiplus::Ok;
	}

	//общее
	virtual void RotateWorld( const Gdiplus::PointF & /*ref_pt*/, double /*angle*/, Gdiplus::Matrix & /*start_matrix*/ )
	{
	}
	virtual void Restore( const Gdiplus::GraphicsState & gstate )
	{
	}
	virtual Gdiplus::GraphicsState FlipX( const Gdiplus::PointF & ref_pt )
	{
		return Gdiplus::GraphicsState();
	}
	virtual Gdiplus::Status SetTransform( const Gdiplus::Matrix * /*matrix*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status TranslateTransform( Gdiplus::REAL /*dx*/, Gdiplus::REAL /*dy*/, Gdiplus::MatrixOrder /*matrix_order*/ )
	{
		return Gdiplus::Ok;
	}
	virtual Gdiplus::Status ScaleTransform( Gdiplus::REAL /*sx*/, Gdiplus::REAL /*sy*/, Gdiplus::MatrixOrder /*matrix_order*/ )
	{
		return Gdiplus::Ok;
	}
    virtual void ResetTransform()
    {
    }
	virtual Gdiplus::Status GetClipBounds( Gdiplus::RectF * /*rectf*/ ) const
	{
		return Gdiplus::Ok;
	}
    virtual Gdiplus::Status SetClip(const Gdiplus::RectF& /*rect*/, Gdiplus::CombineMode /*combineMode = Gdiplus::CombineMode::CombineModeReplace*/)
    {
        return Gdiplus::Ok;
    }
	virtual boost::shared_ptr <Gdiplus::Bitmap> CreateBitmap( INT width, INT height ) const
	{
		return 	boost::shared_ptr <Gdiplus::Bitmap>( new Gdiplus::Bitmap( width, height ) );
	}

    virtual HDC GetHDC()
    {return 0;}
    virtual VOID ReleaseHDC(HDC)
    {}

	//специальные методы
	static void SetSymHeight( Gdiplus::REAL symheight ){ symHeight = symheight; }
	typedef std::pair <Gdiplus::PointF, Gdiplus::PointF> Line;
	std::list <Line> GetPaintedLines() const{ return painted_lines; }

private:
	static Gdiplus::REAL GetSymHeight() { return symHeight; }
	static Gdiplus::REAL symHeight; //высота символа
	std::list <Line> painted_lines; //все отрисованные линии
};