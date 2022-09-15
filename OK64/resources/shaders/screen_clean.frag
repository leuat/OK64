varying vec2 v_pos;
uniform sampler2D screen;

uniform vec2 CD;
// Barrel distortion shrinks the display area a bit, this will allow us to counteract that.
uniform vec2 barrelScale;
// Curvature distortion
uniform float chromatic;
uniform float lsca;
uniform float lamp;
uniform float saturation;
uniform float gamma;
uniform float time;


void main()
{

    vec2 PUV = vec2(v_pos.x/2.0+0.5, (0.0-v_pos.y/2.0)+0.5);
    vec3 col = texture2D(screen, PUV).xyz;
    gl_FragColor=vec4(col,1);
 }
