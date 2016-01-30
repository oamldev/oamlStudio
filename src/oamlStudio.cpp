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


IMPLEMENT_APP(oamlStudio)

oamlApi *oaml;

oamlTrackInfo* GetTrackInfo(int trackIndex) {
	oamlTracksInfo* info = oaml->GetTracksInfo();
	if (info == NULL || trackIndex >= info->tracks.size())
		return NULL;
	return &info->tracks[trackIndex];
}

oamlAudioInfo* GetAudioInfo(int trackIndex, int audioIndex) {
	oamlTracksInfo* info = oaml->GetTracksInfo();
	if (info == NULL || trackIndex >= info->tracks.size() || audioIndex >= info->tracks[trackIndex].audios.size())
		return NULL;
	return &info->tracks[trackIndex].audios[audioIndex];
}

int AddAudioInfo(int trackIndex, oamlAudioInfo& audio) {
	oamlTrackInfo* info = GetTrackInfo(trackIndex);
	if (info == NULL)
		return -1;

	info->audios.push_back(audio);
	return info->audios.size()-1;
}


class AudioPanel : public wxPanel {
private:
	wxBoxSizer *sizer;
	int trackIndex;
	int panelIndex;

public:
	AudioPanel(wxFrame* parent, int index, int trackIdx) : wxPanel(parent) {
		wxString texts[5] = { "Intro", "Main loop", "With random chance", "Conditional", "Ending" };

		panelIndex = index;
		trackIndex = trackIdx;

		sizer = new wxBoxSizer(wxVERTICAL);
		wxStaticText *staticText = new wxStaticText(this, wxID_ANY, texts[index], wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
		staticText->SetBackgroundColour(wxColour(0xD0, 0xD0, 0xD0));
		sizer->Add(staticText, 0, wxALL | wxEXPAND | wxGROW, 5);
		SetSizer(sizer);

		Bind(wxEVT_PAINT, &AudioPanel::OnPaint, this);
		Bind(wxEVT_RIGHT_UP, &AudioPanel::OnRightUp, this);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &AudioPanel::OnMenuEvent, this, ID_AddAudio);
	}

	void OnPaint(wxPaintEvent& WXUNUSED(evt)) {
		wxPaintDC dc(this);

		wxSize size = GetSize();
		int x2 = size.GetWidth();
		int y2 = size.GetHeight();

		dc.SetPen(wxPen(wxColour(0, 0, 0), 4));
		dc.DrawLine(0,  0,  0,  y2);
		dc.DrawLine(x2, 0,  x2, y2);
		dc.DrawLine(0,  0,  x2, 0);
		dc.DrawLine(0,  y2, x2, y2);
	}

	void AddWaveform(int trackIndex, int audioIndex, wxFrame *topWnd) {
		WaveformDisplay *waveDisplay = new WaveformDisplay((wxFrame*)this, topWnd);
		waveDisplay->SetSource(trackIndex, audioIndex);

		sizer->Add(waveDisplay, 0, wxALL, 5);
		Layout();
	}

