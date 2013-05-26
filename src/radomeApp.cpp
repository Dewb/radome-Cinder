
#include "radomeApp.h"


class radomeApp::DisplayMode
{
public:
    typedef function<void()> updateFn;
    DisplayMode(string n, updateFn u) { name = n; updater = u; }
    string name;
    updateFn updater;
};

void radomeApp::prepareSettings(Settings *pSettings)
{
    Window::Format defaultWindowFormat;
    Window::Format projectorWindowFormat;
    
    pSettings->prepareWindow(Window::Format().fullScreenButton());

    RendererRef projectorRenderer(new RendererGl());
    pSettings->prepareWindow(Window::Format().borderless().renderer(projectorRenderer));
}

void radomeApp::setup()
{
    getWindowIndex(1)->hide();
    
    _cam.setCenterOfInterestPoint(Vec3f(0, 0, 0));
    _cam.setAspectRatio(getWindowWidth()/getWindowHeight());
    _cam.connectMouseEvents();

    _gfx.setCamera(&_cam);
    _gfx.initializeDomeGeometry(150, 110);
    _gfx.initCubeMap(1024);
    
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
    if (getNumWindows() > 0 && getWindow() == getWindowIndex(0))
    {
        _cam.updateWindowSize(getWindowWidth(), getWindowHeight());
    
        _gfx.update();
        _ui.update();
    }
}

void radomeApp::draw()
{
    // there's got to be a better way to have different draw procs on different windows...
    if (getWindow() && getWindow() == getWindowIndex(0))
    {
        if (_currentDisplayMode < _displayModes.size())
            _displayModes[_currentDisplayMode].updater();
        _ui.draw();
    }
    else
    {
        gl::clear(Color(0, 0, 0));
    }
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

void radomeApp::browseForModel()
{
    auto path = getOpenFilePath();
    if (path.empty())
        return;

    _gfx.loadModel(path);
}

void radomeApp::showProjectorWindow(bool bShow)
{
    if (getNumWindows() < 2)
        return;
    
    WindowRef win = getWindowIndex(1);
    if (bShow)
        win->show();
    else
        win->hide();
}


CINDER_APP_NATIVE( radomeApp, RendererGl )



