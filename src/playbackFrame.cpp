#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"


PlaybackFrame::PlaybackFrame(wxWindow *parent, wxWindowID id) : wxFrame(parent, id, _("Playback"), wxPoint(50, 50), wxSize(360, 180), wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxRESIZE_BORDER) {
	mSizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);

	wxImage::AddHandler(new wxPNGHandler);

	playBtn = new wxBitmapButton(this, ID_Play, wxBitmap(wxT("images/play.png"), wxBITMAP_TYPE_PNG));
	playBtn->Bind(wxEVT_BUTTON, &PlaybackFrame::OnPlay, this);
	hSizer->Add(playBtn, 0, wxALL, 5);

	pauseBtn = new wxBitmapButton(this, ID_Pause, wxBitmap(wxT("images/pause.png"), wxBITMAP_TYPE_PNG));
	pauseBtn->Bind(wxEVT_BUTTON, &PlaybackFrame::OnPause, this);
	hSizer->Add(pauseBtn, 0, wxALL, 5);

	mSizer->Add(hSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL);

	hSizer = new wxBoxSizer(wxHORIZONTAL);

	mSizer->Add(hSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL);

	hSizer = new wxBoxSizer(wxHORIZONTAL);

	infoText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(360, 40), wxTE_READONLY | wxTE_MULTILINE);
	hSizer->Add(infoText, 0, wxEXPAND | wxGROW | wxALL, 5);

	mSizer->Add(hSizer, 0, wxEXPAND | wxGROW | wxALL);

	SetSizer(mSizer);
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