	void AddAudio() {
		wxFileDialog openFileDialog(this, _("Open audio file"), ".", "", "Audio files (*.wav;*.aif;*.ogg)|*.aif;*.aiff;*.wav;*.wave;*.ogg", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
		if (openFileDialog.ShowModal() == wxID_CANCEL)
			return;

		oamlAudioInfo audio;
		memset(&audio, 0, sizeof(oamlAudioInfo));
		audio.filename = openFileDialog.GetPath();
		switch (panelIndex) {
			case 0: audio.type = 1; break;
			case 1: audio.type = 2; break;
			case 2: audio.type = 2; audio.randomChance = 1; break;
			case 3: audio.type = 4; break;
			case 4: audio.type = 3; break;
		}

		int audioIndex = AddAudioInfo(trackIndex, audio);

		wxCommandEvent event(EVENT_ADD_AUDIO);
		event.SetInt((trackIndex << 16) | (audioIndex & 0xFFFF));
		wxPostEvent(GetParent(), event);
	}

	void OnMenuEvent(wxCommandEvent& event) {
		switch (event.GetId()) {
			case ID_AddAudio:
				AddAudio();
				break;
		}
	}

	void OnRightUp(wxMouseEvent& WXUNUSED(event)) {
		wxMenu menu(wxT(""));
		menu.Append(ID_AddAudio, wxT("&Add Audio"));
		PopupMenu(&menu);
	}
};

class ScrolledWidgetsPane : public wxScrolledWindow {
private:
	wxBoxSizer* sizer;
	AudioPanel* audioPanel[5];
	int trackIndex;

public:
	ScrolledWidgetsPane(wxWindow* parent, wxWindowID id, int trackIdx) : wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL) {
		trackIndex = trackIdx;

		SetBackgroundColour(wxColour(0x40, 0x40, 0x40));
		SetScrollRate(50, 50);

		sizer = new wxBoxSizer(wxHORIZONTAL);

		for (int i=0; i<5; i++) {
			audioPanel[i] = new AudioPanel((wxFrame*)this, i, trackIndex);

			sizer->Add(audioPanel[i], 0, wxALL | wxEXPAND | wxGROW, 0);
		}

		SetSizer(sizer);
		Layout();

		sizer->Fit(this);
	}

	void AddDisplay(int audioIndex) {
		int i = 1;

		oamlAudioInfo *audio = GetAudioInfo(trackIndex, audioIndex);
		if (audio == NULL)
			return;

		if (audio->type == 1) {
			i = 0;
		} else if (audio->type == 3) {
			i = 4;
		} else if (audio->type == 4) {
			i = 3;
		} else if (audio->randomChance > 0) {
			i = 2;
		}
		audioPanel[i]->AddWaveform(trackIndex, audioIndex, (wxFrame*)GetParent());

		SetSizer(sizer);
		Layout();
		sizer->Fit(this);
	}
};

class ControlPanel : public wxPanel {
private:
	wxTextCtrl *fileCtrl;
	wxTextCtrl *bpmCtrl;
	wxTextCtrl *bpbCtrl;
	wxTextCtrl *barsCtrl;
	wxGridSizer *sizer;

public:
	ControlPanel(wxFrame* parent, wxWindowID id) : wxPanel(parent, id) {
		sizer = new wxGridSizer(2, 5, 5);

		wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("Filename"));
		sizer->Add(staticText, 0, wxALL, 10);

		fileCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(320, -1), wxTE_READONLY);
		sizer->Add(fileCtrl, 0, wxALL, 10);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Bpm"));
		sizer->Add(staticText, 0, wxALL, 10);

		bpmCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(120, -1));
		sizer->Add(bpmCtrl, 0, wxALL, 10);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Beats Per Bar"));
		sizer->Add(staticText, 0, wxALL, 10);

		bpbCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(120, -1));
		sizer->Add(bpbCtrl, 0, wxALL, 10);

		staticText = new wxStaticText(this, wxID_ANY, wxString("Bars"));
		sizer->Add(staticText, 0, wxALL, 10);

		barsCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(120, -1));
		sizer->Add(barsCtrl, 0, wxALL, 10);

		SetSizer(sizer);
		SetMinSize(wxSize(-1, 180));

		Layout();
	}

	void OnSelectAudio(int trackIndex, int audioIndex) {
		fileCtrl->Clear();
		bpmCtrl->Clear();
		bpbCtrl->Clear();
		barsCtrl->Clear();

		oamlAudioInfo *info = GetAudioInfo(trackIndex, audioIndex);
		if (info) {
			*fileCtrl << info->filename;
			*bpmCtrl << info->bpm;
			*bpbCtrl << info->beatsPerBar;
			*barsCtrl << info->bars;
		}
	}
};

class StudioTimer : public wxTimer {
	wxWindow* pane;
public:
	StudioTimer(wxWindow* pane);

