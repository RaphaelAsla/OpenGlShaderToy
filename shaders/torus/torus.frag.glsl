#version 430 core

out vec4 fragColor;
uniform vec2 resolution;
uniform vec2 mouse_pos;
uniform float time;

#define LIGHT_POS vec3(-2.0, 4.0, -3.0)
#define LIGHT_COLOR vec3(1.0, 0.9, 0.7)
#define AMBIENT 0.1

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

float sdTorus(vec3 p, vec2 t) {
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

vec3 getColor(vec3 p, float majorRadius, float scale) {
    float u = atan(p.z, p.x);
    vec2 q = vec2(length(p.xz) - majorRadius, p.y);
    float v = atan(q.y, q.x);

    vec2 uv = vec2(u, v) / (2.0 * 3.14159265);
    uv = fract(uv * scale);

    float checker = mod(step(0.5, uv.x) + step(0.5, uv.y), 2.0);
    return vec3(checker);
}

float sdMap(vec3 p) {
    float ground = p.y + 0.2;

    vec3 center = vec3(0.0, 0.5, 0.0);

    vec3 d = p - center;

    d.xz *= rot2D(time * 0.5);
    d.yz *= rot2D(time * 0.5);

    vec3 ip = d;
    vec3 ip2 = d;

    d.zy *= rot2D(radians(90.0));
    ip.xy *= rot2D(time);
    ip.x -= 0.3;
    ip2.xy *= rot2D(time);
    ip2.yz *= rot2D(radians(90.0));
    ip2.x -= 0.3;
    ip2.xy *= rot2D(time);
    ip2.x -= 0.2;

    float outer = sdTorus(d, vec2(0.3, 0.04));
    float inner = sdTorus(ip, vec2(0.2, 0.03));
    float inner2 = sdTorus(ip2, vec2(0.1, 0.02));

    float dist = min(inner2, min(inner, outer));

    return min(ground, dist);
}

vec3 getSurfaceNormal(vec3 p) {
    float d = sdMap(p);
    vec2 e = vec2(.001, 0);
    vec3 n = d - vec3(
                sdMap(p - e.xyy),
                sdMap(p - e.yxy),
                sdMap(p - e.yyx));
    return normalize(n);
}

float rayMarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    int i = 0;
    for (; i < 128; i++) {
        vec3 p = ro + rd * t;
        float d = sdMap(p);
        t += d;
        if (d < 0.001 || t > 100.0) {
            break;
        }
    }
    return t;
}

float softShadow(vec3 ro, vec3 rd, float mint, float maxt, float k) {
    float res = 1.0;
    float t = mint;

    for (int i = 0; i < 32; i++) {
        if (t < maxt) {
            float h = sdMap(ro + rd * t);
            if (h < 0.001)
                return 0.0;
            res = min(res, k * h / t);
            t += h;
        }
    }

    return res;
}

vec3 getLight(vec3 p) {
    vec3 n = getSurfaceNormal(p);
    vec3 l = normalize(LIGHT_POS - p);

    float diff = max(dot(n, l), 0.0);

    vec3 r = reflect(-l, n);
    vec3 v = normalize(vec3(0.0, 0.0, 5.0) - p);
    float spec = pow(max(dot(r, v), 0.0), 32.0);

    vec3 diffuse = diff * LIGHT_COLOR;
    vec3 specular = spec * LIGHT_COLOR * 0.5;
    vec3 ambient = vec3(AMBIENT);

    float shadow = softShadow(p + n * 0.001 * 2.0, l, 0.02, 5.0, 16.0);

    return ambient + (diffuse + specular) * shadow;
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 finalColor = vec3(0.8, 0.4, 0.6);

    vec3 ro = vec3(0.0, 1.0, -0.5);
    vec3 rd = normalize(vec3(uv, 0.8));

    rd.yz *= rot2D(radians(45.0));

    float t = rayMarch(ro, rd);
    vec3 p = ro + rd * t;

    finalColor = getLight(p) * getColor(p, 0.5, 4.0);
    finalColor = pow(finalColor, vec3(0.4545));
    fragColor = vec4(finalColor, 1.0);
}
