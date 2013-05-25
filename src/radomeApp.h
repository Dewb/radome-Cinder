//
//  radomeApp.h
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#ifndef radome_radomeApp_h
#define radome_radomeApp_h

#include "cinder/app/AppNative.h"

#include "radomeAppUI.h"
#include "radomeGraphics.h"

#include "radomeCamera.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class radomeApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
    
    vector<string> getDisplayModeNames();
    void setDisplayMode(int modeIndex);
    void cycleDisplayMode();
    int getCurrentDisplayMode() { return _currentDisplayMode; }
    
    void keyDown(KeyEvent event);
    
protected:
    
    radomeAppUI _ui;
    radomeGraphics _gfx;

    turntableCamera _cam;

    class DisplayMode;
    vector<DisplayMode> _displayModes;
    int _currentDisplayMode;
};

#endif
