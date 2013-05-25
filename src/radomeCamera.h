//
//  turntableCam.h
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#ifndef __radome__turntableCam__
#define __radome__turntableCam__

#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class radomeCamera : public CameraPersp
{
public:
    void setDistance(float dist);
    void begin();
    void end();
    void updateWindowSize(float w, float h);
protected:
    int _windowWidth;
    int _windowHeight;
};

class turntableCamera : public radomeCamera
{
public:
    turntableCamera();
    ~turntableCamera();

    void connectMouseEvents();
    void onMouseDown(MouseEvent event);
    void onMouseDrag(MouseEvent event);
    void onMouseUp(MouseEvent event);
    
    void rotate(float leftAngle, float upAngle);
    void updateAngles();

protected:
    bool _bDragging;
    Vec2i _startDragPoint;
    
    float _theta;
    float _phi;
        
    signals::connection _mouseDownHandler;
    signals::connection _mouseDragHandler;
    signals::connection _mouseUpHandler;
};

#endif /* defined(__radome__turntableCam__) */
