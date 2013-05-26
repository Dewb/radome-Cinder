//
//  radomeProjector.cpp
//  radome
//
//  Created by Michael Dewberry on 5/26/13.
//
//

#include "radomeProjector.h"

radomeProjector::radomeProjector(Vec2i resolution, float heading, float distance, float height, float fov, float targetHeight)
: _heading(heading)
, _distance(distance)
, _height(height)
, _fov(fov)
, _targetHeight(targetHeight)
{
    _fbo = gl::Fbo(resolution.x, resolution.y);
    _fbo.bindFramebuffer();
    gl::clear();
    _fbo.unbindFramebuffer();

    updateCamera();
}

void radomeProjector::updateCamera()
{
    _camera.setPerspective(_fov, _fbo.getWidth()/_fbo.getHeight(), 0.1, 90000.0);
    _camera.setEyePoint(Vec3f(_distance * cos(_heading*3.14159/180.0), _height, _distance * sin(_heading*3.14159/180.0)));
    _camera.lookAt(Vec3f(0.0, _targetHeight, 0.0));
}

void radomeProjector::renderBegin()
{
    _fbo.bindTexture();
    gl::clear();
    _camera.begin();
}

void radomeProjector::renderEnd()
{
    _camera.end();
    _fbo.unbindTexture();
}

void radomeProjector::drawFramebuffer(int x, int y, int w, int h)
{
    _fbo.blitToScreen(Area(0, 0, _fbo.getWidth(), _fbo.getHeight()), Area(x, y, w, h));
}

void radomeProjector::drawSceneRepresentation(Color color)
{
    gl::pushMatrices();
    
    gl::rotate(Vec3f(0, _heading, 0));
    gl::translate(Vec3f(_distance, 0, 0));
    
    Vec3f boxSize(30.0, 12.0, 30.0);
    Vec3f tripodTop(boxSize.x / 2.0, _height - boxSize.y, 0.0);
    
    for (int pass = 0; pass < 2; pass++)
    {
        if (pass == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            gl::color(color);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            gl::color(color * 0.7);
        }
        
        gl::lineWidth(3.0);
        gl::drawCube(Vec3f(boxSize.x / 2.0, _height - boxSize.y / 2.0, 0.0), boxSize);
        gl::lineWidth(5.0);
        gl::drawLine(tripodTop, Vec3f(           0.0, 0.0,  boxSize.z / 2.0));
        gl::drawLine(tripodTop, Vec3f(           0.0, 0.0, -boxSize.z / 2.0));
        gl::drawLine(tripodTop, Vec3f(boxSize.x, 0.0,  0.0));
    }

    gl::popMatrices();
}

/*
radomeProjectorWindowController::radomeProjectorWindowController(vector<radomeProjector*>* pProjectors) {
    _pProjectors = pProjectors;
}

void radomeProjectorWindowController::setup() {
    
}

void radomeProjectorWindowController::draw() {
    if (_pProjectors) {
        int w = getWidth() / _pProjectors->size();
        int h = getHeight();
        int x = 0;
        for (auto p : *_pProjectors) {
            p->drawFramebuffer(x, 0, w, h);
            x += w;
        }
    }
}
*/