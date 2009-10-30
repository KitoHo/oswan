/*
$Date$
$Rev$
*/

#include "entry.h"
#include "WSHard.h"
#include "WS.h"
#include "WSFileio.h"
#include "WSDraw.h"
#include "WSError.h"
#include "WSRender.h"
#include "necintrf.h"

static char *SaveDir = "RAM";
static char *StateDir = "STATE";
static char SaveName[512];   // ".sav"
static char StateName[512];
static char* IEepPath = "A:\\GAME\\Oswan.dat";

void WsSetDir(char* path)
{
	char temp[512];
	char file[512];
	char* p;

	SaveName[0] = 0;
	StateName[0] = 0;
	if (path == NULL)
	{
		return;
	}
	strcpy(temp, path);
	p = strrchr(temp, '\\');
	if (p == NULL)
	{
		return;
	}
	strcpy(file, p++);
	*p = 0;
	p = strrchr(file, '.');
	if (p == NULL)
	{
		return;
	}
	*p = 0;
	strcpy(SaveName, temp);
	strcat(SaveName, SaveDir);
	strcat(SaveName, file);
	strcat(SaveName, ".sav");
	strcpy(StateName, temp);
	strcat(StateName, StateDir);
	strcat(StateName, file);
}

int WsSetPdata(void)
{
    ROMBanks = 1;
    RAMBanks = 0;
    RAMSize = 0;
    CartKind = 0;
    if ((ROMMap[0xFF] = (BYTE*)malloc(0x10000)) == NULL)
    {
        ErrorMsg(ERR_MALLOC);
        return -1;
    }
    WsCpyPdata(ROMMap[0xFF] + 0xF080);
    WsReset();
    SetHVMode(0);
    return 0;
}

int WsCreate(char *CartName)
{
    int Checksum, i, j;
    FILE* fp;
    BYTE buf[16];

    for (i = 0; i < 256; i++)
    {
        ROMMap[i] = MemDummy;
        RAMMap[i] = MemDummy;
    }
    memset(IRAM, 0, sizeof(IRAM));
    memset(MemDummy, 0xA0, sizeof(MemDummy));
    memset(IO, 0, sizeof(IO));
    if (CartName == NULL)
    {
        return WsSetPdata();
    }
    if ((fp = fsys_fopen(CartName, "rb")) == NULL)
    {
        ErrorMsg(ERR_FOPEN);
        return ERR_FOPEN;
    }
    fsys_fseek(fp, -10, SEEK_END);
    if (fsys_fread(buf, 1, 10, fp) != 10)
    {
        ErrorMsg(ERR_FREAD_ROMINFO);
        return ERR_FREAD_ROMINFO;
    }
    switch (buf[4])
    {
    case 1:
        ROMBanks = 4;
        break;
    case 2:
        ROMBanks = 8;
        break;
    case 3:
        ROMBanks = 16;
        break;
    case 4:
        ROMBanks = 32;
        break;
    case 5:
        ROMBanks = 48;
        break;
    case 6:
        ROMBanks = 64;
        break;
    case 7:
        ROMBanks = 96;
        break;
    case 8:
        ROMBanks = 128;
        break;
    case 9:
        ROMBanks = 256;
        break;
    default:
        ROMBanks = 0;
        break;
    }
    if (ROMBanks == 0)
    {
        ErrorMsg(ERR_ILLEGAL_ROMSIZE);
        return ERR_ILLEGAL_ROMSIZE;
    }
    switch (buf[5])
    {
    case 0x01:
        RAMBanks = 1;
        RAMSize = 0x2000;
        CartKind = 0;
        break;
    case 0x02:
        RAMBanks = 1;
        RAMSize = 0x8000;
        CartKind = 0;
        break;
    case 0x03:
        RAMBanks = 2;
        RAMSize = 0x20000;
        CartKind = 0;
        break;
    case 0x04:
        RAMBanks = 4;
        RAMSize = 0x40000;
        CartKind = 0;
        break;
    case 0x10:
        RAMBanks = 1;
        RAMSize = 0x80;
        CartKind = CK_EEP;
        break;
    case 0x20:
        RAMBanks = 1;
        RAMSize = 0x800;
        CartKind = CK_EEP;
        break;
    case 0x50:
        RAMBanks = 1;
        RAMSize = 0x400;
        CartKind = CK_EEP;
        break;
    default:
        RAMBanks = 0;
        RAMSize = 0;
        CartKind = 0;
        break;
    }
    WsRomPatch(buf);
    Checksum = (int)((buf[9] << 8) + buf[8]);
    Checksum += (int)(buf[9] + buf[8]);
    for (i = ROMBanks - 1; i >= 0; i--)
    {
        fsys_fseek(fp, (ROMBanks - i) * -0x10000, 2);
        if ((ROMMap[0x100 - ROMBanks + i] = (BYTE*)malloc(0x10000)) != NULL)
        {
            if (fsys_fread(ROMMap[0x100 - ROMBanks + i], 1, 0x10000, fp) == 0x10000)
            {
                for (j = 0; j < 0x10000; j++)
                {
                    Checksum -= ROMMap[0x100 - ROMBanks + i][j];
                }
            }
            else
            {
                ErrorMsg(ERR_FREAD_ROM);
                break;
            }
        }
        else
        {
            ErrorMsg(ERR_MALLOC);
            return ERR_MALLOC;
        }
    }
    fsys_fclose(fp);
    if (i >= 0)
    {
        return -1;
    }
    if (Checksum & 0xFFFF)
    {
        //ErrorMsg(ERR_CHECKSUM);
    }
    if (RAMBanks)
    {
        for (i = 0; i < RAMBanks; i++)
        {
            if ((RAMMap[i] = (BYTE*)malloc(0x10000)) != NULL)
            {
                memset(RAMMap[i], 0x00, 0x10000);
            }
            else
            {
                ErrorMsg(ERR_MALLOC);
				return ERR_MALLOC;
            }
        }
    }
    if (RAMSize)
    {
        if ((fp = fsys_fopen(SaveName, "rb")) != NULL)
        {
            for (i = 0; i < RAMBanks; i++)
            {
                if (RAMSize < 0x10000)
                {
                    if (fsys_fread(RAMMap[i], 1, RAMSize, fp) != RAMSize)
                    {
                        ErrorMsg(ERR_FREAD_SAVE);
						break;
                    }
                }
                else
                {
                    if (fsys_fread(RAMMap[i], 1, 0x10000, fp) != 0x10000)
                    {
                        ErrorMsg(ERR_FREAD_SAVE);
                        break;
                    }
                }
            }
            fsys_fclose(fp);
        }
    }
    else
    {
        SaveName[0] = 0;
    }
    WsReset();
	SetHVMode(buf[6] & 1); // 0:‰¡ 1:c
    return 0;
}

