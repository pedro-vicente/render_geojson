#include "render_geojson.hh"

const int panel_size = 510;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppAlert::OnInit()
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_APP(wxAppAlert);

bool wxAppAlert::OnInit()
{
  if (!wxApp::OnInit())
  {
    return false;
  }
  wxFrameMain *frame = new wxFrameMain();
  frame->Show(true);
  frame->Maximize();
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxFrameMain, wxFrame)
EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxFrameMain::OnMRUFile)
EVT_MENU(wxID_OPEN, wxFrameMain::OnFileOpen)
EVT_MENU(wxID_EXIT, wxFrameMain::OnQuit)
EVT_MENU(wxID_ABOUT, wxFrameMain::OnAbout)
wxEND_EVENT_TABLE()

wxFrameMain::wxFrameMain()
  : wxFrame(NULL, wxID_ANY, "m_geojson reader")
{
  SetIcon(wxICON(sample));
  wxMenu *menu_file = new wxMenu;
  menu_file->Append(wxID_OPEN, "O&pen\tAlt-O", "Open file");
  menu_file->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");
  wxMenu *menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
  CreateStatusBar(2);

  m_splitter = new wxSplitterWindow(this);
  m_splitter->SetSashInvisible(true);
  wxPanel *grid = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);
  grid->SetMinSize(wxSize(panel_size, -1));
  wxWindow *chart = new wxWindow(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);
  m_splitter->SplitVertically(grid, chart, panel_size);

  //avoid update on empty windows 
  m_win_grid = NULL;
  m_win_chart = NULL;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //file history
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_file_history.UseMenu(menu_file);
  m_file_history.AddFilesToMenu(menu_file);
  m_file_history.Load(*wxConfig::Get());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::~wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxFrameMain::~wxFrameMain()
{
  m_file_history.Save(*wxConfig::Get());
  if (m_win_chart)
  {
    m_win_chart->Destroy();
  }
  if (m_win_grid)
  {
    m_win_grid->Destroy();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnQuit
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  m_file_history.Save(*wxConfig::Get());
  Close(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnAbout
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString str("Pedro Vicente (2018)\n\n");
  wxMessageBox(str, "m_geojson reader", wxOK, this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnMRUFile
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnMRUFile(wxCommandEvent& event)
{
  size_t idx = event.GetId() - wxID_FILE1;
  wxString path(m_file_history.GetHistoryFile(idx));
  if (!path.empty())
  {
    if (read(path.ToStdString()) < 0)
    {
      m_file_history.RemoveFileFromHistory(idx);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnFileOpen
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnFileOpen(wxCommandEvent &WXUNUSED(event))
{
  wxString path;
  wxFileDialog dlg(this, wxT("Open file"),
    wxEmptyString,
    wxEmptyString,
    wxString::Format
    (
      wxT("m_geojson (*.geojson)|*.geojson|All files (%s)|%s"),
      wxFileSelectorDefaultWildcardStr,
      wxFileSelectorDefaultWildcardStr
    ),
    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
  if (dlg.ShowModal() != wxID_OK)
  {
    return;
  }
  path = dlg.GetPath();
  if (this->read(path.ToStdString()) != 0)
  {
    return;
  }
  m_file_history.AddFileToHistory(path);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int wxFrameMain::read(const std::string &file_name)
{
  int is_topo = is_topojson(file_name.c_str());
  if (is_topo == 1)
  {
    return -1;
  }
  m_win_chart = m_splitter->GetWindow2();
  wxChart *chart = new wxChart(m_splitter);
  if (chart->read_geojson(file_name.c_str()) < 0)
  {
    chart->Destroy();
    return -1;
  }
  m_splitter->ReplaceWindow(m_win_chart, chart);
  m_win_chart->Destroy();
  m_win_chart = chart;
  m_current_file = file_name;
  SetTitle(file_name);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::wxChart()
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxChart, wxScrolledWindow)
EVT_LEFT_DOWN(wxChart::OnMouseDown)
EVT_MOTION(wxChart::OnMouseMove)
wxEND_EVENT_TABLE()

wxChart::wxChart(wxWindow *parent) :
  wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::read_geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

int wxChart::read_geojson(const char* file_name)
{
  if (m_geojson.convert(file_name) < 0)
  {
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //initialize_chart
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  double x_low, y_low, x_high, y_high; //data
  int x_min, x_max, y_min, y_max; //screen
  x_min = 80;
  y_min = 30;
  x_max = 1300;
  y_max = 750;

  //data
  x_low = std::numeric_limits<double>::max();
  y_low = std::numeric_limits<double>::max();
  x_high = -std::numeric_limits<double>::max();
  y_high = -std::numeric_limits<double>::max();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //get maximum and minimum values
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t size_features = m_geojson.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = m_geojson.m_feature.at(idx_fet);
    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();
      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();
        if (size_crd == 0)
        {
          continue;
        }
        std::vector<double> lat;
        std::vector<double> lon;
        for (size_t idx = 0; idx < size_crd; idx++)
        {
          lat.push_back(polygon.m_coord[idx].m_lat);
          lon.push_back(polygon.m_coord[idx].m_lon);
        }
        for (size_t idx = 0; idx < size_crd; idx++)
        {
          double lat_ = lat.at(idx);
          double lon_ = lon.at(idx);
          if (lat_ > y_high)
          {
            y_high = lat.at(idx);
          }
          if (lon_ > x_high)
          {
            x_high = lon.at(idx);
          }
          if (lat_ < y_low)
          {
            y_low = lat.at(idx);
          }
          if (lon_ < x_low)
          {
            x_low = lon.at(idx);
          }
        }
      }  //idx_pol
    } //idx_geo
  } //idx_fet

  SetScrollbar(wxVERTICAL, 0, 0, 0);
  m_graf.init(x_min, y_min, x_max, y_max,
    x_low, y_low,
    x_high, y_high);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnDraw
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnDraw(wxDC& dc)
{
  m_graf.draw_back(dc);
  m_graf.draw_scale(dc);

  ///////////////////////////////////////////////////////////////////////////////////////
  //render m_geojson
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t size_features = m_geojson.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = m_geojson.m_feature.at(idx_fet);

    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();

      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();

        if (size_crd == 0)
        {
          continue;
        }

        std::vector<double> lat;
        std::vector<double> lon;

        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
        {
          lat.push_back(polygon.m_coord[idx_crd].m_lat);
          lon.push_back(polygon.m_coord[idx_crd].m_lon);
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        //render each polygon as a vector of vertices 
        ///////////////////////////////////////////////////////////////////////////////////////

        if (geometry.m_type.compare("Point") == 0)
        {

        }
        else if (geometry.m_type.compare("Polygon") == 0 ||
          geometry.m_type.compare("MultiPolygon") == 0)
        {
          std::vector<PointData> points;
          for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
          {
            points.push_back(PointData(lon.at(idx_crd), lat.at(idx_crd)));
          }
          m_graf.draw_polygon(dc, points, wxColour(0, 255, 0));
          for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
          {
            double lat_ = lat.at(idx_crd);
            double lon_ = lon.at(idx_crd);
            m_graf.draw_circle(dc, lon_, lat_, wxColour(255, 0, 0), 2);
          }
        }
      }  //idx_pol
    } //idx_geo
  } //idx_fet

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnMouseDown
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseDown(wxMouseEvent &event)
{
  int x, y, xx, yy;
  event.GetPosition(&x, &y);
  CalcUnscrolledPosition(x, y, &xx, &yy);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnMouseMove
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseMove(wxMouseEvent &event)
{
  int x, y, xx, yy;
  event.GetPosition(&x, &y);
  CalcUnscrolledPosition(x, y, &xx, &yy);
}

