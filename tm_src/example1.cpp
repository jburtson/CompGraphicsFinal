#if 1
// spherical camera + persp + lighting + texture
//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;

#include <GLUT/glut.h>

#include "Angel.h"
#include "simplemodels.h"

GLuint loadBMP_custom(unsigned char** data, int* w, int* h, const char * imagepath);


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 80.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 30.0, 0.0 };
GLfloat  Theta3[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  Theta4[NumAxes] = { 0.0, 0.0, 0.0 };


size_t CUBE_OFFSET;
size_t COLOR_OFFSET;
size_t CUBE_NORMALS_OFFSET;
size_t CUBE_TEXCOORDS_OFFSET;
size_t CYLINDER_OFFSET;
size_t CYLINDER_NORMALS_OFFSET;
size_t SPHERE_OFFSET;
size_t SPHERE_NORMALS_OFFSET;
size_t SPHERE_TEXCOORDS_OFFSET;
size_t SPHERE_TAN_OFFSET;


//----------------------------------------------------------------------------

GLuint program;

GLuint vPosition;
GLuint vNormal;
GLuint vColor;
GLuint vTexCoord;

GLuint textures[18];

void
init()
{
    // Load shaders and use the resulting shader program
    program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );
    
    //---------------------------------------------------------------------
    colorcube();
    colortube();
    colorbube();
    CalculateTangentArray( NumVerticesSphere, points_sphere, normals_sphere, tex_coords_sphere, tangents_sphere);
    //---------------------------------------------------------------------
    
    //----- generate a checkerboard pattern for texture mapping
    const int  TextureSize  = 256;
    GLubyte checker_tex[TextureSize][TextureSize][3];
    
    for ( int i = 0; i < TextureSize; i++ )
    {
        for ( int j = 0; j < TextureSize; j++ )
        {
            //GLubyte c = (((i & 0x4) == 0) ^ ((j & 0x1)  == 0)) * 255;
            GLubyte c = (((j & 0x2)  == 0)) * 255;
            checker_tex[i][j][0]  = 200;//c;
            checker_tex[i][j][1]  = 200;//c;
            checker_tex[i][j][2]  = 200;//c;
        }
    }
    
    //---- Initialize texture objects
    glGenTextures( 18, textures );
    
    glActiveTexture( GL_TEXTURE0 );
    
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_BGR, GL_UNSIGNED_BYTE, checker_tex );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic1 = NULL;
    int w,h;
    loadBMP_custom(&pic1, &w, &h, "cilo.bmp");
    
    glBindTexture( GL_TEXTURE_2D, textures[1] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic1 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    //---- cube maps
    
    unsigned char* negx_s = NULL;
    loadBMP_custom(&negx_s, &w, &h, "negx_s.bmp");
    unsigned char* negy_s = NULL;
    loadBMP_custom(&negy_s, &w, &h, "negy_s.bmp");
    unsigned char* negz_s = NULL;
    loadBMP_custom(&negz_s, &w, &h, "negz_s.bmp");
    unsigned char* posx_s = NULL;
    loadBMP_custom(&posx_s, &w, &h, "posx_s.bmp");
    unsigned char* posy_s = NULL;
    loadBMP_custom(&posy_s, &w, &h, "posy_s.bmp");
    unsigned char* posz_s = NULL;
    loadBMP_custom(&posz_s, &w, &h, "posz_s.bmp");
    
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, textures[2] );
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, posx_s);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, posy_s);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, posz_s);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, negx_s);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, negy_s);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, negz_s);
    
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    
    //---- bump map
    
    unsigned char* pic2 = NULL;
    loadBMP_custom(&pic2, &w, &h, "drops.bmp");
    
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, textures[3] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic2 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    // BRICK TEXTURE
    unsigned char* pic3 = NULL;
    loadBMP_custom(&pic3, &w, &h, "burningwell.bmp");
    
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, textures[4] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic3 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    // BRICK TEXTURE BUMPMAP
    unsigned char* pic4 = NULL;
    loadBMP_custom(&pic4, &w, &h, "burningwellBump.bmp");
    
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, textures[5] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic4 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    // FINAL TIME text texture
    unsigned char* pic5 = NULL;
    loadBMP_custom(&pic5, &w, &h, "finaltime.bmp");
    
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_2D, textures[6] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic5 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    // number textures
    unsigned char* pic6 = NULL;
    loadBMP_custom(&pic6, &w, &h, "0.bmp");
    
    glActiveTexture( GL_TEXTURE6 );
    glBindTexture( GL_TEXTURE_2D, textures[7] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic6 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic7 = NULL;
    loadBMP_custom(&pic7, &w, &h, "1.bmp");
    
    glActiveTexture( GL_TEXTURE7 );
    glBindTexture( GL_TEXTURE_2D, textures[8] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic7 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    // DO
    unsigned char* pic8 = NULL;
    loadBMP_custom(&pic8, &w, &h, "2.bmp");
    
    glActiveTexture( GL_TEXTURE8 );
    glBindTexture( GL_TEXTURE_2D, textures[9] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic8 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic9 = NULL;
    loadBMP_custom(&pic9, &w, &h, "3.bmp");
    
    glActiveTexture( GL_TEXTURE9 );
    glBindTexture( GL_TEXTURE_2D, textures[10] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic9 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic10 = NULL;
    loadBMP_custom(&pic10, &w, &h, "4.bmp");
    
    glActiveTexture( GL_TEXTURE10 );
    glBindTexture( GL_TEXTURE_2D, textures[11] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic10 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic11 = NULL;
    loadBMP_custom(&pic11, &w, &h, "5.bmp");
    
    glActiveTexture( GL_TEXTURE11 );
    glBindTexture( GL_TEXTURE_2D, textures[12] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic11 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic12 = NULL;
    loadBMP_custom(&pic12, &w, &h, "6.bmp");
    
    glActiveTexture( GL_TEXTURE12 );
    glBindTexture( GL_TEXTURE_2D, textures[13] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic12 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic13 = NULL;
    loadBMP_custom(&pic13, &w, &h, "7.bmp");
    
    glActiveTexture( GL_TEXTURE13 );
    glBindTexture( GL_TEXTURE_2D, textures[14] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic13 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic14 = NULL;
    loadBMP_custom(&pic14, &w, &h, "8.bmp");
    
    glActiveTexture( GL_TEXTURE14 );
    glBindTexture( GL_TEXTURE_2D, textures[15] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic14 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic15 = NULL;
    loadBMP_custom(&pic15, &w, &h, "9.bmp");
    
    glActiveTexture( GL_TEXTURE15 );
    glBindTexture( GL_TEXTURE_2D, textures[16] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic15 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    unsigned char* pic16 = NULL;
    loadBMP_custom(&pic16, &w, &h, "reset.bmp");
    
    glActiveTexture( GL_TEXTURE16 );
    glBindTexture( GL_TEXTURE_2D, textures[17] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic16 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    

#if 0
    for(int i = 0; i < 1000; i++)
    {
        //cout << 2.0*((int)(*(pic1+i)))/255.0-1.0 << ' ';
        cout << ((int)(*(pic1+i))) << ' ';
        if (!((i+1)%3)) cout << endl;
    }
#endif
    
    
    
    
    //----set offset variables
    
    CUBE_OFFSET = 0;
    COLOR_OFFSET = sizeof(points_cube);
    CUBE_NORMALS_OFFSET = COLOR_OFFSET + sizeof(colors);
    CUBE_TEXCOORDS_OFFSET = CUBE_NORMALS_OFFSET + sizeof(normals_cube);
    CYLINDER_OFFSET = CUBE_TEXCOORDS_OFFSET + sizeof(tex_coords_cube);
    CYLINDER_NORMALS_OFFSET = CYLINDER_OFFSET + sizeof(points_cylinder);
    SPHERE_OFFSET = CYLINDER_NORMALS_OFFSET + sizeof(normals_cylinder);
    SPHERE_NORMALS_OFFSET = SPHERE_OFFSET + sizeof(points_sphere);
    SPHERE_TEXCOORDS_OFFSET = SPHERE_NORMALS_OFFSET + sizeof(normals_sphere);
    SPHERE_TAN_OFFSET = SPHERE_TEXCOORDS_OFFSET + sizeof(tex_coords_sphere);
    
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );  // removed 'APPLE' suffix for 3.2
    glBindVertexArray( vao );
    
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(normals_cube) + sizeof(tex_coords_cube) + sizeof(colors) + sizeof(points_cylinder) + sizeof(normals_cylinder) + sizeof(points_sphere) + sizeof(normals_sphere) + sizeof(tex_coords_sphere) + sizeof(tangents_sphere), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube );
    glBufferSubData( GL_ARRAY_BUFFER, COLOR_OFFSET, sizeof(colors), colors );
    glBufferSubData( GL_ARRAY_BUFFER, CUBE_NORMALS_OFFSET, sizeof(normals_cube), normals_cube );
    glBufferSubData( GL_ARRAY_BUFFER, CUBE_TEXCOORDS_OFFSET, sizeof(tex_coords_cube), tex_coords_cube );
    glBufferSubData( GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder );
    glBufferSubData( GL_ARRAY_BUFFER, CYLINDER_NORMALS_OFFSET, sizeof(normals_cylinder), normals_cylinder );
    glBufferSubData( GL_ARRAY_BUFFER, SPHERE_OFFSET, sizeof(points_sphere), points_sphere );
    glBufferSubData( GL_ARRAY_BUFFER, SPHERE_NORMALS_OFFSET, sizeof(normals_sphere), normals_sphere );
    glBufferSubData( GL_ARRAY_BUFFER, SPHERE_TEXCOORDS_OFFSET, sizeof(tex_coords_sphere), tex_coords_sphere );
    glBufferSubData( GL_ARRAY_BUFFER, SPHERE_TAN_OFFSET, sizeof(tangents_sphere), tangents_sphere );
    
    
    //---------------------------------------------------------------------
    
    // set up vertex arrays
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET) );
    
    vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET) );
    
    vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(COLOR_OFFSET) );
    
    vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET) );
    
    
    glEnableVertexAttribArray( glGetAttribLocation( program, "vTangent" ) );
    glVertexAttribPointer( glGetAttribLocation( program, "vTangent" ), 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TAN_OFFSET) );
    
    
    // Set the value of the fragment shader texture sampler variable (myTextureSampler) to GL_TEXTURE0
    glUniform1i( glGetUniformLocation(program, "myTextureSampler"), 0 );
    glUniform1i( glGetUniformLocation(program, "myCubeSampler"), 1 );
    glUniform1i(glGetUniformLocation(program, "normal_map"),  2);
    
    glUniform1i( glGetUniformLocation(program, "cube_map_on"), false );
    
    //---------------------------------------------------------------------
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
}




