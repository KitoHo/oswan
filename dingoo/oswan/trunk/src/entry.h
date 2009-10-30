#ifndef __entry_h__
#define __entry_h__

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define NULL (void*)0

typedef unsigned short wchar_t;
typedef long int FILE;
typedef unsigned int size_t;
typedef struct {
	unsigned long pressed;
	unsigned long released;
	unsigned long status;
} KEY_STATUS;
typedef struct
{
	unsigned int   frequency;
	unsigned short bits;
	unsigned char  channels;
	unsigned char  volume;
} WAVEOUT_INFO;

extern void     _kbd_get_status(KEY_STATUS* ks);
extern void     _lcd_set_frame();
extern unsigned short* _lcd_get_frame();
extern void     lcd_flip();
extern void     __dcache_writeback_all();
extern char*    strcpy(char*, const char*);
extern char*    strcat(char*, const char*);
extern char*    strrchr(char*, int);
extern void*    memcpy(void*, const void*, size_t);
extern void*    memset(void*, int, size_t);
extern void*    malloc(size_t);
extern void     free(void*);
extern int      fprintf(FILE*, const char*, ...);
extern int      printf(const char*, ...);
extern int      sprintf(char*, const char*, ...);
extern FILE*    fsys_fopen(const char*, const char*);
extern int      fsys_fread(void*, size_t, size_t, FILE*);
extern int      fsys_fclose(FILE*);
extern int      fsys_fseek(FILE*, long int, int);
extern long int fsys_ftell(FILE*);
extern int      fsys_ferror(FILE*);
extern int      fsys_feof(FILE*);
extern int      fsys_fwrite(const void*, size_t, size_t, FILE*);
extern int      OSTaskCreate(void (*)(void *), void *, void *, int);
extern int      OSTaskDel(int prio);
extern void*    waveout_open(WAVEOUT_INFO *);
extern int      waveout_close(void *);
extern int      waveout_write(void *, void *, int);
extern int      waveout_can_write(void);
extern int      waveout_set_volume(unsigned char);

#endif
