
#include "wx/wxprec.h"
#include "wx/wx.h"
#include "sample.xpm"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppMinimal
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxAppMinimal : public wxApp
{
public:
  virtual bool OnInit() wxOVERRIDE;
};

wxIMPLEMENT_APP(wxAppMinimal);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMinimal
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxFrameMinimal : public wxFrame
{
public:
  wxFrameMinimal(const wxString& title);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
private:
  wxDECLARE_EVENT_TABLE();
};

enum
{
  Minimal_Quit = wxID_EXIT,
  Minimal_About = wxID_ABOUT
};

wxBEGIN_EVENT_TABLE(wxFrameMinimal, wxFrame)
EVT_MENU(Minimal_Quit, wxFrameMinimal::OnQuit)
EVT_MENU(Minimal_About, wxFrameMinimal::OnAbout)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppMinimal::OnInit()
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool wxAppMinimal::OnInit()
{
  if (!wxApp::OnInit())
  {
    return false;
  }
  wxFrameMinimal *frame = new wxFrameMinimal("Minimal");
  frame->Show(true);
  frame->Maximize();
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMinimal::wxFrameMinimal
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxFrameMinimal::wxFrameMinimal(const wxString& title)
  : wxFrame(NULL, wxID_ANY, title)
{
  SetIcon(wxICON(sample));
  wxMenu *menu_file = new wxMenu;
  menu_file->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");
  wxMenu *menu_help = new wxMenu;
  menu_help->Append(Minimal_About, "&About\tF1", "Show about dialog");
  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
  CreateStatusBar(2);
  SetStatusText("Ready");

  wxPanel *win_input = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxDOUBLE_BORDER);
  win_input->SetMinSize(wxSize(600, -1));

  wxWindow *win_grid = new wxWindow(this, wxID_ANY);
  win_grid->SetMinSize(wxSize(200, -1));

  wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(win_input, 0, wxGROW);
  sizer->Add(win_grid, 1, wxGROW);
  SetSizerAndFit(sizer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMinimal::OnQuit
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMinimal::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMinimal::OnAbout
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMinimal::OnAbout(wxCommandEvent& WXUNUSED(event))
{
}
