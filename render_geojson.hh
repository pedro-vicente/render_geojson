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
#include "wx/treectrl.h"
#include "grafix.hh"
#include "geojson.hh"
#include "topojson.hh"

enum
{
  ID_NEXT_GEOMETRY = wxID_HIGHEST + 1,
  ID_NEXT_POINT
};

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
  void next_geometry();
  void next_point();
  int m_is_topo;
  int read_geojson(const char* file_name);
  int read_topojson(const char* file_name, wxTreeCtrl *tree, wxTreeItemId item_id);

private:
  double x_low, y_low, x_high, y_high; //data
  int x_min, x_max, y_min, y_max; //screen
  std::vector<rgb_t> rgb_256;
  graf_t m_graf;
  geojson_t m_geojson;
  topojson_t m_topojson;
  size_t m_curr_geom;
  size_t m_curr_point;
  wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxTreeCtrlExplorer
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxTreeCtrlExplorer : public wxTreeCtrl
{
public:
  wxTreeCtrlExplorer(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
  void OnSelChanged(wxTreeEvent& event);
private:
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
  void OnNextGeometry(wxCommandEvent& event);
  void OnNextPoint(wxCommandEvent& event);
  wxString m_current_file;

protected:
  wxWindow *m_win_grid;
  wxTreeCtrlExplorer *m_tree;
  wxTreeItemId m_tree_root;
  wxChart *m_win_chart;
  wxSplitterWindow* m_splitter;
  int read(const std::string &file_name);
  wxFileHistory m_file_history;

  //tree icons
  enum
  {
    id_folder,
    id_variable
  };

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

