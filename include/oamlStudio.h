#ifndef __OAMLSTUDIO_H__
#define __OAMLSTUDIO_H__

#include <wx/wx.h>


extern oamlApi *oaml;

oamlTrackInfo* GetTrackInfo(int trackIndex);
oamlAudioInfo* GetAudioInfo(int trackIndex, int audioIndex);
int AddAudioInfo(int trackIndex, oamlAudioInfo& audio);

wxDECLARE_EVENT(EVENT_ADD_AUDIO, wxCommandEvent);
wxDECLARE_EVENT(EVENT_SELECT_AUDIO, wxCommandEvent);

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

class oamlStudio : public wxApp {
public:
	virtual bool OnInit();
};

#endif /* __OAMLSTUDIO_H__ */
