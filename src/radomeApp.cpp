
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
    pSettings->prepareWindow(Window::Format().fullScreenButton());
}

void radomeApp::setup()
{
    getWindowIndex(0)->connectDraw(&radomeApp::drawMainWindow, this);
        
    _cam.setCenterOfInterestPoint(Vec3f(0, 0, 0));
    _cam.setAspectRatio(getWindowWidth()/getWindowHeight());
    _cam.connectMouseEvents();

    _gfx.setCamera(&_cam);
    _gfx.initializeDomeGeometry(150, 110);
    _gfx.initCubeMap(1024);
    _gfx.initProjectors(3, Vec2i(1280,800), 150, 150);
    
    _displayModes.push_back(DisplayMode("3D Scene",       [this]() { _gfx.display3DScene(); }));
    _displayModes.push_back(DisplayMode("Cube Map",       [this]() { _gfx.displayCubeMap(); }));
    _displayModes.push_back(DisplayMode("Dome Preview",   [this]() { _gfx.displayDomePreview(); }));
    _displayModes.push_back(DisplayMode("Output Preview", [this]() { _gfx.displayProjectorOutput(); }));

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

void radomeApp::drawMainWindow()
{
    if (_currentDisplayMode < _displayModes.size())
        _displayModes[_currentDisplayMode].updater();
    _ui.draw();
}

void radomeApp::drawProjWindow()
{
    gl::clear(Color(0.6, 0, 0.4));
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
    {
        if (bShow)
        {
            WindowRef projWindow = createWindow(Window::Format().size(640,160).fullScreenButton());
            projWindow->connectDraw(&radomeApp::drawProjWindow, this);
        }
        return;
    }
    
    WindowRef win = getWindowIndex(1);
    if (bShow)
        win->show();
    else
        win->hide();
}


CINDER_APP_NATIVE( radomeApp, RendererGl )



