#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct App {
	char app_title[32];
	char app_creator[32];
	char app_icon[4096];
	char app_banner[4096];
	char app_sound[4096];
};

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Usage: %s input.elf output.cia\n", argv[0]);
		return 0;
	}

	char* elf = argv[1];
	char* cia = argv[2];

	FILE* fp;
	if ((fp = (FILE*)fopen(elf, "r")))
		fclose(fp);
	else {
		fprintf(stderr, "%s: %s: No such file.\n", argv[0], elf);
		return -1;
	}

	if ((fp = (FILE*)fopen("bin/bannertool", "r")))
		fclose(fp);
	else {
		fprintf(stderr, "%s: bannertool not found in bin/.\n", argv[0]);
		return -1;
	}

	if ((fp = (FILE*)fopen("bin/makerom", "r")))
		fclose(fp);
	else {
		fprintf(stderr, "%s: makerom not found in bin/.\n", argv[0]);
		return -1;
	}

	struct App hb_app;
	
	printf("App title: ");
	fgets(hb_app.app_title, 31, stdin);
	hb_app.app_title[strcspn(hb_app.app_title, "\n")] = 0;

	printf("App creator: ");
	fgets(hb_app.app_creator, 31, stdin);
	hb_app.app_creator[strcspn(hb_app.app_creator, "\n")] = 0;

	printf("App icon (48x48) path: ");
	fgets(hb_app.app_icon, 4095, stdin);
	hb_app.app_icon[strcspn(hb_app.app_icon, "\n")] = 0;

	printf("App banner (256x128) path: ");
	fgets(hb_app.app_banner, 4095, stdin);
	hb_app.app_banner[strcspn(hb_app.app_banner, "\n")] = 0;

	printf("App sound (.wav) path: ");
	fgets(hb_app.app_sound, 4095, stdin);
	hb_app.app_sound[strcspn(hb_app.app_sound, "\n")] = 0;

	if ((fp = (FILE*)fopen(hb_app.app_icon, "r")))
		fclose(fp);
	else {
		fprintf(stderr, "%s: %s: No such file.\n", argv[0], hb_app.app_icon);
		return -1;
	}

	if ((fp = (FILE*)fopen(hb_app.app_banner, "r")))
		fclose(fp);
	else {
		fprintf(stderr, "%s: %s: No such file.\n", argv[0], hb_app.app_banner);
		return -1;
	}

	if ((fp = (FILE*)fopen(hb_app.app_sound, "r")))
		fclose(fp);
	else {
		fprintf(stderr, "%s: %s: No such file.\n", argv[0], hb_app.app_sound);
		return -1;
	}

	if ((fp = (FILE*)fopen("rsf/template.rsf", "r")))
		fclose(fp);
	else {
		fprintf(stderr, "%s: rsf/template.rsf: No such file.\n", argv[0]);
		return -1;
	}

	char command[10524];

	printf("+ Creating banner ...\n");
	snprintf(command, sizeof(command), "bin/bannertool makebanner -i %s -a %s -o banner.bnr > /dev/null", hb_app.app_banner, hb_app.app_sound, hb_app.app_banner);
	system(command);

	printf("+ Creating SMDH ...\n");
	snprintf(command, sizeof(command), "bin/bannertool makesmdh -s %s -l %s -p %s -i %s -o icon.icn > /dev/null", hb_app.app_title, hb_app.app_title, hb_app.app_creator, hb_app.app_icon);
	system(command);

	printf("+ Building .CIA ...\n");
	snprintf(command, sizeof(command), "bin/makerom -f cia -o %s -DAPP_ENCRYPTED=false -rsf rsf/template.rsf -target t -exefslogo -elf %s -icon icon.icn -banner banner.bnr > /dev/null", cia, elf);
	system(command);

	printf("+ Cleaning up ...\n");
	system("rm banner.bnr icon.icn");

	return 0;
}
