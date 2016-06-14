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
#include <wx/statline.h>
#include <archive.h>
#include <archive_entry.h>


wxDEFINE_EVENT(EVENT_ADD_AUDIO, wxCommandEvent);
wxDEFINE_EVENT(EVENT_ADD_LAYER, wxCommandEvent);
wxDEFINE_EVENT(EVENT_CLOSE_PLAYBACK, wxCommandEvent);
wxDEFINE_EVENT(EVENT_CLOSE_SETTINGS, wxCommandEvent);
wxDEFINE_EVENT(EVENT_LOAD_PROJECT, wxCommandEvent);
wxDEFINE_EVENT(EVENT_LOAD_OTHER, wxCommandEvent);
wxDEFINE_EVENT(EVENT_NEW_PROJECT, wxCommandEvent);
wxDEFINE_EVENT(EVENT_PLAY, wxCommandEvent);
wxDEFINE_EVENT(EVENT_REMOVE_AUDIO_FILE, wxCommandEvent);
wxDEFINE_EVENT(EVENT_QUIT, wxCommandEvent);
wxDEFINE_EVENT(EVENT_SET_PROJECT_DIRTY, wxCommandEvent);
wxDEFINE_EVENT(EVENT_SET_STATUS_TEXT, wxCommandEvent);
wxDEFINE_EVENT(EVENT_SELECT_AUDIO, wxCommandEvent);
wxDEFINE_EVENT(EVENT_UPDATE_AUDIO_NAME, wxCommandEvent);
wxDEFINE_EVENT(EVENT_UPDATE_LAYOUT, wxCommandEvent);


BEGIN_EVENT_TABLE(StudioFrame, wxFrame)
	EVT_CLOSE(StudioFrame::OnClose)
	EVT_MENU(ID_New, StudioFrame::OnNew)
	EVT_MENU(ID_Load, StudioFrame::OnLoad)
	EVT_MENU(ID_Save, StudioFrame::OnSave)
	EVT_MENU(ID_SaveAs, StudioFrame::OnSaveAs)
	EVT_MENU(ID_Export, StudioFrame::OnExport)
	EVT_MENU(ID_Quit, StudioFrame::OnQuit)
	EVT_MENU(ID_About, StudioFrame::OnAbout)
	EVT_MENU(ID_AddMusicTrack, StudioFrame::OnAddMusicTrack)
	EVT_MENU(ID_AddSfxTrack, StudioFrame::OnAddSfxTrack)
	EVT_MENU(ID_EditMusicTrackName, StudioFrame::OnEditMusicTrackName)
	EVT_MENU(ID_EditSfxTrackName, StudioFrame::OnEditSfxTrackName)
	EVT_MENU(ID_RemoveMusicTrack, StudioFrame::OnRemoveMusicTrack)
	EVT_MENU(ID_RemoveSfxTrack, StudioFrame::OnRemoveSfxTrack)
	EVT_MENU(ID_PlaybackPanel, StudioFrame::OnPlaybackPanel)
	EVT_MENU(ID_SettingsPanel, StudioFrame::OnSettingsPanel)
	EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, StudioFrame::OnRecentFile)
	EVT_COMMAND(wxID_ANY, EVENT_ADD_AUDIO, StudioFrame::OnAddAudio)
	EVT_COMMAND(wxID_ANY, EVENT_ADD_LAYER, StudioFrame::OnAddLayer)
	EVT_COMMAND(wxID_ANY, EVENT_CLOSE_PLAYBACK, StudioFrame::OnClosePlayback)
	EVT_COMMAND(wxID_ANY, EVENT_CLOSE_SETTINGS, StudioFrame::OnCloseSettings)
	EVT_COMMAND(wxID_ANY, EVENT_LOAD_PROJECT, StudioFrame::OnLoadProject)
	EVT_COMMAND(wxID_ANY, EVENT_LOAD_OTHER, StudioFrame::OnLoad)
	EVT_COMMAND(wxID_ANY, EVENT_NEW_PROJECT, StudioFrame::OnNew)
	EVT_COMMAND(wxID_ANY, EVENT_PLAY, StudioFrame::OnPlay)
	EVT_COMMAND(wxID_ANY, EVENT_QUIT, StudioFrame::OnQuit)
	EVT_COMMAND(wxID_ANY, EVENT_SELECT_AUDIO, StudioFrame::OnSelectAudio)
	EVT_COMMAND(wxID_ANY, EVENT_SET_PROJECT_DIRTY, StudioFrame::OnSetProjectDirty)
	EVT_COMMAND(wxID_ANY, EVENT_SET_STATUS_TEXT, StudioFrame::OnSetStatusText)
	EVT_COMMAND(wxID_ANY, EVENT_UPDATE_AUDIO_NAME, StudioFrame::OnUpdateAudioName)
	EVT_COMMAND(wxID_ANY, EVENT_UPDATE_LAYOUT, StudioFrame::OnUpdateLayout)
