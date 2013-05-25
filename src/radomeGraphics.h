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
class CubeMap;

class radomeGraphics
{
public:
    radomeGraphics();
    ~radomeGraphics();
    
    void setCamera(radomeCamera* pCamera);
    void initializeDomeGeometry(int radius, int height);
    void display3DScene();
    void displayDomePreview();
    void displayCubeMap();
    
protected:
    void internalDrawScene();
    void internalDrawDome();
    void internalDrawGroundPlane();
    
    int _domeDrawListIndex;
    float _domeRadius;
    float _domeHeight;
    
    CubeMap* _pCubeMap;
    
    radomeCamera* _pCamera;
};

#endif /* defined(__radome__radomeGraphics__) */
