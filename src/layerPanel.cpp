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

#include "oamlCommon.h"

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/scrolbar.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/filename.h>
#include <wx/filehistory.h>
#include <wx/config.h>


LayerPanel::LayerPanel(wxFrame* parent) : wxPanel(parent) {
	waveformHeight = 104;

	sizer = new wxBoxSizer(wxHORIZONTAL);

	vSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("-- Layers --"));
	vSizer->Add(staticText, 0, wxALL, 5);

	wxSize s = staticText->GetClientSize();
	vSizer->Add(0, waveformHeight/2 - s.GetHeight());

	sizer->Add(vSizer);

	wxStaticLine *staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	sizer->Add(staticLine, 0, wxEXPAND | wxALL, 0);

	Bind(wxEVT_RIGHT_UP, &LayerPanel::OnRightUp, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &LayerPanel::OnMenuEvent, this);

	SetSizer(sizer);
	Layout();
}

void LayerPanel::AddLayer(std::string name) {
	wxTextCtrl *textCtrl = new wxTextCtrl(this, wxID_ANY, name, wxDefaultPosition, wxSize(80, -1));
	layerData *data = new layerData();

	textCtrl->SetClientData(data);
	textCtrl->Bind(wxEVT_TEXT, &LayerPanel::OnLayerNameChange, this, wxID_ANY, wxID_ANY);
	vSizer->Add(textCtrl, 0, wxALL, 5);

	wxSize s = textCtrl->GetClientSize();
	vSizer->Add(0, waveformHeight - s.GetHeight());

	data->nameCtrl = textCtrl;
	data->name = name;
	data->id = studioApi->LayerGetId(name);

	layers.push_back(data);

	Layout();
}

void LayerPanel::LoadLayers() {
	std::vector<std::string> list;

	studioApi->LayerList(list);
	if (list.size() == 0) {
		AddNewLayer();
	} else {
		for (std::vector<std::string>::iterator it=list.begin(); it<list.end(); ++it) {
			std::string name = *it;
			AddLayer(name);
		}
	}

	Layout();
}

void LayerPanel::OnLayerNameChange(wxCommandEvent& event) {
	wxTextCtrl *textCtrl = (wxTextCtrl*)event.GetEventObject();
	layerData *data = (layerData*)textCtrl->GetClientData();

	wxString str = textCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	studioApi->LayerRename(data->name, str.ToStdString());

	// Mark the project dirty
	wxCommandEvent event2(EVENT_SET_PROJECT_DIRTY);
	wxPostEvent(GetParent(), event2);
}

void LayerPanel::AddNewLayer() {
	std::vector<std::string> list;

	studioApi->LayerList(list);

	char str[256];
	snprintf(str, 256, "Layer%d", int(list.size() + 1));
	std::string name = str;
	studioApi->LayerNew(name);
	AddLayer(name);
}

void LayerPanel::OnMenuEvent(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_AddLayer:
			AddNewLayer();
			break;
	}
}

void LayerPanel::OnRightUp(wxMouseEvent& WXUNUSED(event)) {
	wxMenu menu(wxT(""));
	menu.Append(ID_AddLayer, wxT("&Add Layer"));
	PopupMenu(&menu);
}
