
#include "Angel.h"
#include "simplemodels.h"




//----------------------------------------------------------------------------
vec4 points_cube[NumVerticesCube];
vec4 colors[NumVerticesCube];
vec3 normals_cube[NumVerticesCube];
vec2 tex_coords_cube[NumVerticesCube];

// Vertices of a unit cube centered at origin, sides aligned with axes
vec4 vertices[8] = {
    vec4( -0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5, -0.5, -0.5, 1.0 ),
    vec4( -0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA colors
vec4 vertex_colors[8] = {
    vec4( 0.1, 0.1, 0.1, 1.0 ),  // black
    vec4( 1.0, 0.0, 0.0, 1.0 ),  // red
    vec4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    vec4( 0.0, 1.0, 0.0, 1.0 ),  // green
    vec4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    vec4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    vec4( 0.9, 0.9, 0.9, 1.0 ),  // white
    vec4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};



//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad( int a, int b, int c, int d )
{
    // Initialize temporary vectors along the quad's edge to compute its face normal
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];
    
    vec3 normal = normalize( cross(u, v) );
    
    normals_cube[Index] = normal; colors[Index] = vertex_colors[a]; points_cube[Index] = vertices[a];
    tex_coords_cube[Index] = vec2( 1.0, 1.0 ); Index++;
    normals_cube[Index] = normal; colors[Index] = vertex_colors[a]; points_cube[Index] = vertices[b];
    tex_coords_cube[Index] = vec2( 1.0, 0.0 ); Index++;
    normals_cube[Index] = normal; colors[Index] = vertex_colors[a]; points_cube[Index] = vertices[c];
    tex_coords_cube[Index] = vec2( 0.0, 0.0 ); Index++;
    normals_cube[Index] = normal; colors[Index] = vertex_colors[a]; points_cube[Index] = vertices[a];
    tex_coords_cube[Index] = vec2( 1.0, 1.0 ); Index++;
    normals_cube[Index] = normal; colors[Index] = vertex_colors[a]; points_cube[Index] = vertices[c];
    tex_coords_cube[Index] = vec2( 0.0, 0.0 ); Index++;
    normals_cube[Index] = normal; colors[Index] = vertex_colors[a]; points_cube[Index] = vertices[d];
    tex_coords_cube[Index] = vec2( 0.0, 1.0 ); Index++;

}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}






//---- cylinder model
//----------------------------------------------------------------------------
vec4 points_cylinder[NumVerticesCylinder];
vec4 vertices_cylinder[4];
vec3 normals_cylinder[NumVerticesCylinder];


// quad generates two triangles for each face and assigns colors to the vertices
void
quad_cylinder( int a, int b, int c, int d )
{
    points_cylinder[Index] = vertices_cylinder[a];
    normals_cylinder[Index] = vec3(vertices_cylinder[a].x, 0.0, vertices_cylinder[a].z);
    Index++;
    points_cylinder[Index] = vertices_cylinder[b];
    normals_cylinder[Index] = vec3(vertices_cylinder[b].x, 0.0, vertices_cylinder[b].z);
    Index++;
    points_cylinder[Index] = vertices_cylinder[c];
    normals_cylinder[Index] = vec3(vertices_cylinder[c].x, 0.0, vertices_cylinder[c].z);
    Index++;
    points_cylinder[Index] = vertices_cylinder[a];
    normals_cylinder[Index] = vec3(vertices_cylinder[a].x, 0.0, vertices_cylinder[a].z);
    Index++;
    points_cylinder[Index] = vertices_cylinder[c];
    normals_cylinder[Index] = vec3(vertices_cylinder[c].x, 0.0, vertices_cylinder[c].z);
    Index++;
    points_cylinder[Index] = vertices_cylinder[d];
    normals_cylinder[Index] = vec3(vertices_cylinder[d].x, 0.0, vertices_cylinder[d].z);
    Index++;
}


float const bottom = -0.5;
float const top    = 0.5;