//----------------------------------------------------------------------------

void
SetMaterial( vec4 ka, vec4 kd, vec4 ks, float s )
{
    glUniform4fv( glGetUniformLocation(program, "ka"), 1, ka );
    glUniform4fv( glGetUniformLocation(program, "kd"), 1, kd );
    glUniform4fv( glGetUniformLocation(program, "ks"), 1, ks );
    glUniform1f( glGetUniformLocation(program, "Shininess"), s );
}

//----------------------------------------------------------------------------

void
SetLight( vec4 lpos, vec4 la, vec4 ld, vec4 ls )
{
    glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, lpos);
    glUniform4fv( glGetUniformLocation(program, "AmbientLight"), 1, la);
    glUniform4fv( glGetUniformLocation(program, "DiffuseLight"), 1, ld);
    glUniform4fv( glGetUniformLocation(program, "SpecularLight"), 1, ls);
    
}


//----------------------------------------------------------------------------


void
makeShape(int shape, vec4 ka, vec4 kd, vec4 ks, float s, mat4 transform, int texture, bool light_out)
{
    // texture
    if (texture>=0){
        //glBindTexture( GL_TEXTURE_2D, textures[texture] );
        glUniform1i( glGetUniformLocation(program, "texture_on"), true );
    }
    else{
        glUniform1i( glGetUniformLocation(program, "texture_on"), false );
    }
    
    // lighting
    glUniform4fv( glGetUniformLocation(program, "ka"), 1, ka );
    glUniform4fv( glGetUniformLocation(program, "kd"), 1, kd );
    glUniform4fv( glGetUniformLocation(program, "ks"), 1, ks );
    glUniform1f( glGetUniformLocation(program, "Shininess"), s );
    
    //inverted normal?
    glUniform1i( glGetUniformLocation(program, "light_out"), light_out );
    
    glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform );
    
    switch (shape) {
        case 0: // CUBE
            glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET) );
            glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET) );
            if (texture>=0)
                
                glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET) );
            glDrawArrays( GL_TRIANGLES, 0, NumVerticesCube );
            break;
        case 1: // TUBE
            glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET) );
            glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET) );
            
            glDrawArrays( GL_TRIANGLES, 0, NumVerticesCylinder );
            break;
        case 2: // SPHERE
            glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET) );
            glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET) );
            if (texture>=0)
                glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET) );
            
            glDrawArrays( GL_TRIANGLES, 0, NumVerticesSphere );
            break;
            
        default:
            break;
    }
    
}



