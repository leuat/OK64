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

vec2 bss;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

vec2 Dd(vec2 coord)
{
        float rsq = coord.x * coord.x + coord.y * coord.y;
        coord += coord * (CD * rsq);
        coord *= bss;
        return coord;
}


vec3 gt(in vec2 pr, in vec2 pg, in vec2 pb) {
    vec3 c;
    c.x = texture2D( screen, pr*0.5 + vec2(0.5,0.5) ).x;
    c.y = texture2D( screen, pg*0.5 + vec2(0.5,0.5) ).y;
    c.z = texture2D( screen, pb*0.5 + vec2(0.5,0.5) ).z;

    return c;
}

void main()
{

    float s =(exp(-time*0.05));
    s = s + 0.001*random(vec2(time*1.52,0.0));
    bss = barrelScale*vec2(1.0+s*0.3,1.0+s);
    vec2 PUV = vec2(v_pos.x, (0.0-v_pos.y));
	PUV = Dd(PUV);

    vec2 pr = PUV*(0.85+chromatic);
    vec2 pg = PUV*(0.85);
    vec2 pb = PUV*(0.85-chromatic);

    vec3 col;

    col = gt(pr,pg,pb);
  //  col.a = 1;

    float noiseAmp = 0.01 + 0.004*random(vec2(time*1.231,0));

    col.r +=noiseAmp*random(v_pos*11.23+vec2(time*11.234,time*51.123));
    col.g +=noiseAmp*random(v_pos*11.23+vec2(time*11.234,time*51.123));
    col.b +=noiseAmp*random(v_pos*11.23+vec2(time*11.234,time*51.123));


    float t = -time*0.1511;
    col.r *=1.0*(1.0 + lamp*sin(pr.y*lsca*0.85-t*0.8));
    col.g *=1.0*(1.0 + lamp*sin(pg.y*lsca*0.9-t*0.7));
    col.b *=1.0*(1.0 + lamp*sin(pb.y*lsca*0.95-t*0.6));

    if (pr.x>1.0 || pr.x<-1.0 || pr.y>1.0 || pr.y<-1.0) { col.r = 0.0;}
    if (pg.x>1.0 || pg.x<-1.0 || pg.y>1.0 || pg.y<-1.0) { col.g = 0.0;}
    if (pb.x>1.0 || pb.x<-1.0 || pb.y>1.0|| pb.y<-1.0) { col.b = 0.0;}
    
    float c = (col.r+col.g+col.b)/3.0;

    float g = gamma + 10.0*(exp(-time*0.10));

    float d = 2.0;
    float l1 = 1.0-pow(pow(-0.3+PUV.x,d)+ pow(0.3+PUV.y,d),1.0/d);
    d = clamp(2.0-time/30.0,0.0,1.0);
    float l2 = 1.0-pow(pow(0.3*abs(PUV.x),d)+ pow(abs(PUV.y*5.0),d),1.0/d);
    l1 = clamp(l1,0.0,1.0);
    l2 = clamp(l2,0.0,1.0);
    float add = 1.0*pow(l2*1.0,2.0)*exp(-time*0.01);
    g-= 1.0*pow(l1*1.0,2.0)*exp(-time*0.01);
    //g = clamp(g,0.0,1.0);

    col.r = pow(saturation*(col.r) + (1.0-saturation)*c,g);
    col.g = pow(saturation*(col.g) + (1.0-saturation)*c,g);
    col.b = pow(saturation*(col.b) + (1.0-saturation)*c,g);

    col = col + vec3(1.0,1.0,1.0)*add*d;

    gl_FragColor=vec4(col,1);
 }