END_EVENT_TABLE()

StudioTimer::StudioTimer(StudioFrame* pane) : wxTimer() {
	StudioTimer::pane = pane;
	musicList = NULL;
	sfxList = NULL;
}

void StudioTimer::Notify() {
	wxString str = musicList ? musicList->GetItemText(labelIndex) : sfxList->GetItemText(labelIndex);
	studioApi->TrackRename(trackName, str.ToStdString());
	pane->UpdateTrackName(trackName, str.ToStdString());
}

void StudioFrame::UpdateTrackName(std::string trackName, std::string newName) {
	if (trackPane) {
		trackPane->UpdateTrackName(trackName, newName);
	}
	if (controlPane) {
		controlPane->UpdateTrackName(trackName, newName);
	}
	trackControl->UpdateTrackName(trackName, newName);

	Layout();
}

StudioFrame::StudioFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(NULL, -1, title, pos, size, style) {
	config = new wxConfig("oamlStudio");
	timer = NULL;
	trackPane = NULL;
	controlPane = NULL;
	rightLine = NULL;
//	layerPanel = NULL;

	wxMenuBar *menuBar = new wxMenuBar;
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_New, _("&New...\tCtrl-N"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_Load, _("&Load...\tCtrl-L"));
	menuFile->Append(ID_Save, _("&Save...\tCtrl-S"));
	menuFile->Append(ID_SaveAs, _("&Save As..."));
	menuFile->Append(ID_Export, _("&Export...\tCtrl-E"));
	menuFile->AppendSeparator();

	fileHistory = new wxFileHistory();

	wxMenu *recent = new wxMenu(long(0));
	menuFile->Append(ID_Recent, "Recent files", recent);
	fileHistory->UseMenu(recent);
	fileHistory->Load(*config);

	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, _("E&xit\tCtrl-Q"));

	menuBar->Append(menuFile, _("&File"));

	menuFile = new wxMenu;
	menuFile->Append(ID_AddMusicTrack, _("Add &music track"));
	menuFile->Append(ID_AddSfxTrack, _("Add &sfx track"));
	menuFile->AppendSeparator();

	menuBar->Append(menuFile, _("&Tracks"));

