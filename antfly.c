#include <signal.h>
#ifdef __unix__
#include <linux/time.h>
#else
#include <time.h>
#endif
#include <stdio.h>  
#include <unistd.h> 
#include "antfx.h"
#include "lib/lua-5.3.4/lua.h"
#include "lib/lua-5.3.4/lauxlib.h"
#include "lib/lua-5.3.4/lualib.h"

int luaopen_antfx(lua_State *L);

void shutdown(int sig)
{
    antfx_release();
    exit(0);
}

void print_help(char* cmd)
{
    printf("Usage: %s <options> script_name.lua\n",cmd);
    printf("\t-w: screen width\n");
    printf("\t-h: screen height\n");
    printf("\t-b: screen bits per pixel\n");
    printf("\t-d: fb device\n");
}

static int l_init(lua_State *L)
 {
	const char* data = luaL_checkstring(L,1);
	printf("DATA: %s\n", data);
	return 1;
 }

static const struct luaL_Reg antfx_api [] = {
        {"init", l_init},
	   {NULL,NULL}  
};

int main(int argc, char* argv[]) 
{
    signal(SIGPIPE, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	signal(SIGINT, shutdown);
    antfx_prepare();


    int opt;
    int w = 480;
    int h = 320;
    int bpp = 16;
    char* dev = NULL;
    char* script;

    while((opt = getopt(argc, argv, "w:h:b:d:s:")) != -1)  
    {  
        switch(opt)  
        {   
            case 'w':  
                w = atoi(optarg);  
                break;  
            case 'h':  
                h = atoi(optarg);
                break;  
            case 'b':  
                bpp = atoi(optarg); 
                break;  
            case 'd':  
                dev = optarg; 
                break;  
            case 's':
                script = optarg;
                break;
            case '?':
                print_help(argv[0]);
                exit(1);
        }
    }  
    if(optind > 0)
        script = argv[optind];
    
    if(!dev)
        dev = "/dev/fb1";

    LOG("Configuration: w:%d h%d bpp:%d dev:%s script: %s\n",w,h,bpp, dev, script);
    engine_config_t conf;
    conf.default_w = w;
    conf.default_h = h;
    conf.defaut_bbp = bpp;
    conf.dev = dev;
    // start display engine
    antfx_init(conf);
    if(script)
    {
        lua_State* L = NULL;
        L = luaL_newstate();
        luaL_openlibs(L);
        luaL_newlib(L, antfx_api);
        lua_setglobal(L, "antfx");
        if (luaL_loadfile(L, script) || lua_pcall(L, 0, 0, 0))
        {
            LOG( "cannot run apis. file: %s\n", lua_tostring(L, -1));
        }
        if(L)
            lua_close(L);
    }
    else
    {
        afx_font_t font = SYS_FONT;
        while(1)
        {
            all_white();
            _put_text("Welcome to antfx display engine",_P(10,20),(color_t){0,122,204,0},font);
            render();
            nanosleep((const struct timespec[]){{0, 30000000L}}, NULL);
        }

    }
    shutdown(0);
}

