#ifndef __OAMLSTUDIO_H__
#define __OAMLSTUDIO_H__

#include <wx/wx.h>

class oamlStudio : public wxApp {
public:
	virtual bool OnInit();
};

class Listbox : public wxFrame
{
public:
	Listbox(const wxString& title);

	void OnDblClick(wxCommandEvent& event);

	wxListBox *listbox;
//	MyPanel *btnPanel;
};

#endif /* __OAMLSTUDIO_H__ */
