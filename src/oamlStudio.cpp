#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"
#include "oamlStudio.h"
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

static void* oamlOpen(const char *filename) {
	return fopen(filename, "rb");
}

static size_t oamlRead(void *ptr, size_t size, size_t nitems, void *fd) {
	return fread(ptr, size, nitems, (FILE*)fd);
}

static int oamlSeek(void *fd, long offset, int whence) {
	return fseek((FILE*)fd, offset, whence);
}

static long oamlTell(void *fd) {
	return ftell((FILE*)fd);
}

static int oamlClose(void *fd) {
	return fclose((FILE*)fd);
}


static oamlFileCallbacks defCbs = {
	&oamlOpen,
	&oamlRead,
	&oamlSeek,
	&oamlTell,
	&oamlClose
};

class RenderTimer : public wxTimer {
	wxWindow* pane;
public:
	RenderTimer(wxWindow* pane);

	void Notify();
};

RenderTimer::RenderTimer(wxWindow* pane) : wxTimer() {
	RenderTimer::pane = pane;
}

void RenderTimer::Notify() {
	pane->Refresh();
}

class WaveformDisplay : public wxPanel {
private:
	RenderTimer* timer;

	std::string audiofile;
	audioFile *handle;
	ByteBuffer buffer;

	int peakl;
	int peakr;
	int count;

	std::vector<int> peaksL;
	std::vector<int> peaksR;

	int bytesPerSec;
	int samplesPerPixel;

	wxFrame* topWnd;

public:
	WaveformDisplay(wxFrame* parent, wxFrame* wnd);
	~WaveformDisplay();

	int read32();
	void setFile(std::string filename);
	void OnPaint(wxPaintEvent& evt);
};

WaveformDisplay::WaveformDisplay(wxFrame* parent, wxFrame* wnd) : wxPanel(parent) {
	topWnd = wnd;
	handle = NULL;
	timer = NULL;

	Bind(wxEVT_PAINT, &WaveformDisplay::OnPaint, this);
}

WaveformDisplay::~WaveformDisplay() {
	if (timer) {
		delete timer;
		timer = NULL;
	}

	if (handle) {
		delete handle;
		handle = NULL;
	}
}

int WaveformDisplay::read32() {
	int ret = 0;

	if (handle->GetBytesPerSample() == 3) {
		ret|= ((unsigned int)buffer.get())<<8;
		ret|= ((unsigned int)buffer.get())<<16;
		ret|= ((unsigned int)buffer.get())<<24;
	} else if (handle->GetBytesPerSample() == 2) {
		ret|= ((unsigned int)buffer.get())<<16;
		ret|= ((unsigned int)buffer.get())<<24;
	} else if (handle->GetBytesPerSample() == 1) {
		ret|= ((unsigned int)buffer.get())<<23;
	}
	return ret;
}

void WaveformDisplay::setFile(std::string filename) {
	audiofile = filename;

	buffer.clear();
	peaksL.clear();
	peaksR.clear();
	peakl = 0;
	peakr = 0;
	count = 0;

	std::string ext = filename.substr(filename.find_last_of(".") + 1);
	if (ext == "ogg") {
		handle = (audioFile*)new oggFile(&defCbs);
	} else if (ext == "aif" || ext == "aiff") {
		handle = (audioFile*)new aifFile(&defCbs);
	} else if (ext == "wav" || ext == "wave") {
		handle = new wavFile(&defCbs);
	} else {
		fprintf(stderr, "liboaml: Unknown audio format: '%s'\n", filename.c_str());
		return;
	}

	if (handle->Open(filename.c_str()) == -1) {
		fprintf(stderr, "liboaml: Error opening: '%s'\n", filename.c_str());
		return;
	}

	int w = (handle->GetTotalSamples() / handle->GetChannels()) / (handle->GetSamplesPerSec() / 10);
	wxSize size(w, 100);
	SetSize(size);
	SetMinSize(size);
	SetMaxSize(size);

	PostSizeEventToParent();

	bytesPerSec = handle->GetSamplesPerSec() * handle->GetBytesPerSample() * handle->GetChannels();

	if (timer == NULL) {
		timer = new RenderTimer(this);
	}

	timer->Start(10);

	topWnd->SetStatusText(_("Reading.."));
}