	void Notify();
};

StudioTimer::StudioTimer(wxWindow* pane) : wxTimer() {
	StudioTimer::pane = pane;
}

void StudioTimer::Notify() {
	pane->Layout();
}

class StudioFrame: public wxFrame {
private:
	std::string defsPath;
	wxListView* trackList;
	wxBoxSizer* mainSizer;
	wxBoxSizer* vSizer;
	ControlPanel* controlPane;
	ScrolledWidgetsPane* trackPane;
	StudioTimer* timer;

	oamlTrackInfo* curTrack;
	oamlTracksInfo* tinfo;

	void AddSimpleChildToNode(tinyxml2::XMLNode *node, const char *name, const char *value);
	void AddSimpleChildToNode(tinyxml2::XMLNode *node, const char *name, int value);
public:
	StudioFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnTrackListActivated(wxListEvent& event);
	void OnTrackListMenu(wxMouseEvent& event);
	void OnTrackEndLabelEdit(wxCommandEvent& event);
	void OnNew(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnExport(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnAddTrack(wxCommandEvent& event);
	void OnEditTrackName(wxCommandEvent& event);
	void OnSelectAudio(wxCommandEvent& event);
	void OnAddAudio(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(StudioFrame, wxFrame)
	EVT_MENU(ID_New, StudioFrame::OnNew)
	EVT_MENU(ID_Load, StudioFrame::OnLoad)
	EVT_MENU(ID_Save, StudioFrame::OnSave)
	EVT_MENU(ID_SaveAs, StudioFrame::OnSaveAs)
	EVT_MENU(ID_Export, StudioFrame::OnExport)
	EVT_MENU(ID_Quit, StudioFrame::OnQuit)
	EVT_MENU(ID_About, StudioFrame::OnAbout)
	EVT_MENU(ID_AddTrack, StudioFrame::OnAddTrack)
	EVT_MENU(ID_EditTrackName, StudioFrame::OnEditTrackName)
	EVT_COMMAND(wxID_ANY, EVENT_SELECT_AUDIO, StudioFrame::OnSelectAudio)
	EVT_COMMAND(wxID_ANY, EVENT_ADD_AUDIO, StudioFrame::OnAddAudio)
END_EVENT_TABLE()

bool oamlStudio::OnInit() {
	oaml = new oamlApi();
	oaml->Init("oaml.defs");

	StudioFrame *frame = new StudioFrame(_("oamlStudio"), wxPoint(50, 50), wxSize(1024, 768));
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

StudioFrame::StudioFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, -1, title, pos, size) {
	timer = NULL;
	curTrack = NULL;

	wxMenuBar *menuBar = new wxMenuBar;
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_New, _("&New..."));
	menuFile->AppendSeparator();
	menuFile->Append(ID_Load, _("&Load..."));
	menuFile->Append(ID_Save, _("&Save..."));
	menuFile->Append(ID_SaveAs, _("&Save As..."));
	menuFile->Append(ID_Export, _("&Export..."));
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, _("E&xit"));

	menuBar->Append(menuFile, _("&File"));

	menuFile = new wxMenu;
	menuFile->Append(ID_AddTrack, _("&Add track"));
	menuFile->AppendSeparator();

	menuBar->Append(menuFile, _("&Tracks"));

/*	menuFile = new wxMenu;
	menuFile->Append(ID_AddAudio, _("&Add audio"));
	menuFile->AppendSeparator();

	menuBar->Append(menuFile, _("&Audios"));*/

	menuFile = new wxMenu;
	menuFile->Append(ID_About, _("A&bout..."));
	menuFile->AppendSeparator();

	menuBar->Append(menuFile, _("&About"));

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText(_("Ready"));

	mainSizer = new wxBoxSizer(wxHORIZONTAL);

	SetBackgroundColour(wxColour(0x40, 0x40, 0x40));

	tinfo = oaml->GetTracksInfo();

	trackList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(240, -1), wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
	trackList->SetBackgroundColour(wxColour(0x80, 0x80, 0x80));
	trackList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &StudioFrame::OnTrackListActivated, this);
	trackList->Bind(wxEVT_RIGHT_UP, &StudioFrame::OnTrackListMenu, this);
	trackList->Bind(wxEVT_LIST_END_LABEL_EDIT, &StudioFrame::OnTrackEndLabelEdit, this);
	for (size_t i=0; i<tinfo->tracks.size(); i++) {
		oamlTrackInfo *track = &tinfo->tracks[i];
		trackList->InsertItem(i, wxString(track->name));
	}

	trackPane = NULL;

	mainSizer->Add(trackList, 0, wxEXPAND | wxALL, 5);

	vSizer = new wxBoxSizer(wxVERTICAL);

	controlPane = new ControlPanel(this, wxID_ANY);
	vSizer->Add(controlPane, 0, wxEXPAND | wxALL, 5);

	mainSizer->Add(vSizer, 1, wxEXPAND | wxALL, 0);

	SetSizer(mainSizer);
	Layout();

	Centre(wxBOTH);
}

void StudioFrame::OnTrackListActivated(wxListEvent& event) {
	int index = event.GetIndex();
	if (index == -1) {
//		WxUtils::ShowErrorDialog(_("You must choose a track!"));
		return;
	}

	if (trackPane) {
		trackPane->Destroy();
	}

	trackPane = new ScrolledWidgetsPane(this, wxID_ANY, index);
	vSizer->Add(trackPane, 1, wxEXPAND | wxALL, 5);

	curTrack = &tinfo->tracks[index];
	for (size_t i=0; i<curTrack->audios.size(); i++) {
		trackPane->AddDisplay(i);
	}

	SetSizer(mainSizer);
	Layout();

	controlPane->OnSelectAudio(-1, -1);
}

void StudioFrame::OnTrackListMenu(wxMouseEvent& WXUNUSED(event)) {
	wxMenu menu(wxT(""));
	menu.Append(ID_AddTrack, wxT("&Add Track"));
	menu.Append(ID_EditTrackName, wxT("Edit Track &Name"));
	PopupMenu(&menu);
}

void StudioFrame::OnTrackEndLabelEdit(wxCommandEvent& WXUNUSED(event)) {
	if (timer == NULL) {
		timer = new StudioTimer(this);
	}

	timer->StartOnce(10);
}

void StudioFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
}