/*	menuFile = new wxMenu;
	menuFile->Append(ID_AddAudio, _("&Add audio"));
	menuFile->AppendSeparator();

	menuBar->Append(menuFile, _("&Audios"));*/

	viewMenu = new wxMenu;
	viewMenu->AppendCheckItem(ID_PlaybackPanel, _("&Playback Panel"));
	viewMenu->AppendCheckItem(ID_SettingsPanel, _("&Settings Panel"));
	viewMenu->AppendSeparator();

	menuBar->Append(viewMenu, _("&View"));

	menuFile = new wxMenu;
	menuFile->Append(ID_About, _("A&bout..."));
	menuFile->AppendSeparator();

	menuBar->Append(menuFile, _("&About"));

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText(_("Ready"));

	mainSizer = new wxBoxSizer(wxHORIZONTAL);

	SetBackgroundColour(wxColour(0x40, 0x40, 0x40));

	// Left panel
	vSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("-- Music Tracks --"));
	vSizer->Add(staticText, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

	musicList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(240, -1), wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
	musicList->SetBackgroundColour(wxColour(0x80, 0x80, 0x80));
	musicList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &StudioFrame::OnMusicListActivated, this);
	musicList->Bind(wxEVT_RIGHT_UP, &StudioFrame::OnMusicListMenu, this);
	musicList->Bind(wxEVT_LIST_END_LABEL_EDIT, &StudioFrame::OnMusicEndLabelEdit, this);

	vSizer->Add(musicList, 1, wxALL, 5);

	staticText = new wxStaticText(this, wxID_ANY, wxString("-- Sfx Tracks --"));
	vSizer->Add(staticText, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

	sfxList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(240, -1), wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
	sfxList->SetBackgroundColour(wxColour(0x80, 0x80, 0x80));
	sfxList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &StudioFrame::OnSfxListActivated, this);
	sfxList->Bind(wxEVT_RIGHT_UP, &StudioFrame::OnSfxListMenu, this);
	sfxList->Bind(wxEVT_LIST_END_LABEL_EDIT, &StudioFrame::OnSfxEndLabelEdit, this);

	vSizer->Add(sfxList, 1, wxALL, 5);

	wxStaticLine *staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	vSizer->Add(staticLine, 0, wxEXPAND | wxALL, 0);

	trackControl = new TrackControl(this, wxID_ANY);
	vSizer->Add(trackControl, 0, wxALL, 5);

	mainSizer->Add(vSizer, 0, wxEXPAND | wxALL, 0);

	staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	mainSizer->Add(staticLine, 0, wxEXPAND | wxALL, 0);

	// Right panel
	vSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(vSizer, 1, wxEXPAND | wxALL, 0);

	SetSizer(mainSizer);
	Layout();

	Centre(wxBOTH);

	playbackFrame = new PlaybackFrame(this, wxID_ANY);
	playbackFrame->Show(true);
	viewMenu->Check(ID_PlaybackPanel, playbackFrame->IsShown());

	settingsFrame = new SettingsFrame(this, wxID_ANY);
	settingsFrame->Show(false);
	viewMenu->Check(ID_SettingsPanel, settingsFrame->IsShown());

	startupFrame = new StartupFrame(this);
	startupFrame->Show(true);

	dirty = false;
}

StudioFrame::~StudioFrame() {
	if (config) {
		delete config;
		config = NULL;
	}

	if (fileHistory) {
		delete fileHistory;
		fileHistory = NULL;
	}
}

void StudioFrame::SelectTrack(std::string name) {
	if (controlPane) {
		controlPane->Destroy();
		controlPane = NULL;
	}
	if (rightLine) {
		rightLine->Destroy();
		rightLine = NULL;
	}
	if (trackPane) {
		trackPane->Destroy();
		trackPane = NULL;
	}
/*	if (layerPanel) {
		layerPanel->Destroy();
		layerPanel = NULL;
	}*/

	if (name == "") {
		trackControl->SetTrack(name);

		Layout();
		return;
	}

	controlPane = new ControlPanel(this, wxID_ANY);
	controlPane->SetTrackMode(studioApi->TrackIsMusicTrack(name));
	controlPane->OnSelectAudio("", "");
	vSizer->Add(controlPane, 0, wxEXPAND | wxALL, 5);

	rightLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	vSizer->Add(rightLine, 0, wxEXPAND | wxALL, 0);

	hSizer = new wxBoxSizer(wxHORIZONTAL);

//	layerPanel = new LayerPanel(this);
//	hSizer->Add(layerPanel, 0, wxEXPAND | wxALL, 5);

	trackPane = new TrackPanel(this, wxID_ANY, name);
	trackPane->SetTrackMode(studioApi->TrackIsMusicTrack(name));
	hSizer->Add(trackPane, 1, wxEXPAND | wxALL, 5);

	vSizer->Add(hSizer, 1, wxEXPAND | wxALL, 5);

//	layerPanel->LoadLayers();

	std::vector<std::string> list;
	studioApi->TrackGetAudioList(name, list);
	for (std::vector<std::string>::iterator it=list.begin(); it<list.end(); ++it) {
		trackPane->AddAudio(*it);
	}

	SetSizer(mainSizer);
	Layout();

	controlPane->SetTrack(name);
	controlPane->OnSelectAudio("", "");

	trackControl->SetTrack(name);
}

void StudioFrame::OnSetProjectDirty(wxCommandEvent& WXUNUSED(event)) {
	dirty = true;
}

void StudioFrame::OnMusicListActivated(wxListEvent& event) {
	int index = event.GetIndex();
	if (index == -1) {
		wxMessageBox(_("You must choose a track!"));
		return;
	}

	wxString str = musicList->GetItemText(index);
	SelectTrack(str.ToStdString());
}

