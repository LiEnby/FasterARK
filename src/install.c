#include <vitasdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "promote.h"
#include "pbp.h"
#include "install.h"
#include "ui.h"


void createPspEmuDirectories() {
	
	CreateDirAndUpdateUi("ux0:pspemu");
	CreateDirAndUpdateUi("ux0:pspemu/PSP");
	CreateDirAndUpdateUi("ux0:pspemu/PSP/SAVEDATA");

	CreateDirAndUpdateUi("ux0:pspemu/temp");
	CreateDirAndUpdateUi("ux0:pspemu/temp/game");
	CreateDirAndUpdateUi("ux0:pspemu/temp/game/PSP");
	CreateDirAndUpdateUi("ux0:pspemu/temp/game/PSP/GAME");
	CreateDirAndUpdateUi("ux0:pspemu/temp/game/PSP/LICENSE");
}


void genEbootSignature(char* ebootPath) {
	char ebootSigFilePath[MAX_PATH];
	char ebootSig[0x200];	
	unsigned char pbpHash[0x20];
	
	int swVer = 0;
	
	memset(ebootSig, 0x00, sizeof(ebootSig));
	memset(pbpHash, 0x00, sizeof(pbpHash));
	
	snprintf(ebootSigFilePath, MAX_PATH, "ux0:/pspemu/temp/game/PSP/GAME/%s/__sce_ebootpbp", TITLE_ID);
	
	updateUi("Calculating EBOOT.PBP hash ...");
	HashPbp(ebootPath, pbpHash);
	
	updateUi("Generating EBOOT.PBP Signature ...");
	int res = _vshNpDrmEbootSigGenPsp(ebootPath, pbpHash, ebootSig, &swVer);
	if(res >= 0) {
		WriteFile(ebootSigFilePath, ebootSig, sizeof(ebootSig));
	}
}

void placePspGameData() {
	char gameFolder[MAX_PATH];
	char ebootFile[MAX_PATH];
	char pbootFile[MAX_PATH];
	char rifFile[MAX_PATH];
	
	// create game folder .
	snprintf(gameFolder, MAX_PATH, "ux0:/pspemu/temp/game/PSP/GAME/%s", TITLE_ID);
	CreateDirAndUpdateUi(gameFolder);
	
	// get path to EBOOT.PBP and PBOOT.PBP
	snprintf(ebootFile, MAX_PATH, "ux0:/pspemu/temp/game/PSP/GAME/%s/EBOOT.PBP", TITLE_ID);
	snprintf(pbootFile, MAX_PATH, "ux0:/pspemu/temp/game/PSP/GAME/%s/PBOOT.PBP", TITLE_ID);
	snprintf(rifFile, MAX_PATH, "ux0:/pspemu/temp/game/PSP/LICENSE/%s.rif", CONTENT_ID);
	
	CopyFileAndUpdateUi("app0:/psp/EBOOT.PBP", ebootFile);
	CopyFileAndUpdateUi("app0:/psp/PBOOT.PBP", pbootFile);
	CopyFileAndUpdateUi("app0:/rif/game.rif", rifFile);

	genEbootSignature(ebootFile);
}
void createBubble() {
	updateUi("Promoting ...");
	promoteCma("ux0:/pspemu/temp/game", TITLE_ID, SCE_PKG_TYPE_PSP);
}

void copySaveFiles() {
	CopyTree("app0:/save", "ux0:/pspemu/PSP/SAVEDATA");
}

void doInstall() {
	createPspEmuDirectories();
	placePspGameData();
	createBubble();
	copySaveFiles();
}