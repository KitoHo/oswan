/*
$Date$
$Rev$
*/

#include <shlwapi.h>
#include <stdio.h>
#include "WSHard.h"
#include "WS.h"
#include "WSDraw.h"
#include "WSFileio.h"
#include "WSPdata.h"
#include "WSError.h"
#include "WSRender.h"
#include "nec/necintrf.h"

// ファイル操作は表などとディレクトリセパレータがかぶるのでUNICODEを使います
static wchar_t CurDir[512];
static wchar_t *SaveDir = L"RAM";
static wchar_t *StateDir = L"STATE";
static wchar_t SaveName[512];   // ".sav"
static wchar_t StateName[512];
wchar_t IniPath[512];

void WsSetDir(void)
{
    wchar_t *p, argv[512];

    MultiByteToWideChar(932, MB_PRECOMPOSED, __argv[0], -1, argv, 512);
    wcscpy(CurDir, argv);
    p = wcsrchr(CurDir, L'\\');
    if (p == NULL)
    {
        return;
    }
    *(++p) = 0;
	wcscpy(IniPath, CurDir);
	wcscat(IniPath, L"OswanJ.ini");
}

int WsSetPdata(void)
{
    ROMBanks = 1;
    RAMBanks = 0;
    RAMSize = 0;
    CartKind = 0;
    SaveName[0] = 0;
    if ((ROMMap[0xFF] = (BYTE*)malloc(0x10000)) == NULL)
    {
        ErrorMsg(ERR_MALLOC);
        return -1;
    }
    memcpy(ROMMap[0xFF] + 0xF080, pdata, size_pdata);
    WsReset();
    SetDrawMode(0);
    return 0;
}

int WsCreate(wchar_t *CartName)
{
    int Checksum, i, j;
    FILE* fp;
    BYTE buf[16];
    wchar_t* p;

    for (i = 0; i < 256; i++)
    {
        ROMMap[i] = MemDummy;
        RAMMap[i] = MemDummy;
    }
    memset(IRAM, 0, sizeof(IRAM));
    memset(MemDummy, 0xFF, sizeof(MemDummy));
    memset(IO, 0, sizeof(IO));
    if (CartName == NULL)
    {
        return WsSetPdata();
    }
    if ((fp = _wfopen(CartName, L"rb")) == NULL)
    {
        ErrorMsg(ERR_FOPEN);
        return -1;
    }
    fseek(fp, -10, 2);
    if (fread(buf, 1, 10, fp) != 10)
    {
        ErrorMsg(ERR_FREAD_ROMINFO);
        return -1;
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
        return -1;
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
        fseek(fp, (ROMBanks - i) * -0x10000, 2);
        if ((ROMMap[0x100 - ROMBanks + i] = (BYTE*)malloc(0x10000)) != NULL)
        {
            if (fread(ROMMap[0x100 - ROMBanks + i], 1, 0x10000, fp) == 0x10000)
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
            break;
        }
    }
    fclose(fp);
    if (i >= 0)
    {
        return -1;
    }
    if (Checksum & 0xFFFF)
    {
        ErrorMsg(ERR_CHECKSUM);
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
                return -1;
            }
        }
    }
    if (RAMSize)
    {
        wcscpy (SaveName, CurDir);
        p = wcsrchr(CartName, L'\\');
        if (p)
        {
            wcscat(SaveName, SaveDir);
            if (PathIsDirectoryW(SaveName) == FALSE)
            {
                CreateDirectoryW(SaveName, NULL);
            }
            wcscat(SaveName, p);
			p = wcsrchr(SaveName, L'.');
			if (p)
			{
				*p = 0;
			}
            wcscat(SaveName, L".sav");
        }
        else
        {
            SaveName[0] = 0;
        }
        if ((fp = _wfopen(SaveName, L"rb")) != NULL)
        {
            for (i = 0; i < RAMBanks; i++)
            {
                if (RAMSize < 0x10000)
                {
                    if (fread(RAMMap[i], 1, RAMSize, fp) != (size_t)RAMSize)
                    {
                        ErrorMsg(ERR_FREAD_SAVE);
						break;
                    }
                }
                else
                {
                    if (fread(RAMMap[i], 1, 0x10000, fp) != 0x10000)
                    {
                        ErrorMsg(ERR_FREAD_SAVE);
                        break;
                    }
                }
            }
            fclose(fp);
        }
    }
    else
    {
        SaveName[0] = 0;
    }
    wcscpy (StateName, CurDir);
    p = wcsrchr(CartName, L'\\');
    if (p)
    {
        wcscat(StateName, StateDir);
        if (PathIsDirectoryW(StateName) == FALSE)
        {
            CreateDirectoryW(StateName, NULL);
        }
        wcscat(StateName, p);
		p = wcsrchr(StateName, L'.');
		if (p)
		{
			*p = 0;
		}
    }
    WsReset();
	SetDrawMode(buf[6] & 1); // 0:横 1:縦
    return 0;
}

