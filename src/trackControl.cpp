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
#include <archive.h>
#include <archive_entry.h>


TrackControl::TrackControl(wxFrame* parent, wxWindowID id) : wxPanel(parent, id) {
	trackName = "";

	mSizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("-- Track Controls --"));
	hSizer->Add(staticText, 0, wxEXPAND | wxALL, 5);

	mSizer->Add(hSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL);

	sizer = new wxGridSizer(2, 0, 0);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Volume"));
	sizer->Add(staticText, 0, wxALL, 5);

	volumeCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	volumeCtrl->SetRange(0.0, 1.0);
	volumeCtrl->SetIncrement(0.1);
	volumeCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &TrackControl::OnVolumeChange, this);
	sizer->Add(volumeCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Fade In"));
	sizer->Add(staticText, 0, wxALL, 5);

	fadeInCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	fadeInCtrl->SetRange(0, 10000000);
	fadeInCtrl->SetIncrement(100);
	fadeInCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &TrackControl::OnFadeInChange, this);
	sizer->Add(fadeInCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Fade Out"));
	sizer->Add(staticText, 0, wxALL, 5);

	fadeOutCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	fadeOutCtrl->SetRange(0, 10000000);
	fadeOutCtrl->SetIncrement(100);
	fadeOutCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &TrackControl::OnFadeOutChange, this);
	sizer->Add(fadeOutCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade In"));
	sizer->Add(staticText, 0, wxALL, 5);

	xfadeInCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	xfadeInCtrl->SetRange(0, 10000000);
	xfadeInCtrl->SetIncrement(100);
	xfadeInCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &TrackControl::OnXFadeInChange, this);
	sizer->Add(xfadeInCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade Out"));
	sizer->Add(staticText, 0, wxALL, 5);

	xfadeOutCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	xfadeOutCtrl->SetRange(0, 10000000);
	xfadeOutCtrl->SetIncrement(100);
	xfadeOutCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &TrackControl::OnXFadeOutChange, this);
	sizer->Add(xfadeOutCtrl, 0, wxALL, 5);

	mSizer->Add(sizer);

	SetSizer(mSizer);
	SetMinSize(wxSize(-1, 240));

	Layout();

	SetTrack("");
}

TrackControl::~TrackControl() {
}


void TrackControl::OnVolumeChange(wxCommandEvent& WXUNUSED(event)) {
	float vol = (float)volumeCtrl->GetValue();

	// Don't change the actual volume unless it's different
	if (studioApi->TrackGetVolume(trackName) != vol) {
		// Send the actual change to oaml through the studioApi
		studioApi->TrackSetVolume(trackName, vol);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void TrackControl::OnFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	int value = fadeInCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->TrackGetFadeIn(trackName) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->TrackSetFadeIn(trackName, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void TrackControl::OnFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	int value = fadeOutCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->TrackGetFadeOut(trackName) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->TrackSetFadeOut(trackName, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void TrackControl::OnXFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	int value = xfadeInCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->TrackGetXFadeIn(trackName) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->TrackSetXFadeIn(trackName, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void TrackControl::OnXFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	int value = xfadeOutCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->TrackGetXFadeOut(trackName) != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->TrackSetXFadeOut(trackName, value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void TrackControl::SetTrack(std::string name) {
	bool enable;

	trackName = name;

	oamlTrackInfo *info = GetTrackInfo(trackName);
	if (info) {
		volumeCtrl->SetValue(info->volume);
		fadeInCtrl->SetValue(info->fadeIn);
		fadeOutCtrl->SetValue(info->fadeOut);
		xfadeInCtrl->SetValue(info->xfadeIn);
		xfadeOutCtrl->SetValue(info->xfadeOut);

		enable = true;
	} else {
		volumeCtrl->SetValue(0.0);
		fadeInCtrl->SetValue(0);
		fadeOutCtrl->SetValue(0);
		xfadeInCtrl->SetValue(0);
		xfadeOutCtrl->SetValue(0);

		enable = false;
	}

	volumeCtrl->Enable(enable);
	fadeInCtrl->Enable(enable);
	fadeOutCtrl->Enable(enable);
	xfadeInCtrl->Enable(enable);
	xfadeOutCtrl->Enable(enable);
}

void TrackControl::UpdateTrackName(std::string oldName, std::string newName) {
	if (trackName.compare(oldName) != 0)
		return;

	trackName = newName;
}
