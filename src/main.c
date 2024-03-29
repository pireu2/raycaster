#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>


typedef float f32;
typedef double f64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long int i64;
typedef size_t usize;
typedef ssize_t isize;

typedef struct{
    f64 x,y;
}vec2;
typedef struct{
    i32 x,y;
}vec2i;

#define length(v) ({ const vec2 _v = (v); sqrtf((_v.x * _v.x) + (_v.y * _v.y)); })
#define normalize(v) ({ const vec2 _v = (v); f32 _l = length(_v); (vec2){_v.x / _l, _v.y / _l}; })
#define dot(v0,v1) ({ const vec2 _v0 = (v0), _v1 = (v1); (_v0.x * _v1.x) + (_v0.y * _v1.y); })
#define max(a,b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b);_a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b); _a < _b ? _a : _b; })
#define sign(a) ({ __typeof__(a) _a = (a); _a > 0 ? 1 : _a < 0 ? -1 : 0; })

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 224

#define MAP_SIZE 8

static u8 MAP_DATA[MAP_SIZE * MAP_SIZE] = {
        1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,1,
        1,0,0,0,0,3,0,1,
        1,0,0,0,0,0,0,1,
        1,2,0,0,4,4,0,1,
        1,0,0,0,4,0,0,1,
        1,3,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1
};

struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;
    vec2 pos,dir,plane;
} state;

static void draw_vertical_line(i32 x, i32 y0, i32 y1, u32 color){
    y0 = max(0, y0);
    y1 = min(SCREEN_HEIGHT, y1);
    for(i32 y = y0; y < y1; y++) {
        state.pixels[y * SCREEN_WIDTH + x] = color;
    }
}

static void render(){
    for(int x = 0; x<SCREEN_WIDTH;x++){
        const f64 xcam =  2 * x / (f64)(SCREEN_WIDTH)  - 1;
        const vec2 raydir = (vec2){
            state.dir.x + state.plane.x * xcam,
            state.dir.y + state.plane.y * xcam
        };

        vec2 pos = state.pos;
        vec2i map_pos = { (int) pos.x, (int) pos.y };

        const vec2 delta_dist = (vec2){
            (raydir.x == 0)  ? 1e30 : fabs( 1 / raydir.x),
            (raydir.y == 0) ? 1e30 : fabs(1 / raydir.y)
        };



        vec2 side_dist = (vec2){
            delta_dist.x * (raydir.x < 0 ? (pos.x - map_pos.x) : (map_pos.x + 1 - pos.x)),
            delta_dist.y * (raydir.y < 0 ? (pos.y - map_pos.y ): (map_pos.y + 1 - pos.y))
        };

        const vec2i step = {(int) sign(raydir.x), (int) sign(raydir.y)};

        i32 hit = 0, side;

        while(!hit){
            if(side_dist.x < side_dist.y){
                side_dist.x += delta_dist.x;
                map_pos.x += step.x;
                side = 0;
            }
            else{
                side_dist.y += delta_dist.y;
                map_pos.y += step.y;
                side = 1;
            }

            if(MAP_DATA[map_pos.y * MAP_SIZE + map_pos.x]){
                hit = 1;
            }
        }


        u32 color;
        switch(MAP_DATA[map_pos.y * MAP_SIZE + map_pos.x]){
            case 1:
                color = 0xFF0000;
                break;
            case 2:
                color = 0x00FF00;
                break;
            case 3:
                color = 0x0000FF;
                break;
            case 4:
                color = 0xFFFFFF;
                break;
            default:
                color = 0xFFFF00;
                break;
        }

        if(side == 1){
            color = (color >> 1) & 0x7F7F7F;
        }
        const f64 wall_dist =
                side == 0 ?
                (side_dist.x - delta_dist.x) :
                (side_dist.y - delta_dist.y);
        const i32 h = (i32) (SCREEN_HEIGHT / wall_dist);
        const i32 y0 = max((SCREEN_HEIGHT / 2) - (h / 2), 0);
        const i32 y1 = min((SCREEN_HEIGHT / 2) + (h / 2), SCREEN_HEIGHT - 1);

        draw_vertical_line(x, y0, y1, color);
        draw_vertical_line(x, 0, y0, 0xFF202020);
        draw_vertical_line(x, y1, SCREEN_HEIGHT - 1, 0xFF505050);
    }
}

static void rotate(f32 rotation){
    const vec2 d = state.dir;
    const vec2 p = state.plane;
    state.dir.x = d.x * cosf(rotation) - d.y * sinf(rotation);
    state.dir.y = d.x * sinf(rotation) + d.y * cosf(rotation);
    state.plane.x = p.x * cosf(rotation) - p.y * sinf(rotation);
    state.plane.y = p.x * sinf(rotation) + p.y * cosf(rotation);
}

static void process_events(){
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_QUIT:
                state.quit = true;
                break;
        }
    }
    const f32 rotation_speed= 3.0f * 0.0174533f;
    const f32 move_speed = 3.0f * 0.0174533f;

    const u8 *keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_A]){
        rotate(+rotation_speed);
    }
    if(keys[SDL_SCANCODE_D]){
        rotate(-rotation_speed);
    }
    if(keys[SDL_SCANCODE_W]){
        state.pos.x += state.dir.x * move_speed;
        state.pos.y += state.dir.y * move_speed;
    }
    if(keys[SDL_SCANCODE_S]){
        state.pos.x -= state.dir.x * move_speed;
        state.pos.y -= state.dir.y * move_speed;
    }
}

static void init_window(){
    if(SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    }
    state.window =
            SDL_CreateWindow(
                    "Raycaster",
                    SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                    SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                    1280,
                    720,
                    SDL_WINDOW_ALLOW_HIGHDPI
            );
    if(!state.window){
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    }

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    if(!state.renderer){
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
    }

    state.texture =
            SDL_CreateTexture(
                    state.renderer,
                    SDL_PIXELFORMAT_ARGB8888,
                    SDL_TEXTUREACCESS_STREAMING,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT
            );
    if(!state.texture){
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
    }
}

int main(int argc, char* argv[]) {
    init_window();

    state.pos = (vec2) {2,1};
    state.dir = (vec2) { -1.0f, 0.0f };
    state.plane = (vec2) {0,0.66f};

    u32 time;
    u32 old_time;

    while(!state.quit){
        old_time = time;
        time = SDL_GetTicks();
        f32 delta_time = (f32) (time - old_time) / 1000.0f;
        printf("FPS: %f\n", 1.0f / delta_time);

        process_events();
        memset(state.pixels, 0, sizeof(state.pixels));
        render();

        SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * sizeof(u32));
        SDL_RenderCopyEx(state.renderer, state.texture, NULL, NULL,0.0,NULL,SDL_FLIP_VERTICAL);
        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
}