void WsRelease(void)
{
    FILE* fp;
    int i;

    if (SaveName[0] != 0)
    {
        if ((fp = _wfopen(SaveName, L"wb"))!= NULL)
        {
            for (i  =0; i < RAMBanks; i++)
            {
                if (RAMSize<0x10000)
                {
                    if (fwrite(RAMMap[i], 1, RAMSize, fp) != (size_t)RAMSize)
                    {
                        break;
                    }
                }
                else
                {
                    if (fwrite(RAMMap[i], 1, 0x10000, fp)!=0x10000)
                    {
                        break;
                    }
                }
                free(RAMMap[i]);
            }
            fclose(fp);
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
    wchar_t buf[512];

    wcscpy(buf, CurDir);
    wcscat(buf, L"eeprom.dat");
    if ((fp = _wfopen(buf, L"rb")) != NULL)
    {
        fread(IEep, sizeof(WORD), 64, fp);
        fclose(fp);
    }
	else
	{
		memset(IEep, 0xFF, 0x60);
		memset(IEep + 0x60, 0, 0x20);
	}
}

void WsSaveIEep(void)
{
    FILE* fp;
    wchar_t buf[512];

    wcscpy(buf, CurDir);
    wcscat(buf, L"eeprom.dat");
    if ((fp = _wfopen(buf, L"wb")) != NULL)
    {
        fwrite(IEep, sizeof(WORD), 64, fp);
        fclose(fp);
    }
}

#define MacroLoadNecRegisterFromFile(F,R) \
		fread(&value, sizeof(unsigned int), 1, fp); \
	    nec_set_reg(R,value); 
void WsLoadState(int num)
{
    FILE* fp;
    wchar_t buf[512];
	unsigned int value;
	int i;

	wsprintf(buf, L"%s.%03d", StateName, num);
    if ((fp = _wfopen(buf, L"rb")) == NULL)
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
    fread(IRAM, sizeof(BYTE), 0x10000, fp);
    fread(IO, sizeof(BYTE), 0x100, fp);
    for (i  =0; i < RAMBanks; i++)
    {
        if (RAMSize < 0x10000)
        {
            fread(RAMMap[i], 1, RAMSize, fp);
        }
        else
        {
            fread(RAMMap[i], 1, 0x10000, fp);
        }
    }
	fread(Palette, sizeof(WORD), 16 * 16, fp);
    fclose(fp);
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
		fwrite(&value, sizeof(unsigned int), 1, fp);
void WsSaveState(int num)
{
    FILE* fp;
    wchar_t buf[512];
	unsigned int value;
	int i;

	wsprintf(buf, L"%s.%03d", StateName, num);
    if ((fp = _wfopen(buf, L"wb")) == NULL)
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
    fwrite(IRAM, sizeof(BYTE), 0x10000, fp);
    fwrite(IO, sizeof(BYTE), 0x100, fp);
    for (i  =0; i < RAMBanks; i++)
    {
        if (RAMSize < 0x10000)
        {
            fwrite(RAMMap[i], 1, RAMSize, fp);
        }
        else
        {
            fwrite(RAMMap[i], 1, 0x10000, fp);
        }
    }
	fwrite(Palette, sizeof(WORD), 16 * 16, fp);
    fclose(fp);
}

