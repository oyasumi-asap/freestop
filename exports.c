#include <pspmoduleexport.h>
#define NULL ((void *) 0)

extern void module_start;
extern void module_info;
extern void module_stop;
static const unsigned int __syslib_exports[6] __attribute__((section(".rodata.sceResident"))) = {
	0xD632ACDB,
	0xF01D73A7,
	0xCEE8593C,
	(unsigned int) &module_start,
	(unsigned int) &module_info,
	(unsigned int) &module_stop,
};

const struct _PspLibraryEntry __library_exports[1] __attribute__((section(".lib.ent"), used)) = {
	{ NULL, 0x0000, 0x8000, 4, 2, 1, &__syslib_exports },
};
