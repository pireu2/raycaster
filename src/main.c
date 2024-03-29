#include "config.h"

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 224

#define MAP_SIZE 8
#define MINIMAP_SCALE 5

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

static void draw_player_position(){

}

static void draw_minimap(){
    for(i32 i = 0; i < MAP_SIZE; i++) {
        for (i32 j = 0; j < MAP_SIZE; j++) {
            u32 color;
            switch (MAP_DATA[j * MAP_SIZE + i]) {
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
                    color = 0x00000;
                    break;
            }
            for(i32 x = 0; x < MINIMAP_SCALE; x++){
                for(i32 y = 0; y < MINIMAP_SCALE; y++){
                    state.pixels[(j * MINIMAP_SCALE + y) * SCREEN_WIDTH + (i * MINIMAP_SCALE + x)] = color;
                }
            }
        }
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
        vec2i map_pos = { (i32) pos.x, (i32) pos.y };

        const vec2 delta_dist = (vec2){
            (raydir.x == 0)  ? 1e30 : fabs( 1 / raydir.x),
            (raydir.y == 0) ? 1e30 : fabs(1 / raydir.y)
        };

        vec2 side_dist = (vec2){
            delta_dist.x * (raydir.x < 0 ? (pos.x - map_pos.x) : (map_pos.x + 1 - pos.x)),
            delta_dist.y * (raydir.y < 0 ? (pos.y - map_pos.y ): (map_pos.y + 1 - pos.y))
        };

        const vec2i step = {(i32) sign(raydir.x), (i32) sign(raydir.y)};

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
    draw_minimap();
    draw_player_position();
}

static void rotate(f64 rotation){
    const vec2 d = state.dir;
    const vec2 p = state.plane;
    state.dir.x = d.x * cos(rotation) - d.y * sin(rotation);
    state.dir.y = d.x * sin(rotation) + d.y * cos(rotation);
    state.plane.x = p.x * cos(rotation) - p.y * sin(rotation);
    state.plane.y = p.x * sin(rotation) + p.y * cos(rotation);
}

static void process_events(f64 dt){
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_QUIT:
                state.quit = true;
                break;
            case SDL_MOUSEMOTION:
                rotate(-event.motion.xrel * 0.05f *  dt);
                break;
        }
    }
    const f64 rotation_speed= 3.0f * dt;
    const f64 move_speed = 3.0f * dt;

    const u8 *keys = SDL_GetKeyboardState(NULL);

    if(keys[SDL_SCANCODE_ESCAPE]){
        state.quit = true;
    }
    if(keys[SDL_SCANCODE_A]){
        vec2 new_pos = {
            state.pos.x - state.dir.x * move_speed,
            state.pos.y
        };
        if(MAP_DATA[(i32)new_pos.y * MAP_SIZE + (i32)new_pos.x] == 0)
            state.pos = new_pos;
    }
    if(keys[SDL_SCANCODE_D]){
        vec2 new_pos = {
            state.pos.x + state.dir.x * move_speed,
            state.pos.y
        };
        if(MAP_DATA[(i32)new_pos.y * MAP_SIZE + (i32)new_pos.x] == 0)
            state.pos = new_pos;

    }
    if(keys[SDL_SCANCODE_W]){
        vec2 new_pos = {
            state.pos.x + state.dir.x * move_speed,
            state.pos.y + state.dir.y * move_speed
        };
        if(MAP_DATA[(i32)new_pos.y * MAP_SIZE + (i32)new_pos.x] == 0)
            state.pos = new_pos;

    }
    if(keys[SDL_SCANCODE_S]){
        vec2 new_pos = {
            state.pos.x - state.dir.x * move_speed,
            state.pos.y - state.dir.y * move_speed
        };
        if(MAP_DATA[(i32)new_pos.y * MAP_SIZE + (i32)new_pos.x] == 0)
            state.pos = new_pos;

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

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_WarpMouseInWindow(state.window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

int main(int argc, char* argv[]) {
    init_window();

    state.pos = (vec2) {2,2};
    state.dir = (vec2) { -1.0f, 0.0f };
    state.plane = (vec2) {0,0.66f};

    u32 time;
    u32 old_time;

    while(!state.quit){
        old_time = time;
        time = SDL_GetTicks();
        f64 delta_time = (f64) (time - old_time) / 1000.0f;

        process_events(delta_time);
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