void WsRelease(void)
{
    FILE* fp;
    int i;

    if (SaveName[0] != 0)
    {
        if ((fp = fsys_fopen(SaveName, "wb"))!= NULL)
        {
            for (i  =0; i < RAMBanks; i++)
            {
                if (RAMSize<0x10000)
                {
                    if (fsys_fwrite(RAMMap[i], 1, RAMSize, fp) != RAMSize)
                    {
                        break;
                    }
                }
                else
                {
                    if (fsys_fwrite(RAMMap[i], 1, 0x10000, fp)!=0x10000)
                    {
                        break;
                    }
                }
                free(RAMMap[i]);
            }
            fsys_fclose(fp);
        }
    }
    for (i = 0xFF; i >= 0; i--)
    {
        if (ROMMap[i] == MemDummy)
        {
            break;
        }
        free(ROMMap[i]);
        ROMMap[i] = MemDummy;
    }
}

void WsLoadIEep(void)
{
    FILE* fp;

    if ((fp = fsys_fopen(IEepPath, "rb")) != NULL)
    {
        fsys_fread(IEep, sizeof(WORD), 64, fp);
        fsys_fclose(fp);
    }
	else
	{
		WORD* p = IEep + 0x30;
		memset(IEep, 0xFF, 0x60);
		memset(p, 0, 0x20);
		*p++ = 0x211D;
		*p++ = 0x180B;
		*p++ = 0x1C0D;
		*p++ = 0x1D23;
		*p++ = 0x0B1E;
		*p   = 0x0016;
	}
}

void WsSaveIEep(void)
{
    FILE* fp;

    if ((fp = fsys_fopen(IEepPath, "wb")) != NULL)
    {
        fsys_fwrite(IEep, sizeof(WORD), 64, fp);
        fsys_fclose(fp);
    }
}

#define MacroLoadNecRegisterFromFile(F,R) \
		fsys_fread(&value, sizeof(unsigned int), 1, fp); \
	    nec_set_reg(R,value); 
