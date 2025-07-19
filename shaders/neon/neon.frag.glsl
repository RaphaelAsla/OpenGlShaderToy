#version 430 core
out vec4 fragColor;

uniform vec2 resolution;
uniform vec2 mouse_pos;
uniform float time;

#define S(x, y, t) smoothstep(x, y, t)

//Thx to Las^Mercury (I don't know who that is I just stole it form someones code)
float noise(vec3 p) {
    vec3 i = floor(p);
    vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
    vec3 f = cos((p - i) * acos(-1.)) * (-.5) + .5;
    a = mix(sin(cos(a) * a), sin(cos(1. + a) * (1. + a)), f.x);
    a.xy = mix(a.xz, a.yw, f.y);
    return mix(a.x, a.y, f.z);
}

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);
    return a + b * cos(6.28318 * (c * t * d));
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec2 offset = vec2(0.15, -0.8);
    float k = length(offset - uv * noise(offset.xyy - uv.xyy) * 4.0);

    vec3 finalColor = palette(k + time);

    k = sin(k * 8.0 + time) / 8.0;
    k = 0.02 / abs(k);

    float d = length(offset - uv * noise(offset.xyx - uv.yyx));
    d = sin(d * 8.0 + time) / 8.0;
    d = 0.04 / abs(d);
    d = S(0.4, 0.5, d);

    k *= (1.0 - d);

    finalColor *= d + k;

    fragColor = vec4(finalColor, 1.0);
}