void StudioFrame::OnNew(wxCommandEvent& WXUNUSED(event)) {
	if (tinfo) {
//		delete tinfo;
		tinfo = NULL;
	}
	if (trackPane) {
		trackPane->Destroy();
	}
	trackList->ClearAll();
}

void StudioFrame::OnLoad(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog openFileDialog(this, _("Open oaml.defs"), ".", "oaml.defs", "*.defs", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	oaml->Init(openFileDialog.GetPath());

	tinfo = oaml->GetTracksInfo();

	for (size_t i=0; i<tinfo->tracks.size(); i++) {
		oamlTrackInfo *track = &tinfo->tracks[i];
		trackList->InsertItem(i, wxString(track->name));
	}
}

void StudioFrame::AddSimpleChildToNode(tinyxml2::XMLNode *node, const char *name, const char *value) {
	tinyxml2::XMLElement *el = node->GetDocument()->NewElement(name);
	el->SetText(value);
	node->InsertEndChild(el);
}

void StudioFrame::AddSimpleChildToNode(tinyxml2::XMLNode *node, const char *name, int value) {
	tinyxml2::XMLElement *el = node->GetDocument()->NewElement(name);
	el->SetText(value);
	node->InsertEndChild(el);
}

void StudioFrame::OnSave(wxCommandEvent& WXUNUSED(event)) {
	tinyxml2::XMLDocument xmlDoc;

	xmlDoc.InsertFirstChild(xmlDoc.NewDeclaration());

	for (size_t i=0; i<tinfo->tracks.size(); i++) {
		oamlTrackInfo *track = &tinfo->tracks[i];

		tinyxml2::XMLNode *trackEl = xmlDoc.NewElement("track");

		AddSimpleChildToNode(trackEl, "name", track->name.c_str());

		if (track->fadeIn) AddSimpleChildToNode(trackEl, "fadeIn", track->fadeIn);
		if (track->fadeOut) AddSimpleChildToNode(trackEl, "fadeOut", track->fadeOut);
		if (track->xfadeIn) AddSimpleChildToNode(trackEl, "xfadeIn", track->xfadeIn);
		if (track->xfadeOut) AddSimpleChildToNode(trackEl, "xfadeOut", track->xfadeOut);

		for (size_t j=0; j<track->audios.size(); j++) {
			oamlAudioInfo *audio = &track->audios[j];

			tinyxml2::XMLNode *audioEl = xmlDoc.NewElement("audio");
			AddSimpleChildToNode(audioEl, "filename", audio->filename.c_str());
			if (audio->type) AddSimpleChildToNode(audioEl, "type", audio->type);
			if (audio->bpm) AddSimpleChildToNode(audioEl, "bpm", audio->bpm);
			if (audio->beatsPerBar) AddSimpleChildToNode(audioEl, "beatsPerBar", audio->beatsPerBar);
			if (audio->bars) AddSimpleChildToNode(audioEl, "bars", audio->bars);
			if (audio->minMovementBars) AddSimpleChildToNode(audioEl, "minMovementBars", audio->minMovementBars);
			if (audio->randomChance) AddSimpleChildToNode(audioEl, "randomChance", audio->randomChance);
			if (audio->fadeIn) AddSimpleChildToNode(audioEl, "fadeIn", audio->fadeIn);
			if (audio->fadeOut) AddSimpleChildToNode(audioEl, "fadeOut", audio->fadeOut);
			if (audio->xfadeIn) AddSimpleChildToNode(audioEl, "xfadeIn", audio->xfadeIn);
			if (audio->xfadeOut) AddSimpleChildToNode(audioEl, "xfadeOut", audio->xfadeOut);

			trackEl->InsertEndChild(audioEl);
		}

		xmlDoc.InsertEndChild(trackEl);
	}

	xmlDoc.SaveFile(defsPath.c_str());
}

void StudioFrame::OnSaveAs(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, _("Save oaml.defs"), ".", "oaml.defs", "*.defs", wxFD_SAVE);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	defsPath = openFileDialog.GetPath();
	OnSave(event);
}

void StudioFrame::OnExport(wxCommandEvent& WXUNUSED(event)) {
}

void StudioFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
	wxMessageBox(_("oamlStudio"), _("About oamlStudio"), wxOK | wxICON_INFORMATION, this);
}

void StudioFrame::OnAddTrack(wxCommandEvent& WXUNUSED(event)) {
	oamlTrackInfo track;
	char name[1024];
	snprintf(name, 1024, "Track%ld", tinfo->tracks.size()+1);
	track.name = name;

	if (tinfo == NULL) {
		tinfo = new oamlTracksInfo;
	}
	tinfo->tracks.push_back(track);

	trackList->InsertItem(tinfo->tracks.size()-1, wxString(track.name));
}

void StudioFrame::OnEditTrackName(wxCommandEvent& WXUNUSED(event)) {
	trackList->EditLabel(trackList->GetFirstSelected());
}

void StudioFrame::OnSelectAudio(wxCommandEvent& event) {
	controlPane->OnSelectAudio(event.GetInt() >> 16, event.GetInt() & 0xFFFF);
}

void StudioFrame::OnAddAudio(wxCommandEvent& event) {
	trackPane->AddDisplay(event.GetInt() & 0xFFFF);

	SetSizer(mainSizer);
	Layout();
}

