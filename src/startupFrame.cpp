//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Marcelo Fernandez
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wx/wx.h>
#include <wx/button.h>
#include <wx/config.h>
#include <wx/filehistory.h>
#include <wx/listctrl.h>
#include <wx/popupwin.h>
#include <wx/stattext.h>

#include "oamlCommon.h"


StartupFrame::StartupFrame(wxWindow *parent) : wxFrame(parent, wxID_ANY, wxT("Startup"), wxDefaultPosition, wxSize(640, 480), wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxRESIZE_BORDER | wxSTAY_ON_TOP) {
	config = new wxConfig("oamlStudio");

	fileHistory = new wxFileHistory();
	fileHistory->Load(*config);

	mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *text = new wxStaticText(this, wxID_ANY, wxString("Welcome to oamlStudio!\n\nHere you can load one of the latest projects used or start a new project from scratch."), wxDefaultPosition, wxSize(-1, -1), wxALIGN_CENTRE_HORIZONTAL);
	mainSizer->Add(text, 0, wxEXPAND | wxALL, 15);

	wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	prjList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLC_LIST | wxLC_SINGLE_SEL);
	prjList->SetBackgroundColour(wxColour(0xA0, 0xA0, 0xA0));
	prjList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &StartupFrame::OnPrjListActivated, this);
	for (size_t i=0; i<fileHistory->GetCount(); i++) {
		prjList->InsertItem(0, fileHistory->GetHistoryFile(i));
	}

	sizer->Add(prjList, 1, wxEXPAND | wxGROW | wxALL, 15);
	mainSizer->Add(sizer, 1, wxEXPAND | wxGROW | wxALL, 0);

	sizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *newBtn = new wxButton(this, wxID_ANY, wxString("&New project"));
	newBtn->Bind(wxEVT_BUTTON, &StartupFrame::OnNewProject, this);
	sizer->Add(newBtn, 1, wxALL, 5);

	wxButton *loadBtn = new wxButton(this, wxID_ANY, wxString("&Load project"));
	loadBtn->Bind(wxEVT_BUTTON, &StartupFrame::OnLoadProject, this);
	sizer->Add(loadBtn, 1, wxALL, 5);

	wxButton *loadOtherBtn = new wxButton(this, wxID_ANY, wxString("Load &other..."));
	loadOtherBtn->Bind(wxEVT_BUTTON, &StartupFrame::OnLoadOther, this);
	sizer->Add(loadOtherBtn, 1, wxALL, 5);

	wxButton *exitBtn = new wxButton(this, wxID_ANY, wxString("&Exit"));
	exitBtn->Bind(wxEVT_BUTTON, &StartupFrame::OnExit, this);
	sizer->Add(exitBtn, 1, wxALL, 5);

	mainSizer->Add(sizer, 0, wxALIGN_CENTER | wxALL, 10);

	SetSizer(mainSizer);

	Layout();

	Centre(wxBOTH);
	SetFocus();
}

void StartupFrame::OnPrjListActivated(wxListEvent& event) {
	int index = event.GetIndex();
	if (index == -1) {
//		WxUtils::ShowErrorDialog(_("You must choose a track!"));
		return;
	}

	wxString str = prjList->GetItemText(index);

	wxCommandEvent event2(EVENT_LOAD_PROJECT);
	event2.SetString(str);
	wxPostEvent(GetParent(), event2);

	Show(false);
}

void StartupFrame::OnNewProject(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent event(EVENT_NEW_PROJECT);
	wxPostEvent(GetParent(), event);

	Show(false);
}

void StartupFrame::OnLoadProject(wxCommandEvent& WXUNUSED(event)) {
	long index = prjList->GetFirstSelected();
	if (index == -1) {
//		WxUtils::ShowErrorDialog(_("You must choose a track!"));
		return;
	}

	wxString str = prjList->GetItemText(index);

	wxCommandEvent event(EVENT_LOAD_PROJECT);
	event.SetString(str);
	wxPostEvent(GetParent(), event);

	Show(false);
}

void StartupFrame::OnLoadOther(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent event(EVENT_LOAD_OTHER);
	wxPostEvent(GetParent(), event);

	Show(false);
}

void StartupFrame::OnExit(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent event(EVENT_QUIT);
	wxPostEvent(GetParent(), event);

	Show(false);
}
