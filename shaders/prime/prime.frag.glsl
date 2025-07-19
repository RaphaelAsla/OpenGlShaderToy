#version 330 core

out vec4 fragColor;

uniform vec2 resolution;
uniform vec2 mouse_pos;
uniform float time;

#define MAX_STEPS 256
#define STEP_SIZE 0.01
#define GRID_SIZE 0.05
#define WALL_THICKNESS 0.01
#define BOX_SIZE 0.4999

bool intersectBox(vec3 ro, vec3 rd, vec3 bmin, vec3 bmax, out float t0, out float t1) {
    vec3 inv_dir = 1.0 / rd;
    vec3 tmin = (bmin - ro) * inv_dir;
    vec3 tmax = (bmax - ro) * inv_dir;
    vec3 t1v = min(tmin, tmax);
    vec3 t2v = max(tmin, tmax);
    t0 = max(max(t1v.x, t1v.y), t1v.z);
    t1 = min(min(t2v.x, t2v.y), t2v.z);
    return t1 >= max(t0, 0.0);
}

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

ivec3 getGridCoords(vec3 p) {
    return ivec3(floor(p / GRID_SIZE));
}

bool hasAnyPrimeCoord(ivec3 coords) {
    return isPrime(abs(coords.x)) || isPrime(abs(coords.y)) || isPrime(abs(coords.z));
}

bool hasAllPrimeCoords(ivec3 coords) {
    return isPrime(abs(coords.x)) && isPrime(abs(coords.y)) && isPrime(abs(coords.z));
}

float isWall(vec3 p) {
    vec3 local = mod(p + 0.5 * GRID_SIZE, GRID_SIZE) - 0.5 * GRID_SIZE;
    vec3 d = abs(local);
    float wall = step(min(d.x, min(d.y, d.z)), WALL_THICKNESS);
    return wall;
}

vec3 getPrimeColor(vec3 p) {
    ivec3 coords = getGridCoords(p);

    if (hasAllPrimeCoords(coords)) {
        return vec3(0.0, 4.0, 0.0);
    } else if (hasAnyPrimeCoord(coords)) {
        return vec3(0.2, 0.1, 0.7);
    } else {
        return vec3(0.8, 0.8, 0.8);
    }
}

mat2 rot2D(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat2(
        c, -s,
        s, c
    );
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    float rot_time = 0.6;

    mat2 rotation_2d = rot2D(rot_time);

    vec3 ro = vec3(0.0, 0.0, 1.5);
    vec3 rd = normalize(vec3(uv, -1.0));

    rd.xz *= rotation_2d;
    ro.xz *= rotation_2d;
    rd.yx *= rotation_2d;
    ro.yx *= rotation_2d;

    vec3 box_min = vec3(-BOX_SIZE);
    vec3 box_max = vec3(BOX_SIZE);

    float t_enter, t_exit;
    if (!intersectBox(ro, rd, box_min, box_max, t_enter, t_exit)) {
        fragColor = vec4(0.0);
        return;
    }

    float t = t_enter;
    vec4 color = vec4(0.0);

    for (int i = 0; i < MAX_STEPS && t < t_exit && color.a < 0.99; ++i) {
        vec3 pos = ro + t * rd;

        vec3 prime_color = getPrimeColor(pos);
        vec4 sample_color = vec4(prime_color, 0.08) * isWall(pos);

        color.rgb += (1.0 - color.a) * sample_color.a * sample_color.rgb;
        color.a += (1.0 - color.a) * sample_color.a;

        t += STEP_SIZE;
    }

    fragColor = color;
}
