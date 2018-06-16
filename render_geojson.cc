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
  : wxFrame(NULL, wxID_ANY, "GeoJSON reader")
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
  wxMessageBox(str, "GeoJSON reader", wxOK, this);
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
      wxT("geojson (*.geojson)|*.geojson|All files (%s)|%s"),
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
  m_win_chart = m_splitter->GetWindow2();
  wxChart *chart = new wxChart(m_splitter);
  if (chart->read_file(file_name) < 0)
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
  wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxDOUBLE_BORDER)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::read_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

int wxChart::read_file(const std::string &file_name)
{

  return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnDraw
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnDraw(wxDC& dc)
{
  dc.DrawRectangle(0, 0, 200, 200);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnMouseDown
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseDown(wxMouseEvent &event)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnMouseMove
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseMove(wxMouseEvent &event)
{

}