void StudioFrame::OnMusicListMenu(wxMouseEvent& WXUNUSED(event)) {
	wxMenu menu(wxT(""));
	menu.Append(ID_AddMusicTrack, wxT("&Add Track"));
	menu.Append(ID_EditMusicTrackName, wxT("Edit Track &Name"));
	menu.Append(ID_RemoveMusicTrack, wxT("&Remove Track"));
	PopupMenu(&menu);
}

void StudioFrame::OnMusicEndLabelEdit(wxListEvent& event) {
	if (timer == NULL) {
		timer = new StudioTimer(this);
	}

	wxString str = musicList->GetItemText(event.GetIndex());
	timer->labelIndex = event.GetIndex();
	timer->musicList = musicList;
	timer->trackName = str.ToStdString();
	timer->StartOnce(10);
}

void StudioFrame::OnSfxListActivated(wxListEvent& event) {
	int index = event.GetIndex();
	if (index == -1) {
		wxMessageBox(_("You must choose a track!"));
		return;
	}

	wxString str = sfxList->GetItemText(index);
	SelectTrack(str.ToStdString());
}

void StudioFrame::OnSfxListMenu(wxMouseEvent& WXUNUSED(event)) {
	wxMenu menu(wxT(""));
	menu.Append(ID_AddSfxTrack, wxT("&Add Track"));
	menu.Append(ID_EditSfxTrackName, wxT("Edit Track &Name"));
	menu.Append(ID_RemoveSfxTrack, wxT("&Remove Track"));
	PopupMenu(&menu);
}

void StudioFrame::OnSfxEndLabelEdit(wxListEvent& event) {
	if (timer == NULL) {
		timer = new StudioTimer(this);
	}

	wxString str = sfxList->GetItemText(event.GetIndex());
	timer->labelIndex = event.GetIndex();
	timer->sfxList = sfxList;
	timer->trackName = str.ToStdString();
	timer->StartOnce(10);
}

void StudioFrame::OnClose(wxCloseEvent& WXUNUSED(event)) {
	if (dirty) {
		int ret = wxMessageBox("There are unsaved changes on the project, do you really want to quit without saving?", "Confirm", wxYES_NO, this);
		if (ret == wxNO) {
			return;
		}
	}

	if (fileHistory && config) {
		// Save our file history into config
		fileHistory->Save(*config);
	}

	Destroy();
}

void StudioFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	// Close the app
	Close(TRUE);
}

void StudioFrame::OnNew(wxCommandEvent& event) {
	// Destroy the track panel
	if (trackPane) {
		trackPane->Destroy();
	}

	// Clear music and sfx listviews
	musicList->ClearAll();
	sfxList->ClearAll();

	// Tell oaml we're creating a new project
	studioApi->ProjectNew();

	// Ask the user to save it, path resolution will be based on the project path
	if (SaveAs() == false) {
		startupFrame->Show(true);
	}
}

void StudioFrame::Load(std::string filename) {
	defsPath = filename;
	wxFileName fname(defsPath);
	projectPath = fname.GetPathWithSep();
	InitCallbacks(projectPath);

	if (oaml->Init(fname.GetFullName().ToStdString().c_str()) != OAML_OK) {
		wxMessageBox(_("Error loading project"));

		for (size_t i=0; i<fileHistory->GetCount(); i++) {
			if (fileHistory->GetHistoryFile(i) == fname.GetFullPath()) {
				fileHistory->RemoveFileFromHistory(i);
				break;
			}
		}
		return;
	}

	fileHistory->AddFileToHistory(filename);

	musicList->ClearAll();
	sfxList->ClearAll();

	oamlTracksInfo *info = oaml->GetTracksInfo();

	for (size_t i=0; i<info->tracks.size(); i++) {
		oamlTrackInfo *track = &info->tracks[i];
		if (track->musicTrack) {
			musicList->InsertItem(musicList->GetItemCount(), wxString(track->name));
		} else if (track->sfxTrack) {
			sfxList->InsertItem(sfxList->GetItemCount(), wxString(track->name));
		}
	}

	settingsFrame->OnLoad();
}

void StudioFrame::OnLoadProject(wxCommandEvent& event) {
	Load(event.GetString().ToStdString());
}

