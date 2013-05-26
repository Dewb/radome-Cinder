//
//  radomeGraphics.cpp
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#include "radomeGraphics.h"
#include "icosohedron.h"
#include "radomeCamera.h"
#include "radomeModel.h"
#include "radomeProjector.h"
#include "CubeMap.h"
#include "Resources.h"

#include "cinder/gl/gl.h"


using namespace ci;
using namespace std;

radomeGraphics::radomeGraphics()
{
    _pCubeMap = new CubeMap();
}

radomeGraphics::~radomeGraphics()
{
    if (_pCubeMap)
        delete _pCubeMap;
}

void radomeGraphics::setCamera(radomeCamera* pCamera)
{
    _pCamera = pCamera;
}

void radomeGraphics::initCubeMap(int textureSize)
{
    _pCubeMap->initEmptyTextures(textureSize, GL_RGBA);
    _pCubeMap->setNearFar(Vec2f(0.01, 8192.0));
}

void radomeGraphics::initializeDomeGeometry(int radius, int height)
{
    _domeRadius = radius;
    _domeHeight = height;
    
    int sx = radius, sy = height, sz = radius;
    _domeDrawListIndex = glGenLists(1);
    glNewList(_domeDrawListIndex, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    
    auto triangles = icosohedron::createsphere(4);
    for (auto t : triangles)
    {        
        float dx, dy, dz;
        
        dx = t.v0[0] * sx;
        dy = t.v0[1] * sy;
        dz = t.v0[2] * sz;
        glNormal3f(dx, dy, dz);
        glVertex3f(dx, dy, dz);
        
        dx = t.v1[0] * sx;
        dy = t.v1[1] * sy;
        dz = t.v1[2] * sz;
        glNormal3f(dx, dy, dz);
        glVertex3f(dx, dy, dz);
        
        dx = t.v2[0] * sx;
        dy = t.v2[1] * sy;
        dz = t.v2[2] * sz;
        glNormal3f(dx, dy, dz);
        glVertex3f(dx, dy, dz);
    }
    
    glEnd();
    glEndList();
    
    _shader = gl::GlslProg::create(loadResource(RADOME_VERTEX_SHADER), loadResource(RADOME_FRAGMENT_SHADER));
    _testPatternTexture = gl::Texture::create( loadImage( loadResource( RES_TEST_PATTERN_JPG ) ) );

}

void radomeGraphics::initProjectors(int count, Vec2i resolution, float distance, float height)
{
    for (int ii = 0; ii < count; ii++) {
        float heading = ii * 360.0/(count*1.0) + 60.0;
        _projectorList.push_back(new radomeProjector(resolution, heading, distance, height));
    }
}

void radomeGraphics::update()
{
    for (auto model : _modelList) {
        model->update();
    }
    
    renderToCubeMap();
    renderToProjectors();
}

void radomeGraphics::renderToCubeMap()
{
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::pushMatrices();
    
    for(int i = 0; i < 6; i++) {
        _pCubeMap->beginDrawingInto3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        gl::clear(Color(0,0,1.0));
        internalDrawScene();
        _pCubeMap->endDrawingInto3D();

        //_pCubeMap->beginDrawingInto2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        //gl::clear(Color(0,0,1.0));
        //_pCubeMap->endDrawingInto2D();
        
    }
    gl::popMatrices();
}

void radomeGraphics::renderToProjectors()
{
    
}

void radomeGraphics::display3DScene()
{
    if (!_pCamera)
        return;
    
	gl::clear(Color( 0, 0, 0 ));
    gl::enableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    _pCamera->setDistance(_domeRadius * 4.3);
    _pCamera->begin();
    
    gl::pushMatrices();
    internalDrawScene();
    gl::popMatrices();
    
    gl::color(0.5, 0.5, 1.0, 0.5);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    gl::lineWidth(1);
    internalDrawDome();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    gl::color(0.31, 0.31, 0.75, 0.5);
    internalDrawGroundPlane();
    
    //_pCubeMap->debugDrawCubemapCameras();
    
    _pCamera->end();
}

void radomeGraphics::internalDrawScene()
{
    for (auto model : _modelList)
    {
        model->drawWithTransform();
    }
}

void radomeGraphics::internalDrawDome()
{
    double clipPlane[4] = { 0.0, 1.0, 0.0, -0.1 };
    glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE0, clipPlane);
    glCallList(_domeDrawListIndex);
    glDisable(GL_CLIP_PLANE0);
}

