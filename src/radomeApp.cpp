
#include "radomeApp.h"


void radomeApp::setup()
{
    _ui.init(this);
    _cam.setCenterOfInterestPoint(Vec3f(0, 0, 0));
    _cam.setAspectRatio(getWindowWidth()/getWindowHeight());
    _cam.connectMouseEvents();

    _gfx.setCamera(&_cam);
    _gfx.initializeDomeGeometry(150, 110);

}

void radomeApp::mouseDown( MouseEvent event )
{
}

void radomeApp::update()
{
    _cam.updateWindowSize(getWindowWidth(), getWindowHeight());
    
    _ui.update();
}

void radomeApp::draw()
{
	_gfx.display3DScene();
    //_gfx.displayCubeMap();
    _ui.draw();
}

CINDER_APP_NATIVE( radomeApp, RendererGl )