void StudioFrame::OnLoad(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog openFileDialog(this, _("Open oaml.defs"), wxEmptyString, "oaml.defs", "*.defs", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	wxString path = openFileDialog.GetPath();
	Load(path.ToStdString());
}

void StudioFrame::OnRecentFile(wxCommandEvent& event) {
	wxString path(fileHistory->GetHistoryFile(event.GetId() - wxID_FILE1));
	if (path.empty() == false) {
		Load(path.ToStdString());
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

void StudioFrame::AddSimpleChildToNode(tinyxml2::XMLNode *node, const char *name, float value) {
	tinyxml2::XMLElement *el = node->GetDocument()->NewElement(name);
	el->SetText(value);
	node->InsertEndChild(el);
}

tinyxml2::XMLNode* StudioFrame::CreateAudioDefs(tinyxml2::XMLDocument& xmlDoc, oamlAudioInfo *audio, bool createPkg) {
	tinyxml2::XMLNode *audioEl = xmlDoc.NewElement("audio");
	if (audioEl == NULL)
		return NULL;

	AddSimpleChildToNode(audioEl, "name", audio->name.c_str());

	for (std::vector<oamlAudioFileInfo>::iterator file=audio->files.begin(); file<audio->files.end(); ++file) {
		tinyxml2::XMLElement *el = audioEl->GetDocument()->NewElement("filename");

		if (createPkg) {
			wxFileName fname(file->filename);
			el->SetText(fname.GetFullName().ToStdString().c_str());
		} else {
			el->SetText(file->filename.c_str());
		}

		if (file->layer != "") {
			el->SetAttribute("layer", file->layer.c_str());
		}
		if (file->randomChance != -1) {
			el->SetAttribute("randomChance", file->randomChance);
		}

		audioEl->InsertEndChild(el);
	}

	if (audio->type) AddSimpleChildToNode(audioEl, "type", audio->type);
	if (audio->volume) AddSimpleChildToNode(audioEl, "volume", audio->volume);
	if (audio->bpm) AddSimpleChildToNode(audioEl, "bpm", audio->bpm);
	if (audio->beatsPerBar) AddSimpleChildToNode(audioEl, "beatsPerBar", audio->beatsPerBar);
	if (audio->bars) AddSimpleChildToNode(audioEl, "bars", audio->bars);
	if (audio->minMovementBars) AddSimpleChildToNode(audioEl, "minMovementBars", audio->minMovementBars);
	if (audio->randomChance) AddSimpleChildToNode(audioEl, "randomChance", audio->randomChance);
	if (audio->fadeIn) AddSimpleChildToNode(audioEl, "fadeIn", audio->fadeIn);
	if (audio->fadeOut) AddSimpleChildToNode(audioEl, "fadeOut", audio->fadeOut);
	if (audio->xfadeIn) AddSimpleChildToNode(audioEl, "xfadeIn", audio->xfadeIn);
	if (audio->xfadeOut) AddSimpleChildToNode(audioEl, "xfadeOut", audio->xfadeOut);
	if (audio->condId) AddSimpleChildToNode(audioEl, "condId", audio->condId);
	if (audio->condType) AddSimpleChildToNode(audioEl, "condType", audio->condType);
	if (audio->condValue) AddSimpleChildToNode(audioEl, "condValue", audio->condValue);
	if (audio->condValue2) AddSimpleChildToNode(audioEl, "condValue2", audio->condValue2);

	return audioEl;
}

tinyxml2::XMLNode* StudioFrame::CreateTrackDefs(tinyxml2::XMLDocument& xmlDoc, oamlTrackInfo *track, bool createPkg) {
	tinyxml2::XMLNode *trackEl = xmlDoc.NewElement("track");
	if (track->sfxTrack) {
		trackEl->ToElement()->SetAttribute("type", "sfx");
	} else {
		trackEl->ToElement()->SetAttribute("type", "music");
	}

	AddSimpleChildToNode(trackEl, "name", track->name.c_str());

	if (track->groups.size() > 0) {
		for (std::vector<std::string>::iterator it=track->groups.begin(); it<track->groups.end(); ++it) {
			AddSimpleChildToNode(trackEl, "group", it->c_str());
		}
	}
	if (track->subgroups.size() > 0) {
		for (std::vector<std::string>::iterator it=track->subgroups.begin(); it<track->subgroups.end(); ++it) {
			AddSimpleChildToNode(trackEl, "subgroup", it->c_str());
		}
	}
	if (track->volume) AddSimpleChildToNode(trackEl, "volume", track->volume);
	if (track->fadeIn) AddSimpleChildToNode(trackEl, "fadeIn", track->fadeIn);
	if (track->fadeOut) AddSimpleChildToNode(trackEl, "fadeOut", track->fadeOut);
	if (track->xfadeIn) AddSimpleChildToNode(trackEl, "xfadeIn", track->xfadeIn);
	if (track->xfadeOut) AddSimpleChildToNode(trackEl, "xfadeOut", track->xfadeOut);

	for (std::vector<oamlAudioInfo>::iterator audio=track->audios.begin(); audio<track->audios.end(); ++audio) {
		tinyxml2::XMLNode *el = CreateAudioDefs(xmlDoc, &(*audio), createPkg);
		if (el != NULL) {
			trackEl->InsertEndChild(el);
		}
	}

	return trackEl;
}

void StudioFrame::CreateDefs(tinyxml2::XMLDocument& xmlDoc, bool createPkg) {
	xmlDoc.InsertFirstChild(xmlDoc.NewDeclaration());

	oamlTracksInfo *info = oaml->GetTracksInfo();
	tinyxml2::XMLNode *prjEl = xmlDoc.NewElement("project");

	if (info->bpm) AddSimpleChildToNode(prjEl, "bpm", info->bpm);
	if (info->beatsPerBar) AddSimpleChildToNode(prjEl, "beatsPerBar", info->beatsPerBar);

	for (std::vector<oamlTrackInfo>::iterator track=info->tracks.begin(); track<info->tracks.end(); ++track) {
		tinyxml2::XMLNode *el = CreateTrackDefs(xmlDoc, &(*track), createPkg);
		if (el != NULL) {
			prjEl->InsertEndChild(el);
		}
	}

	xmlDoc.InsertEndChild(prjEl);
}

void StudioFrame::Save() {
	tinyxml2::XMLDocument xmlDoc;

	// Create the xml definitions and save the file
	CreateDefs(xmlDoc);
	xmlDoc.SaveFile(defsPath.c_str());

	// We've saved our changes, we're clean!
	dirty = false;
}

bool StudioFrame::SaveAs() {
	wxFileDialog openFileDialog(this, _("Save oaml.defs"), wxEmptyString, "oaml.defs", "*.defs", wxFD_SAVE);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return false;

	defsPath = openFileDialog.GetPath();
	wxFileName fname(defsPath);
	projectPath = fname.GetPathWithSep();
	InitCallbacks(projectPath);

	fileHistory->AddFileToHistory(defsPath);

	Save();
	return true;
}

void StudioFrame::OnSave(wxCommandEvent& WXUNUSED(event)) {
	Save();
}

void StudioFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event)) {
	SaveAs();
}

int StudioFrame::WriteDefsToZip(struct archive *zip) {
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLPrinter printer;

	CreateDefs(xmlDoc, true);
	xmlDoc.Accept(&printer);
	const char *buffer = printer.CStr();

	struct archive_entry *entry = archive_entry_new();
	if (entry == NULL) {
		wxMessageBox(_("archive_entry_new error"));
		return -1;
	}

	archive_entry_set_pathname(entry, "oaml.defs");
	archive_entry_set_size(entry, strlen(buffer));
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);
	archive_write_header(zip, entry);
	if (archive_write_data(zip, buffer, strlen(buffer)) != strlen(buffer)) {
		wxMessageBox(_("archive_write_data error"));
		return -1;
	}
	archive_entry_free(entry);

	return 0;
}

