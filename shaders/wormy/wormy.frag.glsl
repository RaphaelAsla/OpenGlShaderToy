#version 430 core

out vec4 fragColor;
uniform vec2 resolution;
uniform vec2 mouse_pos;
uniform float time;

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

float smin(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(d2, d1, h) - k * h * (1.0 - h);
}

float sdfSphere(vec3 p, float r) {
    return length(p) - r;
}

float sdfBox(vec3 p, vec3 s) {
    vec3 q = abs(p) - s;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdfMap(vec3 p) {
    float hbox = sdfBox(p, vec3(5.0));
    hbox = abs(hbox) * 0.1;

    vec3 b1 = p;
    b1.x += time * 0.8;
    b1.z += 1.5;
    b1.y += sin(p.x * 3.14) * 0.5;
    b1.x = mod(b1.x, 0.3) - 0.15;
    b1.y = mod(b1.y, 1.0) - 0.5;
    b1.z = mod(b1.z, 1.5) - 0.75;
    b1.yz *= rot2D(sin(p.x * 6.28));
    b1.xy *= rot2D(time * 2.0);
    float w1 = sdfBox(b1, vec3(0.1));

    vec3 b2 = p;
    b2.y += time * 0.8;
    b2.x += 0.5;
    b2.z += sin(p.y * 3.14) * 0.5;
    b2.y = mod(b2.y, 0.3) - 0.15;
    b2.x = mod(b2.x, 1.0) - 0.5;
    b2.z = mod(b2.z, 1.5) - 0.75;
    b2.xz *= rot2D(sin(p.y * 6.28));
    b2.xy *= rot2D(time * 3.14);
    float w2 = sdfBox(b2, vec3(0.1));

    vec3 b3 = p;
    b3.xy = fract(b3.xy) - 0.5;
    b3.z = mod(b3.z, 0.2) - 0.1;
    b3.xy *= rot2D(sin(p.z * 6.28));
    b3.xy *= rot2D(time * 3.14);
    float c = sdfBox(b3, vec3(0.1));

    return smin(hbox, min(min(w1, w2), c), 0.55);
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 finalColor = vec3(0.4, 0.4, 1.0);

    vec3 ro = vec3(0.0, 0.0, 0.0);
    vec3 rd = normalize(vec3(uv, 1.0));
    rd.yz *= rot2D((-2.0 + sin(time * 0.5)) * 0.5);
    rd.xy *= rot2D(time * 0.2);

    float t = 0.0;
    int i = 0;
    for (i = 0; i < 128; i++) {
        vec3 p = ro + rd * t;
        float d = sdfMap(p);
        t += d / 3.5;
        if (d < 0.01 || t > 100.0) {
            break;
        }
    }

    finalColor *= vec3(t * 0.04 + float(i) * 0.013);

    fragColor = vec4(finalColor, 1.0);
}
