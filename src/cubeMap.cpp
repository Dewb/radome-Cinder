
#include "CubeMap.h"

#define PI 3.14159

//--------------------------------------------------------------
CubeMap::CubeMap()
{
	fov = 90.0f;
	nearDist = 0.01f;
	farDist = 1024.0f;
	
	cubeMapCamerasRenderPosition.set( 0.0f, 0.0f, 0.0f );
	
	setupSkyBoxVertices();
	
}

//--------------------------------------------------------------
// these should all be the same size and all power of two
void CubeMap::loadImages(string pos_x, string neg_x,
                         string pos_y, string neg_y,
                         string pos_z, string neg_z)
{
	
	// We don't want the texture border hack to be on
	/*bool wantsTextureBorderHack = false;
     if( ofGetTextureEdgeHackEnabled() ) {
     wantsTextureBorderHack = true;
     ofDisableTextureEdgeHack();
     ofLogVerbose() << "ofxCubeMap:loadImages (string version), disabled texture hack, re-enabling when done.";
     }*/
	
	Surface8u surfaces[6];
	surfaces[0] = loadImage(pos_x);
	surfaces[1] = loadImage(neg_x);
	surfaces[2] = loadImage(pos_y);
	surfaces[3] = loadImage(neg_y);
	surfaces[4] = loadImage(pos_z);
	surfaces[5] = loadImage(neg_z);

	bool bLoaded = true;
	for (int ii = 0; ii < 6; ii++)
    {
        if (!surfaces[ii])
        {
            bLoaded = false;
            break;
        }
    }
    if (bLoaded)
    {
        //ofLogError() << "ofxCubeMap: failed to load one of the cubemaps!";
        return;
    }

	
	loadFromSurfaces(surfaces[0],
					 surfaces[1],
					 surfaces[2],
					 surfaces[3],
					 surfaces[4],
					 surfaces[5]);
	
	/* if( wantsTextureBorderHack ) {
     ofEnableTextureEdgeHack();
     } */
	
}

//--------------------------------------------------------------

void CubeMap::loadFromSurfaces(Surface8u pos_x, Surface8u neg_x,
                                  Surface8u pos_y, Surface8u neg_y,
                                  Surface8u pos_z, Surface8u neg_z)
{
	
	//_ofEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
	
	//create a texture object
	glGenTextures(1, &textureObjectID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObjectID);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
#ifndef TARGET_OPENGLES
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // GL_TEXTURE_WRAP_R is not in the ES2 header, hmm..
#endif
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	
	unsigned char * data_px, * data_nx, * data_py, * data_ny, * data_pz, * data_nz;
	
	m_size = pos_x.getWidth();
	
	//cout << "ofxCubeMap::loadFromOfImages, size: " << size << "  bpp: " << pos_x.bpp << endl;
	
    //	data_px = new unsigned char [size * size * 3];
    //	data_py = new unsigned char [size * size * 3];
    //	data_pz = new unsigned char [size * size * 3];
	
    //	data_nx = new unsigned char [size * size * 3];
    //	data_ny = new unsigned char [size * size * 3];
    //	data_nz = new unsigned char [size * size * 3];
	
	data_px = pos_x.getData();
	data_py = pos_y.getData();
	data_pz = pos_z.getData();
	
	data_nx = neg_x.getData();
	data_ny = neg_y.getData();
	data_nz = neg_z.getData();
	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, m_size, m_size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_px); // positive x
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, m_size, m_size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_py); // positive y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, m_size, m_size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pz); // positive z
	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, m_size, m_size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_nx); // negative x
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, m_size, m_size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_ny); // negative y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, m_size, m_size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_nz); // negative z
}


//--------------------------------------------------------------
void CubeMap::initEmptyTextures(int size, GLuint channels, GLuint storageFormat)
{
	m_size = size;
	
	//create a texture object
	glGenTextures(1, &textureObjectID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObjectID);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
#ifndef TARGET_OPENGLES
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // GL_TEXTURE_WRAP_R is not in the ES2 header, hmm..
#endif
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	// set textures
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, channels, size, size, 0, GL_RGB, storageFormat, 0);
	}
    
    gl::Fbo::Format format;
    format.enableColorBuffer(true); // we intend to attach our own colour buffers
	format.enableDepthBuffer(true);
    
	fbo = gl::Fbo(m_size, m_size, format);
}