void
colortube(void)
{
    float r = 0.5;
    Index = 0;
    
    for ( int n = 0; n < segments; n++ )
    {
        GLfloat const t0 = 2 * M_PI * (float)n / (float)segments;
        GLfloat const t1 = 2 * M_PI * (float)(n+1) / (float)segments;
        
        points_cylinder[Index].x = 0.0;
        points_cylinder[Index].y = top;
        points_cylinder[Index].z = 0.0;
        points_cylinder[Index].w = 1.0;
        normals_cylinder[Index] = vec3(0.0, 1.0, 0.0);
        Index++;
        points_cylinder[Index].x = cos(t0) * r;
        points_cylinder[Index].y = top;
        points_cylinder[Index].z = sin(t0) * r;
        points_cylinder[Index].w = 1.0;
        normals_cylinder[Index] = vec3(0.0, 1.0, 0.0);
        Index++;
        points_cylinder[Index].x = cos(t1) * r;
        points_cylinder[Index].y = top;
        points_cylinder[Index].z = sin(t1) * r;
        points_cylinder[Index].w = 1.0;
        normals_cylinder[Index] = vec3(0.0, 1.0, 0.0);
        Index++;
    }
    
    
    int num;
    for ( int n = 0; n < segments; n++ )
    {
        num = 0;
        float x = 0.0, y = 0.0, r = 0.5;
        
        GLfloat const t0 = 2 * M_PI * (float)n / (float)segments;
        GLfloat const t1 = 2 * M_PI * (float)(n+1) / (float)segments;
        
        //quad vertex 0
        vertices_cylinder[num].x = cos(t0) * r;
        vertices_cylinder[num].y = bottom;
        vertices_cylinder[num].z = sin(t0) * r;
        vertices_cylinder[num++].w = 1.0;
        //quad vertex 1
        vertices_cylinder[num].x = cos(t1) * r;
        vertices_cylinder[num].y = bottom;
        vertices_cylinder[num].z = sin(t1) * r;
        vertices_cylinder[num++].w = 1.0;
        //quad vertex 2
        vertices_cylinder[num].x = cos(t1) * r;
        vertices_cylinder[num].y = top;
        vertices_cylinder[num].z = sin(t1) * r;
        vertices_cylinder[num++].w = 1.0;
        //quad vertex 3
        vertices_cylinder[num].x = cos(t0) * r;
        vertices_cylinder[num].y = top;
        vertices_cylinder[num].z = sin(t0) * r;
        vertices_cylinder[num++].w = 1.0;
        
        quad_cylinder( 0, 1, 2, 3 );
    }
 
    
    for ( int n = 0; n < segments; n++ )
    {
        GLfloat const t0 = 2 * M_PI * (float)n / (float)segments;
        GLfloat const t1 = 2 * M_PI * (float)(n+1) / (float)segments;
        
        points_cylinder[Index].x = 0.0;
        points_cylinder[Index].y = bottom;
        points_cylinder[Index].z = 0.0;
        points_cylinder[Index].w = 1.0;
        normals_cylinder[Index] = vec3(0.0, -1.0, 0.0);
        Index++;
        points_cylinder[Index].x = cos(t1) * r;
        points_cylinder[Index].y = bottom;
        points_cylinder[Index].z = sin(t1) * r;
        points_cylinder[Index].w = 1.0;
        normals_cylinder[Index] = vec3(0.0, -1.0, 0.0);
        Index++;
        points_cylinder[Index].x = cos(t0) * r;
        points_cylinder[Index].y = bottom;
        points_cylinder[Index].z = sin(t0) * r;
        points_cylinder[Index].w = 1.0;
        normals_cylinder[Index] = vec3(0.0, -1.0, 0.0);
        Index++;
    }
    
}



//---- sphere model
//----------------------------------------------------------------------------
vec4 points_sphere[NumVerticesSphere];
vec4 vertices_sphere[4];
vec3 normals_sphere[NumVerticesSphere];
vec2 tex_coords_sphere[NumVerticesSphere];
vec4 tangents_sphere[NumVerticesSphere];
vec3 bitangents_sphere[NumVerticesSphere];

