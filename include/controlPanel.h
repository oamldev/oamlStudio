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

#ifndef __CONTROLPANEL_H__
#define __CONTROLPANEL_H__

class ControlTimer;

class ControlPanel : public wxPanel {
private:
	wxTextCtrl *fileCtrl;
	wxTextCtrl *bpmCtrl;
	wxTextCtrl *bpbCtrl;
	wxTextCtrl *barsCtrl;
	wxTextCtrl *randomChanceCtrl;
	wxTextCtrl *minMovementBarsCtrl;
	wxTextCtrl *fadeInCtrl;
	wxTextCtrl *fadeOutCtrl;
	wxTextCtrl *xfadeInCtrl;
	wxTextCtrl *xfadeOutCtrl;
	wxTextCtrl *condIdCtrl;
	wxTextCtrl *condTypeCtrl;
	wxTextCtrl *condValueCtrl;
	wxTextCtrl *condValue2Ctrl;

	wxBoxSizer *mSizer;
	wxBoxSizer *hSizer;
	wxGridSizer *sizer;
	std::string trackName;
	std::string audioFile;

public:
	ControlPanel(wxFrame* parent, wxWindowID id);
	~ControlPanel();

	const char *GetTrack() const { return trackName.c_str(); }

	void OnBpmChange(wxCommandEvent& WXUNUSED(event));
	void OnBpbChange(wxCommandEvent& WXUNUSED(event));
	void OnBarsChange(wxCommandEvent& WXUNUSED(event));
	void OnRandomChanceChange(wxCommandEvent& WXUNUSED(event));
	void OnMinMovementBarsChange(wxCommandEvent& WXUNUSED(event));
	void OnFadeInChange(wxCommandEvent& WXUNUSED(event));
	void OnFadeOutChange(wxCommandEvent& WXUNUSED(event));
	void OnXFadeInChange(wxCommandEvent& WXUNUSED(event));
	void OnXFadeOutChange(wxCommandEvent& WXUNUSED(event));
	void OnCondIdChange(wxCommandEvent& WXUNUSED(event));
	void OnCondTypeChange(wxCommandEvent& WXUNUSED(event));
	void OnCondValueChange(wxCommandEvent& WXUNUSED(event));
	void OnCondValue2Change(wxCommandEvent& WXUNUSED(event));
	void OnPlay(wxCommandEvent& WXUNUSED(event));
	void OnPause(wxCommandEvent& WXUNUSED(event));
	void SetTrack(std::string name);
	void OnSelectAudio(std::string audio);

	void UpdateTrackName(std::string oldName, std::string newName);
};

#endif
