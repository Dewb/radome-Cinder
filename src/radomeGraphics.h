//
//  radomeGraphics.h
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#ifndef __radome__radomeGraphics__
#define __radome__radomeGraphics__

class radomeCamera;
class radomeModel;
class radomeProjector;
class CubeMap;

using namespace ci;
using namespace std;

#include "cinder/gl/GlslProg.h"

class radomeGraphics
{
public:
    radomeGraphics();
    ~radomeGraphics();
    
    void setCamera(radomeCamera* pCamera);
    void initCubeMap(int textureSize);
    void initializeDomeGeometry(int radius, int height);
    void initProjectors(int count, Vec2i resolution, float distance, float height);
    
    void update();
    
    void display3DScene();
    void displayDomePreview();
    void displayCubeMap();
    void displayProjectorOutput();
    
    void loadModel(fs::path filepath);
    
protected:
    void internalDrawScene();
    void internalDrawDome();
    void internalDrawGroundPlane();
    
    void renderToCubeMap();
    void renderToProjectors();
    
    void beginShader();
    void endShader();
    
    int _domeDrawListIndex;
    float _domeRadius;
    float _domeHeight;
    
    CubeMap* _pCubeMap;
    
    radomeCamera* _pCamera;
    
    list<radomeModel*> _modelList;
    vector<radomeProjector*> _projectorList;
    
    gl::GlslProgRef _shader;
    gl::TextureRef _testPatternTexture;
};

#endif /* defined(__radome__radomeGraphics__) */
