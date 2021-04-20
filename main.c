#include <pspkernel.h>
#include <string.h>
#include <pspusbdevice.h>
#include <pspusbstor.h>
#include <pspusb.h>
#include <vlf.h>
#include <main.h>
#include <IPLs.h>
#include <pspiofilemgr_stat.h>

PSP_MODULE_INFO("Memory Stick Utility", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();

#define appname "Memory Stick Utility"
#define version "0.6"

char buffer[10485760];
char *mode = "Main";
char *path;
MSStruct MSInfo;

/*
	Callbacks
*/
int exit_callback(int arg1, int arg2, void *common)
{
	sceIoRemove(strcat(strdup(path), "/iop.prx"));
	sceIoRemove(strcat(strdup(path), "/intraFont.prx"));
	sceIoRemove(strcat(strdup(path), "/vlf.prx"));

	sceKernelExitGame();
	return 0;
}
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

int SetupCallbacks()
{
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0){sceKernelStartThread(thid, 0, 0);}
	return thid;
}
/*
	Menus & VLF
*/
int OnBackToMainMenu(int enter)
{
	if(!enter){
		int i;

		if(mode == "Main"){int exit = vlfGuiMessageDialog("Do you want to exit?", VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO);if(exit == 1){exit_callback(0, 0, NULL);}};
		if(mode == "USB Connection"){mode = "Main";}
		else if(mode == "Inject IPL to Memory Stick"){mode = "Main";}
		else if(mode == "Erase IPL from Memory Stick"){mode = "Main";}
		else if(mode == "Check Memory Stick Information"){mode = "Main";}
		else if(mode == "Extract Memory Stick Data"){mode = "Main";}
		else if(mode == "USB Connection.1"){pspUsbDeviceFinishDevice();sceUsbDeactivate(0x1c8);sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);mode = "USB Connection";}
		else if(mode == "Inject IPL to Memory Stick.1"){mode = "Inject IPL to Memory Stick";}
		else if(mode == "Erase IPL from Memory Stick.1"){mode = "Erase IPL from Memory Stick";}
		else if(mode == "Check Memory Stick Information.1"){mode = "Check Memory Stick Information";}
		else if(mode == "Extract Memory Stick Data.1"){mode = "Extract Memory Stick Data";}

		vlfGuiRemovePicture(pic);
		for(i = 0; i < N_HI_ITEMS; i++){vlfGuiRemoveText(hi_texts[i]);}
		vlfGuiCancelBottomDialog();
		vlfGuiCancelCentralMenu();
		MainMenu(0);

		vlfGuiBottomDialog(VLF_DI_BACK, -1, 1, 0, VLF_DEFAULT, OnBackToMainMenu);
	}

	return VLF_EV_RET_NOTHING;
}
int ResetScreen(int showmenu, int showback)
{
	int i;

	vlfGuiRemovePicture(pic);
	for(i = 0; i < N_HI_ITEMS; i++){vlfGuiRemoveText(hi_texts[i]);}
	vlfGuiCancelBottomDialog();
	vlfGuiCancelCentralMenu();

	if(showmenu == 1){MainMenu(0);}
	if(showback == 1){vlfGuiBottomDialog(VLF_DI_BACK, -1, 1, 0, VLF_DEFAULT, OnBackToMainMenu);}

	return VLF_EV_RET_NOTHING;
}