//----------------------------------------------------------------------------

mat4 proj_matrix;

mat4 view_matrix;

bool dude_camera= true;
float r = 5.0;
int finalTime=0;
float currentTime;


// DUDE variables
int score = 0;
//vec2 facing = vec2(0.0, 1.0);
bool isDead = false;
bool isHurt = false;
bool looksHurt = true;
int hurtTime = 0;
float hurtStart = 0;
float facing_angle = 90;
float dudeX= 0.0;
float dudeY= -2.5;
float dudeZ= 20.0;
float dudeX_speed= 0.0;
float dudeY_speed= 0.0;
float dudeZ_speed= 0.0;
float dude_acc = 0.2; // how fast the dude moves from a button press
float dude_turning_speed = 3;
float dude_resistence = .8; // from 0-1 how fast dude will slow when not actively moving (1 for frictionless, 0 for maximum friction)
float invincibility_length = 2.0;
int maxHealth = 5;

// BAD varibles
// boulder 1
float boulder1X= 10.0;
float boulder1Y= -1.0;
float boulder1Z= 10.0;
float boulder1X_speed= -0.75;
float boulder1Z_speed= -0.5;
// boulder 2
float boulder2X= 0.0;
float boulder2Y= -1.0;
float boulder2Z= -22.5;
float boulder2X_speed= -0.45;
float boulder2Z_speed= 0.45;
// Zombie
float zom1X= 0.0;
float zom1Y= -2.5;
float zom1Z= -10.0;
float zom1_facing_angle = 0;
float zom1_speed = 0.06;

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    //---- lights
    
    SetLight( vec4( 2.0, 2.0, 2.0, 1.0 ), vec4(0.4, 0.4, 0.4, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.7, 0.7, 0.7, 1.0) );
    
    //---- camera intrinsic parameters
    
    //float left = -1.0;
    //float right = 1.0;
    //float bottom = -1.0;
    //float top = 1.0;
    //float zNear = 1.0;
    //float zFar = 15.0;
    float left = -0.5;
    float right = 0.5;
    float bottom = -0.5;
    float top = 0.5;
    float zNear = 0.5;
    float zFar = 100.0;
    
    //proj_matrix = Scale(2.0/(right-left), 2.0/(top-bottom), 2.0/(zNear-zFar));
    
    proj_matrix = Frustum( left, right, bottom, top, zNear, zFar );
    
    glUniformMatrix4fv( glGetUniformLocation( program, "projection" ), 1, GL_TRUE, proj_matrix );
    
    
    //---- camera extrinsic parameters
    if (!dude_camera){
        float tr_y = Theta3[Yaxis]* M_PI/180.0;
        float tr_z = Theta3[Zaxis]* M_PI/180.0;
        float eye_z = r * (cos(tr_z)) * cos(tr_y);
        float eye_x = r * (cos(tr_z)) * sin(tr_y);
        float eye_y = r * sin(tr_z);
    
        vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);
        cout << up << ' ' << normalize(up) << endl;
    
        view_matrix = LookAt( vec4(eye_x, eye_y, eye_z, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, cos(tr_z), 0.0, 0.0));
    }
    else{
        float angle = DegreesToRadians * facing_angle;
        float eye_x = dudeX + cos(angle);
        //float eye_x = dudeX;
        float eye_y = dudeY+3;
        float eye_z = dudeZ + sin(angle);
        //float eye_z = dudeZ + 1;
        float at_x = dudeX - 2*cos(angle);
        //float at_x = dudeX;
        float at_y = dudeY+1.5;
        float at_z = dudeZ - 2*sin(angle);
        //float at_z = dudeZ-2;

        view_matrix = LookAt( vec4(eye_x, eye_y, eye_z, 1.0), vec4(at_x, at_y, at_z, 1.0), vec4(0.0, 1.0, 0.0, 0.0));
    }
    
    glUniformMatrix4fv( glGetUniformLocation( program, "view" ), 1, GL_TRUE, view_matrix );
    
    ////////////////////////////// CREATION OF ALL OBJECTS IN SCENE ////////////////////////////////////////
    mat4 transform;
    
    if (!isDead){
        //---- action
        float lightX = 0.0;
        float lightY = 3.0;
        float lightZ = 0.0;
        SetLight( vec4( lightX, lightY, lightZ, 1.0 ), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.5, 0.5, 0.5, 1.0) );
        
        // light location object
        transform = Translate( lightX, lightY, lightZ);
        makeShape(0, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), 0.5, transform, -1, true);
        
        /*
         //---- spherical sun
         transform = Translate( 0.0, 0.0, 0.0 ) * Scale(2.5, 2.5, 2.5);
         makeShape(2, vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.0, 1.0), 0.5, transform, 0, true);
         
         //---- spherical planet
         transform = RotateY( Theta4[Xaxis] ) * Translate( 3.0, 0.0, 0.0 ) * Scale(2.0, -2.0, 2.0);
         makeShape(2, vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5, transform, 1, false);
         
         //---- spherical planet
         transform = RotateY( Theta4[Xaxis]+30 ) * Translate( 4.0, 0.0, 0.0 ) * Scale(1.0, 1.0, 1.0);
         makeShape(2, vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5, transform, -1, false);
         
         //---- cylindrical object
         transform = Translate( 0.0, -2.5, 0.0 ) * Scale(1.5, 2.0, 1.5);
         makeShape(1, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, -1, false);
         */
        
        //---- stage
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, textures[4] );
        glActiveTexture( GL_TEXTURE2 );
        glBindTexture( GL_TEXTURE_2D, textures[5] );
        glUniform1i( glGetUniformLocation(program, "bump_on"), true );
        glUniform1i( glGetUniformLocation(program, "light_out"), false );
        glUniform1i( glGetUniformLocation(program, "texture_on"), true );
        //glUniform1f( glGetUniformLocation(program, "st_factor"), 100.0 );
        transform = Translate( 0.0, -4.0, 0.0 ) * Scale(50.0, 1.0, 50.0);
        makeShape(0, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, 4, false);
        // NORTH WALL
        transform = Translate( 0.0, 6.5, -25.5 ) * Scale(50.0, 50.0, 1.0);
        makeShape(0, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, 4, false);
        // SOUTH WALL
        transform = Translate( 0.0, 6.5, 25.5 ) * Scale(50.0, 50.0, 1.0);
        makeShape(0, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, 4, false);
        // WEST WALL
        transform = Translate( -25.5, 6.5, 0.0 ) * Scale(1.0, 50.0, 50.0);
        makeShape(0, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, 4, false);
        // EAST WALL
        transform = Translate( 25.5, 6.5, 0.0 ) * Scale(1.0, 50.0, 50.0);
        makeShape(0, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, 4, false);
        glUniform1i( glGetUniformLocation(program, "bump_on"), false );
        
        if (isHurt){ // cause flashing when hurt
            hurtTime++;
            if (hurtTime>5){ // change this number for quicker or slower flashing invinsibility frames
                looksHurt=!looksHurt;
                hurtTime=0;
            }
        }
        //---- dude
        if (isHurt && looksHurt){
            transform = Translate( dudeX, dudeY, dudeZ ) * Scale(1.0, 2.0, 1.0);
            makeShape(1, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, -1, false); // body
            transform = Translate( dudeX, dudeY+1.75, dudeZ ) * Scale(0.7, 1.0, 0.7);
            makeShape(1, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, -1, false); // head
        }
        //---- HURT dude
        else{
            transform = Translate( dudeX, dudeY, dudeZ ) * Scale(1.0, 2.0, 1.0);
            makeShape(1, vec4(0.0, 0.0, 0.7, 1.0), vec4(0.0, 0.0, 1.0, 1.0), vec4(0.5, 0.5, 1.5, 1.0), 5.5, transform, -1, false); // body
            transform = Translate( dudeX, dudeY+1.75, dudeZ ) * Scale(0.7, 1.0, 0.7);
            makeShape(1, vec4(1.0, 0.88, 0.74, 1.0), vec4(1.0, 0.88, 0.74, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5, transform, -1, false); // head
        }
        
        //---- zom1
        transform = Translate( zom1X, zom1Y, zom1Z ) * Scale(1.0, 2.0, 1.0);
        makeShape(1, vec4(0.54, 0.33, 0.0, 1.0), vec4(0.54, 0.33, 0.0, 1.0), vec4(0.5, 0.5, 1.5, 1.0), 5.5, transform, -1, false); // body
        transform = Translate( zom1X, zom1Y+1.75, zom1Z ) * Scale(0.7, 1.0, 0.7);
        makeShape(1, vec4(0.7, 0.86, 0.42, 1.0), vec4(0.7, 0.86, 0.42, 1.0), vec4(0.5, 1.0, 0.5, 1.0), 5.5, transform, -1, false); // head
        
        //---- boulders
        transform = Translate( boulder1X, boulder1Y, boulder1Z ) * Scale(5.0, 5.0, 5.0);
        //makeShape(2, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 10.0, transform, -1, false); // for red boulder color
        makeShape(2, vec4(0.54, 0.33, 0.0, 1.0), vec4(0.54, 0.33, 0.0, 1.0), vec4(0.5, 0.5, 1.5, 1.0), 10.0, transform, -1, false);
        
        transform = Translate( boulder2X, boulder2Y, boulder2Z ) * Scale(5.0, 5.0, 5.0);
        makeShape(2, vec4(0.0, 0.7, 0.0, 1.0), vec4(0.0, 0.1, 0.0, 1.0), vec4(0.5, 1.0, 0.5, 1.0), 10.0, transform, -1, false);
        
        //---- hearts
        for (int i=0; i< maxHealth-score; i++){
            float dist = i * (360.0/(maxHealth-score));
            transform = RotateY( Theta4[Xaxis] + dist ) * Translate( 3.0, lightY, 0.0 ) * Scale(0.7, 0.7, 0.7);
            makeShape(0, vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 10.0, transform, -1, true);
        }
        
        
        /*
         //---- bumpy sphere
         glActiveTexture( GL_TEXTURE1 );
         glBindTexture( GL_TEXTURE_CUBE_MAP, textures[1] );
         glActiveTexture( GL_TEXTURE2 );
         glBindTexture( GL_TEXTURE_2D, textures[3] );
         
         glUniform1i( glGetUniformLocation(program, "bump_on"), true );
         glUniform1i( glGetUniformLocation(program, "light_out"), false );
         glUniform1i( glGetUniformLocation(program, "texture_on"), true );
         glUniform1i( glGetUniformLocation(program, "cube_map_on"), true );
         glUniform1f( glGetUniformLocation(program, "st_factor"), 1.0 );
         SetMaterial( vec4(1.0,1.0,1.0,1.0), vec4(1.0,1.0,1.0,1.0), vec4(1.0,1.0,1.0,1.0), 2.0);
         
         //mat4 transform_bube = RotateY(Theta1[Yaxis]) * Translate( 0.0, 0.0, 0.0 ) * Scale(3.5, 3.5, 3.5);
         mat4 transform_bube = Translate( 2.0, -1.0, 2.0 ) * Scale(3.5, 3.5, 3.5);
         glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform_bube );
         
         glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET) );
         glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET) );
         glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET) );
         glDrawArrays( GL_TRIANGLES, 0, NumVerticesSphere );
         glUniform1i( glGetUniformLocation(program, "texture_on"), false );
         glUniform1i( glGetUniformLocation(program, "cube_map_on"), false );
         glUniform1i( glGetUniformLocation(program, "bump_on"), false );
         
         #if 1
         //---- cube
         glBindTexture( GL_TEXTURE_2D, textures[1] );
         glUniform1i( glGetUniformLocation(program, "texture_on"), true );
         
         SetMaterial( vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5);
         glUniform1i( glGetUniformLocation(program, "light_out"), false );
         
         mat4 transform_cube = Translate( 0.0, 3.0, 0.0 ) * RotateZ( Theta4[Xaxis] ) * Scale(2.0, -2.0, 2.0);
         glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform_cube );
         
         glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET) );
         glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET) );
         glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET) );
         //glDrawArrays( GL_TRIANGLES, 0, NumVerticesCube );
         
         for(int i=0; i<6; i++)
         {
         glBindTexture( GL_TEXTURE_2D, textures[i%2] );
         glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET+6*i*sizeof(vec4)));
         glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET + 6*i*sizeof(vec3)));
         glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET + 6*i*sizeof(vec2)));
         glDrawArrays( GL_TRIANGLES, 0, 6 );
         }
         #endif
         */
    }
    else if (isDead){
        /////////////////////// GUI ELEMENTS
        glUniform1i( glGetUniformLocation(program, "isGui"), true ); // no longer draws with viewing matrix
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, textures[6] );
        transform = Translate( 0.0, 0.5, 0.0 ) * Scale(1.6, -0.45, 1.0);
        makeShape(0, vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.0, 1.0), 0.5, transform, 1, true);
        
        // first digit of final time
        glBindTexture( GL_TEXTURE_2D, textures[((finalTime/100)%10)+7] );
        transform = Translate( -0.2, 0.0, 0.0 ) * Scale(0.2, -0.25, 1.0);
        makeShape(0, vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.0, 1.0), 0.5, transform, 1, true);
        
        // second digit of final time
        glBindTexture( GL_TEXTURE_2D, textures[((finalTime/10)%10)+7] );
        transform = Translate( 0.0, 0.0, 0.0 ) * Scale(0.2, -0.25, 1.0);
        makeShape(0, vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.0, 1.0), 0.5, transform, 1, true);
        
        // third digit of final time
        glBindTexture( GL_TEXTURE_2D, textures[(finalTime%10)+7] );
        transform = Translate( 0.2, 0.0, 0.0 ) * Scale(0.2, -0.25, 1.0);
        makeShape(0, vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.0, 1.0), 0.5, transform, 1, true);
        
        glBindTexture( GL_TEXTURE_2D, textures[17] );
        transform = Translate( 0.0, -0.5, 0.0 ) * Scale(1.8, -0.25, 1.0);
        makeShape(0, vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.0, 1.0), 0.5, transform, 1, true);
        
        
        glUniform1i( glGetUniformLocation(program, "isGui"), false );
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }
    
    glutSwapBuffers();
}


