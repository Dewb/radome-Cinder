
#include "radomeApp.h"

class radomeApp::DisplayMode
{
public:
    typedef function<void()> updateFn;
    DisplayMode(string n, updateFn u) { name = n; updater = u; }
    string name;
    updateFn updater;
};


void radomeApp::setup()
{
    _cam.setCenterOfInterestPoint(Vec3f(0, 0, 0));
    _cam.setAspectRatio(getWindowWidth()/getWindowHeight());
    _cam.connectMouseEvents();

    _gfx.setCamera(&_cam);
    _gfx.initializeDomeGeometry(150, 110);
    
    _displayModes.push_back(DisplayMode("3D Scene",       [this]() { _gfx.display3DScene(); }));
    _displayModes.push_back(DisplayMode("Cube Map",       [this]() { _gfx.displayCubeMap(); }));
    _displayModes.push_back(DisplayMode("Dome Preview",   [this]() { _gfx.displayDomePreview(); }));
    //_displayModes.push_back(DisplayMode("Output Preview", [this]() { _gfx.displayProjectorOutput(); }));

    _currentDisplayMode = 0;
    _ui.init(this);
}

void radomeApp::mouseDown(MouseEvent event)
{
}

void radomeApp::keyDown(KeyEvent event)
{
    _ui.keyDown(event);
}

void radomeApp::update()
{
    _cam.updateWindowSize(getWindowWidth(), getWindowHeight());
    
    _ui.update();
}

void radomeApp::draw()
{
    if (_currentDisplayMode < _displayModes.size())
        _displayModes[_currentDisplayMode].updater();

    _ui.draw();
}

void radomeApp::setDisplayMode(int modeIndex)
{
    if (modeIndex >= 0 && modeIndex < _displayModes.size())
        _currentDisplayMode = modeIndex;
}

void radomeApp::cycleDisplayMode()
{
    _currentDisplayMode = (_currentDisplayMode + 1) % _displayModes.size();
}

vector<string> radomeApp::getDisplayModeNames()
{
    vector<string> names;
    for (auto m : _displayModes)
    {
        names.push_back(m.name);
    }
    return names;
}
CINDER_APP_NATIVE( radomeApp, RendererGl )
