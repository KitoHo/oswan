/*
$Date: 2009-09-10 11:00:27 +0900 (木, 10 9 2009) $
$Rev: 5 $
*/

#include "WSDraw.h"
#include "WS.h"
#include "WSInput.h"
#include "WSRender.h"

extern HWND hWnd;
static enum DRAWSIZE DrawSize = DS_1;           // �`��T�C�Y �t���X�N���[���Ax1 x2 x3
static int DrawMode = 0;                        // �c���t���O 0:��
static LPDIRECT3D9                 pD3D;        // IDirect3D9�C���^�[�t�F�C�X�ւ̃|�C���^
static LPDIRECT3DDEVICE9           pD3DDevice;  // IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
static D3DPRESENT_PARAMETERS       D3DPP;       // �f�o�C�X�̃v���[���e�[�V�����p�����[�^
static LPDIRECT3DVERTEXBUFFER9     pMyVB;       // Vertex Buffer
static LPDIRECT3DTEXTURE9          pTexture;    // Texture
static LPDIRECT3DSURFACE9          pSurface;    // Surface
static LPDIRECT3DTEXTURE9          pSegTexture;
static LPDIRECT3DSURFACE9          pSegSurface;

// ���_�P�̃f�[�^�^
struct MY_VERTEX{
    D3DXVECTOR3 p;      // �ʒu
    DWORD       color;  // �F
    D3DXVECTOR2 t;      // �e�N�X�`���[�̉摜�̈ʒu
};
// MY_VERTEX�̃t�H�[�}�b�g�ݒ�
#define MY_VERTEX_FVF  (D3DFVF_XYZ | D3DFVF_DIFFUSE  | D3DFVF_TEX1)
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//-------------------------------------------------------------
// �����_���[�̏�����
// ����
//      isFullScreen : �t���X�N���[���̏ꍇTRUE
// �߂�l
//      ����������S_OK 
//-------------------------------------------------------------
HRESULT drawInitialize(BOOL isFullScreen)
{
    D3DDISPLAYMODE d3ddm;

    // Direct3D9�I�u�W�F�N�g�̍쐬
    if((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        return E_FAIL;  // �擾���s
    }
    // ���݂̃f�B�X�v���C���[�h���擾
    if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    {
        return E_FAIL;
    }
    // �f�o�C�X�̃v���[���e�[�V�����p�����[�^��������
    ZeroMemory(&D3DPP, sizeof(D3DPRESENT_PARAMETERS));
    if(isFullScreen) {                                  // �t���X�N���[���̏ꍇ
        D3DPP.Windowed              = FALSE;            // �t���X�N���[���\���̎w��
        D3DPP.BackBufferWidth       = 800;              // �t���X�N���[�����̉���
        D3DPP.BackBufferHeight      = 600;              // �t���X�N���[�����̏c��
    }
    else {
        D3DPP.Windowed              = TRUE;             // �E�C���h�E���\���̎w��
        D3DPP.BackBufferWidth       = 2016;             // 224��144�̍ŏ����{��
        D3DPP.BackBufferHeight      = 2016;
    }
    D3DPP.BackBufferCount           = 1;
    D3DPP.BackBufferFormat          = d3ddm.Format;                     // �J���[���[�h�̎w��
    D3DPP.SwapEffect                = D3DSWAPEFFECT_DISCARD;            // 
    D3DPP.EnableAutoDepthStencil    = TRUE;                             // �G���[�΍�
    D3DPP.AutoDepthStencilFormat    = D3DFMT_D16;                       // �G���[�΍�
    D3DPP.PresentationInterval      = D3DPRESENT_INTERVAL_IMMEDIATE;    // �����������Ȃ�
    // �f�B�X�v���C�A�_�v�^��\�����߂̃f�o�C�X���쐬
    // �`��ƒ��_�������n�[�h�E�F�A�ōs�Ȃ�
    if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
    {
        // ��L�̐ݒ肪���s������
        // �`����n�[�h�E�F�A�ōs���A���_������CPU�ōs�Ȃ�
        if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
        {
            // ��L�̐ݒ肪���s������
            // �`��ƒ��_������CPU�ōs�Ȃ�
            if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF,hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
            {
                // ���������s
                return E_FAIL;
            }
        }
    }
    return S_OK;
}

//-------------------------------------------------------------
// �I������
//-------------------------------------------------------------
void drawFinalize(void)
{
    // �f�o�C�X�I�u�W�F�N�g�̉��
    SAFE_RELEASE(pD3DDevice);
    // DirectXGraphics�̉��
    SAFE_RELEASE(pD3D);
}

//-------------------------------------------------------------
// �V�[���𐶐�
// �߂�l
//      ����������S_OK
//-------------------------------------------------------------
HRESULT drawCreate(void)
{
    if(pD3DDevice == 0)
    {
        return E_FAIL;
    }
    drawDestroy();
    //--------------------------------------
    // �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
    //--------------------------------------
    // 8�̒��_����Ȃ钸�_�o�b�t�@�����
    if(FAILED( pD3DDevice->CreateVertexBuffer(8 * sizeof(MY_VERTEX), D3DUSAGE_WRITEONLY, MY_VERTEX_FVF, D3DPOOL_MANAGED, &pMyVB, NULL)))
    {
        return E_FAIL;
    }
    pD3DDevice->CreateTexture(256, 256, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pTexture, NULL);
    pTexture->GetSurfaceLevel(0, &pSurface);
    pD3DDevice->CreateTexture(32, 1024, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pSegTexture, NULL);
    pSegTexture->GetSurfaceLevel(0, &pSegSurface);
    //--------------------------------------
    // �e�N�X�`���X�e�[�W�̐ݒ�
    //--------------------------------------
    pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    //--------------------------------------
    // �����_�����O�X�e�[�g�p�����[�^�̐ݒ�
    //--------------------------------------
    // ���ʕ`�惂�[�h�̎w��
    pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    // �f�B�U�����O���s�Ȃ��i���i���`��j
    pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    // �m�[���C�e�B���O���[�h   
    pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    return S_OK;
}

//-------------------------------------------------------------
// �V�[���̔j��
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
// �`�揈��
//-------------------------------------------------------------
#define MAIN_X (224.0f/256.0f)
#define MAIN_Y (144.0f/256.0f)
#define MAIN_W (2 * 224.0f/234.0f - 1.0f)
#define SEG_X (4 * 8.0f/32.0f)
#define SEG_Y (4 * 144.0f/1024.0f)
#define SEG_W (1.0f - 2 * 8.0f/234.0f)
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
    // �摜�f�[�^���e�N�X�`���ɓ]��
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
    // �t���Z�O�����g�f�[�^���e�N�X�`���ɓ]��
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
    //���_�o�b�t�@�̒��g�𖄂߂�
    MY_VERTEX* v;
    pMyVB->Lock( 0, 0, (void**)&v, 0 );
    // ���_���W�̐ݒ�
    v[0].p = D3DXVECTOR3(-1.0f,  1.0f, 0.0f);
    v[1].p = D3DXVECTOR3(MAIN_W, 1.0f, 0.0f);
    v[2].p = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
    v[3].p = D3DXVECTOR3(MAIN_W, -1.0f, 0.0f);
    v[4].p = D3DXVECTOR3(SEG_W,  1.0f, 0.0f);
    v[5].p = D3DXVECTOR3( 1.0f,  1.0f, 0.0f);
    v[6].p = D3DXVECTOR3(SEG_W, -1.0f, 0.0f);
    v[7].p = D3DXVECTOR3( 1.0f, -1.0f, 0.0f);
    // ���_�J���[�̐ݒ�
    v[0].color = v[1].color = v[2].color = v[3].color = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
    v[4].color = v[5].color = v[6].color = v[7].color = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
    // �e�N�X�`�����W�̐ݒ�
    v[0].t = D3DXVECTOR2(0.0f, 0.0f);
    v[1].t = D3DXVECTOR2(MAIN_X, 0.0f);
    v[2].t = D3DXVECTOR2(0.0f, MAIN_Y);
    v[3].t = D3DXVECTOR2(MAIN_X, MAIN_Y);
    v[4].t = D3DXVECTOR2(0.0f, 0.0f);
    v[5].t = D3DXVECTOR2(SEG_X, 0.0f);
    v[6].t = D3DXVECTOR2(0.0f, SEG_Y);
    v[7].t = D3DXVECTOR2(SEG_X, SEG_Y);
    pMyVB->Unlock();
    // ��]����
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    // �c�i��90�x��]�j
    if (DrawMode & 0x01)
    {
        D3DXMatrixRotationZ(&mat, D3DXToRadian(90));
    }
    // �`��J�n�錾
    if(SUCCEEDED(pD3DDevice->BeginScene()))
    {
        pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF666666, 0.0f, 0);
        pD3DDevice->SetStreamSource(0, pMyVB, 0, sizeof(MY_VERTEX));
        pD3DDevice->SetFVF(MY_VERTEX_FVF);
        pD3DDevice->SetTexture( 0, pTexture);
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // 0�Ԃ̒��_����O�p�`��2��
        pD3DDevice->SetTexture( 0, pSegTexture);
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2); // 4�Ԃ̒��_����O�p�`��2��
        pD3DDevice->SetTransform(D3DTS_VIEW, &mat);
        // �`��I���錾
        pD3DDevice->EndScene();
    }
    // �`�挋�ʂ̓]��
    if(FAILED(pD3DDevice->Present(NULL, NULL, NULL, NULL)))
    {
        // �f�o�C�X�������畜�A
        pD3DDevice->Reset(&D3DPP);
    }
}

//-------------------------------------------------------------
void WsResize(void)
{
    RECT wind;
    RECT wind2;
    int  lcdHeight = 144;
    int  lcdWidth  = 224 + 10; // +10�̓Z�O�����g��
    int  client_width;
    int  client_height;

    if (DrawMode & 0x01)
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

