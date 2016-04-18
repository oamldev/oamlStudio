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
#include <archive.h>
#include <archive_entry.h>


wxIMPLEMENT_APP_NO_MAIN(oamlStudio);

oamlApi *oaml;

oamlTrackInfo* GetTrackInfo(std::string trackName) {
	oamlTracksInfo* info = oaml->GetTracksInfo();
	if (info == NULL)
		return NULL;

	for (size_t i=0; i<info->tracks.size(); i++) {
		if (info->tracks[i].name == trackName) {
			return &info->tracks[i];
		}
	}

	return NULL;
}

oamlAudioInfo* GetAudioInfo(std::string trackName, std::string audioFile) {
	oamlTrackInfo* track = GetTrackInfo(trackName);
	if (track == NULL)
		return NULL;

	for (size_t i=0; i<track->audios.size(); i++) {
		for (size_t j=0; j<track->audios[i].layers.size(); j++) {
			if (track->audios[i].layers[j].filename == audioFile) {
				return &track->audios[i];
			}
		}
	}
	return NULL;
}

oamlLayerInfo* GetLayerInfo(std::string trackName, std::string audioFile) {
	oamlTrackInfo* track = GetTrackInfo(trackName);
	if (track == NULL)
		return NULL;

	for (size_t i=0; i<track->audios.size(); i++) {
		for (size_t j=0; j<track->audios[i].layers.size(); j++) {
			if (track->audios[i].layers[j].filename == audioFile) {
				return &track->audios[i].layers[j];
			}
		}
	}
	return NULL;
}

void AddAudioInfo(std::string trackName, oamlAudioInfo& audio) {
	oamlTrackInfo* info = GetTrackInfo(trackName);
	if (info == NULL)
		return;

	info->audios.push_back(audio);
}

void RemoveAudioInfo(std::string trackName, std::string audioFile) {
	oamlTrackInfo* track = GetTrackInfo(trackName);
	if (track == NULL)
		return;

	for (std::vector<oamlAudioInfo>::iterator audio=track->audios.begin(); audio<track->audios.end(); ++audio) {
		for (std::vector<oamlLayerInfo>::iterator layer=audio->layers.begin(); layer<audio->layers.end(); ++layer) {
			if (layer->filename == audioFile) {
				audio->layers.erase(layer);
				if (audio->layers.size() == 0) {
					track->audios.erase(audio);
				}
				return;
			}
		}
	}
}

void RenameTrack(std::string trackName, std::string newName) {
	oamlTracksInfo* info = oaml->GetTracksInfo();
	if (info == NULL)
		return;

	for (size_t i=0; i<info->tracks.size(); i++) {
		if (info->tracks[i].name == trackName) {
			info->tracks[i].name = newName;
			break;
		}
	}
}

void audioCallback(void* WXUNUSED(userdata), Uint8* stream, int len) {
	oaml->MixToBuffer(stream, len/2);
}

int oamlStudio::OpenSDL() {
	SDL_AudioSpec spec;

	SDL_memset(&spec, 0, sizeof(spec));
	spec.freq = 44100;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 4096;
	spec.callback = audioCallback;

	if (SDL_OpenAudio(&spec, NULL) < 0) {
		fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
		return -1;
	}

	oaml->SetAudioFormat(44100, 2, 2);

	SDL_PauseAudio(0);

	return 0;
}

bool oamlStudio::OnInit() {
	oaml = new oamlApi();
	oaml->SetFileCallbacks(&studioCbs);

	if (OpenSDL() == -1)
		return false;

	StudioFrame *frame = new StudioFrame(_("oamlStudio"), wxPoint(0, 0), wxSize(1024, 768), wxDEFAULT_FRAME_STYLE | wxMAXIMIZE);
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

#undef main
int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "Could not initialize SDL.\n";
		return 1;
	}

	oamlStudio* app = new oamlStudio();
	wxApp::SetInstance(app);
	return wxEntry(argc, argv);
}

#ifdef _MSC_VER
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
	return main(0, NULL);
}
#endif