int OnMainMenuSelect(int sel)
{
	ResetScreen(0, 0);

	if(mode == "Main"){
		if(sel == 0){mode = "USB Connection"; ResetScreen(1, 1);}
		else if(sel == 1){mode = "Inject IPL to Memory Stick"; ResetScreen(1, 1);} // show ipl injection menu
		else if(sel == 2){mode = "Erase IPL from Memory Stick"; ResetScreen(1, 1);} // show ipl erase menu
		else if(sel == 3){mode = "Check Memory Stick Information"; ResetScreen(1, 1);} // show ipl check menu
		else if(sel == 4){mode = "Extract Memory Stick Data"; ResetScreen(1, 1);} // show ipl extraction menu
		else if(sel == 5){about();} // show about
	}
	else if(mode == "USB Connection")
	{
		mode = "USB Connection.1";
		if(sel == 0){ConnectUSB(-1);} // connect ms0 to usb
		else if(sel == 1){ConnectUSB(PSP_USBDEVICE_FLASH0);} // connect flash0 to usb
		else if(sel == 2){ConnectUSB(PSP_USBDEVICE_FLASH1);} // connect flash1 to usb
		else if(sel == 3){ConnectUSB(PSP_USBDEVICE_FLASH2);} // connect flash2 to usb
		else if(sel == 4){ConnectUSB(PSP_USBDEVICE_FLASH3);} // connect flash3 to usb
		else if(sel == 5){ConnectUSB(PSP_USBDEVICE_UMD9660);} // connect umd disc to usb
	}
	else if(mode == "Inject IPL to Memory Stick"){
		mode = "Inject IPL to Memory Stick.1";
		if(sel == 0){injectIPL("msstor:", TimeMachineIPL, size_TimeMachineIPL);} // inject time machine ipl
		else if(sel == 1){injectIPL("msstor:", BoostersMultiIPL, size_BoostersMultiIPL);} // inject boosters multi ipl
		else if(sel == 2){injectIPL("msstor:", z3ros0ulSingleIPL, size_z3ros0ulSingleIPL);} // inject z3ros0ul single ipl
		else if(sel == 3){injectIPL("msstor:", z3ros0ulMultiIPL, size_z3ros0ulMultiIPL);} // inject z3ros0ul multi ipl
		else if(sel == 4){injectIPL("msstor:", ClassicPandoraIPL, size_ClassicPandoraIPL);} // inject classic pandora ipl
		else if(sel == 5){injectIPL_File("msstor:", "ms0:/ipl.bin");} // inject ms0:/ipl.bin
	}
	else if(mode == "Erase IPL from Memory Stick"){
		mode = "Erase IPL from Memory Stick.1";
		if(sel == 0){eraseIPL("msstor:");} // erase ipl from memory stick
	}
	else if(mode == "Check Memory Stick Information"){
		mode = "Check Memory Stick Information.1";
		if(sel == 0){checkIPL("msstor:", 0);} // check injected ipl
		else if(sel == 1){checkIPL("ms0:/ipl.bin", 1);} // check ms0:/ipl.bin
		else if(sel == 2){checkMSinfo("msstor:", 0);} // check ms mbr
		else if(sel == 3){checkMSinfo("ms0:/mbr.bin", 2);} // check ms0:/mbr.bin
		else if(sel == 4){createCheckSum("msstor:", "ms0:/checksum.bin", 0);} // create a checksum of the first 4096 bytes of the injected ipl
		else if(sel == 5){createCheckSum("ms0:/ipl.bin", "ms0:/checksum.bin", 1);} // create a checksum of the first 4096 bytes of the ipl at ms0:/ipl.bin
	}
	else if(mode == "Extract Memory Stick Data"){
		mode = "Extract Memory Stick Data.1";
		if(sel == 0){extractIPL("msstor:", "ms0:/ipl.bin", 0);} // extract ipl from ms to ms0:/ipl.bin
		else if(sel == 1){extractIPL("msstor:", "ms0:/ipl.bin", 1);} // extract entire ipl space from ms to ms0:/ipl.bin
		else if(sel == 2){extractData("msstor:", "ms0:/mbr.bin", 512, 0);} // extract mbr from ms to ms0:/mbr.bin
	}

    return VLF_EV_RET_NOTHING;
}

