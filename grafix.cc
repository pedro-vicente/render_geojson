#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/filehistory.h"
#include "wx/config.h"

#include "grafix.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::graf_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

graf_t::graf_t() :
  nbr_decimals(0)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::~graf_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

graf_t::~graf_t()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//void graf_t::init
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::init(int x_min_, int y_min_, int x_max_, int y_max_, double x_low_, double y_low_, double x_high_, double y_high_)
{
  x_min = x_min_;
  x_max = x_max_;
  y_min = y_min_;
  y_max = y_max_;
  x_low = x_low_;
  x_high = x_high_;
  y_low = y_low_;
  y_high = y_high_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::data_to_screen
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::data_to_screen(double x, double y)
{
  P.x = int(x_min + (x_max - x_min)*(x - x_low) / (x_high - x_low));
  P.y = int(y_max + (y_min - y_max)*(y - y_low) / (y_high - y_low));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::data_to_screen
/////////////////////////////////////////////////////////////////////////////////////////////////////

double graf_t::screen_to_data_x(int x_screen)
{
  return (x_low + (x_high - x_low)*(x_screen - x_min) / (x_max - x_min));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_circle
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_circle(wxDC &dc, double x1_data, double y1_data, wxColour color)
{
  dc.SetBrush(wxBrush(color));
  data_to_screen(x1_data, y1_data);
  wxCoord x1 = P.x;
  wxCoord y1 = P.y;
  dc.DrawCircle(x1, y1, 3);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_circle
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_circle(wxDC &dc, double x1_data, double y1_data, wxColour color, int size)
{
  dc.SetBrush(wxBrush(color));
  data_to_screen(x1_data, y1_data);
  wxCoord x1 = P.x;
  wxCoord y1 = P.y;
  dc.DrawCircle(x1, y1, size);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_text
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_text(wxDC &dc, const wxString& str, double x1_data, double y1_data, wxColour color, bool up)
{
  data_to_screen(x1_data, y1_data);
  wxFont font(10, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD);
  dc.SetFont(font);
  dc.SetTextForeground(color);
  if (up)
  {
    P.y -= 30;
  }
  P.x -= 7;
  dc.DrawText(str, P);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_rectangle
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_rectangle(wxDC &dc, double x1_data, double y1_data, double x2_data, double y2_data, wxColour color,
  wxBrushStyle style)
{
  dc.SetBrush(wxBrush(color, style));
  data_to_screen(x1_data, y1_data);
  wxCoord x1 = P.x;
  wxCoord y1 = P.y;
  data_to_screen(x2_data, y2_data);
  wxCoord x2 = P.x;
  wxCoord y2 = P.y;
  wxCoord width = x2 - x1;
  wxCoord height = y2 - y1;
  dc.DrawRectangle(x1, y1, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_polygon
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_polygon(wxDC &dc, size_t nbr_points, PointData *points_data, wxColour color, bool border, bool is_bitmap)
{
  if (border == false)
  {
    dc.SetPen(*wxTRANSPARENT_PEN);
  }
  dc.SetBrush(wxBrush(color));
  wxPoint *points_screen = new wxPoint[nbr_points];
  for (size_t idx = 0; idx < nbr_points; idx++)
  {
    data_to_screen(points_data[idx].x, points_data[idx].y);
    points_screen[idx].x = P.x;
    points_screen[idx].y = P.y;
    if (is_bitmap)
    {
      points_screen[idx].x -= x_min;
      points_screen[idx].y -= y_min;
    }
  }
  dc.DrawPolygon(nbr_points, points_screen);
  delete[] points_screen;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_polygon
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_polygon(wxDC &dc, std::vector<PointData> &points_data, wxColour color_brush, wxColour color_pen)
{
  dc.SetBrush(wxBrush(color_brush));
  dc.SetPen(wxPen(color_pen));
  wxPoint *points_screen = new wxPoint[points_data.size()];
  for (size_t idx = 0; idx < points_data.size(); idx++)
  {
    data_to_screen(points_data[idx].x, points_data[idx].y);
    points_screen[idx].x = P.x;
    points_screen[idx].y = P.y;
  }
  dc.DrawPolygon(points_data.size(), points_screen);
  delete[] points_screen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_back
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_back(wxDC &dc)
{
  dc.SetBrush(wxBrush(wxColour(192, 192, 192)));
  wxCoord width = x_max - x_min;
  wxCoord height = y_max - y_min;
  dc.DrawRectangle(x_min, y_min, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_line
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_line(wxDC &dc, double x1_data, double y1_data, double x2_data, double y2_data)
{
  data_to_screen(x1_data, y1_data);
  wxCoord x1 = P.x;
  wxCoord y1 = P.y;
  data_to_screen(x2_data, y2_data);
  wxCoord x2 = P.x;
  wxCoord y2 = P.y;
  dc.DrawLine(x1, y1, x2, y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_spline
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_spline(wxDC &dc, size_t nbr_points, PointData *points_data, wxColour color)
{
  dc.SetBrush(wxBrush(color));
  wxPoint *points_screen = new wxPoint[nbr_points];
  for (size_t idx = 0; idx < nbr_points; idx++)
  {
    data_to_screen(points_data[idx].x, points_data[idx].y);
    points_screen[idx].x = P.x;
    points_screen[idx].y = P.y;
  }
  dc.DrawSpline(nbr_points, points_screen);
  delete[] points_screen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_scale
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_scale(wxDC &dc)
{
  int idx, step_screen, tick;
  double value, step_value;
  char buf[10];

  //x

  tick = 20;
  idx = x_min;
  step_screen = int((x_max - x_min) / tick);
  value = x_low;
  step_value = (x_high - x_low) / tick;

  while (idx <= x_max)
  {
    dc.DrawLine(idx, y_max, idx, y_max + 7);
    sprintf(buf, "%.0f", value);
    wxString str(buf);
    dc.DrawText(str, idx - strlen(buf) * 3, y_max + 10);
    idx = idx + step_screen;
    value = value + step_value;
  }

  //y

  tick = 30;
  step_screen = int((y_max - y_min) / tick);
  value = y_low;
  if (y_high > y_low) step_value = (y_high - y_low) / tick;
  else step_value = (y_low - y_high) / tick;

  idx = y_max;
  while (idx >= y_min)
  {
    dc.DrawLine(x_min, idx, x_min - 7, idx);
    if (idx < y_max)
    {
      dc.DrawLine(x_min, idx + step_screen / 2, x_min - 4, idx + step_screen / 2);
    }
    if (nbr_decimals)
    {
      sprintf(buf, "%.2f", value);
    }
    else
    {
      sprintf(buf, "%.0f", value);
    }
    wxString str(buf);
    dc.DrawText(str, x_min - strlen(buf) * 15, idx - 12);
    idx = idx - step_screen;
    value = value + step_value;
  }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxGraphicsContext
/////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//graf_t::draw_polygon
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_polygon(wxGraphicsContext *gc, size_t nbr_points, PointData *points_data, wxColour color, bool border, bool is_bitmap)
{
  if (border == false)
  {
    gc->SetPen(*wxTRANSPARENT_PEN);
  }
  gc->SetBrush(wxBrush(color));
  wxPoint2DDouble *points_screen = new wxPoint2DDouble[nbr_points];
  for (size_t idx = 0; idx < nbr_points; idx++)
  {
    data_to_screen(points_data[idx].x, points_data[idx].y);
    points_screen[idx].m_x = P.x;
    points_screen[idx].m_y = P.y;
    if (is_bitmap)
    {
      points_screen[idx].m_x -= x_min;
      points_screen[idx].m_y -= y_min;
    }
  }
  gc->DrawLines(nbr_points, points_screen);
  delete[] points_screen;
}




