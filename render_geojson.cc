#include "render_geojson.hh"
#include "pal_rgb.h"
#include "icons/sample.xpm"
#include "icons/forward.xpm"
#include "icons/folder.xpm"

const int panel_size = 510;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//is_polygon
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool is_polygon(const Geometry_t &g)
{
  if (g.type.compare("Polygon") == 0 || g.type.compare("MultiPolygon") == 0)
  {
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GetPathComponent
//return last component of POSIX path name
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxString GetPathComponent(const wxString &path)
{
  wxString name;
  bool isurl = (path.SubString(0, 3) == "http");
  if (isurl)
  {
    return path;
  }
  else
  {
#ifdef __WINDOWS__
    name = path.AfterLast('\\');
#else
    name = path.AfterLast('/');
#endif
  }
  return name;
}

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
EVT_TOOL(ID_NEXT_GEOMETRY, wxFrameMain::OnNextGeometry)
EVT_TOOL(ID_NEXT_POINT, wxFrameMain::OnNextPoint)
wxEND_EVENT_TABLE()

wxFrameMain::wxFrameMain()
  : wxFrame(NULL, wxID_ANY, "geojson reader"),
  m_win_chart(NULL)
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

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //toolbar
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
  wxToolBar* tb = GetToolBar();
  tb->AddTool(wxID_OPEN, wxT("Open file"), wxBitmap(xpm_folder), wxT("Open file"));
  tb->AddTool(ID_NEXT_GEOMETRY, wxT("Next geometry"), wxBitmap(forward_xpm), wxT("Next geometry"));
  tb->AddTool(ID_NEXT_POINT, wxT("Next point"), wxBitmap(forward_xpm), wxT("Next point"));
  tb->Realize();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //splitter
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_splitter = new wxSplitterWindow(this);
  m_splitter->SetSashInvisible(true);
  m_win_grid = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);
  m_win_grid->SetBackgroundColour(*wxWHITE);
  m_win_grid->SetMinSize(wxSize(panel_size, -1));
  wxWindow *chart = new wxWindow(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);
  m_splitter->SplitVertically(m_win_grid, chart, panel_size);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //tree
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_tree = new wxTreeCtrlExplorer(m_win_grid, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT);
  wxImageList* imglist = new wxImageList(16, 16, true, 2);
  wxBitmap bitmaps[3];
  bitmaps[id_folder] = wxBitmap(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
  bitmaps[id_variable] = wxBitmap(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16)));
  imglist->Add(bitmaps[id_folder]);
  imglist->Add(bitmaps[id_variable]);
  m_tree->AssignImageList(imglist);

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
  wxMessageBox(str, "geojson reader", wxOK, this);
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
      wxT("geojson/topojson files (*.geojson;*.topojson;*.json)|*.geojson;*.topojson;*.json|All files (%s)|%s"),
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
  wxSize size = m_win_grid->GetSize();
  m_tree->SetSize(size);
  m_tree->DeleteAllItems();
  m_tree_root = m_tree->AddRoot("");
  wxTreeItemId root = m_tree->AppendItem(m_tree_root, GetPathComponent(file_name), 0, 0, NULL);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //read
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxChart *chart = new wxChart(m_splitter);
  chart->m_is_topojson = is_topojson(file_name.c_str());
  if (chart->m_is_topojson == 1)
  {
    if (chart->read_topojson(file_name.c_str(), m_tree, root) < 0)
    {
      chart->Destroy();
      return -1;
    }
  }
  else
  {
    if (chart->read_geojson(file_name.c_str()) < 0)
    {
      chart->Destroy();
      return -1;
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //set windows
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_win_chart = (wxChart *)m_splitter->GetWindow2();
  m_splitter->ReplaceWindow(m_win_chart, chart);
  m_win_chart->Destroy();
  m_win_chart = chart;
  m_current_file = file_name;
  SetTitle(file_name);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnNextGeometry
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnNextGeometry(wxCommandEvent&)
{
  if (m_win_chart)
  {
    m_win_chart->next_geometry();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnNextPoint
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnNextPoint(wxCommandEvent&)
{
  if (m_win_chart)
  {
    m_win_chart->next_point();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::wxChart()
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxChart, wxScrolledWindow)
EVT_LEFT_DOWN(wxChart::OnMouseDown)
EVT_MOTION(wxChart::OnMouseMove)
wxEND_EVENT_TABLE()

wxChart::wxChart(wxWindow *parent) :
  wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER),
  m_curr_geom(0),
  m_curr_point(0)
{
  x_min = 80;
  y_min = 30;
  x_max = 1300;
  y_max = 750;
  x_low = std::numeric_limits<double>::max();
  y_low = std::numeric_limits<double>::max();
  x_high = -std::numeric_limits<double>::max();
  y_high = -std::numeric_limits<double>::max();
  for (size_t idx = 0; idx < 3 * 256; idx += 3)
  {
    unsigned char r = pal_rgb[idx];
    unsigned char g = pal_rgb[idx + 1];
    unsigned char b = pal_rgb[idx + 2];
    rgb_256.push_back(rgb_t(r, g, b));
  }
  SetScrollbar(wxVERTICAL, 0, 0, 0);
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
          lat.push_back(polygon.m_coord[idx].y);
          lon.push_back(polygon.m_coord[idx].x);
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
  m_graf.init(x_min, y_min, x_max, y_max, x_low, y_low, x_high, y_high);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::read_topojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

int wxChart::read_topojson(const char* file_name, wxTreeCtrl *tree, wxTreeItemId item_id)
{
  if (m_topojson.convert(file_name) < 0)
  {
    return -1;
  }

  //no objects
  if (!m_topojson.m_topology.size())
  {
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //must make the coordinates for the topology first; assume the first topology
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t topology_index = 0;
  m_topojson.make_coordinates(topology_index);
  topology_object_t topology = m_topojson.m_topology.at(topology_index);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //initialize_chart
  //get maximum and minimum values
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t size_geom = topology.m_geom.size();
  for (size_t idx_geom = 0; idx_geom < size_geom; idx_geom++)
  {
    Geometry_t geometry = topology.m_geom.at(idx_geom);
    wxTreeItemId item_geom = tree->AppendItem(item_id, geometry.type, 1, 1, new ItemData(idx_geom));
    if (is_polygon(geometry))
    {
      size_t size_pol = geometry.m_polygon.size();
      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        Polygon_topojson_t polygon = geometry.m_polygon.at(idx_pol);
        wxString name_pol = wxString::Format(wxT("%i"), (int)idx_pol + 1);
        wxTreeItemId item_pol = tree->AppendItem(item_geom, name_pol, 1, 1, NULL);
        size_t size_arcs = polygon.arcs.size();
        for (size_t idx_arc = 0; idx_arc < size_arcs; idx_arc++)
        {
          int index_arc = polygon.arcs.at(idx_arc);
          wxString name_arc = wxString::Format(wxT("%i"), (int)index_arc);
          tree->AppendItem(item_pol, name_arc, 1, 1, NULL);
        }
        size_t size_points = geometry.m_polygon.at(idx_pol).m_y.size();
        for (size_t idx = 0; idx < size_points; idx++)
        {
          double lat = geometry.m_polygon.at(idx_pol).m_y.at(idx);
          double lon = geometry.m_polygon.at(idx_pol).m_x.at(idx);
          if (lat > y_high)
          {
            y_high = lat;
          }
          if (lon > x_high)
          {
            x_high = lon;
          }
          if (lat < y_low)
          {
            y_low = lat;
          }
          if (lon < x_low)
          {
            x_low = lon;
          }
        }//size_pol
      }//"Polygon"
    }//size_geom
  }
  m_graf.init(x_min, y_min, x_max, y_max, x_low, y_low, x_high, y_high);
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
  //render topojson
  ///////////////////////////////////////////////////////////////////////////////////////

  if (m_is_topojson == 1)
  {
    if (!m_topojson.m_topology.size())
    {
      return;
    }
    topology_object_t topology = m_topojson.m_topology.at(0);
    draw_topology(dc, topology);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //render geojson
  ///////////////////////////////////////////////////////////////////////////////////////

  else
  {
    size_t size_features = m_geojson.m_feature.size();
    size_t range = rgb_256.size() / size_features;
    size_t idx_pal = 0;
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
            lat.push_back(polygon.m_coord[idx_crd].y);
            lon.push_back(polygon.m_coord[idx_crd].x);
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
            wxColour color = wxColour(rgb_256.at(idx_pal).red, rgb_256.at(idx_pal).green, rgb_256.at(idx_pal).blue);
            idx_pal += range;
            if (idx_pal >= rgb_256.size())
            {
              idx_pal = 0;
            }
            m_graf.draw_polygon(dc, points, color, wxColour(0, 0, 0));
          }
        }  //idx_pol
      } //idx_geo
    } //idx_fet
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::draw_topology
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::draw_topology(wxDC& dc, const topology_object_t& topology)
{
  size_t size_geom = topology.m_geom.size();
  for (size_t idx_geom = 0; idx_geom < size_geom; idx_geom++)
  {
    draw_geometry(dc, topology, idx_geom);
  }
  draw_geometry(dc, topology, m_curr_geom);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::draw_geometry
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::draw_geometry(wxDC& dc, const topology_object_t& topology, size_t idx_geom)
{
  Geometry_t geometry = topology.m_geom.at(idx_geom);
  if (is_polygon(geometry))
  {
    size_t size_pol = geometry.m_polygon.size();
    for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
    {
      Polygon_topojson_t polygon = geometry.m_polygon.at(idx_pol);

      ///////////////////////////////////////////////////////////////////////////////////////
      //render each polygon as a vector of vertices passed to draw_polygon
      ///////////////////////////////////////////////////////////////////////////////////////

      std::vector<PointData> points;
      size_t size_points = geometry.m_polygon.at(idx_pol).m_y.size();
      for (size_t idx_crd = 0; idx_crd < size_points; idx_crd++)
      {
        double lat = geometry.m_polygon.at(idx_pol).m_y.at(idx_crd);
        double lon = geometry.m_polygon.at(idx_pol).m_x.at(idx_crd);
        points.push_back(PointData(lon, lat));
      }
      if (idx_geom == m_curr_geom)
      {
        m_graf.draw_polygon(dc, points, wxColour(128, 0, 0), wxColour(255, 0, 0));
      }
      else
      {
        m_graf.draw_polygon(dc, points, wxColour(0, 128, 0), wxColour(0, 0, 0));
      }
      for (size_t idx_crd = 0; idx_crd < points.size(); idx_crd++)
      {
        double px = points.at(idx_crd).x;
        double py = points.at(idx_crd).y;
        if (idx_crd == m_curr_point && idx_geom == m_curr_geom)
        {
          m_graf.draw_circle(dc, px, py, wxColour(255, 0, 0), 4);
        }
        else
        {
          m_graf.draw_circle(dc, px, py, wxColour(0, 255, 0), 2);
        }
      }
    }//size_pol
  }//"Polygon"
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


/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::next_geometry
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::next_geometry()
{
  if (!m_topojson.m_topology.size())
  {
    return;
  }
  topology_object_t topology = m_topojson.m_topology.at(0);
  m_curr_point = 0;
  size_t size_geom = topology.m_geom.size();
  m_curr_geom++;
  if (m_curr_geom > size_geom - 1)
  {
    m_curr_geom = 0;
  }
  Refresh();
  wxString str = wxString::Format(_T("Geometry %ld of %ld."), (long)m_curr_geom, (long)size_geom);
  wxFrameMain *main = (wxFrameMain*)wxGetApp().GetTopWindow();
  main->SetStatusText(str);
  main->m_tree->select_item(m_curr_geom);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::next_point
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::next_point()
{
  if (!m_topojson.m_topology.size())
  {
    return;
  }
  topology_object_t topology = m_topojson.m_topology.at(0);
  size_t size_geom = topology.m_geom.size();
  Geometry_t geometry = topology.m_geom.at(m_curr_geom);
  if (is_polygon(geometry))
  {
    Polygon_topojson_t polygon = geometry.m_polygon.at(0);
    size_t size_points = polygon.m_x.size();
    m_curr_point++;
    if (m_curr_point > size_points - 1)
    {
      m_curr_point = 0;
    }
    Refresh();
    wxString str = wxString::Format(_T("Geometry %ld of %ld. Point %ld of %ld, coordinate (%.2f, %.2f)"),
      (long)m_curr_geom, (long)size_geom, (long)m_curr_point, (long)size_points,
      polygon.m_x.at(m_curr_point), polygon.m_y.at(m_curr_point));
    wxFrameMain *main = (wxFrameMain*)wxGetApp().GetTopWindow();
    main->SetStatusText(str);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxTreeCtrlExplorer::wxTreeCtrlExplorer
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxTreeCtrlExplorer, wxTreeCtrl)
EVT_TREE_SEL_CHANGED(wxID_ANY, wxTreeCtrlExplorer::OnSelChanged)
wxEND_EVENT_TABLE()

wxTreeCtrlExplorer::wxTreeCtrlExplorer(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
  : wxTreeCtrl(parent, id, pos, size, style)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxTreeCtrlExplorer::OnSelChanged
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxTreeCtrlExplorer::OnSelChanged(wxTreeEvent& event)
{
  event.Skip();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxTreeCtrlExplorer::select_item
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxTreeCtrlExplorer::select_item(size_t curr_geom)
{
  //find the tree item with data of geometry index
  wxTreeItemId root = GetRootItem();
  if (!root.IsOk())
    return;
  wxTreeItemId found = find_item(root, curr_geom);
  if (found.IsOk())
  {
    SelectItem(found);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxTreeCtrlExplorer::find_item
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxTreeItemId wxTreeCtrlExplorer::find_item(wxTreeItemId parent, size_t curr_geom)
{
  wxTreeItemIdValue cookie;
  wxTreeItemId child = GetFirstChild(parent, cookie);
  while (child.IsOk())
  {
    ItemData* data = (ItemData*)GetItemData(child);
    if (data && data->m_curr_geom == curr_geom)
    {
      return child;
    }
    if (ItemHasChildren(child))
    {
      wxTreeItemId found = find_item(child, curr_geom);
      if (found.IsOk())
        return found;
    }
    child = GetNextChild(parent, cookie);
  }
  return wxTreeItemId();
}