int StudioFrame::WriteFileToZip(struct archive *zip, std::string file) {
	const char *filename = file.c_str();
	void *fd = studioCbs.open(filename);
	if (fd == NULL) {
		wxString str;
		str.Printf(wxT("Error creating file %s"), filename);
		wxMessageBox(str);

		return -1;
	}

	studioCbs.seek(fd, 0, SEEK_END);
	size_t size = studioCbs.tell(fd);
	studioCbs.seek(fd, 0, SEEK_SET);

	wxFileName fname(filename);

	struct archive_entry *entry = archive_entry_new();
	if (entry == NULL) {
		wxMessageBox(_("archive_entry_new error"));
		return -1;
	}

	archive_entry_set_pathname(entry, fname.GetFullName().ToStdString().c_str());
	archive_entry_set_size(entry, size);
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);
	archive_write_header(zip, entry);

	char buffer[4096];
	while (size > 0) {
		int bytes = studioCbs.read(buffer, 1, 4096, fd);
		if (bytes == 0) break;

		if (archive_write_data(zip, buffer, bytes) != bytes) {
			studioCbs.close(fd);
			wxMessageBox(_("archive_write_data error"));
			return -1;
		}
	}

	studioCbs.close(fd);
	archive_entry_free(entry);

	return 0;
}

