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
class CubeMap;

using namespace ci;
using namespace std;

class radomeGraphics
{
public:
    radomeGraphics();
    ~radomeGraphics();
    
    void setCamera(radomeCamera* pCamera);
    void initCubeMap(int textureSize);
    void initializeDomeGeometry(int radius, int height);
    
    void update();
    
    void display3DScene();
    void displayDomePreview();
    void displayCubeMap();
    
    void loadModel(fs::path filepath);
    
protected:
    void internalDrawScene();
    void internalDrawDome();
    void internalDrawGroundPlane();
    
    void renderToCubeMap();
    void renderToProjectors();
    
    int _domeDrawListIndex;
    float _domeRadius;
    float _domeHeight;
    
    CubeMap* _pCubeMap;
    
    radomeCamera* _pCamera;
    
    list<radomeModel*> _modelList;
};

#endif /* defined(__radome__radomeGraphics__) */
