//
//  radomeProjector.h
//  radome
//
//  Created by Michael Dewberry on 5/26/13.
//
//

#ifndef __radome__radomeProjector__
#define __radome__radomeProjector__

#include "cinder/gl/fbo.h"
#include "radomeCamera.h"

using namespace std;

class radomeProjector {
public:
    radomeProjector(Vec2i resolution, float heading, float distance, float height, float fov = 30, float targetHeight = 20);
    void drawSceneRepresentation(Color color = Color::white());
    void drawFramebuffer(int x, int y, int w, int h);
    
    void renderBegin();
    void renderEnd();
    
    void setHeading(float h) { _heading = h; updateCamera(); }
    float getHeading() const { return _heading; }
    void setDistance(float d) { _distance = d; updateCamera(); }
    float getDistance() const { return _distance; }
    void setHeight(float h) { _height = h; updateCamera(); }
    float getHeight() const { return _height; }
    void setFOV(float f) { _fov = f; updateCamera(); }
    float getFOV() const { return _fov; }
    void setTargetHeight(float h) { _targetHeight = h; updateCamera(); }
    float getTargetHeight() const { return _targetHeight; }
    
protected:
    void updateCamera();
    
    radomeCamera _camera;
    gl::Fbo _fbo;
    
    float _heading;
    float _distance;
    float _height;
    float _fov;
    float _targetHeight;
};

/*
class radomeProjectorWindowController
{
public:
    radomeProjectorWindowController(vector<radomeProjector*>* pProjectors);
    void setup();
    void draw();
protected:
    vector<radomeProjector*>* _pProjectors;
};
*/

#endif /* defined(__radome__radomeProjector__) */

