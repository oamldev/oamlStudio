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
	int trackIndex;
	int audioIndex;

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
	void SetSource(int trackIdx, int audioIdx);
	void OnPaint(wxPaintEvent& evt);
	void OnLeftUp(wxMouseEvent& evt);
};

#endif
