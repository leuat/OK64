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



vec2 Dd(vec2 coord)
{
        float rsq = coord.x * coord.x + coord.y * coord.y;
        coord += coord * (CD * rsq);
        coord *= barrelScale;
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

    vec2 PUV = vec2(v_pos.x, (0.0-v_pos.y));
	PUV = Dd(PUV);

    vec2 pr = PUV*(0.85+chromatic);
    vec2 pg = PUV*(0.85);
    vec2 pb = PUV*(0.85-chromatic);

    vec3 col;

    col = gt(pr,pg,pb);
  //  col.a = 1;


    col.r *=1.0*(1.0 + lamp*sin(pr.y*lsca));
    col.g *=1.0*(1.0 + lamp*sin(pg.y*lsca));
    col.b *=1.0*(1.0 + lamp*sin(pb.y*lsca));

    if (pr.x>1.0 || pr.x<-1.0 || pr.y>1.0 || pr.y<-1.0) { col.r = 0.0;}
    if (pg.x>1.0 || pg.x<-1.0 || pg.y>1.0 || pg.y<-1.0) { col.g = 0.0;}
    if (pb.x>1.0 || pb.x<-1.0 || pb.y>1.0|| pb.y<-1.0) { col.b = 0.0;}
    
    float c = (col.r+col.g+col.b)/3.0;
    col.r = pow(saturation*(col.r) + (1.0-saturation)*c,gamma);
    col.g = pow(saturation*(col.g) + (1.0-saturation)*c,gamma);
    col.b = pow(saturation*(col.b) + (1.0-saturation)*c,gamma);

    gl_FragColor=vec4(col,1);
 }