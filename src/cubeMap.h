/*
 *  Inspired by Cinder CubeMap David Wicks on 2/27/09.
 *  Ported to oF by brenfer on 7/2/10.
 *  Updated for of 007 and extended by James George 2/13/11
 *
 *  Made working-happily by Joshy-pants and James George
 *
 *  Not copyrighted, no rights reserved, long live Christiania.
 *
 *  Edited some more by Andreas Müller. Come on, edit moar!
 *
 *  Ported back to Cinder by Michael Dewberry on 5/25/13!
 */


/*
 * OpenGL defines the cube faces in this order:
 *
 * #define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
 * #define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
 *
 * #define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
 * #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
 *
 * #define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
 * #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
 *
 * You can therefore loop through all of them like this
 *
 *
 *   for (int i = 0; i < 6; ++i)
 *   {
 * 	    someFunction( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
 *   }
 *
 *
 */

#ifndef CUBE_MAP_H
#define CUBE_MAP_H

using namespace std;
using namespace ci;

#include "cinder/gl/fbo.h"

class CubeMap
{
public:
    
    CubeMap();
    
    // Init ---------------------
	
    // These should all be the same size and all power of two, i.e. 256, 512, 1024 (I think).
    void loadImages(string pos_x, string neg_x,
                    string pos_y, string neg_y,
                    string pos_z, string neg_z );
    
    void loadFromSurfaces(Surface8u pos_x, Surface8u neg_x,
                        Surface8u pos_y, Surface8u neg_y,
                        Surface8u pos_z, Surface8u neg_z);
    
    // Drawing ------------------
	
    void initEmptyTextures(int size, GLuint channels = GL_RGB, GLuint storageFormat = GL_UNSIGNED_BYTE);			
    void bind(int unit = 0);
    void unbind();
	
    void drawSkybox(float _size);
	
    // Drawing into -------------
	
    void beginDrawingInto2D(GLuint face);		// Pass in GL_TEXTURE_CUBE_MAP_POSITIVE_X, etc.
    void endDrawingInto2D();
	
    void beginDrawingInto3D(GLuint face);		// Gives you camera looking in the direction of the cube face
    void endDrawingInto3D();
    
    // Misc ---------------------
	
    void debugDrawCubemapCameras();

    void drawFace(GLuint _face, float _x, float _y);
    void drawFace(GLuint _face, float _x, float _y, float _w, float _h);
	
    unsigned int getTextureID();
	
    int getWidth();
    int getHeight();
	
    float getFov();
    void  setFov(float fov);
    
    Vec2f getNearFar();
    void setNearFar(Vec2f nearFar);
	
    void setPosition(Vec3f& pos);
    void setPosition(float x, float y, float z);
    Vec3f* getPosition();
    
    Matrix44f getProjectionMatrix();
    Matrix44f getLookAtMatrixForFace(GLuint face);
    
    string getDescriptiveStringForFace(GLuint face);
    
private:
    void setupSkyBoxVertices();
	
    int m_size;
	
    unsigned int textureObjectID;
    
    gl::Fbo fbo;
    
    // swap for an ofMesh when it supports ofVec3f tex coordinates
    vector<Vec3f> scratchVertices;
    vector<Vec3f> scratchTexCoords;
    vector<int> scratchIndices;
	
    vector<Vec3f> cubemapVertices;
    vector<Vec3f> cubemapTexCoords;
	
    float fov;
    float nearDist;
    float farDist;
	
    Vec3f		cubeMapCamerasRenderPosition;
	
    int boundToTextureUnit;
};

#endif 