// quad generates two triangles for each face and assigns colors to the vertices
void
quad_sphere( int a, int b, int c, int d, float t0, float t1, float p0, float p1 )
{
    points_sphere[Index] = vertices_sphere[a];
    normals_sphere[Index] = vec3(vertices_sphere[a].x, vertices_sphere[a].y, vertices_sphere[a].z);
    tex_coords_sphere[Index] = vec2( t0/(2*M_PI), -(p1-M_PI/2.0)/M_PI );
    Index++;
    
    points_sphere[Index] = vertices_sphere[b];
    normals_sphere[Index] = vec3(vertices_sphere[b].x, vertices_sphere[b].y, vertices_sphere[b].z);
    tex_coords_sphere[Index] = vec2( t1/(2*M_PI), -(p1-M_PI/2.0)/M_PI );
    Index++;
    
    points_sphere[Index] = vertices_sphere[c];
    normals_sphere[Index] = vec3(vertices_sphere[c].x, vertices_sphere[c].y, vertices_sphere[c].z);
    tex_coords_sphere[Index] = vec2( t1/(2*M_PI), -(p0-M_PI/2.0)/M_PI );
    Index++;
    
    points_sphere[Index] = vertices_sphere[a];
    normals_sphere[Index] = vec3(vertices_sphere[a].x, vertices_sphere[a].y, vertices_sphere[a].z);
    tex_coords_sphere[Index] = vec2( t0/(2*M_PI), -(p1-M_PI/2.0)/M_PI );
    Index++;
    
    points_sphere[Index] = vertices_sphere[c];
    normals_sphere[Index] = vec3(vertices_sphere[c].x, vertices_sphere[c].y, vertices_sphere[c].z);
    tex_coords_sphere[Index] = vec2( t1/(2*M_PI), -(p1-M_PI/2.0)/M_PI );
    Index++;
    
    points_sphere[Index] = vertices_sphere[d];
    normals_sphere[Index] = vec3(vertices_sphere[d].x, vertices_sphere[d].y, vertices_sphere[d].z);
    tex_coords_sphere[Index] = vec2( t0/(2*M_PI), -(p0-M_PI/2.0)/M_PI );
    Index++;
}


void
colorbube(void)
{
    float r = 0.5;
    Index = 0;
    float ph_top = ((float)((ssegments/2)-1)/(float)(ssegments/2)) * M_PI/2.0;
    float ph_bottom = -ph_top;
    
    //cout << ph_bottom << endl;
    
    for ( int n = 0; n < ssegments; n++ )
    {
        GLfloat const t0 = 2 * M_PI * (float)n / (float)ssegments;
        GLfloat const t1 = 2 * M_PI * (float)(n+1) / (float)ssegments;
        
        points_sphere[Index].x = 0.0;
        points_sphere[Index].y = top;
        points_sphere[Index].z = 0.0;
        points_sphere[Index].w = 1.0;
        normals_sphere[Index] = vec3(points_sphere[Index].x, points_sphere[Index].y, points_sphere[Index].z);
        tex_coords_sphere[Index] = vec2( 0.0, 0.0);
        
        Index++;
        
        points_sphere[Index].x = cos(ph_top) * cos(t0) * r;
        points_sphere[Index].y = sin(ph_top) * r;
        points_sphere[Index].z = cos(ph_top) * sin(t0) * r;
        points_sphere[Index].w = 1.0;
        normals_sphere[Index] = vec3(points_sphere[Index].x, points_sphere[Index].y, points_sphere[Index].z);
        tex_coords_sphere[Index] = vec2( t0/(2*M_PI), -(ph_top-M_PI/2.0)/M_PI);
        
        Index++;
        
        points_sphere[Index].x = cos(ph_top) * cos(t1) * r;
        points_sphere[Index].y = sin(ph_top) * r;
        points_sphere[Index].z = cos(ph_top) * sin(t1) * r;
        points_sphere[Index].w = 1.0;
        normals_sphere[Index] = vec3(points_sphere[Index].x, points_sphere[Index].y, points_sphere[Index].z);
        tex_coords_sphere[Index] = vec2( t1/(2*M_PI), -(ph_top-M_PI/2.0)/M_PI);
        
        Index++;
    }
    
    for( int m = 1; m < ssegments-1; m++ )
    {
        float p0 = M_PI/2.0 - m * M_PI/(float)ssegments;
        float p1 = M_PI/2.0 - (m+1) * M_PI/(float)ssegments;
        
        int num;
        for ( int n = 0; n < ssegments; n++ )
        {
            num = 0;
            float x = 0.0, y = 0.0, r = 0.5;
            
            GLfloat const t0 = 2 * M_PI * (float)n / (float)ssegments;
            GLfloat const t1 = 2 * M_PI * (float)(n+1) / (float)ssegments;
            
            //quad vertex 0
            vertices_sphere[num].x = cos(p1)*cos(t0) * r;
            vertices_sphere[num].y = sin(p1) * r;
            vertices_sphere[num].z = cos(p1)*sin(t0) * r;
            vertices_sphere[num++].w = 1.0;
            //quad vertex 1
            vertices_sphere[num].x = cos(p1)*cos(t1) * r;
            vertices_sphere[num].y = sin(p1) * r;
            vertices_sphere[num].z = cos(p1)*sin(t1) * r;
            vertices_sphere[num++].w = 1.0;
            //quad vertex 2
            vertices_sphere[num].x = cos(p0)*cos(t1) * r;
            vertices_sphere[num].y = sin(p0) * r;
            vertices_sphere[num].z = cos(p0)*sin(t1) * r;
            vertices_sphere[num++].w = 1.0;
            //quad vertex 3
            vertices_sphere[num].x = cos(p0)*cos(t0) * r;
            vertices_sphere[num].y = sin(p0) * r;
            vertices_sphere[num].z = cos(p0)*sin(t0) * r;
            vertices_sphere[num++].w = 1.0;
            
            quad_sphere( 0, 1, 2, 3, t0, t1, p0, p1 );
        }
    }
    
    for ( int n = 0; n < ssegments; n++ )
    {
        GLfloat const t0 = 2 * M_PI * (float)n / (float)ssegments;
        GLfloat const t1 = 2 * M_PI * (float)(n+1) / (float)ssegments;
        
        points_sphere[Index].x = 0.0;
        points_sphere[Index].y = bottom;
        points_sphere[Index].z = 0.0;
        points_sphere[Index].w = 1.0;
        normals_sphere[Index] = vec3(points_sphere[Index].x, points_sphere[Index].y, points_sphere[Index].z);
        tex_coords_sphere[Index] = vec2( 0.0, 1.0);
        
        Index++;
        
        points_sphere[Index].x = cos(ph_bottom) * cos(t0) * r;
        points_sphere[Index].y = sin(ph_bottom) * r;
        points_sphere[Index].z = cos(ph_bottom) * sin(t0) * r;
        points_sphere[Index].w = 1.0;
        normals_sphere[Index] = vec3(points_sphere[Index].x, points_sphere[Index].y, points_sphere[Index].z);
        tex_coords_sphere[Index] = vec2( t0/(2*M_PI), -(ph_bottom-M_PI/2.0)/M_PI);
        
        Index++;
        
        points_sphere[Index].x = cos(ph_bottom) * cos(t1) * r;
        points_sphere[Index].y = sin(ph_bottom) * r;
        points_sphere[Index].z = cos(ph_bottom) * sin(t1) * r;
        points_sphere[Index].w = 1.0;
        normals_sphere[Index] = vec3(points_sphere[Index].x, points_sphere[Index].y, points_sphere[Index].z);
        tex_coords_sphere[Index] = vec2( t1/(2*M_PI), -(ph_bottom-M_PI/2.0)/M_PI);
        
        Index++;
    }
    
}


