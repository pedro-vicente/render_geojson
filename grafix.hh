#ifndef WX_GRAFIX_HPP
#define WX_GRAFIX_HPP 1

#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/filehistory.h"
#include "wx/config.h"
#include "wx/graphics.h"

struct PointData
{
  double x;
  double y;
};

class graf_t
{
public:
  graf_t();
  void init(int, int, int, int, double, double, double, double);
  virtual ~graf_t();

  void draw_back(wxDC &dc);
  void draw_text(wxDC &dc, const wxString& str, double x1_data, double y1_data, wxColour color, bool up);
  void draw_circle(wxDC &dc, double x1_data, double y1_data, wxColour color);
  void draw_circle(wxDC &dc, double x1_data, double y1_data, wxColour color, int size);
  void draw_line(wxDC &dc, double x1_data, double y1_data, double x2_data, double y2_data);
  void draw_rectangle(wxDC &dc, double x1_data, double y1_data, double x2_data, double y2_data, wxColour color);
  void draw_polygon(wxDC &dc, size_t nbr_points, PointData *points_data, wxColour color, bool border, bool is_bitmap);
  void draw_spline(wxDC &dc, size_t nbr_points, PointData *points_data, wxColour color);
  void draw_scale(wxDC &dc);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //wxGraphicsContext
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void draw_polygon(wxGraphicsContext *gc, size_t nbr_points, PointData *points_data, wxColour color, bool border, bool is_bitmap);
  double screen_to_data_x(int x_screen);

  int nbr_decimals;

protected:
  //screen
  int x_min, x_max, y_min, y_max;

  //data
  double x_low, y_low, x_high, y_high;
  wxPoint P;
  void data_to_screen(double x1_data, double y1_data);
};


#endif