int StudioFrame::CreateZip(std::string zfile, std::vector<std::string> files) {
	struct archive *zip;

	zip = archive_write_new();
	if (zip == NULL) {
		wxMessageBox(_("archive_write_new error"));
		return -1;
	}
	archive_write_set_format_zip(zip);
//	archive_write_zip_set_compression_store(zip);
	archive_write_open_filename(zip, zfile.c_str());

	if (WriteDefsToZip(zip)) {
		archive_write_close(zip);
		archive_write_finish(zip);
		return -1;
	}

	for (size_t i=0; i<files.size(); i++) {
		if (WriteFileToZip(zip, files[i])) {
			archive_write_close(zip);
			archive_write_finish(zip);
			return -1;
		}
	}

	archive_write_close(zip);
	archive_write_finish(zip);

	return 0;
}

void StudioFrame::OnExport(wxCommandEvent& WXUNUSED(event)) {
	std::vector<std::string> list;

	oamlTracksInfo* info = oaml->GetTracksInfo();
	for (size_t i=0; i<info->tracks.size(); i++) {
		for (size_t j=0; j<info->tracks[i].audios.size(); j++) {
			for (size_t k=0; k<info->tracks[i].audios[j].files.size(); k++) {
				list.push_back(info->tracks[i].audios[j].files[k].filename);
			}
		}
	}

	wxFileDialog openFileDialog(this, _("Save oamlPackage.zip"), wxEmptyString, "oamlPackage.zip", "*.zip", wxFD_SAVE);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	CreateZip(wxString(openFileDialog.GetPath()).ToStdString(), list);
}

void StudioFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
	wxString str;

	// TODO - Make a nice custom dialog

	str.Printf("oamlStudio - Studio for Open Adaptive Music Library\r\n"
		   "oaml and oamlStudio are both licensed under the MIT license.\r\n"
		   "\r\n"
		   "https://oamldev.github.io\r\n"
		   "https://github.com/oamldev/oaml\r\n"
		   "https://github.com/oamldev/oamlStudio\r\n"
		   "Copyright (c) 2015-2016 Marcelo Fernandez");

	wxMessageBox(str, _("About oamlStudio"), wxOK | wxICON_INFORMATION, this);
}

void StudioFrame::OnAddMusicTrack(wxCommandEvent& WXUNUSED(event)) {
	oamlTracksInfo *info = oaml->GetTracksInfo();
	int index = info ? info->tracks.size() : 0;

	char name[1024];
	snprintf(name, 1024, "Track%d", index);
	studioApi->TrackNew(std::string(name), false);

	musicList->InsertItem(index, wxString(name));
	SelectTrack(name);

	musicList->EditLabel(index);
}

void StudioFrame::OnAddSfxTrack(wxCommandEvent& WXUNUSED(event)) {
	oamlTracksInfo *info = oaml->GetTracksInfo();
	int index = info ? info->tracks.size() : 0;

	char name[1024];
	snprintf(name, 1024, "Track%d", index);
	studioApi->TrackNew(std::string(name), true);

	sfxList->InsertItem(index, wxString(name));
	SelectTrack(name);

	sfxList->EditLabel(index);
}

void StudioFrame::OnEditMusicTrackName(wxCommandEvent& WXUNUSED(event)) {
	musicList->EditLabel(musicList->GetFirstSelected());
}

void StudioFrame::OnEditSfxTrackName(wxCommandEvent& WXUNUSED(event)) {
	sfxList->EditLabel(sfxList->GetFirstSelected());
}

