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
#include <wx/mstream.h>

#include "oamlCommon.h"
#include "resources.h"

PlaybackFrame::PlaybackFrame(wxWindow *parent, wxWindowID id) : wxFrame(parent, id, _("Playback"), wxPoint(50, 50), wxSize(360, 180), wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX) {
	mSizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);

	wxImage::AddHandler(new wxPNGHandler);

	wxMemoryInputStream playStream(play_png, play_png_size);
	playBtn = new wxBitmapButton(this, ID_Play, wxBitmap(wxImage(playStream, wxBITMAP_TYPE_ANY, -1), -1));
	playBtn->Bind(wxEVT_BUTTON, &PlaybackFrame::OnPlay, this);
	hSizer->Add(playBtn, 0, wxALL, 5);

	wxMemoryInputStream pauseStream(pause_png, pause_png_size);
	pauseBtn = new wxBitmapButton(this, ID_Pause, wxBitmap(wxImage(pauseStream, wxBITMAP_TYPE_ANY, -1), -1));
	pauseBtn->Bind(wxEVT_BUTTON, &PlaybackFrame::OnPause, this);
	hSizer->Add(pauseBtn, 0, wxALL, 5);

	mSizer->Add(hSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL);

	hSizer = new wxBoxSizer(wxHORIZONTAL);

	wxGridSizer *sizer = new wxGridSizer(2, 0, 0);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Id:"));
	sizer->Add(staticText, 0, wxALL, 5);

	condIdCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1));
	sizer->Add(condIdCtrl, 0, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("Condition Value:"));
	sizer->Add(staticText, 0, wxALL, 5);

	condValueCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1));
	sizer->Add(condValueCtrl, 0, wxALL, 5);

	hSizer->Add(sizer, 0, wxALL);

	condBtn = new wxButton(this, ID_Condition, wxT("Set condition"));
	condBtn->Bind(wxEVT_BUTTON, &PlaybackFrame::OnCondition, this);
	hSizer->Add(condBtn, 0, wxALL, 5);

	mSizer->Add(hSizer, 0, wxALL);

	hSizer = new wxBoxSizer(wxHORIZONTAL);

	infoText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, -1), wxTE_READONLY | wxTE_MULTILINE);
	infoText->SetMinSize(wxSize(320, 80));
	hSizer->Add(infoText, 1, wxEXPAND | wxGROW | wxALL, 5);

	mSizer->Add(hSizer, 1, wxEXPAND | wxGROW | wxALL);

	Bind(wxEVT_CLOSE_WINDOW, &PlaybackFrame::OnClose, this);

	SetSizerAndFit(mSizer);
	Layout();

	wxRect rect = parent->GetRect();
	wxSize size = GetSize();
	int posX = (rect.GetX() + rect.GetWidth()) - size.GetWidth() - 20;
	int posY = (rect.GetY() + rect.GetHeight()) - size.GetHeight() - 20;
	SetPosition(wxPoint(posX, posY));

	timer = new PlaybackTimer(this);
	timer->Start(10);
}

PlaybackFrame::~PlaybackFrame() {
	delete timer;
}

void PlaybackFrame::OnClose(wxCloseEvent& event) {
	wxCommandEvent event2(EVENT_CLOSE_PLAYBACK);
	wxPostEvent(GetParent(), event2);

	event.Veto();
}

void PlaybackFrame::OnPlay(wxCommandEvent& WXUNUSED(event)) {
	if (oaml->IsPaused()) {
		oaml->Resume();
	} else {
		wxCommandEvent event(EVENT_PLAY);
		wxPostEvent(GetParent(), event);
	}
}

void PlaybackFrame::OnPause(wxCommandEvent& WXUNUSED(event)) {
	oaml->PauseToggle();
}

void PlaybackFrame::OnCondition(wxCommandEvent& WXUNUSED(event)) {
	wxString condIdStr = condIdCtrl->GetLineText(0);
	wxString condValueStr = condValueCtrl->GetLineText(0);
	long condId = 0;
	long condValue = 0;

	condIdStr.ToLong(&condId);
	condValueStr.ToLong(&condValue);

	oaml->SetCondition(condId, condValue);
}

void PlaybackFrame::Update() {
	infoText->Clear();
	*infoText << oaml->GetPlayingInfo();
}

PlaybackTimer::PlaybackTimer(PlaybackFrame* pane) : wxTimer() {
	PlaybackTimer::pane = pane;
}

void PlaybackTimer::Notify() {
	pane->Update();
}
