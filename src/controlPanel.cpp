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
#include <wx/spinctrl.h>
#include <archive.h>
#include <archive_entry.h>


ControlPanel::ControlPanel(wxFrame* parent, wxWindowID id) : wxPanel(parent, id) {
	trackName = "";
	audioFile = "";
	musicMode = true;

	mSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("-- Audio Controls --"));

	hSizer = new wxBoxSizer(wxHORIZONTAL);

	hSizer->Add(staticText, 1, wxEXPAND | wxALL, 0);

	mSizer->Add(hSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL);

	hSizer = new wxBoxSizer(wxHORIZONTAL);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Filename"));
	hSizer->Add(staticText, 0, wxALL, 5);

	fileCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(320, -1), wxTE_READONLY);
	hSizer->Add(fileCtrl, 0, wxALL, 5);

	mSizer->Add(hSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL);

	sizer = new wxGridSizer(4, 0, 0);

	mSizer->Add(sizer, 1, wxEXPAND | wxALL, 0);

	SetSizer(mSizer);
	SetMinSize(wxSize(-1, 240));

	Layout();
}

ControlPanel::~ControlPanel() {
}

void ControlPanel::SetTrackMode(bool mode) {
	int ctrlWidth = 160;
	int ctrlHeight = -1;

	musicMode = mode;

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("Volume"));
	sizer->Add(staticText, 0, wxALL, 5);

	volumeCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	volumeCtrl->SetRange(0.0, 1.0);
	volumeCtrl->SetIncrement(0.1);
	volumeCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnVolumeChange, this);
	sizer->Add(volumeCtrl, 0, wxALL, 5);

	if (musicMode) {
		staticText = new wxStaticText(this, wxID_ANY, wxString("Bpm"));
		sizer->Add(staticText, 0, wxALL, 5);

		bpmCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		bpmCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnBpmChange, this);
		sizer->Add(bpmCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Beats Per Bar"));
		sizer->Add(staticText, 0, wxALL, 5);

		bpbCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		bpbCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnBpbChange, this);
		sizer->Add(bpbCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Bars"));
		sizer->Add(staticText, 0, wxALL, 5);

		barsCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		barsCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnBarsChange, this);
		sizer->Add(barsCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Random Chance"));
		sizer->Add(staticText, 0, wxALL, 5);

		randomChanceCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		randomChanceCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnRandomChanceChange, this);
		sizer->Add(randomChanceCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Min movement bars"));
		sizer->Add(staticText, 0, wxALL, 5);

		minMovementBarsCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		minMovementBarsCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnMinMovementBarsChange, this);
		sizer->Add(minMovementBarsCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Fade In"));
		sizer->Add(staticText, 0, wxALL, 5);

		fadeInCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		fadeInCtrl->SetRange(0, 1000000);
		fadeInCtrl->SetIncrement(100);
		fadeInCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnFadeInChange, this);
		sizer->Add(fadeInCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Fade Out"));
		sizer->Add(staticText, 0, wxALL, 5);

		fadeOutCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		fadeOutCtrl->SetRange(0, 1000000);
		fadeOutCtrl->SetIncrement(100);
		fadeOutCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnFadeOutChange, this);
		sizer->Add(fadeOutCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade In"));
		sizer->Add(staticText, 0, wxALL, 5);

		xfadeInCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		xfadeInCtrl->SetRange(0, 1000000);
		xfadeInCtrl->SetIncrement(100);
		xfadeInCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnXFadeInChange, this);
		sizer->Add(xfadeInCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade Out"));
		sizer->Add(staticText, 0, wxALL, 5);

		xfadeOutCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
		xfadeOutCtrl->SetRange(0, 1000000);
		xfadeOutCtrl->SetIncrement(100);
		xfadeOutCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &ControlPanel::OnXFadeOutChange, this);
		sizer->Add(xfadeOutCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Id"));
		sizer->Add(staticText, 0, wxALL, 5);

		condIdCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		condIdCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnCondIdChange, this);
		sizer->Add(condIdCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Type"));
		sizer->Add(staticText, 0, wxALL, 5);

		wxString condTypeStrings[4] = { "Equal", "Greater", "Less", "Range" };
		condTypeCtrl = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight), 4, condTypeStrings, wxCB_READONLY);
		condTypeCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnCondTypeChange, this);
		sizer->Add(condTypeCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Value"));
		sizer->Add(staticText, 0, wxALL, 5);

		condValueCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		condValueCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnCondValueChange, this);
		sizer->Add(condValueCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Value2"));
		sizer->Add(staticText, 0, wxALL, 5);

		condValue2Ctrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		condValue2Ctrl->Bind(wxEVT_TEXT, &ControlPanel::OnCondValue2Change, this);
		sizer->Add(condValue2Ctrl, 0, wxALL, 5);
	}

	Layout();
}

