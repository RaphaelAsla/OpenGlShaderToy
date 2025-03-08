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

float sdfSphere(vec3 p, float radius) {
    return length(p) - radius;
}

float sdfBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdfMap(vec3 p) {
    float limits = sdfBox(p - vec3(0.5, 0.2, -3.0), vec3(5.0));
    limits = abs(limits) - 0.1;

    vec3 boxOne = p;
    boxOne.x += time * 0.8;
    boxOne.z += 1.5;
    boxOne.y += sin(p.x * 3.14) * 0.5;
    float id = floor(boxOne.x);
    boxOne.x = mod(boxOne.x, 0.3) - 0.15;
    boxOne.y = mod(boxOne.y, 1.0) - 0.5;
    boxOne.z = mod(boxOne.z, 1.5) - 0.75;
    boxOne.yz *= rot2D(sin(p.x * 6.28 + time));
    boxOne.yz *= rot2D(time * 2.0);
    float waveOne = sdfBox(boxOne, vec3(0.1));

    vec3 boxTwo = p;
    boxTwo.y += time * 0.8;
    boxTwo.z += 1.5;
    boxTwo.x += 0.5;
    boxTwo.z += sin(p.y * 3.14) * 0.5;
    id = floor(boxTwo.y);
    boxTwo.y = mod(boxTwo.y, 0.3) - 0.15;
    boxTwo.x = mod(boxTwo.x, 1.0) - 0.5;
    boxTwo.z = mod(boxTwo.z, 1.5) - 0.75;
    boxTwo.xz *= rot2D(sin(p.y * 6.28 + time));
    boxTwo.xz *= rot2D(-time * 2.0);
    float waveTwo = sdfBox(boxTwo, vec3(0.1));

    vec3 boxThree = p;
    boxThree.xy = fract(boxThree.xy) - 0.5;
    boxThree.z = mod(boxThree.z, 0.2) - 0.1;
    boxThree.xy *= rot2D(sin(p.z * 6.28 + time));
    boxThree.xy *= rot2D(time * 2.0);
    float columns = sdfBox(boxThree, vec3(0.1));

    return smin(limits, min(min(waveOne, waveTwo), columns), 1.0);
}

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);
    return a + b * cos(6.28318 * (c * t * d));
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 finalColor = vec3(0.4, 0.4, 1.0);

    vec3 ro = vec3(0.5, 0.2, -3.0);
    vec3 rd = normalize(vec3(uv, 1.0));

    rd.yz *= rot2D((-2.0 + sin(time * 0.5)) * 0.5);
    rd.xy *= rot2D(time * 0.2);

    float t = 0.0;

    int i = 0;
    for (i = 0; i < 128; i++) {
        vec3 p = ro + rd * t;

        float d = sdfMap(p);

        t += d / 2.0;

        if (d < 0.01 || t > 100.0) {
            break;
        }
    }

    finalColor *= palette(t * 0.04 + float(i) * 0.013);

    fragColor = vec4(finalColor, 1.0);
}