void
CalculateTangentArray(long vertexCount, vec4 (&vertex)[NumVerticesSphere], vec3 (&normal)[NumVerticesSphere], vec2 (&texcoord)[NumVerticesSphere], vec4 (&tangent)[NumVerticesSphere])
{
    vec3* tan1 = new vec3[vertexCount * 2];
    vec3* tan2 = tan1 + vertexCount;
    
    memset( tan1, 0, vertexCount * sizeof(vec3) * 2 );
    
    int index = 0;
    
    for (long a = 0; a < vertexCount/3; a++)
    {
        int i1 = index++;
        int i2 = index++;
        int i3 = index++;
        
        vec4& v1 = vertex[i1];  vec4& v2 = vertex[i2];  vec4& v3 = vertex[i3];
        
        float x1 = v2.x - v1.x; float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y; float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z; float z2 = v3.z - v1.z;
        
        vec2& w1 = texcoord[i1];  vec2& w2 = texcoord[i2];  vec2& w3 = texcoord[i3];
        
        float s1 = w2.x - w1.x; float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y; float t2 = w3.y - w1.y;
        
        float r = 1.0F / (s1 * t2 - s2 * t1);
        vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
        
        tan1[i1] += sdir;  tan1[i2] += sdir;  tan1[i3] += sdir;
        tan2[i1] += tdir;  tan2[i2] += tdir;  tan2[i3] += tdir;
    }
    
    for (int i = 0; i < vertexCount; i++)
    {
        // Gram-Schmidt orthogonalize
        tangent[i] = normalize( tan1[i] -  dot(normal[i], tan1[i]) * normal[i] );
        
        // Calculate handedness
        tangent[i].w = (dot(cross(normal[i], tan1[i]), tan2[i]) < 0.0F) ? 1.0F : 1.0F;
    }
    
    delete[] tan1;
}