//----------------------------------------------------------------------------
bool goingL = false;
bool goingR = false;
bool goingF = false;
bool goingB = false;
bool turningL = false;
bool turningR = false;

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
        case 033:    // Escape key
        case '`': case 'Q':
            exit( EXIT_SUCCESS );
            break;
            
        case 'x':    //---- theta
            Axis = Yaxis;
            Theta3[Axis] += 5.0;
            Theta3[Axis] = fmod(Theta3[Axis], 360.0);
            glutPostRedisplay();
            break;
        case 'X':    //---- theta -
            Axis = Yaxis;
            Theta3[Axis] -= 5.0;
            Theta3[Axis] = fmod(Theta3[Axis], 360.0);
            glutPostRedisplay();
            break;
        case 'z':    //---- phi
            Axis = Zaxis;
            Theta3[Axis] += 5.0;
            Theta3[Axis] = fmod(Theta3[Axis], 360.0);
            glutPostRedisplay();
            break;
        case 'Z':    //---- phi
            Axis = Zaxis;
            Theta3[Axis] -= 5.0;
            Theta3[Axis] = fmod(Theta3[Axis], 360.0);
            glutPostRedisplay();
            break;
            
        case 'r':    //---- increase radius
            r += 0.5;
            glutPostRedisplay();
            break;
        case 'R':    //---- decrease radius
            r -= 0.5;
            glutPostRedisplay();
            break;
            
            //----------------------------- DUDE CONTROL
        case 'w':    //---- forward
            goingF = true;
            //if (!goingF && !goingB){
            //    goingF = true;
            //    dudeZ_speed -= dude_acc;
            //}
            //dudeZ_speed = -dude_acc;
            break;
        case 's':    //---- backward
            goingB = true;
            //if (!goingF && !goingB){
            //    goingB = true;
            //    dudeZ_speed += dude_acc;
            //}
            //dudeZ_speed = dude_acc;
            break;
        case 'q':    //---- strafe left
            goingL = true;
            //if (!goingL && !goingR){
            //    goingL = true;
            //    dudeX_speed -= dude_acc;
            //}
            //dudeX_speed = -dude_acc;
            break;
        case 'e':    //---- strafe right
            goingR = true;
            //if (!goingL && !goingR){
            //    goingR = true;
            //    dudeX_speed += dude_acc;
            //}
            //dudeX_speed = dude_acc;
            break;
        case 'a':    //---- turn left
            turningL = true;
            //facing_angle = fmod(facing_angle-10, 360.0);
            break;
        case 'd':    //---- turn right
            turningR = true;
            //facing_angle = fmod(facing_angle+10, 360.0);
            break;
            
        case ' ': // reset
            isDead = false;
            isHurt = false;
            dude_camera= true;
            finalTime=0;
            score = 0;
            facing_angle = 90;
            dudeX= 0.0;
            dudeY= -2.5;
            dudeZ= 20.0;
            // BAD varibles
            // boulder 1
            boulder1X= 10.0;
            boulder1Y= -1.0;
            boulder1Z= 10.0;
            boulder1X_speed= -0.75;
            boulder1Z_speed= -0.5;
            // boulder 2
            boulder2X= 0.0;
            boulder2Y= -1.0;
            boulder2Z= -22.5;
            boulder2X_speed= -0.45;
            boulder2Z_speed= 0.45;
            // Zombie
            zom1X= 0.0;
            zom1Y= -2.5;
            zom1Z= -10.0;
            zom1_facing_angle = 0;
            zom1_speed = 0.05;
            break;
            
        case 'D': // DUDE VIEW
            dude_camera = !dude_camera;
            glutPostRedisplay();
            break;
        case 'u':
            //view_matrix = default_view_matrix;
            //Theta3[Xaxis] = 0.0;
            //Theta3[Yaxis] = 0.0;
            //Theta3[Zaxis] = 0.0;
            Theta3[Xaxis] = 0.0;
            Theta3[Yaxis] = 0.0;
            Theta3[Zaxis] = 60.0;
            r= 40;
            glutPostRedisplay();
            break;
    }
}
void
keyboardUp( unsigned char key, int x, int y )
{
    switch( key ) {
        case 'w':    //---- forward
            goingF = false;
            //if (goingF && !goingB){
            //    goingF = false;
            //    dudeZ_speed = 0;
            //}
            //dudeZ_speed = 0;
            break;
        case 's':    //---- backward
            goingB = false;
            //if (goingB && !goingF){
            //    goingB = false;
            //    dudeZ_speed = 0;
            //}
            //dudeZ_speed = 0;
            break;
        case 'q':    //---- strafe left
            goingL = false;
            //if (goingL && !goingR){
            //    goingL = false;
            //    dudeZ_speed = 0;
            //}
            //dudeX_speed = 0;
            break;
        case 'e':    //---- strafe right
            goingR = false;
            //if (goingR && !goingL){
            //    goingR = false;
            //    dudeZ_speed = 0;
            //}
            //dudeX_speed = 0;
            break;
        case 'a':    //---- turn left
            turningL = false;
            break;
        case 'd':    //---- turn right
            turningR = false;
            break;
    }
}




