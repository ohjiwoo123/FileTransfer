
// FileTransferClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "FileTransferClient.h"
#include "FileTransferClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileTransferClientDlg 대화 상자



CFileTransferClientDlg::CFileTransferClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILETRANSFERCLIENT_DIALOG, pParent)
	, m_PortNum(0)
	, m_strFileName(_T(""))
	, m_FileSize(_T(""))
	, m_TransByte(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileTransferClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITPORT, m_PortNum);
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_FILESIZE, m_FileSize);
	DDX_Text(pDX, IDC_TRANSFERBYTE, m_TransByte);
	DDX_Control(pDX, IDC_IPADDRESS1, m_Ctrl_IpAddr);
}

BEGIN_MESSAGE_MAP(CFileTransferClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFileTransferClientDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFileTransferClientDlg 메시지 처리기

BOOL CFileTransferClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetDlgItemText(IDC_EDITPORT,"5000");
	m_Ctrl_IpAddr.SetAddress(192, 168, 1, 34);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

typedef struct FILE_INFO
{
	TCHAR FileName[260];
	int Size;
}FILE_INFO;

void CFileTransferClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFileTransferClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFileTransferClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI SocketClientThread(LPVOID p)
{
	CWnd* pWnd = (CWnd*)p;
	WSADATA wsa;
	// 초기화 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		// printf("윈도우 소켓 초기화 실패\n");
		return -1;
	}
	// printf("윈도우 소켓 초기화 성공\n");

	//--------------------------------------------
	// 1. socket()
	SOCKET Socket;
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// SOCK_STREAM = TCP 
	if (Socket == INVALID_SOCKET)
	{
		return -1;
	}

	//2. connect()

	// 주소확인 cmd >> ipconfig 명령 (192.168.1.34)
	SOCKADDR_IN ServerAddr = { 0, };
	ServerAddr.sin_family = AF_INET;	// TCP/IP 통신을 하겠다.
	ServerAddr.sin_addr.s_addr = inet_addr("192.168.1.34");	// IP 주소를 알려준다
	ServerAddr.sin_port = htons(5000);	// 5000을 뒤집어서 내보낸다. (호스트가 네트워크 내보낼 때)
		
	int retval = connect(Socket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
	if (retval == SOCKET_ERROR)
	{
		return -1;				// 현재 여기서 에러남 
	}

	DWORD dwByte=NULL;
	// LARGE_INTEGER llFileSize = { 0 };	// 파일 사이즈 크기 

	char buf[4096];
	CString strTemp;
	FILE_INFO file_info;
	BOOL bSuccess = FALSE;
	// 구조체 recv 
	int rev;
	rev = recv(Socket, (char*)&file_info, sizeof(file_info), 0);

	HANDLE hSrc;
	hSrc = CreateFile(file_info.FileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	pWnd->SetDlgItemText(IDC_FILENAME,file_info.FileName);
	strTemp.Format("%d", file_info.Size);
	pWnd->SetDlgItemText(IDC_FILESIZE,strTemp);
	pWnd->SetDlgItemText(IDC_TRANSFERBYTE,strTemp);

	for (int i = 0; i < file_info.Size; i+=dwByte)
	{
		// recv ,write (4K단위로)반복
		rev = recv(Socket, buf, sizeof(buf), 0);
		if (rev == SOCKET_ERROR || rev == 0)
			break;
		bSuccess= WriteFile(hSrc, buf, rev, &dwByte, 0);
		if ((bSuccess == FALSE) || (dwByte == 0)) break;
	}
	closesocket(Socket);
	WSACleanup();
	
	return 0;
}
void CFileTransferClientDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CloseHandle(CreateThread(NULL, 0, SocketClientThread, this, 0, 0));
}