void radomeGraphics::internalDrawGroundPlane() {
    float size = _domeRadius * 10;
    float ticks = 40.0;
    
    float step = size / ticks;
    float major =  step * 2.0f;
    
    for (float k =- size; k <= size; k += step) {
        if (fabs(k) == size || k == 0)
            gl::lineWidth(4);
        else if (k / major == floor(k / major))
            gl::lineWidth(2);
        else
            gl::lineWidth(1);
        
        gl::drawLine(Vec3f(k, 0, -size), Vec3f(k, 0, size));
        gl::drawLine(Vec3f(-size, 0, k), Vec3f(size, 0, k));
    }
}

void radomeGraphics::displayCubeMap()
{
    gl::clear(Color( 0, 0, 0.1 ));

    int SIDEBAR_WIDTH = 180; //todo: fix this
    gl::color(Color(0.78, 0.86, 1.0));
    int margin = 2;
    int w = (getWindowWidth() - SIDEBAR_WIDTH - margin * 4) / 3;
    int h = (getWindowHeight() - margin * 3) / 2;
    for (int i = 0; i < 6; i++) {
        int x = 2 * margin + i % 3 * (w + margin) + SIDEBAR_WIDTH;
        int y = margin + i / 3 * (h + margin);
        //gl::drawSolidRect(Rectf(x - 1, y - 1, x - 1 + w + margin, y - 1 + h + margin));
        _pCubeMap->drawFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i , x, y, w, h);
        gl::drawString(_pCubeMap->getDescriptiveStringForFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                       Vec2f(x + margin * 1.5, y + 10 + margin * 1.5));
    }
}


void radomeGraphics::displayDomePreview()
{
    if (!_pCamera)
        return;    
    
    gl::clear(Color( 0.4, 0.03, 0.02 ));
    gl::enableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
        
    _pCamera->setDistance(_domeRadius * 2.20);
    _pCamera->begin();
    
    beginShader();
    internalDrawDome();
    internalDrawGroundPlane();
    endShader();
        
    for (auto p : _projectorList)
    {
        p->drawSceneRepresentation(Color(0.72, 0.15, 0.1));
    }
    
    _pCamera->end();
}


void radomeGraphics::displayProjectorOutput()
{
    if (!_pCamera)
        return;
    
    gl::clear();
    gl::enableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    int SIDEBAR_WIDTH = 180;
    glColor3b(200, 220, 255);
    int margin = 2;
    int w = (getWindowWidth() - SIDEBAR_WIDTH - margin * 4) / 2;
    int h = (getWindowHeight() - margin * 3) / 2;

    int i = 0;
    for (auto p : _projectorList)
    {
        int x = margin + i % 2 * (w + margin) + SIDEBAR_WIDTH;
        int y = margin + i / 2 * (h + margin);
        gl::drawSolidRect(Rectf(x - 1, y - 1, x - 1 + w + margin, y - 1 + h + margin));
        p->drawFramebuffer(x, y, w, h);
        i++;
    }
}

void radomeGraphics::beginShader() {
    _shader->bind();
    _pCubeMap->bind();
    
    _shader->uniform("EnvMap", 0);
    _shader->uniform("mixMode", 0); //_mixMode);
    _shader->uniform("mappingMode", 0); //_mappingMode);
    _shader->uniform("domeRadius", _domeRadius);
    _shader->uniform("domeHeight", _domeHeight);
    
    
    if (false /* _vidOverlay.maybeBind() */) {
        //_shader.setUniform1f("videoMix", _vidOverlay.getFaderValue());
        //_shader.setUniform2f("videoSize", _vidOverlay.getWidth(), _vidOverlay.getHeight());
        //_shader.setUniformTexture("video", _vidOverlay.getTexture(), _vidOverlay.getTextureId());
    } else {
        _testPatternTexture->enableAndBind();
        _shader->uniform("video", 0);
        _shader->uniform("videoMix", 0.5f);
        _shader->uniform("videoSize", Vec2f(0.0, 0.0));
    }

}

void radomeGraphics::endShader()
{
    _testPatternTexture->unbind();
    _pCubeMap->unbind();
    _shader->unbind();
}

void radomeGraphics::loadModel(fs::path filePath)
{
    if (filePath.empty())
        return;
    
    try {
        auto model = new radomeModel(filePath);
        _modelList.push_back(model);
    }
    catch(...)
    {
        // todo: display exception
    }
}