//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN ) {
        switch( button ) {
            case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
            case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
            case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
        }
    }
}

//----------------------------------------------------------------------------

void
idle( void )
{
    currentTime = glutGet( GLUT_ELAPSED_TIME )/1000.0f;
    
    ////////////////////////////////// DUDE MOVEMENT
    // MOVE FORWARD ACCORDING TO WHAT BUTTONS ARE PRESSED, OR STOP IF NONE
    if (goingF && !goingB){ // GO FORWARD
        dudeZ_speed = -dude_acc;
    }
    else if (goingB && !goingF){ // GO BACKWARD
        dudeZ_speed = dude_acc;
    }
    else{ // STOP MOVING F/B
        //dudeZ_speed = 0;
        dudeZ_speed *= dude_resistence; // resistence
    }
    if (goingL && !goingR){ // GO LEFT
        dudeX_speed = -dude_acc;
    }
    else if (goingR && !goingL){ // GO RIGHT
        dudeX_speed = dude_acc;
    }
    else{ // STOP MOVING L/R
        //dudeX_speed = 0;
        dudeX_speed *= dude_resistence; // resistence
    }
    if (turningL && !turningR){
        facing_angle = fmod(facing_angle - dude_turning_speed, 360.0);
    }
    else if (turningR && !turningL){
        facing_angle = fmod(facing_angle + dude_turning_speed, 360.0);
    }
    
    // CHANGE DUDES POSITION EQUAL TO SPEED VARIABLE
    float angle = DegreesToRadians * facing_angle;
    dudeX+= dudeZ_speed*cos(angle) + dudeX_speed*cos(angle-M_PI_2);
    dudeZ+= dudeZ_speed*sin(angle) + dudeX_speed*sin(angle-M_PI_2);
    //dudeX+= dudeX_speed;
    //dudeY+= dudeY_speed;
    //dudeZ+= dudeZ_speed;
    
    // resistence
    //dudeX_speed *= .8;
    //dudeZ_speed *= .8;
    ////////////////////////////////// BAD dude MOVEMENT
    boulder1X += boulder1X_speed;
    boulder1Z += boulder1Z_speed;
    boulder2X += boulder2X_speed;
    boulder2Z += boulder2Z_speed;

    // zom1 movement
    float dx = zom1X - dudeX;
    float dz = zom1Z - dudeZ;
    angle = atan2(dz,dx);
    zom1X-= zom1_speed*cos(angle) + zom1_speed*cos(angle);
    zom1Z-= zom1_speed*sin(angle) + zom1_speed*sin(angle);

    
    ////////////////////////////////// HIT DETECTION
    // STAGE BOUNDRIES
    //float bound = 9.5;
    float bound = 24.5;
    if (dudeX>bound){
        dudeX = bound;
    }
    else if (dudeX<-bound){
        dudeX = -bound;
    }
    if (dudeZ>bound){
        dudeZ = bound;
    }
    else if (dudeZ<-bound){
        dudeZ = -bound;
    }
    
    bound = bound-2;
    // BOULDER BALL HITTING
    // boulder 1
    if (boulder1X>bound){
        boulder1X = bound;
        boulder1X_speed= -boulder1X_speed;
    }
    else if (boulder1X<-bound){
        boulder1X = -bound;
        boulder1X_speed= -boulder1X_speed;
    }
    if (boulder1Z>bound){
        boulder1Z = bound;
        boulder1Z_speed= -boulder1Z_speed;
    }
    else if (boulder1Z<-bound){
        boulder1Z = -bound;
        boulder1Z_speed= -boulder1Z_speed;
    }
    
    // boulder 2
    if (boulder2X>bound){
        boulder2X = bound;
        boulder2X_speed= -boulder2X_speed;
    }
    else if (boulder2X<-bound){
        boulder2X = -bound;
        boulder2X_speed= -boulder2X_speed;
    }
    if (boulder2Z>bound){
        boulder2Z = bound;
        boulder2Z_speed= -boulder2Z_speed;
    }
    else if (boulder2Z<-bound){
        boulder2Z = -bound;
        boulder2Z_speed= -boulder2Z_speed;
    }

    if (isHurt && (currentTime-hurtStart >= invincibility_length)){
        isHurt = false;
    }
    
    // PLAYER GETTING HURT
    if (!isHurt &&
        ((abs(dudeX-boulder1X)<2.5 && abs(dudeZ-boulder1Z)<2.5) || // boulder 1
         (abs(dudeX-boulder2X)<2.5 && abs(dudeZ-boulder2Z)<2.5) || // boulder 2
         (abs(dudeX-zom1X)<1.0 && abs(dudeZ-zom1Z)<1.0))){ // zom1
            isHurt = true;
            score++;
            cout << score << '\n';
            hurtStart = currentTime;
    }
    
    
    // DEATH
    if (!isDead && score>= maxHealth){
        isDead = true;
        finalTime= currentTime;
        cout << "Final score: " << finalTime << '\n';
    }
    
    //Theta1[Axis] = fmod(Theta1[Axis]+1, 360.0);
    //Theta2[Axis] = fmod(Theta2[Axis]+2, 360.0);
    
    Theta4[Xaxis] = fmod(Theta4[Xaxis]+0.5, 360.0);
    
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape( int w, int h )
{
    glViewport(0,0,w,h);
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 800, 800 );
    glutCreateWindow( "Color Cube" );
    
    init();
    
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutKeyboardUpFunc( keyboardUp );
    glutMouseFunc( mouse );
    glutIdleFunc( idle );
    
    glutMainLoop();
    return 0;
}

#endif
















