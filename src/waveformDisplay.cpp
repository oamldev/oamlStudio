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
#include <wx/dcbuffer.h>

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



RenderTimer::RenderTimer(wxWindow* pane) : wxTimer() {
	RenderTimer::pane = pane;
}

void RenderTimer::Notify() {
	pane->Refresh();
}


WaveformDisplay::WaveformDisplay(wxFrame* parent, wxFrame* wnd) : wxPanel(parent) {
	topWnd = wnd;
	handle = NULL;
	timer = NULL;

	Bind(wxEVT_PAINT, &WaveformDisplay::OnPaint, this);
	Bind(wxEVT_LEFT_UP, &WaveformDisplay::OnLeftUp, this);
	Bind(wxEVT_RIGHT_UP, &WaveformDisplay::OnRightUp, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &WaveformDisplay::OnMenuEvent, this, ID_RemoveAudio);
	Bind(wxEVT_ERASE_BACKGROUND, &WaveformDisplay::OnEraseBackground, this);
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

void WaveformDisplay::SetSource(oamlAudioInfo* audio) {
	filename = audio->filename;

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

void WaveformDisplay::OnLeftUp(wxMouseEvent& WXUNUSED(evt)) {
	wxCommandEvent event(EVENT_SELECT_AUDIO);
	event.SetString(wxString(filename));
	wxPostEvent(GetParent(), event);
}

void WaveformDisplay::OnRightUp(wxMouseEvent& WXUNUSED(evt)) {
	wxMenu menu(wxT(""));
	menu.Append(ID_RemoveAudio, wxT("&Remove Audio"));
	PopupMenu(&menu);
}

void WaveformDisplay::OnEraseBackground(wxEraseEvent& WXUNUSED(evt)) {
}

void WaveformDisplay::OnMenuEvent(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_RemoveAudio:
			wxCommandEvent event(EVENT_REMOVE_AUDIO);
			event.SetString(wxString(filename));
			wxPostEvent(GetParent(), event);
			break;
	}
}

void WaveformDisplay::OnPaint(wxPaintEvent&  WXUNUSED(evt)) {
	if (handle == NULL || handle->GetTotalSamples() == 0)
		return;

	wxBufferedPaintDC dc(this);

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
	dc.DrawText(filename.c_str(), 10, 10);

	if (bytesRead == 0) {
		topWnd->SetStatusText(_("Ready"));
	} else {
		topWnd->SetStatusText(_("Reading.."));
	}
}
