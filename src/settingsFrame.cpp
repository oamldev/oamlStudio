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


SettingsFrame::SettingsFrame(wxWindow *parent, wxWindowID id) : wxFrame(parent, id, _("Settings"), wxPoint(50, 50), wxSize(360, 180), wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxRESIZE_BORDER) {
	mSizer = new wxBoxSizer(wxVERTICAL);

	sizer = new wxGridSizer(2, 0, 0);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("Bpm"));
	sizer->Add(staticText, 0, wxALL, 5);

	bpmCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	bpmCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &SettingsFrame::OnBpmChange, this);
	sizer->Add(bpmCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Beats Per Bar"));
	sizer->Add(staticText, 0, wxALL, 5);

	bpbCtrl = new wxSpinCtrlDouble(this, wxID_ANY);
	bpbCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &SettingsFrame::OnBpbChange, this);
	sizer->Add(bpbCtrl, 0, wxALL, 5);

	mSizer->Add(sizer, 1, wxEXPAND | wxALL, 0);

	SetSizer(mSizer);
	Layout();
}

SettingsFrame::~SettingsFrame() {
}

void SettingsFrame::OnLoad() {
	bpmCtrl->SetValue(studioApi->ProjectGetBPM());
	bpbCtrl->SetValue(studioApi->ProjectGetBeatsPerBar());
}

void SettingsFrame::OnBpmChange(wxCommandEvent& WXUNUSED(event)) {
	float value = (float)bpmCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->ProjectGetBPM() != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->ProjectSetBPM(value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}

void SettingsFrame::OnBpbChange(wxCommandEvent& WXUNUSED(event)) {
	int value = (int)bpbCtrl->GetValue();

	// Don't change the actual value unless it's different
	if (studioApi->ProjectGetBeatsPerBar() != value) {
		// Send the actual change to oaml through the studioApi
		studioApi->ProjectSetBeatsPerBar(value);

		// Mark the project dirty
		wxCommandEvent event(EVENT_SET_PROJECT_DIRTY);
		wxPostEvent(GetParent(), event);
	}
}