void MainMenu(int sel)
{
	int numitems;

	if(mode == "Main"){
		char *items[] ={
			"USB Connection",
			"Inject IPL to Memory Stick",
			"Erase IPL from Memory Stick",
			"Check Memory Stick Information",
			"Extract Memory Stick Data",
			"About"
		};
		vlfGuiCentralMenu(sizeof(items) / sizeof(items[0]), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == "USB Connection"){
		char *items[] ={
			"Memory Stick",
			"Flash 0",
			"Flash 1",
			"Flash 2",
			"Flash 3",
			"UMD Disc"
		};
		vlfGuiCentralMenu(sizeof(items) / sizeof(items[0]), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == "Inject IPL to Memory Stick"){
		char *items[] ={
			"Inject Time Machine IPL",
			"Inject Boosters Multi IPL",
			"Inject z3ros0ul Single IPL (Sleep Fix)",
			"Inject z3ros0ul Multi IPL (Sleep Fix)",
			"Inject Classic Pandora IPL",
			"Inject IPL from ms0:/ipl.bin"
		};
		vlfGuiCentralMenu(sizeof(items) / sizeof(items[0]), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == "Erase IPL from Memory Stick"){
		char *items[] ={
			"Erase IPL from Memory Stick"
		};
		vlfGuiCentralMenu(sizeof(items) / sizeof(items[0]), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == "Check Memory Stick Information"){
		char *items[] ={
			"Check Injected IPL",
			"Check IPL at ms0:/ipl.bin",
			"Check Memory Stick MBR",
			"Check MBR at ms0:/mbr.bin",
			"Create Checksum of Injected IPL",
			"Create Checksum of ms0:/ipl.bin"
		};
		vlfGuiCentralMenu(sizeof(items) / sizeof(items[0]), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == "Extract Memory Stick Data"){
		char *items[] ={
			"Extract Memory Stick IPL",
			"Extract Entire Memory Stick IPL Space",
			"Extract Memory Stick MBR"
		};
		vlfGuiCentralMenu(sizeof(items) / sizeof(items[0]), items, sel, OnMainMenuSelect, 0, 0);
	}

	vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, NULL);
}

int app_main()
{
	SetupCallbacks();

    vlfGuiCacheResource("system_plugin");
	vlfGuiCacheResource("system_plugin_fg");

	vlfGuiSetModelSystem();
	vlfGuiSetModelSpeed(1.0f);

	MainMenu(0);

	VlfText title = vlfGuiAddTextF(0, 0, "%s v%s", appname, version);
	VlfPicture pic = vlfGuiAddPictureResource("sysconf_plugin", "tex_bar_init_icon", 4, -2);
	vlfGuiSetTitleBar(title, pic, 1, 0);

	vlfGuiSystemSetup(1, 1, 1);

	vlfGuiBottomDialog(VLF_DI_BACK, -1, 1, 0, VLF_DEFAULT, OnBackToMainMenu);

	while(1){vlfGuiDrawFrame();}

   	return 0;
}
/*
	IPL
*/
void extractData(char* device, char* outfile, SceSize size, int seek) // use 0 as type for size detection
{
	SceUID in, out;
	int i = 0;
	u8 sha1[20];

	sceIoRemove(outfile);

	in = sceIoOpen(device, PSP_O_RDONLY, 0777); // open the input (usually a drive)
	if(in < 1 ){vlfGuiMessageDialog("Unable to open Input Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	out = sceIoOpen(outfile, PSP_O_WRONLY|PSP_O_CREAT, 0777); // open the output (usually a file)
	if(in < 1 ){vlfGuiMessageDialog("Unable to open Output File", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}

	sceIoLseek(in, seek, 0); // go to the data
	sceIoRead(in, buffer, (size + 512) & 0xFFFFFE00);
	sceIoWrite(out, buffer, size);

	sceIoClose(out);
	sceIoClose(in);

	ResetScreen(0, 1);
	hi_texts[1] = vlfGuiAddTextF(40, 100, "%i bytes (%i blocks) written", size, size / 4096);
	hi_texts[2] = vlfGuiAddTextF(40, 120, "Data has been saved to %s", outfile);
}
void extractIPL(char* device, char* outfile, int type) // use 0 as type for size detection
{
	SceUID in, out;
	int i = 0;
	u8 sha1[20];

	sceIoRemove(outfile);

	getMSinfo(device, 0);

	in = sceIoOpen(device, PSP_O_RDONLY, 0777); // open the input (usually a drive)
	if(in < 1 ){vlfGuiMessageDialog("Unable to open Input Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	out = sceIoOpen(outfile, PSP_O_WRONLY|PSP_O_CREAT, 0777); // open the output (usually a file)
	if(in < 1 ){vlfGuiMessageDialog("Unable to open Output File", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}

	sceIoLseek(in, 0x2000, 0); // go to the ipl
		
	if(MSInfo.IPLSize != -1 & type == 0){ // iplsize is -1 when ipl cannot be detected
		i = MSInfo.IPLSize;
		sceIoRead(in, buffer, (MSInfo.IPLSize + 512) & 0xFFFFFE00); // read the ipl (must be read in blocks of 512 bytes)
		sceIoWrite(out, buffer, MSInfo.IPLSize); // write the ipl (excluding extra read bytes)
	}
	else{
		while(1){
			sceIoRead(in, buffer, 4096); // read a block
			
			// generate a checksum of the block to compare with a pre-computer blank checksum
			SceKernelUtilsSha1Context ctx;
			sceKernelUtilsSha1BlockInit(&ctx);
			sceKernelUtilsSha1BlockUpdate(&ctx, (u8*) buffer, 4096);
			sceKernelUtilsSha1BlockResult(&ctx, sha1);

			if(memcmp(sha1, sha1_blank, 20) == 0 & type == 0) {break;} // stop dumping after there is a blank block
			if(i >= MSInfo.IPLSpace){break;} // stop dumping after reaching end of mbr data
	
			
			sceIoWrite(out, buffer, 4096); // write a block
			i = i + 4096; // increase block counter
		}
	}

	sceIoClose(out);
	sceIoClose(in);

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(40, 100, "Detected IPL Name: %s", MSInfo.IPLName);
	hi_texts[1] = vlfGuiAddTextF(40, 120, "%i bytes (%i blocks) written", i, i / 4096);
	hi_texts[2] = vlfGuiAddTextF(40, 140, "IPL has been saved to %s", outfile);
}
void eraseIPL(char *device)
{
	SceUID out;
	int i = 0;

	getMSinfo(device, 0);

	out = sceIoOpen(device, PSP_O_WRONLY, 0777); // open the output (usually a drive)
	if(out < 1){vlfGuiMessageDialog("Unable to open Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	sceIoLseek(out, 0x2000, 0); // go to the ipl
	memset(buffer, 0x00, MSInfo.IPLSpace);
	sceIoWrite(out, buffer, MSInfo.IPLSpace);
	sceIoClose(out);

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(100, 120, "%i bytes (%i blocks) erased", MSInfo.IPLSpace, MSInfo.IPLSpace / 4096);
}
void injectIPL_File(char* device, char* infile)
{
	SceUID in, out;
	int size;

	memset(buffer, 0x00, 10485760);

	in = sceIoOpen(infile, PSP_O_RDONLY, 0777); // open the input (usually a file)
	if(in < 1){vlfGuiMessageDialog("Unable to open Input File", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	size = sceIoRead(in, buffer, 10485760);
	sceIoClose(in);

	if(size != (size / 512) * 512){size = (size + 512) & 0xFFFFFE00;}
	getMSinfo(device, 0);
	if(MSInfo.IPLSpace < size){vlfGuiMessageDialog("Insufficient Reserved Sector Space", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	
	out = sceIoOpen(device, PSP_O_WRONLY, 0777); // open the output (usually a drive)
	if(out < 1){vlfGuiMessageDialog("Unable to open Output Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	sceIoLseek(out, 0x2000, 0); // go to the ipl location
	sceIoWrite(out, buffer, size); // write the ipl
	sceIoClose(out);

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(60, 120, "%i bytes (%i blocks) written", size, size / 4096);
	hi_texts[1] = vlfGuiAddText(60, 140, "IPL has been injected to Memory Stick");
}
void injectIPL(char* device, char* data, int size)
{
	SceUID out;

	memset(buffer, 0x00, 10485760);
	memcpy(buffer, data, size);

	if(size != (size / 512) * 512){size = (size + 512) & 0xFFFFFE00;}
	getMSinfo(device, 0);
	if(MSInfo.IPLSpace < size){vlfGuiMessageDialog("Insufficient Reserved Sector Space", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}

	out = sceIoOpen(device, PSP_O_WRONLY, 0777); // open the output (usually a drive)
	if(out < 1){vlfGuiMessageDialog("Unable to open Output Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	
	sceIoLseek(out, 0x2000, 0); // go to the ipl location
	sceIoWrite(out, buffer, size); // write the ipl
	sceIoClose(out);

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(60, 120, "%i bytes (%i blocks) written", size, size / 4096);
	hi_texts[1] = vlfGuiAddText(60, 140, "IPL has been injected to Memory Stick");
}
void checkIPL(char* input, int type) // use 0 for injected ipl or 1 for file
{
	int error = getMSinfo(input, type);
	if(error == -1){vlfGuiMessageDialog("Unable to open Input File/Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(40, 120, "Detected IPL Name: %s", MSInfo.IPLName);
	if(MSInfo.IPLSize == -1){hi_texts[1] = vlfGuiAddText(40, 140, "Detected IPL Size: Unknown");}else{hi_texts[1] = vlfGuiAddTextF(40, 140, "Detected IPL Size: %i bytes", MSInfo.IPLSize);}
}
void createCheckSum(char* input, char* outfile, int type) // use 0 for injected ipl or 1 for file
{
	SceUID in, out;
	u8 sha1[20];

	in = sceIoOpen(input, PSP_O_RDONLY, 0777); // open the input (usually a drive)
	if(in < 1){vlfGuiMessageDialog("Unable to open Input Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}
	out = sceIoOpen(outfile, PSP_O_WRONLY|PSP_O_CREAT, 0777); // open the output (usually a file)
	if(out < 1){vlfGuiMessageDialog("Unable to open Output File", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}

	if(type == 0){sceIoLseek(in, 0x2000, 0);} // go to the ipl

	memset(buffer, 0x00, 4096);
	sceIoRead(in, buffer, 4096); // read a block

	// generate a checksum of the block
	SceKernelUtilsSha1Context ctx;
	sceKernelUtilsSha1BlockInit(&ctx);
	sceKernelUtilsSha1BlockUpdate(&ctx, (u8*) buffer, 4096);
	sceKernelUtilsSha1BlockResult(&ctx, sha1);

	sceIoWrite(out, sha1, sizeof(sha1)); // write the checksum

	sceIoClose(out);
	sceIoClose(in);

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(40, 100, "%i bytes read", 4096);
	hi_texts[1] = vlfGuiAddTextF(40, 120, "%i bytes written", sizeof(sha1));
	hi_texts[2] = vlfGuiAddTextF(40, 140, "Checksum has been saved to %s", outfile);
}
/*
	Miscellaneous
*/
void about()
{
	char *message ={"                           CREDITS\n\n"
					" - Software developed by raing3\n\n"
					" - Thanks to:\n"
					"    Rain, Torch, Adrahil, Booster, Cswindle\n"
					"    Dark_AleX, Ditlew, Fanjita, Joek2100\n"
					"    Jim, Mathieulh, Nem, Psp250, Skylark\n"
					"    TyRaNiD, z3ros0ul & BenHur."};
	vlfGuiMessageDialog(message, VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_NONE);
	ResetScreen(1, 1);
}
void LoadStartModule(char *path)
{
    SceUID mod;

    mod = sceKernelLoadModule(path, 0, NULL);
	mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}
void LoadStartModuleBuffer(char *path, void *buf, SceSize size)
{
    SceUID mod, out;

	out = sceIoOpen(path, PSP_O_WRONLY|PSP_O_CREAT, 0777);
	sceIoWrite(out, buf, size);
	sceIoClose(out);

    mod = sceKernelLoadModule(path, 0, NULL);
	mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}
void ConnectUSB(u32 device)
{
	LoadStartModule("flash0:/kd/semawm.prx");
	LoadStartModule("flash0:/kd/usbstor.prx");
    LoadStartModule("flash0:/kd/usbstormgr.prx");
    LoadStartModule("flash0:/kd/usbstorms.prx");
    LoadStartModule("flash0:/kd/usbstorboot.prx");
	LoadStartModule("flash0:/kd/usbdevice.prx");

	pspUsbDeviceSetDevice(device, 0, 0);
	sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
	sceUsbstorBootSetCapacity(0x800000);
	
	sceUsbActivate(0x1c8);

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(200, 120, "USB Mode");
}
void checkMSinfo(char* input, int type)
{
	int error = getMSinfo(input, type);
	if(error == -1){vlfGuiMessageDialog("Unable to open Input File/Device", VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);OnBackToMainMenu(0);return;}

	ResetScreen(0, 1);
	hi_texts[0] = vlfGuiAddTextF(20, 80, "Boot Status: 0x%02x", MSInfo.BootStatus);
	hi_texts[1] = vlfGuiAddTextF(20, 100, "Starting Head: 0x%02x", MSInfo.StartHead);
	hi_texts[2] = vlfGuiAddTextF(20, 120, "Start Sec/Clu: 0x%04x", MSInfo.StartSector);
	hi_texts[3] = vlfGuiAddTextF(20, 140, "Partition Type: 0x%02x", MSInfo.PartitionType);
	hi_texts[4] = vlfGuiAddTextF(20, 160, "Last Head: 0x%02x", MSInfo.LastHead);

	hi_texts[5] = vlfGuiAddTextF(220, 80, "Last Sec/Clu: 0x%04x", MSInfo.LastSector);
	hi_texts[6] = vlfGuiAddTextF(220, 100, "Abs Sector: 0x%08x", MSInfo.AbsSector);
	hi_texts[7] = vlfGuiAddTextF(220, 120, "Total Sectors: 0x%08x", MSInfo.TotalSectors);
	hi_texts[8] = vlfGuiAddTextF(220, 140, "Signature: 0x%04x", MSInfo.Signature);
	hi_texts[9] = vlfGuiAddTextF(220, 160, "Available IPL Space: %i KB", MSInfo.IPLSpace / 1024);
}

int getMSinfo(char* input, int type) // use 0 for msstor:, 1 for ipl from file, 2 for mbr from file
{
	SceUID in;
	int i;
	u8 sha1[20];
	char buff[4096];
	
	in = sceIoOpen(input, PSP_O_RDONLY, 0777); // open the input
	if(in < 1){return -1;}
	if(type != 1){sceIoRead(in, buff, 512);} // read the mbr

	MSInfo.BootStatus = buff[446]; // assign the data
	MSInfo.StartHead = buff[447];
	MSInfo.StartSector = (buff[448] & 0xFF) + ((buff[449] & 0xFF)*0x100);
	MSInfo.PartitionType = buff[450];
	MSInfo.LastHead = buff[451];
	MSInfo.LastSector = (buff[452] & 0xFF) + ((buff[453] & 0xFF)*0x100);
	MSInfo.AbsSector = (buff[454] & 0xFF) + ((buff[455] & 0xFF)*0x100) + ((buff[456] & 0xFF)*0x10000) + ((buff[457] & 0xFF)*0x1000000);
	MSInfo.TotalSectors = (buff[458] & 0xFF) + ((buff[459] & 0xFF)*0x100) + ((buff[460] & 0xFF)*0x10000) + ((buff[461] & 0xFF)*0x1000000);
	MSInfo.Signature = (buff[510] & 0xFF) + ((buff[511] & 0xFF)*0x100);
	MSInfo.IPLSpace = (MSInfo.AbsSector - 16) * 512; // ipl space (bytes) = (partition start sector - ipl start sector) * sector size, i can't detect the sector size at the momment so i am using the default of 512

	if(type == 0){sceIoLseek(in, 0x2000, 0);} // go to the ipl (if type is 0)
	if(type != 2){sceIoRead(in, buff, 4096);} // read a block

	SceKernelUtilsSha1Context ctx;
	sceKernelUtilsSha1BlockInit(&ctx);
	sceKernelUtilsSha1BlockUpdate(&ctx, (u8*) buff, 4096);
	sceKernelUtilsSha1BlockResult(&ctx, sha1);

	for (i = 0; i < IPLchecksums_count; i++){
		if(memcmp(sha1, IPLchecksums[i].checksum, 20) == 0){
			MSInfo.IPLName = IPLchecksums[i].name;
			MSInfo.IPLSize = IPLchecksums[i].size;
			break;
		}
	}

	sceIoClose(in); // close the input

	return 0;
}