void ControlPanel::OnVolumeChange(wxCommandEvent& WXUNUSED(event)) {
	float vol = (float)volumeCtrl->GetValue();

	// Don't change the actual volume unless it's different
	if (studioApi->AudioGetVolume(trackName, audioFile) != vol) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetVolume(trackName, audioFile, vol);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnBpmChange(wxCommandEvent& WXUNUSED(event)) {
	float value = (float)bpmCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetBPM(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetBPM(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnBpbChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)bpbCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetBeatsPerBar(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetBeatsPerBar(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnBarsChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)barsCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetBars(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetBars(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnRandomChanceChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)randomChanceCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetRandomChance(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetRandomChance(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnMinMovementBarsChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)minMovementBarsCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetMinMovementBars(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetMinMovementBars(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)fadeInCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetFadeIn(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetFadeIn(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)fadeOutCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetFadeOut(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetFadeOut(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnXFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)xfadeInCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetXFadeIn(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetXFadeIn(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnXFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)xfadeOutCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->AudioGetXFadeOut(trackName, audioFile) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->AudioSetXFadeOut(trackName, audioFile, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void ControlPanel::OnCondIdChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = condIdCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	long l = 0;
	str.ToLong(&l);
	studioApi->AudioSetCondId(trackName, audioFile, (int)l);
}

void ControlPanel::OnCondTypeChange(wxCommandEvent& WXUNUSED(event)) {
	studioApi->AudioSetCondType(trackName, audioFile, condTypeCtrl->GetCurrentSelection());
}

void ControlPanel::OnCondValueChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = condValueCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	long l = 0;
	str.ToLong(&l);
	studioApi->AudioSetCondValue(trackName, audioFile, (int)l);
}

void ControlPanel::OnCondValue2Change(wxCommandEvent& WXUNUSED(event)) {
	wxString str = condValue2Ctrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	long l = 0;
	str.ToLong(&l);
	studioApi->AudioSetCondValue2(trackName, audioFile, (int)l);
}

void ControlPanel::SetTrack(std::string name) {
	trackName = name;
}

void ControlPanel::OnSelectAudio(std::string audio) {
	bool enable;

	audioFile = audio;
	size_t pos = audioFile.find_last_of(PATH_SEPARATOR);
	if (pos != std::string::npos) {
		selectedAudioName = audioFile.substr(pos+1);
		size_t pos = selectedAudioName.find_last_of('.');
		if (pos != std::string::npos) {
			selectedAudioName = selectedAudioName.substr(0, pos);
		}
	}

	fileCtrl->Clear();
	if (musicMode) {
		condIdCtrl->Clear();
		condValueCtrl->Clear();
		condValue2Ctrl->Clear();
	}

	volumeCtrl->SetValue(studioApi->AudioGetVolume(trackName, audioFile));

	if (musicMode) {
		bpmCtrl->SetValue(studioApi->AudioGetBPM(trackName, audioFile));
		bpbCtrl->SetValue(studioApi->AudioGetBeatsPerBar(trackName, audioFile));
		barsCtrl->SetValue(studioApi->AudioGetBars(trackName, audioFile));
		randomChanceCtrl->SetValue(studioApi->AudioGetRandomChance(trackName, audioFile));
		minMovementBarsCtrl->SetValue(studioApi->AudioGetMinMovementBars(trackName, audioFile));
		fadeInCtrl->SetValue(studioApi->AudioGetFadeIn(trackName, audioFile));
		fadeOutCtrl->SetValue(studioApi->AudioGetFadeOut(trackName, audioFile));
		xfadeInCtrl->SetValue(studioApi->AudioGetXFadeIn(trackName, audioFile));
		xfadeOutCtrl->SetValue(studioApi->AudioGetXFadeOut(trackName, audioFile));
		*condIdCtrl << studioApi->AudioGetCondId(trackName, audioFile);
		condTypeCtrl->SetSelection(studioApi->AudioGetCondType(trackName, audioFile));
		*condValueCtrl << studioApi->AudioGetCondValue(trackName, audioFile);
		*condValue2Ctrl << studioApi->AudioGetCondValue2(trackName, audioFile);
	}

	if (studioApi->AudioExists(trackName, audioFile)) {
		*fileCtrl << audioFile;
		enable = true;
	} else {
		enable = false;
	}

	volumeCtrl->Enable(enable);

	if (musicMode) {
		bpmCtrl->Enable(enable);
		bpbCtrl->Enable(enable);
		barsCtrl->Enable(enable);
		randomChanceCtrl->Enable(enable);
		minMovementBarsCtrl->Enable(enable);
		fadeInCtrl->Enable(enable);
		fadeOutCtrl->Enable(enable);
		xfadeInCtrl->Enable(enable);
		xfadeOutCtrl->Enable(enable);
		condIdCtrl->Enable(enable);
		condTypeCtrl->Enable(enable);
		condValueCtrl->Enable(enable);
		condValue2Ctrl->Enable(enable);
	}
}

void ControlPanel::UpdateTrackName(std::string oldName, std::string newName) {
	if (trackName.compare(oldName) != 0)
		return;

	trackName = newName;
}