//--------------------------------------------------------------
void CubeMap::beginDrawingInto2D(GLuint face)
{
	fbo.bindFramebuffer();
	
	// Bind the face we wanted to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, textureObjectID, 0);
	
}

//--------------------------------------------------------------
void CubeMap::endDrawingInto2D()
{
	fbo.unbindFramebuffer();
}


//--------------------------------------------------------------
void CubeMap::beginDrawingInto3D(GLuint face)
{
	//ofPushView();
    
	beginDrawingInto2D(face);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glLoadMatrixf(getProjectionMatrix());
    
  	glMatrixMode(GL_MODELVIEW);
    gl::pushMatrices();
	glLoadMatrixf(getLookAtMatrixForFace(face));
}

//--------------------------------------------------------------
void CubeMap::endDrawingInto3D()
{
	//ofPopView();
    
    gl::popMatrices();
	
	endDrawingInto2D();
}

//--------------------------------------------------------------
void CubeMap::bind(int pos)
{
	boundToTextureUnit = pos;
	
	glActiveTexture(GL_TEXTURE0 + pos);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObjectID);
}

//--------------------------------------------------------------
void CubeMap::unbind()
{
	glActiveTexture(GL_TEXTURE0 + boundToTextureUnit);
    
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);
	
	glActiveTexture(GL_TEXTURE0);
}

//--------------------------------------------------------------
void CubeMap::drawSkybox(float size)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vec3f), &cubemapVertices.data()->x);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, sizeof(Vec3f), &cubemapTexCoords.data()->x);
	
    gl::pushMatrices();
    glScalef(size, size, size);
    glDrawArrays(GL_TRIANGLES, 0, cubemapVertices.size());
    gl::popMatrices();
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//--------------------------------------------------------------
void CubeMap::debugDrawCubemapCameras()
{
	for( int i = 0; i < 6; i++ )
	{
		GLuint face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		Matrix44f modelViewProjectionMatrix = getLookAtMatrixForFace(face) * getProjectionMatrix();
		
		Matrix44f inverseCameraMatrix;
		inverseCameraMatrix = modelViewProjectionMatrix.inverted();
		
        gl::pushMatrices();
        
        glMultMatrixf(inverseCameraMatrix);
        
        // Draw box in camera space, i.e. frustum in world space, box -1, -1, -1 to +1, +1, +1
        gl::drawCube(Vec3f(0, 0, 0), Vec3f(100, 100, 100));
		
        gl::popMatrices();
		
	}
}

//--------------------------------------------------------------
unsigned int CubeMap::getTextureID()
{
	return textureObjectID;
}

//--------------------------------------------------------------
int CubeMap::getWidth()
{
	return m_size;
}

//--------------------------------------------------------------
int CubeMap::getHeight()
{
	return m_size;
}

//--------------------------------------------------------------
float CubeMap::getFov()
{
	return fov;
}

//--------------------------------------------------------------
void CubeMap::setFov( float f )
{
	fov = f;
}

//--------------------------------------------------------------
Vec2f CubeMap::getNearFar()
{
	return Vec2f( nearDist, farDist );
}

//--------------------------------------------------------------
void CubeMap::setNearFar(Vec2f nearFar)
{
	nearDist = nearFar.x;
	farDist  = nearFar.y;
}

//--------------------------------------------------------------
void CubeMap::setPosition(Vec3f& pos)
{
	cubeMapCamerasRenderPosition.set(pos.x, pos.y, pos.z);
}

//--------------------------------------------------------------
void CubeMap::setPosition(float x, float y, float z )
{
	cubeMapCamerasRenderPosition.set(x, y, z);
}

//--------------------------------------------------------------
Vec3f* CubeMap::getPosition()
{
	return &cubeMapCamerasRenderPosition;
}


void makeFrustumMatrix(Matrix44f& mat, double left, double right,
                                    double bottom, double top,
                                    double zNear, double zFar)
{
    // note transpose of ofMatrix4x4 wr.t OpenGL documentation, since the OSG use post multiplication rather than pre.
    double A = (right+left)/(right-left);
    double B = (top+bottom)/(top-bottom);
    double C = -(zFar+zNear)/(zFar-zNear);
    double D = -2.0*zFar*zNear/(zFar-zNear);
    
    mat.setRow(0, Vec4f(2.0*zNear/(right-left),                    0.0,   0.0,  0.0));
    mat.setRow(1, Vec4f(                     0.0, 2.0*zNear/(top-bottom), 0.0,  0.0));
    mat.setRow(2, Vec4f(                     A,                      B,   C,   -1.0));
    mat.setRow(3, Vec4f(                   0.0,                    0.0,   D,    0.0));
}