void WaveformDisplay::OnPaint(wxPaintEvent&  WXUNUSED(evt)) {
	wxPaintDC dc(this);

	if (handle == NULL || handle->GetTotalSamples() == 0)
		return;

	wxSize size = GetSize();
	samplesPerPixel = (handle->GetTotalSamples() / handle->GetChannels()) / size.GetWidth();

	int bytesRead = handle->Read(&buffer, bytesPerSec);
	if (bytesRead == 0) {
		timer->Stop();
	}

	while (buffer.bytesRemaining() > 0) {
		int sl = abs(read32() >> 16);
		int sr = abs(read32() >> 16);

		if (sl > peakl) peakl = sl;
		if (sr > peakr) peakr = sr;
		count++;

		if (count > samplesPerPixel) {
			peaksL.push_back(peakl);
			peaksR.push_back(peakr);

			peakl = 0;
			peakr = 0;
			count = 0;
		}
	}

	int w = size.GetWidth();
	int h = size.GetHeight();
	int h2 = h/2;

	dc.SetBrush(*wxBLACK_BRUSH);
	dc.DrawRectangle(0, 0, w, h);

	dc.SetPen(wxPen(wxColor(107, 216, 37), 1));
	for (int x=0; x<w; x++) {
		float l = 0.0;
		float r = 0.0;

		if (x < (int)peaksL.size() && x < (int)peaksR.size()) {
			l = peaksL[x] / 32768.0;
			r = peaksR[x] / 32768.0;
		}

		dc.DrawLine(x, h2, x, h2 - h2 * l);
		dc.DrawLine(x, h2, x, h2 + h2 * r);
	}

	dc.SetTextForeground(wxColor(228, 228, 228));
	dc.DrawText(audiofile, 10, 10);

	if (bytesRead == 0) {
		topWnd->SetStatusText(_("Ready"));
	} else {
		topWnd->SetStatusText(_("Reading.."));
	}
}

class AudioPanel : public wxPanel {
private:
	wxBoxSizer *sizer;
	int index;

public:
	AudioPanel(wxFrame* parent, int index) : wxPanel(parent) {
		wxString texts[5] = { "Intro", "Main loop", "With random chance", "Conditional", "Ending" };

		sizer = new wxBoxSizer(wxVERTICAL);
		wxStaticText *staticText = new wxStaticText(this, wxID_ANY, texts[index], wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
		staticText->SetBackgroundColour(wxColour(0xD0, 0xD0, 0xD0));
		sizer->Add(staticText, 0, wxALL | wxEXPAND | wxGROW, 5);
		SetSizer(sizer);

		Bind(wxEVT_PAINT, &AudioPanel::OnPaint, this);
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

	void AddWaveform(oamlAudioInfo *audio, wxFrame *topWnd) {
		WaveformDisplay *waveDisplay = new WaveformDisplay((wxFrame*)this, topWnd);
		waveDisplay->setFile(audio->filename);

		sizer->Add(waveDisplay, 0, wxALL, 5);
		Layout();
	}
};

class ScrolledWidgetsPane : public wxScrolledWindow {
private:
	wxBoxSizer* sizer;
	AudioPanel* audioPanel[5];

public:
	ScrolledWidgetsPane(wxWindow* parent, wxWindowID id) : wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL) {

		SetBackgroundColour(wxColour(0x40, 0x40, 0x40));
		SetScrollRate(50, 50);

		sizer = new wxBoxSizer(wxHORIZONTAL);

		for (int i=0; i<5; i++) {
			audioPanel[i] = new AudioPanel((wxFrame*)this, i);

			sizer->Add(audioPanel[i], 0, wxALL | wxEXPAND | wxGROW, 0);
		}

		SetSizer(sizer);
		Layout();

		sizer->Fit(this);
	}

	void AddDisplay(oamlAudioInfo *audio) {
		int i = 1;

		if (audio->type == 1) {
			i = 0;
		} else if (audio->type == 3) {
			i = 4;
		} else if (audio->type == 4) {
			i = 3;
		} else if (audio->randomChance > 0) {
			i = 2;
		}
		audioPanel[i]->AddWaveform(audio, (wxFrame*)GetParent());

		SetSizer(sizer);
		Layout();
		sizer->Fit(this);
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
	ScrolledWidgetsPane* trackPane;
	StudioTimer* timer;

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

	DECLARE_EVENT_TABLE()
};

enum {
	ID_Quit = 1,
	ID_About,
	ID_New,
	ID_Save,
	ID_SaveAs,
	ID_Load,
	ID_Export,
	ID_AddTrack,
	ID_RemoveTrack,
	ID_AddAudio,
	ID_RemoveAudio,
	ID_EditTrackName
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
END_EVENT_TABLE()

oamlApi *oaml;

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
	trackPane = new ScrolledWidgetsPane(this, wxID_ANY);
	mainSizer->Add(trackPane, 1, wxEXPAND | wxALL, 5);

	oamlTrackInfo *track = &tinfo->tracks[index];
	for (size_t i=0; i<track->audios.size(); i++) {
		oamlAudioInfo *audio = &track->audios[i];
		trackPane->AddDisplay(audio);

		SetSizer(mainSizer);
		Layout();
	}
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