void WsLoadState(int num)
{
    FILE* fp;
    char buf[512];
	unsigned int value;
	int i;

	sprintf(buf, "%s.%03d", StateName, num);
    if ((fp = fsys_fopen(buf, "rb")) == NULL)
    {
		return;
	}
	MacroLoadNecRegisterFromFile(fp,NEC_IP);
	MacroLoadNecRegisterFromFile(fp,NEC_AW);
	MacroLoadNecRegisterFromFile(fp,NEC_BW);
	MacroLoadNecRegisterFromFile(fp,NEC_CW);
	MacroLoadNecRegisterFromFile(fp,NEC_DW);
	MacroLoadNecRegisterFromFile(fp,NEC_CS);
	MacroLoadNecRegisterFromFile(fp,NEC_DS);
	MacroLoadNecRegisterFromFile(fp,NEC_ES);
	MacroLoadNecRegisterFromFile(fp,NEC_SS);
	MacroLoadNecRegisterFromFile(fp,NEC_IX);
	MacroLoadNecRegisterFromFile(fp,NEC_IY);
	MacroLoadNecRegisterFromFile(fp,NEC_BP);
	MacroLoadNecRegisterFromFile(fp,NEC_SP);
	MacroLoadNecRegisterFromFile(fp,NEC_FLAGS);
	MacroLoadNecRegisterFromFile(fp,NEC_VECTOR);
	MacroLoadNecRegisterFromFile(fp,NEC_PENDING);
	MacroLoadNecRegisterFromFile(fp,NEC_NMI_STATE);
	MacroLoadNecRegisterFromFile(fp,NEC_IRQ_STATE);
    fsys_fread(IRAM, sizeof(BYTE), 0x10000, fp);
    fsys_fread(IO, sizeof(BYTE), 0x100, fp);
    for (i  =0; i < RAMBanks; i++)
    {
        if (RAMSize < 0x10000)
        {
            fsys_fread(RAMMap[i], 1, RAMSize, fp);
        }
        else
        {
            fsys_fread(RAMMap[i], 1, 0x10000, fp);
        }
    }
	fsys_fread(Palette, sizeof(WORD), 16 * 16, fp);
    fsys_fclose(fp);
	WriteIO(0xC1, IO[0xC1]);
	WriteIO(0xC2, IO[0xC2]);
	WriteIO(0xC3, IO[0xC3]);
	WriteIO(0xC0, IO[0xC0]);
	for (i = 0x80; i <= 0x90; i++)
	{
		WriteIO(i, IO[i]);
	}
}

#define MacroStoreNecRegisterToFile(F,R) \
	    value = nec_get_reg(R); \
		fsys_fwrite(&value, sizeof(unsigned int), 1, fp);
void WsSaveState(int num)
{
    FILE* fp;
    char buf[512];
	unsigned int value;
	int i;

	if (StateName[0] == 0)
	{
		return;
	}
	sprintf(buf, "%s.%03d", StateName, num);
    if ((fp = fsys_fopen(buf, "wb")) == NULL)
    {
		return;
	}
	MacroStoreNecRegisterToFile(fp,NEC_IP);
	MacroStoreNecRegisterToFile(fp,NEC_AW);
	MacroStoreNecRegisterToFile(fp,NEC_BW);
	MacroStoreNecRegisterToFile(fp,NEC_CW);
	MacroStoreNecRegisterToFile(fp,NEC_DW);
	MacroStoreNecRegisterToFile(fp,NEC_CS);
	MacroStoreNecRegisterToFile(fp,NEC_DS);
	MacroStoreNecRegisterToFile(fp,NEC_ES);
	MacroStoreNecRegisterToFile(fp,NEC_SS);
	MacroStoreNecRegisterToFile(fp,NEC_IX);
	MacroStoreNecRegisterToFile(fp,NEC_IY);
	MacroStoreNecRegisterToFile(fp,NEC_BP);
	MacroStoreNecRegisterToFile(fp,NEC_SP);
	MacroStoreNecRegisterToFile(fp,NEC_FLAGS);
	MacroStoreNecRegisterToFile(fp,NEC_VECTOR);
	MacroStoreNecRegisterToFile(fp,NEC_PENDING);
	MacroStoreNecRegisterToFile(fp,NEC_NMI_STATE);
	MacroStoreNecRegisterToFile(fp,NEC_IRQ_STATE);
    fsys_fwrite(IRAM, sizeof(BYTE), 0x10000, fp);
    fsys_fwrite(IO, sizeof(BYTE), 0x100, fp);
    for (i  =0; i < RAMBanks; i++)
    {
        if (RAMSize < 0x10000)
        {
            fsys_fwrite(RAMMap[i], 1, RAMSize, fp);
        }
        else
        {
            fsys_fwrite(RAMMap[i], 1, 0x10000, fp);
        }
    }
	fsys_fwrite(Palette, sizeof(WORD), 16 * 16, fp);
    fsys_fclose(fp);
}
