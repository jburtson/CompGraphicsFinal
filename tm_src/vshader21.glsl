#version 150

//---- ins
in   vec4 vPosition;
in   vec3 vNormal;
in   vec2 vTexCoord;
in   vec4 vTangent;

//---- outs
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;
out  vec4 color;
out  vec2 tex_coord;
out  vec3 rv;

//---- uniforms
uniform vec4 LightPosition;
uniform vec4 cam_location;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 obj_color;
uniform bool obj_color_on = false;
uniform bool light_out;
uniform bool bump_on;
uniform bool isGui = false;


void main()
{
    if (isGui)
        gl_Position = model * vPosition;
    else
        gl_Position = projection * view * model * vPosition;
    
    vec3 pos = (view * model * vPosition).xyz;
    
    //---- lighting vectors
    fN = (view*model*vec4(vNormal, 0.0)).xyz;
    fE = -pos;
    fL = (view * LightPosition).xyz - pos;
    
    tex_coord    = vTexCoord;
    tex_coord.x = - tex_coord.x;

    //---- bump mapping
    if (bump_on)
    {
        vec4 Tangent = view * model * vTangent;
        vec3 bitangent = cross(fN, Tangent.xyz) * vTangent.w;
        
        fE = vec3(dot(normalize(Tangent.xyz), normalize(fE)), dot(normalize(bitangent), normalize(fE)), dot(normalize(fN), normalize(fE)));
        fL = vec3(dot(normalize(Tangent.xyz), normalize(fL)), dot(normalize(bitangent), normalize(fL)), dot(normalize(fN), normalize(fL)));
    }
    
    //---- cube mapping
    vec3 v = (model*vPosition - cam_location).xyz;
    
    if (light_out)
        rv = reflect(-v, normalize(model*vec4(vNormal, 0.0)).xyz);
    else
        rv = reflect(v, normalize(model*vec4(vNormal, 0.0)).xyz);

}


