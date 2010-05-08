/*
$Date: 2009-09-28 06:43:18 +0900 (Êúà, 28 9 2009) $
$Rev$
*/

#include "WSDraw.h"
#include "WS.h"
#include "WSInput.h"
#include "WSRender.h"

extern HWND hWnd;
int	Kerorikan = 0;
static enum DRAWSIZE DrawSize = DS_1;           // ï`âÊÉTÉCÉY ÉtÉãÉXÉNÉäÅ[ÉìÅAx1 x2 x3
static int DrawMode = 0;                        // ècâ°ÉtÉâÉO 0:â°
static LPDIRECT3D9                 pD3D;        // IDirect3D9ÉCÉìÉ^Å[ÉtÉFÉCÉXÇ÷ÇÃÉ|ÉCÉìÉ^
static LPDIRECT3DDEVICE9           pD3DDevice;  // IDirect3DDevice9ÉCÉìÉ^Å[ÉtÉFÉCÉXÇ÷ÇÃÉ|ÉCÉìÉ^
static D3DPRESENT_PARAMETERS       D3DPP;       // ÉfÉoÉCÉXÇÃÉvÉåÉ[ÉìÉeÅ[ÉVÉáÉìÉpÉâÉÅÅ[É^
static LPDIRECT3DVERTEXBUFFER9     pMyVB;       // Vertex Buffer
static LPDIRECT3DTEXTURE9          pTexture;    // Texture
static LPDIRECT3DSURFACE9          pSurface;    // Surface
static LPDIRECT3DTEXTURE9          pSegTexture;
static LPDIRECT3DSURFACE9          pSegSurface;

