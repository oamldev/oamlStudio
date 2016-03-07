#ifndef __PLAYBACKFRAME_H__
#define __PLAYBACKFRAME_H__

class PlaybackTimer;

class PlaybackFrame: public wxFrame {
private:
	wxTextCtrl *infoText;
	PlaybackTimer *timer;
	wxBitmapButton *playBtn;
	wxBitmapButton *pauseBtn;

	wxBoxSizer *mSizer;
	wxBoxSizer *hSizer;

public:
	PlaybackFrame(wxWindow *parent, wxWindowID id);
	~PlaybackFrame();

	void OnPlay(wxCommandEvent& WXUNUSED(event));
	void OnPause(wxCommandEvent& WXUNUSED(event));

	void Update();
};

class PlaybackTimer : public wxTimer {
	PlaybackFrame* pane;
public:
	PlaybackTimer(PlaybackFrame* pane);

	void Notify();
};

#endif
