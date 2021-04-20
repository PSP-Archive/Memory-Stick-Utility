#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include <vlf.h>
#include <PRXs.h>

extern int app_main(int argc, char *argv[]);
extern char *path;

int start_thread(SceSize args, void *argp)
{
    path = (char *)argp;
    int last_trail = -1;
    int i;

    if(path){
       for (i = 0; path[i]; i++){
          if (path[i] == '/')
             last_trail = i;
       }
    }

	if(last_trail >= 0){path[last_trail] = 0;}

	sceIoChdir(path);

	LoadStartModuleBuffer("iop.prx", iop, size_iop);
	LoadStartModuleBuffer("intraFont.prx", intraFont, size_intraFont);
	LoadStartModuleBuffer("vlf.prx", vlf, size_vlf);

	vlfGuiInit(-1, app_main);

	return sceKernelExitDeleteThread(0);
}

int _start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("start_thread", start_thread, 0x10, 0x4000, 0, NULL);
	if (thid < 0)
		return thid;

	sceKernelStartThread(thid, args, argp);
	
	return 0;
}