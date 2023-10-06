// Define "gdi" types for Windows and Linux

#ifndef LINUX
// for Windows use GdiPlus types
typedef Gdiplus::REAL       KReal;
typedef Gdiplus::PointF     KPointF;
typedef Gdiplus::RectF      KRectF;
typedef Gdiplus::SizeF      KSizeF;
#else
// for Linux define own types
typedef GdiLinux::REAL      KReal;
typedef GdiLinux::PointF    KPointF;
typedef GdiLinux::RectF     KRectF;
typedef GdiLinux::SizeF     KSizeF;
#endif
   