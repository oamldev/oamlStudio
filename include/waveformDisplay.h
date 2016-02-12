#ifndef __WAVEFORMDISPLAY_H__
#define __WAVEFORMDISPLAY_H__

class RenderTimer : public wxTimer {
	wxWindow* pane;
public:
	RenderTimer(wxWindow* pane);

	void Notify();
};

class WaveformDisplay : public wxPanel {
private:
	RenderTimer* timer;

	std::string filename;
	audioFile *handle;
	ByteBuffer buffer;

	int peakl;
	int peakr;
	int count;

	std::vector<int> peaksL;
	std::vector<int> peaksR;

	int bytesPerSec;
	int samplesPerPixel;

	wxFrame* topWnd;

public:
	WaveformDisplay(wxFrame* parent, wxFrame* wnd);
	~WaveformDisplay();

	int read32();
	void SetSource(oamlAudioInfo *audio);

	void OnPaint(wxPaintEvent& evt);
	void OnLeftUp(wxMouseEvent& evt);
	void OnRightUp(wxMouseEvent& evt);
	void OnMenuEvent(wxCommandEvent& evt);
	void OnEraseBackground(wxEraseEvent& evt);

	std::string GetAudioFile() { return filename; }
};

#endif