// í∏ì_ÇPÇ¬ÇÃÉfÅ[É^å^
struct MY_VERTEX{
    D3DXVECTOR3 p;      // à íu
    DWORD       color;  // êF
    D3DXVECTOR2 t;      // ÉeÉNÉXÉ`ÉÉÅ[ÇÃâÊëúÇÃà íu
};
// MY_VERTEXÇÃÉtÉHÅ[É}ÉbÉgê›íË
#define MY_VERTEX_FVF  (D3DFVF_XYZ | D3DFVF_DIFFUSE  | D3DFVF_TEX1)
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//-------------------------------------------------------------
// ÉåÉìÉ_ÉâÅ[ÇÃèâä˙âª
// à¯êî
//      isFullScreen : ÉtÉãÉXÉNÉäÅ[ÉìÇÃèÍçáTRUE
// ñﬂÇËíl
//      ê¨å˜ÇµÇΩÇÁS_OK 
//-------------------------------------------------------------
HRESULT drawInitialize(BOOL isFullScreen)
{
    D3DDISPLAYMODE d3ddm;

    // Direct3D9ÉIÉuÉWÉFÉNÉgÇÃçÏê¨
    if((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        return E_FAIL;  // éÊìæé∏îs
    }
    // åªç›ÇÃÉfÉBÉXÉvÉåÉCÉÇÅ[ÉhÇéÊìæ
    if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    {
        return E_FAIL;
    }
    // ÉfÉoÉCÉXÇÃÉvÉåÉ[ÉìÉeÅ[ÉVÉáÉìÉpÉâÉÅÅ[É^Çèâä˙âª
    ZeroMemory(&D3DPP, sizeof(D3DPRESENT_PARAMETERS));
    if(isFullScreen) {                                  // ÉtÉãÉXÉNÉäÅ[ÉìÇÃèÍçá
        D3DPP.Windowed              = FALSE;            // ÉtÉãÉXÉNÉäÅ[Éìï\é¶ÇÃéwíË
        D3DPP.BackBufferWidth       = 800;              // ÉtÉãÉXÉNÉäÅ[ÉìéûÇÃâ°ïù
        D3DPP.BackBufferHeight      = 600;              // ÉtÉãÉXÉNÉäÅ[ÉìéûÇÃècïù
    }
    else {
        D3DPP.Windowed              = TRUE;             // ÉEÉCÉìÉhÉEì‡ï\é¶ÇÃéwíË
        D3DPP.BackBufferWidth       = 2016;             // 224Ç∆144ÇÃç≈è¨åˆî{êî
        D3DPP.BackBufferHeight      = 2016;
    }
    D3DPP.BackBufferCount           = 1;
    D3DPP.BackBufferFormat          = d3ddm.Format;                     // ÉJÉâÅ[ÉÇÅ[ÉhÇÃéwíË
    D3DPP.SwapEffect                = D3DSWAPEFFECT_DISCARD;            // 
    D3DPP.EnableAutoDepthStencil    = TRUE;                             // ÉGÉâÅ[ëŒçÙ
    D3DPP.AutoDepthStencilFormat    = D3DFMT_D16;                       // ÉGÉâÅ[ëŒçÙ
    D3DPP.PresentationInterval      = D3DPRESENT_INTERVAL_IMMEDIATE;    // êÇíºìØä˙ÇµÇ»Ç¢
    // ÉfÉBÉXÉvÉåÉCÉAÉ_ÉvÉ^Çï\Ç∑ÇΩÇﬂÇÃÉfÉoÉCÉXÇçÏê¨
    // ï`âÊÇ∆í∏ì_èàóùÇÉnÅ[ÉhÉEÉFÉAÇ≈çsÇ»Ç§
    if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
    {
        // è„ãLÇÃê›íËÇ™é∏îsÇµÇΩÇÁ
        // ï`âÊÇÉnÅ[ÉhÉEÉFÉAÇ≈çsÇ¢ÅAí∏ì_èàóùÇÕCPUÇ≈çsÇ»Ç§
        if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
        {
            // èâä˙âªé∏îs
			MessageBox(hWnd, TEXT("ÉOÉâÉtÉBÉbÉNÉ`ÉbÉvÇ™ñ¢ëŒâûÇ≈Ç∑"), TEXT("Direct3D Error"), MB_OK);
            return E_FAIL;
        }
    }
    return S_OK;
}

//-------------------------------------------------------------
// èIóπèàóù
//-------------------------------------------------------------
void drawFinalize(void)
{
    // ÉfÉoÉCÉXÉIÉuÉWÉFÉNÉgÇÃâï˙
    SAFE_RELEASE(pD3DDevice);
    // DirectXGraphicsÇÃâï˙
    SAFE_RELEASE(pD3D);
}

//-------------------------------------------------------------
// ÉVÅ[ÉìÇê∂ê¨
// ñﬂÇËíl
//      ê¨å˜ÇµÇΩÇÁS_OK
//-------------------------------------------------------------
HRESULT drawCreate(void)
{
    if(pD3DDevice == 0)
    {
        return E_FAIL;
    }
    drawDestroy();
    //--------------------------------------
    // ÉIÉuÉWÉFÉNÉgÇÃí∏ì_ÉoÉbÉtÉ@Çê∂ê¨
    //--------------------------------------
    // 8Ç¬ÇÃí∏ì_Ç©ÇÁÇ»ÇÈí∏ì_ÉoÉbÉtÉ@ÇçÏÇÈ
    if(FAILED( pD3DDevice->CreateVertexBuffer(8 * sizeof(MY_VERTEX), D3DUSAGE_WRITEONLY, MY_VERTEX_FVF, D3DPOOL_MANAGED, &pMyVB, NULL)))
    {
        return E_FAIL;
    }
    pD3DDevice->CreateTexture(256, 256, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pTexture, NULL);
    pTexture->GetSurfaceLevel(0, &pSurface);
    pD3DDevice->CreateTexture(32, 1024, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pSegTexture, NULL);
    pSegTexture->GetSurfaceLevel(0, &pSegSurface);
    //--------------------------------------
    // ÉeÉNÉXÉ`ÉÉÉXÉeÅ[ÉWÇÃê›íË
    //--------------------------------------
    pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    //--------------------------------------
    // ÉåÉìÉ_ÉäÉìÉOÉXÉeÅ[ÉgÉpÉâÉÅÅ[É^ÇÃê›íË
    //--------------------------------------
    // óºñ ï`âÊÉÇÅ[ÉhÇÃéwíË
    pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    // ÉfÉBÉUÉäÉìÉOÇçsÇ»Ç§ÅiçÇïiéøï`âÊÅj
    pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    // ÉmÅ[ÉâÉCÉeÉBÉìÉOÉÇÅ[Éh   
    pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    return S_OK;
}

//-------------------------------------------------------------
// ÉVÅ[ÉìÇÃîjä¸
//-------------------------------------------------------------
void drawDestroy(void)
{
    SAFE_RELEASE(pSurface);
    SAFE_RELEASE(pTexture);
    SAFE_RELEASE(pSegSurface);
    SAFE_RELEASE(pSegTexture);
    SAFE_RELEASE(pMyVB);
}

//-------------------------------------------------------------
// ï`âÊèàóù
//-------------------------------------------------------------
#define MAIN_X (224.0f/256.0f)
#define MAIN_Y (144.0f/256.0f)
#define MAIN_W (2 * 224.0f/234.0f - 1.0f)
#define SEG_X (4 * 8.0f/32.0f)
#define SEG_Y (4 * 144.0f/1024.0f)
#define SEG_W (1.0f - 2 * 8.0f/234.0f)
#define KERO 0.0075f
void drawDraw()
{
    if(pD3DDevice == 0)
    {
        return;
    }
    if(pMyVB == 0)
    {
        return;
    }
    // âÊëúÉfÅ[É^ÇÉeÉNÉXÉ`ÉÉÇ…ì]ëó
    D3DLOCKED_RECT lockRect;
    int x, y, pitch;
    WORD *p;
    pSurface->LockRect(&lockRect, NULL, D3DLOCK_DISCARD);
    p = FrameBuffer[0] + 8;
    pitch = lockRect.Pitch / 2;
    for (y = 0; y < 144; y++)
    {
        for (x = 0; x < 224; x++)
        {
            *((WORD*)lockRect.pBits + pitch * y + x) = *p++;
        }
        p += 32;
    }
    pSurface->UnlockRect();
    // âtèªÉZÉOÉÅÉìÉgÉfÅ[É^ÇÉeÉNÉXÉ`ÉÉÇ…ì]ëó
    RenderSegment();
    pSegSurface->LockRect(&lockRect, NULL, D3DLOCK_DISCARD);
    p = SegmentBuffer;
    pitch = lockRect.Pitch / 2;
    for (y = 0; y < 144 * 4; y++)
    {
        for (x = 0; x < 32; x++)
        {
            *((WORD*)lockRect.pBits + pitch * y + x) = *p++;
        }
    }
    pSegSurface->UnlockRect();
    //í∏ì_ÉoÉbÉtÉ@ÇÃíÜêgÇñÑÇﬂÇÈ
    MY_VERTEX* v;
    pMyVB->Lock( 0, 0, (void**)&v, 0 );
	if (Kerorikan)
	{
		// í∏ì_ç¿ïWÇÃê›íË
		v[0].p = D3DXVECTOR3(-112*KERO,  72*KERO, 0.0f);
		v[1].p = D3DXVECTOR3( 112*KERO,  72*KERO, 0.0f);
		v[2].p = D3DXVECTOR3(-112*KERO, -72*KERO, 0.0f);
		v[3].p = D3DXVECTOR3( 112*KERO, -72*KERO, 0.0f);
		v[4].p = D3DXVECTOR3( 114*KERO,  72*KERO, 0.0f);
		v[5].p = D3DXVECTOR3( 122*KERO,  72*KERO, 0.0f);
		v[6].p = D3DXVECTOR3( 114*KERO, -72*KERO, 0.0f);
		v[7].p = D3DXVECTOR3( 122*KERO, -72*KERO, 0.0f);
	}
	else
	{
		// í∏ì_ç¿ïWÇÃê›íË
		v[0].p = D3DXVECTOR3(-1.0f,  1.0f, 0.0f);
		v[1].p = D3DXVECTOR3(MAIN_W, 1.0f, 0.0f);
		v[2].p = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		v[3].p = D3DXVECTOR3(MAIN_W,-1.0f, 0.0f);
		v[4].p = D3DXVECTOR3(SEG_W,  1.0f, 0.0f);
		v[5].p = D3DXVECTOR3( 1.0f,  1.0f, 0.0f);
		v[6].p = D3DXVECTOR3(SEG_W, -1.0f, 0.0f);
		v[7].p = D3DXVECTOR3( 1.0f, -1.0f, 0.0f);
	}
	// ÉeÉNÉXÉ`ÉÉç¿ïWÇÃê›íË
	v[0].t = D3DXVECTOR2(0.0f, 0.0f);
	v[1].t = D3DXVECTOR2(MAIN_X, 0.0f);
	v[2].t = D3DXVECTOR2(0.0f, MAIN_Y);
	v[3].t = D3DXVECTOR2(MAIN_X, MAIN_Y);
	v[4].t = D3DXVECTOR2(0.0f, 0.0f);
	v[5].t = D3DXVECTOR2(SEG_X, 0.0f);
	v[6].t = D3DXVECTOR2(0.0f, SEG_Y);
	v[7].t = D3DXVECTOR2(SEG_X, SEG_Y);
    // í∏ì_ÉJÉâÅ[ÇÃê›íË
    v[0].color = v[1].color = v[2].color = v[3].color = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
    v[4].color = v[5].color = v[6].color = v[7].color = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
    pMyVB->Unlock();
    // âÒì]èàóù
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    // éŒÇﬂÅiç∂atan(0.5)âÒì]Åj
    if (Kerorikan)
    {
        D3DXMatrixRotationZ(&mat, 0.4636476f); // atanf(0.5f)ÇæÇ∆ç≈ìKâªÇ≥ÇÍÇ»Ç¢Ç©Ç‡ÇµÇÍÇ»Ç¢ÇÃÇ≈
    }
    // ècÅiç∂90ìxâÒì]Åj
	else if (DrawMode & 0x01)
	{
        D3DXMatrixRotationZ(&mat, D3DXToRadian(90));
	}
    // ï`âÊäJénêÈåæ
    if(SUCCEEDED(pD3DDevice->BeginScene()))
    {
        pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF666666, 0.0f, 0);
        pD3DDevice->SetStreamSource(0, pMyVB, 0, sizeof(MY_VERTEX));
        pD3DDevice->SetFVF(MY_VERTEX_FVF);
        pD3DDevice->SetTexture( 0, pTexture);
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // 0î‘ÇÃí∏ì_Ç©ÇÁéOäpå`Ç2å¬
        pD3DDevice->SetTexture( 0, pSegTexture);
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2); // 4î‘ÇÃí∏ì_Ç©ÇÁéOäpå`Ç2å¬
        pD3DDevice->SetTransform(D3DTS_VIEW, &mat);
        // ï`âÊèIóπêÈåæ
        pD3DDevice->EndScene();
    }
    // ï`âÊåãâ ÇÃì]ëó
    if(FAILED(pD3DDevice->Present(NULL, NULL, NULL, NULL)))
    {
        // ÉfÉoÉCÉXè¡é∏Ç©ÇÁïúãA
        pD3DDevice->Reset(&D3DPP);
    }
}

