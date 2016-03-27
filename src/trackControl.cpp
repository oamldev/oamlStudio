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
	int ctrlWidth = 110;
	int ctrlHeight = -1;

	trackName = "";

	mSizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);

	mSizer->Add(hSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL);

	sizer = new wxGridSizer(2, 0, 0);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("Volume"));
	sizer->Add(staticText, 0, wxALL, 5);

	volumeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
	volumeCtrl->Bind(wxEVT_TEXT, &TrackControl::OnVolumeChange, this);
	sizer->Add(volumeCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Fade In"));
	sizer->Add(staticText, 0, wxALL, 5);

	fadeInCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
	fadeInCtrl->Bind(wxEVT_TEXT, &TrackControl::OnFadeInChange, this);
	sizer->Add(fadeInCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Fade Out"));
	sizer->Add(staticText, 0, wxALL, 5);

	fadeOutCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
	fadeOutCtrl->Bind(wxEVT_TEXT, &TrackControl::OnFadeOutChange, this);
	sizer->Add(fadeOutCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade In"));
	sizer->Add(staticText, 0, wxALL, 5);

	xfadeInCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
	xfadeInCtrl->Bind(wxEVT_TEXT, &TrackControl::OnXFadeInChange, this);
	sizer->Add(xfadeInCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Crossfade Out"));
	sizer->Add(staticText, 0, wxALL, 5);

	xfadeOutCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(ctrlWidth, ctrlHeight));
	xfadeOutCtrl->Bind(wxEVT_TEXT, &TrackControl::OnXFadeOutChange, this);
	sizer->Add(xfadeOutCtrl, 0, wxALL, 5);

	mSizer->Add(sizer);

	SetSizer(mSizer);
	SetMinSize(wxSize(-1, 240));

	Layout();
}

TrackControl::~TrackControl() {
}


void TrackControl::OnVolumeChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = volumeCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlTrackInfo *info = GetTrackInfo(trackName);
	if (info) {
		double d = 0;
		str.ToDouble(&d);
		info->volume = (float)d;
	}
}

void TrackControl::OnFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = fadeInCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlTrackInfo *info = GetTrackInfo(trackName);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->fadeIn = (int)l;
	}
}

void TrackControl::OnFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = fadeOutCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlTrackInfo *info = GetTrackInfo(trackName);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->fadeOut = (int)l;
	}
}

void TrackControl::OnXFadeInChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = xfadeInCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlTrackInfo *info = GetTrackInfo(trackName);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->xfadeIn = (int)l;
	}
}

void TrackControl::OnXFadeOutChange(wxCommandEvent& WXUNUSED(event)) {
	wxString str = xfadeOutCtrl->GetLineText(0);
	if (str.IsEmpty())
		return;

	oamlTrackInfo *info = GetTrackInfo(trackName);
	if (info) {
		long l = 0;
		str.ToLong(&l);
		info->xfadeOut = (int)l;
	}
}

void TrackControl::SetTrack(std::string name) {
	trackName = name;

	volumeCtrl->Clear();
	fadeInCtrl->Clear();
	fadeOutCtrl->Clear();
	xfadeInCtrl->Clear();
	xfadeOutCtrl->Clear();

	oamlTrackInfo *info = GetTrackInfo(trackName);
	if (info) {
		*volumeCtrl << info->volume;
		*fadeInCtrl << info->fadeIn;
		*fadeOutCtrl << info->fadeOut;
		*xfadeInCtrl << info->xfadeIn;
		*xfadeOutCtrl << info->xfadeOut;
	}
}

void TrackControl::UpdateTrackName(std::string oldName, std::string newName) {
	if (trackName.compare(oldName) != 0)
		return;

	trackName = newName;
}
