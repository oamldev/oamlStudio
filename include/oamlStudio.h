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

#ifndef __OAMLSTUDIO_H__
#define __OAMLSTUDIO_H__

#include <wx/wx.h>
#include <SDL/SDL.h>

extern void InitCallbacks(std::string prjPath);
extern oamlFileCallbacks studioCbs;
extern oamlApi *oaml;

oamlTrackInfo* GetTrackInfo(std::string trackName);
oamlAudioInfo* GetAudioInfo(std::string trackName, std::string audioFile);
void AddAudioInfo(std::string trackName, oamlAudioInfo& audio);
void RemoveAudioInfo(std::string trackName, std::string audioFile);

wxDECLARE_EVENT(EVENT_ADD_AUDIO, wxCommandEvent);
wxDECLARE_EVENT(EVENT_REMOVE_AUDIO, wxCommandEvent);
wxDECLARE_EVENT(EVENT_SELECT_AUDIO, wxCommandEvent);
wxDECLARE_EVENT(EVENT_RELOAD_DEFS, wxCommandEvent);
wxDECLARE_EVENT(EVENT_PLAY, wxCommandEvent);

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
	ID_EditTrackName,
	ID_Play,
	ID_Pause,
	ID_Recent,
	ID_Condition,
	ID_AddLayer,
	ID_RemveLayer
};

class oamlStudio : public wxApp {
public:
	virtual bool OnInit();

	int OpenSDL();
};

#endif /* __OAMLSTUDIO_H__ */
