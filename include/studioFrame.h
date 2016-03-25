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

#ifndef __STUDIOFRAME_H__
#define __STUDIOFRAME_H__

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

class StudioFrame;

class StudioTimer : public wxTimer {
	StudioFrame* pane;
public:
	int labelIndex;
	wxListView* trackList;
	std::string trackName;

	StudioTimer(StudioFrame* pane);

	void Notify();
};

class StudioFrame: public wxFrame {
private:
	wxConfig *config;
	wxListView* trackList;
	wxBoxSizer* mainSizer;
	wxBoxSizer* vSizer;
	wxFileHistory* fileHistory;

	ControlPanel* controlPane;
	TrackPanel* trackPane;
	StudioTimer* timer;
	PlaybackFrame* playFrame;

	std::string prjPath;
	std::string defsPath;

	oamlTracksInfo* tinfo;

	void AddSimpleChildToNode(tinyxml2::XMLNode *node, const char *name, const char *value);
	void AddSimpleChildToNode(tinyxml2::XMLNode *node, const char *name, int value);

	void SelectTrack(std::string name);

	void CreateDefs(tinyxml2::XMLDocument& xmlDoc, bool createPkg = false);
	void ReloadDefs();

	int WriteDefsToZip(struct archive *zip);
	int WriteFileToZip(struct archive *zip, std::string file);
	int CreateZip(std::string zfile, std::vector<std::string> files);

	void Load(std::string filename);
public:
	StudioFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);

	tinyxml2::XMLNode* CreateAudioDefs(tinyxml2::XMLDocument& xmlDoc, oamlAudioInfo *audio, bool createPkg);
	void CreateTrackDefs(tinyxml2::XMLDocument& xmlDoc, oamlTrackInfo *track, bool createPkg);

	void OnTrackListActivated(wxListEvent& event);
	void OnTrackListMenu(wxMouseEvent& event);
	void OnTrackEndLabelEdit(wxListEvent& event);
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
	void OnRemoveAudio(wxCommandEvent& event);
	void OnPlay(wxCommandEvent& event);
	void OnRecentFile(wxCommandEvent& event);

	void UpdateTrackName(std::string trackName, std::string newName);

	DECLARE_EVENT_TABLE()
};

#endif

