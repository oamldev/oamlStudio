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
#include "tinyxml2.h"

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
#include <archive.h>
#include <archive_entry.h>


TrackPanel::TrackPanel(wxWindow* parent, wxWindowID id, std::string name) : wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL) {
	trackName = name;
	musicMode = true;
	panelCount = 3;

	SetBackgroundColour(wxColour(0x40, 0x40, 0x40));
	SetScrollRate(50, 50);

	sizer = new wxBoxSizer(wxHORIZONTAL);

	SetSizer(sizer);
	Layout();

	sizer->Fit(this);

	Bind(EVENT_UPDATE_LAYOUT, &TrackPanel::UpdateLayout, this);
}

void TrackPanel::UpdateLayout(wxCommandEvent& event) {
	SetSizer(sizer);
	Layout();
	sizer->Fit(this);

	wxPostEvent(GetParent(), event);
}

void TrackPanel::SetTrackMode(bool mode) {
	wxString musicTexts[3] = { "Intros", "Main loops", "Conditional loops" };
	wxString sfxTexts[1] = { "Sfxs" };

	musicMode = mode;
	panelCount = musicMode ? 3 : 1;

	for (int i=0; i<panelCount; i++) {
		audioPanel[i] = new AudioPanel((wxFrame*)this, i, trackName, musicMode ? musicTexts[i] : sfxTexts[i], !musicMode);

		sizer->Add(audioPanel[i], 0, wxALL | wxEXPAND | wxGROW, 0);
	}

	Layout();
}

int TrackPanel::GetPanelIndex(std::string audioFile) {
	if (musicMode == false) {
		return 0;
	}

	int type = studioApi->AudioGetType(trackName, audioFile);
	int i = 1;
	if (type == 1) {
		i = 0;
	} else if (type == 4) {
		i = 2;
	}
	return i;
}

void TrackPanel::AddAudio(std::string audioFile) {
	int i = GetPanelIndex(audioFile);
	audioPanel[i]->AddAudio(audioFile);

	SetSizer(sizer);
	Layout();
	sizer->Fit(this);
}

void TrackPanel::RemoveAudio(std::string audioFile) {
	int i = GetPanelIndex(audioFile);
	audioPanel[i]->RemoveAudio(audioFile);

	SetSizer(sizer);
	Layout();
	sizer->Fit(this);
}

void TrackPanel::UpdateTrackName(std::string oldName, std::string newName) {
	if (trackName.compare(oldName) != 0)
		return;

	trackName = newName;

	for (int i=0; i<panelCount; i++) {
		audioPanel[i]->UpdateTrackName(trackName);
	}
}

void TrackPanel::UpdateAudioName(std::string oldName, std::string newName) {
	for (int i=0; i<panelCount; i++) {
		audioPanel[i]->UpdateAudioName(oldName, newName);
	}
}
