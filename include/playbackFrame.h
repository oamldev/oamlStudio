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

#ifndef __PLAYBACKFRAME_H__
#define __PLAYBACKFRAME_H__

class PlaybackTimer;

class PlaybackFrame: public wxFrame {
private:
	wxTextCtrl *condIdCtrl;
	wxTextCtrl *condValueCtrl;
	wxTextCtrl *infoText;
	PlaybackTimer *timer;
	wxBitmapButton *playBtn;
	wxBitmapButton *pauseBtn;
	wxButton *condBtn;

	wxBoxSizer *mSizer;
	wxBoxSizer *hSizer;

public:
	PlaybackFrame(wxWindow *parent, wxWindowID id);
	~PlaybackFrame();

	void OnClose(wxCloseEvent& event);
	void OnPlay(wxCommandEvent& WXUNUSED(event));
	void OnPause(wxCommandEvent& WXUNUSED(event));
	void OnCondition(wxCommandEvent& WXUNUSED(event));

	void Update();
};

class PlaybackTimer : public wxTimer {
	PlaybackFrame* pane;
public:
	PlaybackTimer(PlaybackFrame* pane);

	void Notify();
};

#endif
