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


AudioFilePanel::AudioFilePanel(std::string _trackName, std::string _audioName, wxFrame* parent) : wxPanel(parent) {
	trackName = _trackName;
	audioName = _audioName;

	sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	Bind(wxEVT_PAINT, &AudioFilePanel::OnPaint, this);
	Bind(wxEVT_RIGHT_UP, &AudioFilePanel::OnRightUp, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &AudioFilePanel::OnMenuEvent, this);
	Bind(EVENT_REMOVE_AUDIO_FILE, &AudioFilePanel::OnRemoveAudioFile, this);
}

void AudioFilePanel::AddWaveform(std::string filename, std::string audioName, bool sfxMode) {
	WaveformDisplay *waveDisplay = new WaveformDisplay((wxFrame*)this);
	waveDisplay->SetSource(filename, audioName, sfxMode);

	waveDisplays.push_back(waveDisplay);

	sizer->Add(waveDisplay, 0, wxALL, 4);
	Layout();
}

void AudioFilePanel::RemoveWaveform(std::string filename) {
	for (std::vector<WaveformDisplay*>::iterator it=waveDisplays.begin(); it<waveDisplays.end(); ++it) {
		WaveformDisplay *waveDisplay = *it;
		if (waveDisplay->GetFilename().compare(filename) == 0) {
			sizer->Detach((wxWindow*)waveDisplay);
			waveDisplays.erase(it);
			delete waveDisplay;

			studioApi->AudioFileRemove(trackName, audioName, filename);

			// Mark the project dirty
			wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
			wxPostEvent(GetParent(), event);
		}
	}

	if (waveDisplays.size() == 0) {
		// No waveform left on the panel, remove us
		studioApi->AudioRemove(trackName, audioName);

		wxCommandEvent event(EVENT_SELECT_AUDIO);
		event.SetString(wxString(""));
		wxPostEvent(GetParent(), event);

		Destroy();
		return;
	}

	Layout();
}

void AudioFilePanel::UpdateAudioName(std::string oldName, std::string newName) {
	for (std::vector<WaveformDisplay*>::iterator it=waveDisplays.begin(); it<waveDisplays.end(); ++it) {
		WaveformDisplay *waveDisplay = *it;
		if (waveDisplay->GetAudioName() == oldName) {
			waveDisplay->SetAudioName(newName);
		}
	}
}

bool AudioFilePanel::IsEmpty() {
	return waveDisplays.size() == 0;
}

void AudioFilePanel::OnPaint(wxPaintEvent& WXUNUSED(evt)) {
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

void AudioFilePanel::AddAudioFilePath(wxString path) {
/*	wxFileName filename(path);

	filename.MakeRelativeTo(wxString(projectPath));
	std::string fname = filename.GetFullPath().ToStdString();

	int type = 2;
	switch (panelIndex) {
		case 0: type = 1; break;
		case 1: type = 2; break;
		case 2: type = 4; break;
	}

	std::string name;
	for (int i=0; i<1000; i++) {
		char str[1024];
		snprintf(str, 1024, "audio%d", i);
		name = str;
		if (studioApi->AudioExists(trackName, name) == false) {
			break;
		}
	}

	studioApi->AudioNew(trackName, name, type);
	studioApi->AudioAddAudioFile(trackName, name, fname);

	AddAudio(name);

	// Mark the project dirty
	wxCommandEvent event2(EVENT_SET_PROJECT_DIRTY);
	wxPostEvent(GetParent(), event2);*/
}

void AudioFilePanel::AddAudioFileDialog() {
/*	wxFileDialog openFileDialog(this, _("Open audio file"), wxEmptyString, "", "Audio files (*.wav;*.aif;*.ogg)|*.aif;*.aiff;*.wav;*.wave;*.ogg", wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	wxArrayString paths;
	openFileDialog.GetPaths(paths);
	for (size_t i=0; i<paths.GetCount(); i++) {
		AddAudioFilePath(paths.Item(i));
	}*/
}

void AudioFilePanel::OnRemoveAudioFile(wxCommandEvent& event) {
	RemoveWaveform(event.GetString().ToStdString());
}

void AudioFilePanel::OnMenuEvent(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_AddAudioFile:
			AddAudioFileDialog();
			break;
	}
}

void AudioFilePanel::OnRightUp(wxMouseEvent& WXUNUSED(event)) {
/*	wxMenu menu(wxT(""));
	menu.Append(ID_AddAudioFile, wxT("&Add AudioFile"));
	PopupMenu(&menu);*/
}

