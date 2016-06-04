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

#ifndef __LAYERPANEL_H__
#define __LAYERPANEL_H__

class LayerPanel : public wxPanel {
private:
	wxBoxSizer *sizer;
	std::vector<WaveformDisplay*> waveDisplays;

public:
	LayerPanel(wxFrame* parent);

	bool IsEmpty();

	void AddWaveform(std::string filename, std::string audioName, bool sfxMode, wxFrame *topWnd);
	void RemoveWaveform(std::string filename);
	void UpdateAudioName(std::string oldName, std::string newName);

	void OnPaint(wxPaintEvent& WXUNUSED(evt));

	void AddLayerDialog();
};

#endif