void makePerspectiveMatrix(Matrix44f& mat, double fovy,double aspectRatio,
                           double zNear, double zFar)
{
    // calculate the appropriate left, right etc.
    double tan_fovy = tan(fovy * 0.5 * PI / 180.0);
    double right  =  tan_fovy * aspectRatio * zNear;
    double left   = -right;
    double top    =  tan_fovy * zNear;
    double bottom =  -top;
    makeFrustumMatrix(mat, left, right, bottom, top, zNear, zFar);
}

void makeLookAtViewMatrix(Matrix44f& mat, const Vec3f& eye,const Vec3f& center,const Vec3f& up)
{
	Vec3f zaxis = (eye - center).normalized();
	Vec3f xaxis = up.cross(zaxis).normalized();
	Vec3f yaxis = zaxis.cross(xaxis);
    
	mat.setRow(0, Vec4f(xaxis.x, yaxis.x, zaxis.x, 0));
	mat.setRow(1, Vec4f(xaxis.y, yaxis.y, zaxis.y, 0));
	mat.setRow(2, Vec4f(xaxis.z, yaxis.z, zaxis.z, 0));
	mat.setRow(3, Vec4f(-xaxis.dot(eye), -yaxis.dot(eye), -zaxis.dot(eye), 1));
}

//--------------------------------------------------------------
Matrix44f CubeMap::getProjectionMatrix()
{
	Matrix44f perspectiveMatrix;
	makePerspectiveMatrix(perspectiveMatrix, fov, 1.0, nearDist, farDist);
	
	return perspectiveMatrix;
}

//--------------------------------------------------------------
Matrix44f CubeMap::getLookAtMatrixForFace(GLuint face)
{
	Matrix44f lookAt;
	
	switch (face)
	{
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			makeLookAtViewMatrix(lookAt,
                                 Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, -1.0f, 0.0f));
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			makeLookAtViewMatrix(lookAt,
                                 Vec3f(0.0f, 0.0f, 0.0f), Vec3f(-1.0f, 0.0f, 0.0f), Vec3f(0.0f, -1.0f, 0.0f));
			break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			makeLookAtViewMatrix(lookAt,
                                 Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f));
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            makeLookAtViewMatrix(lookAt,
                                 Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, -1.0f, 0.0f), Vec3f(0.0f, 0.0f, -1.0f));
			break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            makeLookAtViewMatrix(lookAt,
                                 Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, -1.0f, 0.0f));
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            makeLookAtViewMatrix(lookAt,
                                 Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, -1.0f), Vec3f(0.0f, -1.0f, 0.0f));
			break;
		default:
			//ofLogError() << "ofxCubeMap::getLookAtMatrixForFace, passed in invalid face.";
			break;
    }
	
	lookAt.translate(Vec3f(cubeMapCamerasRenderPosition.x, cubeMapCamerasRenderPosition.y, cubeMapCamerasRenderPosition.z));
	//lookAt.glTranslate( -cubeMapCamerasRenderPosition.x, -cubeMapCamerasRenderPosition.y, -cubeMapCamerasRenderPosition.z );
	
	return lookAt;
}


//--------------------------------------------------------------
void CubeMap::drawFace(GLuint face, float x, float y)
{
	drawFace(face, x, y, m_size, m_size);
}

