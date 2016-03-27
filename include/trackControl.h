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

#ifndef __TRACKCONTROL_H__
#define __TRACKCONTROL_H__

class TrackControl : public wxPanel {
private:
	wxTextCtrl *volumeCtrl;
	wxTextCtrl *fadeInCtrl;
	wxTextCtrl *fadeOutCtrl;
	wxTextCtrl *xfadeInCtrl;
	wxTextCtrl *xfadeOutCtrl;

	wxBoxSizer *mSizer;
	wxBoxSizer *hSizer;
	wxGridSizer *sizer;
	std::string trackName;

public:
	TrackControl(wxFrame* parent, wxWindowID id);
	~TrackControl();

	const char *GetTrack() const { return trackName.c_str(); }

	void OnVolumeChange(wxCommandEvent& WXUNUSED(event));
	void OnFadeInChange(wxCommandEvent& WXUNUSED(event));
	void OnFadeOutChange(wxCommandEvent& WXUNUSED(event));
	void OnXFadeInChange(wxCommandEvent& WXUNUSED(event));
	void OnXFadeOutChange(wxCommandEvent& WXUNUSED(event));
	void SetTrack(std::string name);

	std::string GetTrackName() const { return trackName; }
	void UpdateTrackName(std::string oldName, std::string newName);
};

#endif
