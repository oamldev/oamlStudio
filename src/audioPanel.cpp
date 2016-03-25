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


AudioPanel::AudioPanel(wxFrame* parent, int index, std::string name) : wxPanel(parent) {
	wxString texts[4] = { "Intro", "Main loop", "Conditional", "Ending" };

	panelIndex = index;
	trackName = name;

	sizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, texts[index], wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	staticText->SetBackgroundColour(wxColour(0xD0, 0xD0, 0xD0));
	sizer->Add(staticText, 0, wxALL | wxEXPAND | wxGROW, 5);
	SetSizer(sizer);

	Bind(wxEVT_PAINT, &AudioPanel::OnPaint, this);
	Bind(wxEVT_RIGHT_UP, &AudioPanel::OnRightUp, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &AudioPanel::OnMenuEvent, this, ID_AddAudio);
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
	for (int i=0; i<audio->layers.size(); i++) {
		oamlLayerInfo *layer = &audio->layers[i];
		LayerPanel *lp = new LayerPanel((wxFrame*)this);
		layerPanels.push_back(lp);
		lp->AddWaveform(layer->filename, topWnd);

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

void AudioPanel::AddAudioDialog() {
	wxFileDialog openFileDialog(this, _("Open audio file"), ".", "", "Audio files (*.wav;*.aif;*.ogg)|*.aif;*.aiff;*.wav;*.wave;*.ogg", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	oamlAudioInfo audio;
	memset(&audio, 0, sizeof(oamlAudioInfo));
	wxFileName filename(openFileDialog.GetPath());
	wxFileName defsPath(oaml->GetDefsFile());
	filename.MakeRelativeTo(wxString(defsPath.GetPath()));
	std::string fname = filename.GetFullPath().ToStdString();

	oamlLayerInfo layer;
	layer.filename = fname;
	audio.layers.push_back(layer);
	switch (panelIndex) {
		case 0: audio.type = 1; break;
		case 1: audio.type = 2; break;
		case 2: audio.type = 4; break;
		case 3: audio.type = 3; break;
	}

	AddAudioInfo(trackName, audio);

	wxCommandEvent event(EVENT_ADD_AUDIO);
	event.SetString(fname);
	wxPostEvent(GetParent(), event);
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

