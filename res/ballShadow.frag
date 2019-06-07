#version 440

in vec3 localPos, worldPos;
uniform vec3 b;
uniform float r;
uniform vec2 h;
uniform float hr;

// https://www.shadertoy.com/view/4djSDy
float sphOcclusion( in vec3 pos, in vec3 nor, in vec4 sph )
{
    vec3  di = sph.xyz - pos;
    float l  = length(di);
    float nl = dot(nor,di/l);
    float h  = l/sph.w;
    float h2 = h*h;
    float k2 = 1.0 - h2*nl*nl;

    // above/below horizon: Quilez - http://iquilezles.org/www/articles/sphereao/sphereao.htm
    float res = max(0.0,nl)/h2;
    // intersecting horizon: Lagarde/de Rousiers - http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    if( k2 > 0.0 ) 
    {
        #if 0
            res = nl*acos(-nl*sqrt( (h2-1.0)/(1.0-nl*nl) )) - sqrt(k2*(h2-1.0));
            res = res/h2 + atan( sqrt(k2/(h2-1.0)));
            res /= 3.141593;
        #else
            // cheap approximation: Quilez
            res = pow( clamp(0.5*(nl*h+1.0)/h2,0.0,1.0), 1.5 );
        #endif
    }

    return res;
}

void main(void) {
	if(distance(h, worldPos.xy) < hr) discard;
	float le = smoothstep(0.5,1.0,length(localPos.xy));
	float a = sphOcclusion(worldPos, vec3(0,0,1), vec4(b,r));
	a = a * pow(1 - le, 1/2.0);
	a = a * mix(0.5, 1.0, 1 - le);
	gl_FragColor = vec4(0,0,0,a);
}