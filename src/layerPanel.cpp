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


LayerPanel::LayerPanel(wxFrame* parent) : wxPanel(parent) {
	sizer = new wxBoxSizer(wxHORIZONTAL);

	vSizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(vSizer);

	wxStaticLine *staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	sizer->Add(staticLine, 0, wxEXPAND | wxALL, 0);

	SetSizer(sizer);
	Layout();
}

void LayerPanel::LoadLayers() {
	int waveformHeight = 104;

	std::vector<std::string> list;
	studioApi->LayerList(list);

	wxStaticText *staticText = new wxStaticText(this, wxID_ANY, wxString("-- Layers --"));
	vSizer->Add(staticText, 0, wxALL, 5);

	wxSize s = staticText->GetClientSize();
	vSizer->Add(0, waveformHeight/2 - s.GetHeight());

	for (std::vector<std::string>::iterator it=list.begin(); it<list.end(); ++it) {
		staticText = new wxStaticText(this, wxID_ANY, wxString(it->c_str()));
		vSizer->Add(staticText, 0, wxALL, 5);

		s = staticText->GetClientSize();
		vSizer->Add(0, waveformHeight - s.GetHeight());
	}

	Layout();
}
