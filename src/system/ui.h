#ifndef UI_H
#define UI_H



namespace JCLib
{

#ifndef MS_DOS
	// usage: changemode(1) for init and changemode(0) before exit
	void changemode(int dir);
	int kbhit (void);
#endif // MS_DOS
}

#endif // UI_H
