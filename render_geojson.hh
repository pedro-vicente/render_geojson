#ifndef WX_RENDER_GEOJSON
#define WX_RENDER_GEOJSON 1

#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/splitter.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/grid.h"
#include "wx/mdi.h"
#include "wx/toolbar.h"
#include "wx/laywin.h"
#include "wx/list.h"
#include "wx/cmdline.h"
#include "wx/datetime.h"
#include "wx/datectrl.h"
#include "wx/stattext.h"
#include "wx/dateevt.h"
#include "wx/panel.h"
#include "wx/calctrl.h"
#include "wx/timectrl.h"
#include "wx/collpane.h"
#include "sample.xpm"
#include "grafix.hh"
#include "geojson.hh"
#include "topojson.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//rgb_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class rgb_t
{
public:
  rgb_t(unsigned char r, unsigned char g, unsigned char b) :
    red(r),
    green(g),
    blue(b)
  {
  }
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxChart : public wxScrolledWindow
{
public:
  wxChart(wxWindow *parent);
  virtual void OnDraw(wxDC& dc);
  void OnMouseDown(wxMouseEvent &event);
  void OnMouseMove(wxMouseEvent &event);
  int read_geojson(const char* file_name);
  int read_topojson(const char* file_name);
  graf_t m_graf;
  geojson_t m_geojson;
  topojson_t m_topojson;
  int m_is_topo;

private:
  double x_low, y_low, x_high, y_high; //data
  int x_min, x_max, y_min, y_max; //screen
  std::vector<rgb_t> rgb_256;
  wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxFrameMain : public wxFrame
{
public:
  wxFrameMain();
  ~wxFrameMain();
  void OnFileOpen(wxCommandEvent &event);
  void OnMRUFile(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  wxString m_current_file;

protected:
  wxWindow *m_win_grid;
  wxWindow *m_win_chart;
  wxSplitterWindow* m_splitter;
  int read(const std::string &file_name);
  wxFileHistory m_file_history;

private:
  wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppAlert
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxAppAlert : public wxApp
{
public:
  virtual bool OnInit();
};

#endif

