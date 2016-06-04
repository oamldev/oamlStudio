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
	sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	Bind(wxEVT_PAINT, &LayerPanel::OnPaint, this);
}

void LayerPanel::AddWaveform(std::string filename, std::string audioName, bool sfxMode, wxFrame *topWnd) {
	WaveformDisplay *waveDisplay = new WaveformDisplay((wxFrame*)this, topWnd);
	waveDisplay->SetSource(filename, audioName, sfxMode);

	waveDisplays.push_back(waveDisplay);

	sizer->Add(waveDisplay, 0, wxALL, 4);
	Layout();
}

void LayerPanel::RemoveWaveform(std::string filename) {
	for (std::vector<WaveformDisplay*>::iterator it=waveDisplays.begin(); it<waveDisplays.end(); ++it) {
		WaveformDisplay *waveDisplay = *it;
		if (waveDisplay->GetFilename().compare(filename) == 0) {
			sizer->Detach((wxWindow*)waveDisplay);
			waveDisplays.erase(it);
			delete waveDisplay;
		}
	}

	Layout();
}

void LayerPanel::UpdateAudioName(std::string oldName, std::string newName) {
	for (std::vector<WaveformDisplay*>::iterator it=waveDisplays.begin(); it<waveDisplays.end(); ++it) {
		WaveformDisplay *waveDisplay = *it;
		if (waveDisplay->GetAudioName() == oldName) {
			waveDisplay->SetAudioName(newName);
		}
	}
}

bool LayerPanel::IsEmpty() {
	return waveDisplays.size() == 0;
}

void LayerPanel::OnPaint(wxPaintEvent& WXUNUSED(evt)) {
	wxPaintDC dc(this);

	wxSize size = GetSize();
	int x2 = size.GetWidth();
	int y2 = size.GetHeight();

	dc.SetPen(wxPen(wxColour(0, 128, 128), 4));
	dc.DrawLine(0,  0,  0,  y2);
	dc.DrawLine(x2, 0,  x2, y2);
	dc.DrawLine(0,  0,  x2, 0);
	dc.DrawLine(0,  y2, x2, y2);
}

void LayerPanel::AddLayerDialog() {
	wxFileDialog openFileDialog(this, _("Open audio file"), wxEmptyString, "", "Audio files (*.wav;*.aif;*.ogg)|*.aif;*.aiff;*.wav;*.wave;*.ogg", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

/*	oamlAudioInfo audio;
	memset(&audio, 0, sizeof(oamlAudioInfo));
	wxFileName filename(openFileDialog.GetPath());
	wxFileName defsPath(oaml->GetDefsFile());
	filename.MakeRelativeTo(wxString(defsPath.GetPath()));
	std::string fname = filename.GetFullPath().ToStdString();*/

/*	oamlLayerInfo layer;
	layer.filename = fname;
	audio.layers.push_back(layer);
	switch (panelIndex) {
		case 0: audio.type = 1; break;
		case 1: audio.type = 2; break;
		case 2: audio.type = 4; break;
		case 3: audio.type = 3; break;
	}

	wxCommandEvent event(EVENT_ADD_AUDIO);
	event.SetString(fname);
	wxPostEvent(GetParent(), event);*/
}