void StudioFrame::OnRemoveMusicTrack(wxCommandEvent& WXUNUSED(event)) {
	wxString str = musicList->GetItemText(musicList->GetFirstSelected());
	std::string name = str.ToStdString();

	wxString msg = "Are you sure you want to remove track '" + str + "'?";
	int ret = wxMessageBox(msg, "Confirm", wxYES_NO, this);
	if (ret == wxNO) {
		return;
	}

	// If the track is currently selected deselect it
	if (trackControl && trackControl->GetTrackName() == name) {
		SelectTrack("");
	}

	// Remove the track from the list
	musicList->DeleteItem(musicList->GetFirstSelected());

	// Remove the track from oaml
	studioApi->TrackRemove(name);

	// Mark the project dirty
	SetProjectDirty();
}

void StudioFrame::OnRemoveSfxTrack(wxCommandEvent& WXUNUSED(event)) {
	wxString str = sfxList->GetItemText(sfxList->GetFirstSelected());
	std::string name = str.ToStdString();

	// If the track is currently selected deselect it
	if (trackControl && trackControl->GetTrackName() == name) {
		SelectTrack("");
	}

	// Remove the track from the list
	sfxList->DeleteItem(sfxList->GetFirstSelected());

	// Remove the track from oaml
	studioApi->TrackRemove(name);

	// Mark the project dirty
	SetProjectDirty();
}

void StudioFrame::OnSelectAudio(wxCommandEvent& event) {
	std::string str = event.GetString().ToStdString();
	int len = event.GetInt();

	if (controlPane) {
		controlPane->OnSelectAudio(str.substr(0, len), str.substr(len));
	}
}

void StudioFrame::OnAddAudio(wxCommandEvent& event) {
	if (controlPane == NULL)
		return;

	trackPane->AddAudio(event.GetString().ToStdString());

	SetSizer(mainSizer);
	Layout();
}

void StudioFrame::OnAddLayer(wxCommandEvent& event) {
/*	oamlAudioInfo* audio = GetAudioInfo(controlPane->GetTrackName(), event.GetString().ToStdString());
	if (audio == NULL)
		return;

	trackPane->AddAudio(audio);

	SetSizer(mainSizer);
	Layout();*/
}

void StudioFrame::OnRemoveAudio(wxCommandEvent& event) {
	if (controlPane) {
		controlPane->OnSelectAudio("", "");
	}

	trackPane->RemoveAudio(event.GetString().ToStdString());

	SetSizer(mainSizer);
	Layout();
}

void StudioFrame::OnPlay(wxCommandEvent& WXUNUSED(event)) {
	if (controlPane == NULL)
		return;

	if (controlPane->IsMusicMode()) {
		oaml->PlayTrack(controlPane->GetTrack());
	} else {
		oaml->PlaySfx(controlPane->GetAudioName());
	}
}

void StudioFrame::OnPlaybackPanel(wxCommandEvent& WXUNUSED(event)) {
	bool show = playbackFrame->IsShown() ? false : true;
	playbackFrame->Show(show);
	viewMenu->Check(ID_PlaybackPanel, show);
}

void StudioFrame::OnClosePlayback(wxCommandEvent& WXUNUSED(event)) {
	playbackFrame->Show(false);
	viewMenu->Check(ID_PlaybackPanel, false);
}

void StudioFrame::OnSettingsPanel(wxCommandEvent& WXUNUSED(event)) {
	bool show = settingsFrame->IsShown() ? false : true;
	settingsFrame->Show(show);
	settingsFrame->Center();
	viewMenu->Check(ID_SettingsPanel, show);
}

void StudioFrame::OnCloseSettings(wxCommandEvent& WXUNUSED(event)) {
	settingsFrame->Show(false);
	viewMenu->Check(ID_SettingsPanel, false);
}

void StudioFrame::OnUpdateAudioName(wxCommandEvent& event) {
	if (controlPane == NULL)
		return;

	std::string str = event.GetString().ToStdString();
	int len = event.GetInt();
	if (trackPane) {
		trackPane->UpdateAudioName(str.substr(0, len), str.substr(len));
	}
}

void StudioFrame::OnSetStatusText(wxCommandEvent& event) {
	SetStatusText(event.GetString());
}

void StudioFrame::OnUpdateLayout(wxCommandEvent& WXUNUSED(event)) {
	Layout();
}

