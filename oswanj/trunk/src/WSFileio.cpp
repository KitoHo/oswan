/*
$Date: 2009-09-10 11:00:27 +0900 (譛ｨ, 10 9 2009) $
$Rev: 5 $
*/

#include <tchar.h>
#include <shlwapi.h>
#include <stdio.h>
#include "WSHard.h"
#include "WS.h"
#include "WSDraw.h"
#include "WSFileio.h"
#include "WSPdata.h"
#include "WSError.h"

// ファイル操作は表などとディレクトリセパレータがかぶるのでUNICODEを使います
static wchar_t CurDir[512];
static wchar_t *SaveDir = L"RAM";
static wchar_t SaveName[512];   // ".sav"

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
    FILE *F;
    BYTE buf[16];

    for(i = 0; i < 256; i++)
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
    if((F = _wfopen(CartName, L"rb")) == NULL)
    {
        ErrorMsg(ERR_FOPEN);
        return -1;
    }
    fseek(F, -10, 2);
    if(fread(buf, 1, 10, F) != 10)
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
    if(ROMBanks == 0)
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
    for(i = ROMBanks - 1; i >= 0; i--)
    {
        fseek(F, (ROMBanks - i) * -0x10000, 2);
        if((ROMMap[0x100 - ROMBanks + i] = (BYTE*)malloc(0x10000)) != NULL)
        {
            if(fread(ROMMap[0x100 - ROMBanks + i], 1, 0x10000, F) == 0x10000)
            {
                for(j = 0; j < 0x10000; j++)
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
    fclose(F);
    if(i >= 0)
    {
        return -1;
    }
    if(Checksum & 0xFFFF)
    {
        ErrorMsg(ERR_CHECKSUM);
    }
    if(RAMBanks)
    {
        for(i = 0; i < RAMBanks; i++)
        {
            if((RAMMap[i] = (BYTE*)malloc(0x10000)) != NULL)
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
    if(RAMSize)
    {
        wchar_t* p;
        wcscpy (SaveName, CurDir);
        p = wcsrchr(CartName, L'\\');
        if (p)
        {
            wcscat(SaveName, SaveDir);
            if (PathIsDirectoryW(SaveName) == FALSE)
            {
                CreateDirectoryW(SaveName, NULL);
            }
            wcscat(SaveName, L"\\");
            wcscat (SaveName, p);
            wcscat(SaveName, L".sav");
        }
        else
        {
            SaveName[0] = 0;
        }
        if((F = _wfopen(SaveName, L"rb")) != NULL)
        {
            for(i = 0; i < RAMBanks; i++)
            {
                if(RAMSize < 0x10000)
                {
                    if(fread(RAMMap[i], 1, RAMSize, F) != (size_t)RAMSize)
                    {
                        fclose(F);
                        ErrorMsg(ERR_FREAD_SAVE);
                    }
                }
                else
                {
                    if(fread(RAMMap[i],1,0x10000,F) != 0x10000)
                    {
                        fclose(F);
                        ErrorMsg(ERR_FREAD_SAVE);
                        return -1;
                    }
                }
            }
            fclose(F);
        }
    }
    else
    {
        SaveName[0] = 0;
    }
    WsReset();
    SetDrawMode(buf[6] & 1); // 0:横 1:縦
    return 0;
}

void WsRelease(void)
{
    FILE *F;
    int i;

    if(SaveName[0] != 0)
    {
        if((F = _wfopen(SaveName, L"wb"))!= NULL)
        {
            for(i=0;i<RAMBanks;i++)
            {
                if(RAMSize<0x10000)
                {
                    if(fwrite(RAMMap[i],1,RAMSize,F)!=(size_t)RAMSize)
                    {
                        break;
                    }
                }
                else
                {
                    if(fwrite(RAMMap[i],1,0x10000,F)!=0x10000)
                    {
                        break;
                    }
                }
                free(RAMMap[i]);
            }
            fclose(F);
        }
    }
    for(i = 0xFF; i >= 0; i--)
    {
        if(ROMMap[i] == MemDummy)
        {
            break;
        }
        free(ROMMap[i]);
        ROMMap[i] = MemDummy;
    }
}

void WsLoadIEep(void)
{
    FILE* F;
    wchar_t buf[512];

    wcscpy(buf, CurDir);
    wcscat(buf, L"eeprom.dat");
    if ((F = _wfopen(buf, L"rb")) != NULL)
    {
        fread(IEep, sizeof(WORD), 64, F);
        fclose(F);
    }

}

void WsSaveIEep(void)
{
    FILE* F;
    wchar_t buf[512];

    wcscpy(buf, CurDir);
    wcscat(buf, L"eeprom.dat");
    if ((F = _wfopen(buf, L"wb")) != NULL)
    {
        fwrite(IEep, sizeof(WORD), 64, F);
        fclose(F);
    }

}