//--------------------------------------------------------------
//
//  Used to draw the faces to screen in 2D, usually to debug.
//	The code would look something like:
//
//  for( int i = 0; i < 6; i++ )
//  {
// 	   myFboCubeMap.drawFace( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i , i * 100, 0, 100, 100 );
//  }
//
void CubeMap::drawFace(GLuint face, float x, float y, float w, float h)
{
	// create a rect with the correct 3D texture coordinates, draw to screen
	scratchVertices.clear();
	scratchTexCoords.clear();
	scratchIndices.clear();
	
	switch (face)
	{
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			
			scratchTexCoords.push_back( Vec3f( 1.0f, -1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f( 1.0f,  1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f( 1.0f,  1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f( 1.0f, -1.0f, -1.0f) );
			
			break;
			
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			
			scratchTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f,  1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f,  1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f, -1.0f,  1.0f) );
			
			break;
			
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			
			scratchTexCoords.push_back( Vec3f( -1.0f,  1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f,  1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f(  1.0f,  1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f(  1.0f,  1.0f,  1.0f) );
			
			break;
			
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			
			scratchTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f, -1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f(  1.0f, -1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f(  1.0f, -1.0f, -1.0f) );
			
			break;
			
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			
			scratchTexCoords.push_back( Vec3f( -1.0f, -1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f,  1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f(  1.0f,  1.0f,  1.0f) );
			scratchTexCoords.push_back( Vec3f(  1.0f, -1.0f,  1.0f) );
			
			break;
			
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			
			scratchTexCoords.push_back( Vec3f(  1.0f, -1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f(  1.0f,  1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f,  1.0f, -1.0f) );
			scratchTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
			
			break;
			
		default:
			
			//ofLogError() << "ofxCubeMap::drawFace, passed in invalid face.";
			
			break;
	}
	
	scratchVertices.push_back( Vec3f( x, 		y + h, 	0.0f ) );
	scratchVertices.push_back( Vec3f( x, 		y, 		0.0f ) );
	scratchVertices.push_back( Vec3f( x + w, 	y, 		0.0f ) );
	scratchVertices.push_back( Vec3f( x + w, 	y + h, 	0.0f ) );
	
	scratchIndices.push_back( 0 );
	scratchIndices.push_back( 1 );
	scratchIndices.push_back( 2 );
	
	scratchIndices.push_back( 0 );
	scratchIndices.push_back( 2 );
	scratchIndices.push_back( 3 );
	
	// swap all this for an ofMesh when it supports Vec3f tex coordinates
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer( 	3, GL_FLOAT, sizeof(Vec3f), &scratchVertices.data()->x );
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer( 	3, GL_FLOAT, sizeof(Vec3f), &scratchTexCoords.data()->x );
	
	bind();
	
#ifdef TARGET_OPENGLES
	glDrawElements( GL_TRIANGLES, scratchIndices.size(), GL_UNSIGNED_SHORT, 	scratchIndices.data() );
#else
	glDrawElements( GL_TRIANGLES, scratchIndices.size(), GL_UNSIGNED_INT, 		scratchIndices.data() );
#endif
	
	unbind();
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
}

//--------------------------------------------------------------
string CubeMap::getDescriptiveStringForFace(GLuint face)
{
	string tmpName = "";
	switch (face )
	{
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			tmpName = "+X";
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			tmpName = "-X";
			break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			tmpName = "+Y";
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			tmpName = "-Y";
			break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			tmpName = "+Z";
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			tmpName = "-Z";
			break;
		default:
			//ofLogError() << "ofxCubeMap::descriptiveStringForFace, passed in invalid face.";
			break;
    }
	
	return tmpName;
	
}

//--------------------------------------------------------------
void CubeMap::setupSkyBoxVertices()
{
	
	//ofScale( _size, _size, _size );
	//ofScale( _size / 2.0f, _size / 2.0f, _size / 2.0f );
	
	float fExtent = 1.0f / 2.0f;
	
	
	
	///////////////////////////////////////////////
	//  Postive X
	cubemapTexCoords.push_back( Vec3f(1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f(1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, fExtent) );
	
	
	//////////////////////////////////////////////
	// Negative X
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent , -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent , -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, -fExtent) );
    
	//////////////////////////////////////////////////
	// Positive Y
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, -fExtent) );
	
	///////////////////////////////////////////////////
	// Negative Y
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, fExtent) );
	
	
	////////////////////////////////////////////////
	// Positive Z
	cubemapTexCoords.push_back( Vec3f( 1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, -1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, 1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, fExtent) );
	
	
	////////////////////////////////////////////////
	// Negative Z
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, -1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, -fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( -1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(-fExtent, fExtent, -fExtent) );
	
	cubemapTexCoords.push_back( Vec3f( 1.0f, 1.0f, -1.0f) );
	cubemapVertices.push_back( Vec3f(fExtent, fExtent, -fExtent) );
	
}
