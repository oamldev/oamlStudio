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
#include "tinyxml2.h"

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
#include <archive.h>
#include <archive_entry.h>


wxIMPLEMENT_APP_NO_MAIN(oamlStudio);

oamlApi *oaml;
oamlStudioApi *studioApi;
std::string projectPath = "";

bool oamlStudio::OnInit() {
	oaml = new oamlApi();
	studioApi = oaml->GetStudioApi();
	printf("Initializing OAML v%s\n", oaml->GetVersion());
	oaml->InitAudioDevice();
	oaml->SetFileCallbacks(&studioCbs);

	StudioFrame *frame = new StudioFrame(_("oamlStudio"), wxPoint(0, 0), wxSize(1024, 768), wxDEFAULT_FRAME_STYLE | wxMAXIMIZE);
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

int main(int argc, char** argv) {
	oamlStudio* app = new oamlStudio();
	wxApp::SetInstance(app);
	return wxEntry(argc, argv);
}

#ifdef _MSC_VER
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
	return main(0, NULL);
}
#endif
