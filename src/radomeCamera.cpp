//
//  turntableCam.cpp
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#include "radomeCamera.h"
#include "cinder/gl/gl.h"

#include <iostream>
#include <algorithm>

#define PI 3.14159
#define EPS 0.000001

using namespace ci;
using namespace std;

void radomeCamera::begin()
{
    gl::pushMatrices();
    gl::setMatrices(*this);
}

void radomeCamera::end()
{
    gl::popMatrices();
}

void radomeCamera::setDistance(float dist)
{
    Vec3f target = getCenterOfInterestPoint();
    Vec3f eye = target + getViewDirection().inverse().normalized() * dist;
    lookAt(eye, target);
    setCenterOfInterest(dist); // Bug in ci::Camera? lookAt doesn't actually update centerOfInterest float
}

void radomeCamera::updateWindowSize(float w, float h)
{
    _windowWidth = w;
    _windowHeight = h;
    setAspectRatio(w/h);
    calcProjection(); // ci::Camera should arguably be doing this in setAspectRatio()
}


turntableCamera::turntableCamera()
: _theta(0)
, _phi(PI/2)
{
    updateAngles();
}

turntableCamera::~turntableCamera()
{
    _mouseDownHandler.disconnect();
    _mouseDragHandler.disconnect();
    _mouseUpHandler.disconnect();
}


void turntableCamera::connectMouseEvents()
{
    auto win = app::App::get()->getWindowIndex(0);
    _mouseDownHandler = win->connectMouseDown( &turntableCamera::onMouseDown, this );
    _mouseDragHandler = win->connectMouseDrag( &turntableCamera::onMouseDrag, this );
    _mouseUpHandler = win->connectMouseUp( &turntableCamera::onMouseUp, this );
}

void turntableCamera::onMouseDown(MouseEvent event)
{
    if (!_bDragging)
    {
        _bDragging = true;
        _startDragPoint = event.getPos();
    }
}

void turntableCamera::onMouseDrag(MouseEvent event)
{
    float dx = (event.getX() - _startDragPoint.x) / (_windowWidth * 1.0);
    float dy = (event.getY() - _startDragPoint.y) / (_windowHeight * 1.0);
        
    float rotateSpeed = 750.0;
    
    rotate(-2 * PI * dx / (_windowWidth * 0.8) * rotateSpeed,
           -2 * PI * dy / (_windowHeight * 0.8) * rotateSpeed);
    updateAngles();

    _startDragPoint = event.getPos();
}

void turntableCamera::onMouseUp(MouseEvent event)
{
    _bDragging = false;
}

void turntableCamera::rotate(float leftAngle, float upAngle)
{
    _phi = fmax(EPS, fmin(PI - EPS, _phi + upAngle));
    _theta += leftAngle;
}

void turntableCamera::updateAngles()
{
    float radius = getCenterOfInterest();
    Vec3f eye, target = getCenterOfInterestPoint();
    eye.x = radius * sin(_phi) * sin(_theta);
    eye.y = radius * cos(_phi);
    eye.z = radius * sin(_phi) * cos(_theta);
    lookAt(eye, target);
    setCenterOfInterest(radius);
}







