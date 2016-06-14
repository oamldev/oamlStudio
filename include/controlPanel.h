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

#include <wx/spinctrl.h>

class ControlTimer;

class ControlPanel : public wxPanel {
private:
	wxTextCtrl *nameCtrl;
	wxTextCtrl *fileCtrl;
	wxSpinCtrlDouble *volumeCtrl;
	wxSpinCtrlDouble *bpmCtrl;
	wxSpinCtrlDouble *bpbCtrl;
	wxSpinCtrlDouble *barsCtrl;
	wxSpinCtrlDouble *randomChanceCtrl;
	wxSpinCtrlDouble *minMovementBarsCtrl;
	wxSpinCtrlDouble *fadeInCtrl;
	wxSpinCtrlDouble *fadeOutCtrl;
	wxSpinCtrlDouble *xfadeInCtrl;
	wxSpinCtrlDouble *xfadeOutCtrl;
	wxTextCtrl *condIdCtrl;
	wxComboBox *condTypeCtrl;
	wxTextCtrl *condValueCtrl;
	wxTextCtrl *condValue2Ctrl;
	wxTextCtrl *afLayerCtrl;
	wxSpinCtrlDouble *afRandomChanceCtrl;

	wxBoxSizer *mSizer;
	wxBoxSizer *hSizer;
	wxGridSizer *sizer;
	std::string trackName;
	std::string audioName;
	std::string filename;

	bool musicMode;

	void MarkProjectDirty();

public:
	ControlPanel(wxFrame* parent, wxWindowID id);
	~ControlPanel();

	const char *GetAudioName() const { return audioName.c_str(); }
	const char *GetTrack() const { return trackName.c_str(); }

	void OnAFLayerChange(wxCommandEvent& event);
	void OnAFRandomChanceChange(wxCommandEvent& event);
	void OnNameChange(wxCommandEvent& event);
	void OnVolumeChange(wxCommandEvent& event);
	void OnBpmChange(wxCommandEvent& event);
	void OnBpbChange(wxCommandEvent& event);
	void OnBarsChange(wxCommandEvent& event);
	void OnRandomChanceChange(wxCommandEvent& event);
	void OnMinMovementBarsChange(wxCommandEvent& event);
	void OnFadeInChange(wxCommandEvent& event);
	void OnFadeOutChange(wxCommandEvent& event);
	void OnXFadeInChange(wxCommandEvent& event);
	void OnXFadeOutChange(wxCommandEvent& event);
	void OnCondIdChange(wxCommandEvent& event);
	void OnCondTypeChange(wxCommandEvent& event);
	void OnCondValueChange(wxCommandEvent& event);
	void OnCondValue2Change(wxCommandEvent& event);
	void OnPlay(wxCommandEvent& event);
	void OnPause(wxCommandEvent& event);
	void SetTrack(std::string name);
	void OnSelectAudio(std::string _audioName, std::string _filename);

	std::string GetTrackName() const { return trackName; }
	void UpdateTrackName(std::string oldName, std::string newName);

	void SetTrackMode(bool mode);

	bool IsMusicMode() const { return musicMode; }
};

#endif