//-------------------------------------------------------------
void WsResize(void)
{
    RECT wind;
    RECT wind2;
    int  lcdHeight = 144;
    int  lcdWidth  = 224 + 10; // +10ÇÕÉZÉOÉÅÉìÉgï™
    int  client_width;
    int  client_height;

    if (Kerorikan)
    {
        client_width  = 256 * DrawSize;
        client_height = 256 * DrawSize;
    }
    else if (DrawMode & 0x01)
    {
        client_width  = lcdHeight * DrawSize;
        client_height = lcdWidth  * DrawSize;
    }
    else
    {
        client_width  = lcdWidth  * DrawSize;
        client_height = lcdHeight * DrawSize;
    }
    wind.top    = 0;
    wind.left   = 0;
    wind.right  = client_width;
    wind.bottom = client_height;
    AdjustWindowRectEx(&wind, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        TRUE, WS_EX_APPWINDOW | WS_EX_ACCEPTFILES);
    wind2 = wind;
    SendMessage(hWnd, WM_NCCALCSIZE, FALSE, (LPARAM) &wind2);
    SetWindowPos(hWnd, NULL, 0, 0,
        (wind.right  - wind.left) + client_width  - (wind2.right  - wind2.left),
        (wind.bottom - wind.top ) + client_height - (wind2.bottom - wind2.top ),
        SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

//-------------------------------------------------------------
int SetDrawMode(int Mode)
{
    if(DrawMode != Mode)
    {
        DrawMode = Mode;
        SetKeyMap(Mode);
        WsResize();
    }
    return 0;
}

//-------------------------------------------------------------
int SetDrawSize(enum DRAWSIZE Size)
{
    DrawSize = Size;
    WsResize();
    return 0;
}

