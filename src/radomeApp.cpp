#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class radomeApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void radomeApp::setup()
{
}

void radomeApp::mouseDown( MouseEvent event )
{
}

void radomeApp::update()
{
}

void radomeApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( radomeApp, RendererGl )
