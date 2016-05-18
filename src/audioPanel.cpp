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


AudioPanel::AudioPanel(wxFrame* parent, int index, std::string name, wxString labelStr, bool mode) : wxPanel(parent) {
	panelIndex = index;
	trackName = name;
	sfxMode = mode;

	sizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, labelStr, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	staticText->SetBackgroundColour(wxColour(0xD0, 0xD0, 0xD0));
	sizer->Add(staticText, 0, wxALL | wxEXPAND | wxGROW, 5);
	SetSizer(sizer);

	Bind(wxEVT_PAINT, &AudioPanel::OnPaint, this);
	Bind(wxEVT_RIGHT_UP, &AudioPanel::OnRightUp, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &AudioPanel::OnMenuEvent, this, ID_AddAudio);

	SetMinSize(wxSize(240, -1));
}

void AudioPanel::OnPaint(wxPaintEvent& WXUNUSED(evt)) {
	wxPaintDC dc(this);

	wxSize size = GetSize();
	int x2 = size.GetWidth();
	int y2 = size.GetHeight();

	dc.SetPen(wxPen(wxColour(0, 0, 0), 4));
	dc.DrawLine(0,  0,  0,  y2);
	dc.DrawLine(x2, 0,  x2, y2);
	dc.DrawLine(0,  0,  x2, 0);
	dc.DrawLine(0,  y2, x2, y2);
}

void AudioPanel::AddAudio(oamlAudioInfo *audio, wxFrame *topWnd) {
	for (std::vector<oamlLayerInfo>::iterator layer=audio->layers.begin(); layer<audio->layers.end(); ++layer) {
		LayerPanel *lp = new LayerPanel((wxFrame*)this);
		layerPanels.push_back(lp);
		lp->AddWaveform(layer->filename, sfxMode, topWnd);

		sizer->Add(lp, 0, wxALL, 5);
	}

	Layout();
}

void AudioPanel::RemoveAudio(std::string filename) {
	for (std::vector<LayerPanel*>::iterator it=layerPanels.begin(); it<layerPanels.end(); ++it) {
		LayerPanel *lp = *it;
		lp->RemoveWaveform(filename);
		if (lp->IsEmpty()) {
			sizer->Detach((wxWindow*)lp);
			layerPanels.erase(it);
			delete lp;

			break;
		}
	}

	Layout();
}

void AudioPanel::AddAudio(wxString path) {
	wxFileName filename(path);
	wxFileName defsPath(oaml->GetDefsFile());
	filename.MakeRelativeTo(wxString(defsPath.GetPath()));
	std::string fname = filename.GetFullPath().ToStdString();

	int type = 2;
	switch (panelIndex) {
		case 0: type = 1; break;
		case 1: type = 2; break;
		case 2: type = 4; break;
	}
	studioApi->AudioNew(trackName, fname, type);

	wxCommandEvent event(EVENT_ADD_AUDIO);
	event.SetString(fname);
	wxPostEvent(GetParent(), event);
}

void AudioPanel::AddAudioDialog() {
	wxFileDialog openFileDialog(this, _("Open audio file"), ".", "", "Audio files (*.wav;*.aif;*.ogg)|*.aif;*.aiff;*.wav;*.wave;*.ogg", wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	wxArrayString paths;
	openFileDialog.GetPaths(paths);
	for (size_t i=0; i<paths.GetCount(); i++) {
		AddAudio(paths.Item(i));
	}
}

void AudioPanel::OnMenuEvent(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_AddAudio:
			AddAudioDialog();
			break;
	}
}

void AudioPanel::OnRightUp(wxMouseEvent& WXUNUSED(event)) {
	wxMenu menu(wxT(""));
	menu.Append(ID_AddAudio, wxT("&Add Audio"));
	PopupMenu(&menu);
}

void AudioPanel::UpdateTrackName(std::string newName) {
	trackName = newName;
}

