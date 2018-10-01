# version 400

layout ( location = 0) in vec4 vPosition ;
layout ( location = 1) in vec3 vNormal ;
layout ( location = 2) in vec2 vTexCoords ;

uniform mat4 model ;
uniform mat4 view ;
uniform mat4 projection ;
uniform mat3 normalMatrix ;
uniform vec4 lightPosition ;

out vec3 fN ;
out vec3 fE ;
out vec3 fL ;
out vec2 ftexCoord ;

void main ()
{
    vec4 VMvPosition = view * model * vPosition ;
    fN = mat3 ( view ) * normalMatrix * vNormal ;
    fL = lightPosition . xyz - VMvPosition . xyz ;
    fE = - VMvPosition . xyz ;
    gl_Position = projection * VMvPosition;
    ftexCoord = vTexCoords;
}
