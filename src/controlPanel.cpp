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

		fadeInCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		fadeInCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnFadeInChange, this);
		sizer->Add(fadeInCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Fade Out"));
		sizer->Add(staticText, 0, wxALL, 5);

		fadeOutCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		fadeOutCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnFadeOutChange, this);
		sizer->Add(fadeOutCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade In"));
		sizer->Add(staticText, 0, wxALL, 5);

		xfadeInCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		xfadeInCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnXFadeInChange, this);
		sizer->Add(xfadeInCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade Out"));
		sizer->Add(staticText, 0, wxALL, 5);

		xfadeOutCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		xfadeOutCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnXFadeOutChange, this);
		sizer->Add(xfadeOutCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Id"));
		sizer->Add(staticText, 0, wxALL, 5);

		condIdCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
		condIdCtrl->Bind(wxEVT_TEXT, &ControlPanel::OnCondIdChange, this);
		sizer->Add(condIdCtrl, 0, wxALL, 5);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Type"));
		sizer->Add(staticText, 0, wxALL, 5);

		condTypeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
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
	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		info->volume = (float)volumeCtrl->GetValue();
	}
}

void ControlPanel::OnBpmChange(wxCommandEvent& WXUNUSED(event)) {
	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		info->bpm = (float)bpmCtrl->GetValue();
	}
}

void ControlPanel::OnBpbChange(wxCommandEvent& WXUNUSED(event)) {
	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		info->beatsPerBar = (int)bpbCtrl->GetValue();
	}
}

void ControlPanel::OnBarsChange(wxCommandEvent& WXUNUSED(event)) {
	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		info->bars = (int)barsCtrl->GetValue();
	}
}

void ControlPanel::OnRandomChanceChange(wxCommandEvent& WXUNUSED(event)) {
	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		info->randomChance = (int)randomChanceCtrl->GetValue();
	}
}

void ControlPanel::OnMinMovementBarsChange(wxCommandEvent& WXUNUSED(event)) {
	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		info->minMovementBars = (int)minMovementBarsCtrl->GetValue();
	}
}

void ControlPanel::OnFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = fadeInCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->fadeIn = (int)l;
	}
}

void ControlPanel::OnFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = fadeOutCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->fadeOut = (int)l;
	}
}

void ControlPanel::OnXFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = xfadeInCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->xfadeIn = (int)l;
	}
}

void ControlPanel::OnXFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = xfadeOutCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->xfadeOut = (int)l;
	}
}

void ControlPanel::OnCondIdChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = condIdCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->condId = (int)l;
	}
}

void ControlPanel::OnCondTypeChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = condTypeCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->condType = (int)l;
	}
}

void ControlPanel::OnCondValueChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = condValueCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->condValue = (int)l;
	}
}

void ControlPanel::OnCondValue2Change(wxCommandEvent& WXUNUSED(event)) {
	wxString str = condValue2Ctrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->condValue2 = (int)l;
	}
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
		fadeInCtrl->Clear();
		fadeOutCtrl->Clear();
		xfadeInCtrl->Clear();
		xfadeOutCtrl->Clear();
		condIdCtrl->Clear();
		condTypeCtrl->Clear();
		condValueCtrl->Clear();
		condValue2Ctrl->Clear();
	}

	oamlAudioInfo *info = GetAudioInfo(trackName, audioFile);
	oamlLayerInfo *layer = GetLayerInfo(trackName, audioFile);
	if (info && layer) {
		*fileCtrl << layer->filename;
		volumeCtrl->SetValue(info->volume);

		if (musicMode) {
			bpmCtrl->SetValue(info->bpm);
			bpbCtrl->SetValue(info->beatsPerBar);
			barsCtrl->SetValue(info->bars);
			randomChanceCtrl->SetValue(info->randomChance);
			minMovementBarsCtrl->SetValue(info->minMovementBars);
			*fadeInCtrl << info->fadeIn;
			*fadeOutCtrl << info->fadeOut;
			*xfadeInCtrl << info->xfadeIn;
			*xfadeOutCtrl << info->xfadeOut;
			*condIdCtrl << info->condId;
			*condTypeCtrl << info->condType;
			*condValueCtrl << info->condValue;
			*condValue2Ctrl << info->condValue2;
		}

		enable = true;
	} else {
		volumeCtrl->SetValue(0.0);

		if (musicMode) {
			bpmCtrl->SetValue(0.0);
			bpbCtrl->SetValue(0.0);
			barsCtrl->SetValue(0.0);
			randomChanceCtrl->SetValue(0.0);
			minMovementBarsCtrl->SetValue(0.0);
		}

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
