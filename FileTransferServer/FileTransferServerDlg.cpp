
// FileTransferServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "FileTransferServer.h"
#include "FileTransferServerDlg.h"
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


// CFileTransferServerDlg 대화 상자



CFileTransferServerDlg::CFileTransferServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILETRANSFERSERVER_DIALOG, pParent)
	, m_FilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileTransferServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILEPATH, m_FilePath);
	DDX_Control(pDX, IDC_LIST1, m_Ctrl_ListBox);
}

BEGIN_MESSAGE_MAP(CFileTransferServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFileTransferServerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFileTransferServerDlg::OnBnClickedButton2)
END_MESSAGE_MAP()



// CFileTransferServerDlg 메시지 처리기

BOOL CFileTransferServerDlg::OnInitDialog()
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
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFileTransferServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileTransferServerDlg::OnPaint()
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
HCURSOR CFileTransferServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <vector>	// 모형자 (템플릿)
//using namespace std;
std::vector<SOCKET> g_sockList;	// 소켓의 배열 <SOCKET> ,int, HUMAN 구조체 등등 사용가능

// 파일 정보 구조체
typedef struct FILE_INFO
{
	TCHAR FileName[260];
	int Size;
}FILE_INFO;

CString NameCopy;

DWORD WINAPI WorkThread(LPVOID p)
{
	//HANDLE hMutex;
	//hMutex = CreateMutex(NULL, FALSE, NULL);

	SOCKET ClientSocket = (SOCKET)p;
	char buf[4096];
	FILE_INFO file_info;
	DWORD dwByte=0;
	int i = 0;
	int rev;
	HANDLE hDest;
	BOOL bSuccess = FALSE;
	LARGE_INTEGER llFileSize = { 0 };

	// 파일 경로를 file_info.FileName에 복사한다.
	lstrcpy(file_info.FileName,NameCopy);

	hDest = CreateFile(file_info.FileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ| FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	// 파일 사이즈를 구한다.
	GetFileSizeEx(hDest,&llFileSize);
	file_info.Size = llFileSize.QuadPart;

	// 파일 이름에 .net을 추가한다. 
	lstrcat(file_info.FileName, _T(".net"));
	send(ClientSocket, (char*)&file_info, sizeof(file_info), 0);

	for (int i = 0; i < file_info.Size; i += dwByte)
	{
		// send ,read (4K단위로)반복
		bSuccess = ReadFile(hDest, buf, sizeof(buf), &dwByte, 0);
		if ((bSuccess == FALSE) || (dwByte == 0)) break;

		// rev = send(ClientSocket, buf, strlen(buf) + 1, 0);
		rev = send(ClientSocket, buf, dwByte, 0);
		if (rev == SOCKET_ERROR || rev == 0)
			break;
	}

	//WaitForSingleObject(hMutex, INFINITE);
	//ReleaseMutex(hMutex);
	//CloseHandle(hMutex);

	// 6. closesocket() 벡터 배열로 소켓 클라이언트 리스트 제거 
	for (int i = 0; i < g_sockList.size(); i++)
	{
		if (g_sockList[i] == ClientSocket)
		{
			//제거
			g_sockList.erase(g_sockList.begin() + i);
			break;
		}
	}
	closesocket(ClientSocket);
	return 0;
}


DWORD WINAPI ListenThread(LPVOID p)
{
	WSADATA wsa;
	CString strTemp;
	// 초기화 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		//printf("윈도우 소켓 초기화 실패\n");
		return -1;
	}
	//printf("윈도우 소켓 초기화 성공\n");
	//------------------------------------------
	//--- 리슨 소켓 생성
	// 1. socket()
	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// SOCK_STREAM = TCP 
	if (ListenSocket == INVALID_SOCKET)
	{
		return -1;
	}
	//------------------------------------------
	// 2. bind()
	SOCKADDR_IN ServerAddr;
	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;	// TCP/IP 통신을 하겠다.
	ServerAddr.sin_port = htons(5000);	// 9000을 뒤집어서 내보낸다. (호스트가 네트워크 내보낼 때)
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// IP 주소를 알려준다

	int retval = bind(ListenSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
	if (retval == SOCKET_ERROR)
	{
		return -1;
	}

	//------------------------------------------
	// 3. listen()
	retval = listen(ListenSocket, SOMAXCONN); // 0x7fffffffffffffff
	if (retval == SOCKET_ERROR)
	{
		return -1;
	}
	// AfxMessageBox("대기 소켓의 초기화 과정 완료\n");

	//--- 클라이언트 접속대기 (통신 소켓 생성)
	while (1)	// 다수의 고객을 위한 무한루프 
	{
		CWnd* pWnd = (CWnd*)p;
		SOCKET ClientSocket;
		// 4. accept()
		SOCKADDR_IN ClientAddr = { 0 };
		int ClientAddrSize = sizeof(ClientAddr);

		ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSize);
		if (ClientSocket == INVALID_SOCKET)
		{
			return -1;
		}

		//g_sockList[g_nCount] = ClientSocket;
		//g_nCount++;
		// 아래 한 줄로 위의 2줄 코드가 대체됨.
		g_sockList.push_back(ClientSocket);
		
		CFileTransferServerDlg* pp = (CFileTransferServerDlg*)p;
		pp->m_Ctrl_ListBox.AddString(_T("클라이언트 접속 완료"));
		
		// 클라이언트 정보 출력
		pp->m_Ctrl_ListBox.AddString(inet_ntoa(ClientAddr.sin_addr));
		strTemp.Format("%d", ntohs(ClientAddr.sin_port));
		pp->m_Ctrl_ListBox.AddString(strTemp);

		// 하나의 클라이언트를 위한 서비스 스레드 생성 
		CloseHandle(CreateThread(NULL, 0, WorkThread, (LPVOID)ClientSocket, NULL, NULL));
	}
	//------------------------------------------
	// 종료
	// 콘솔과 다르게 버튼 누를 때 마다 되므로 리슨소켓을 닫아줘야한다.
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}

void CFileTransferServerDlg::OnBnClickedButton1()	// 파일 전송 시작 버튼 
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// if (SourceFile ==_T(""))
	//{
	//	AfxMessageBox(_T("전송할 파일을 선택하세요"));
	//	return;
	//}
	CloseHandle(CreateThread(NULL, 0, ListenThread, this, 0, 0));
}


void CFileTransferServerDlg::OnBnClickedButton2()	// 전송할 파일 선택 버튼 
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// True 는 열기로 열림, FALSE는 저장으로 열림.
	CFileDialog a(TRUE);
	int nResult = a.DoModal();
	if (nResult == IDOK)
	{
		NameCopy = a.GetPathName();
		SetDlgItemText(IDC_FILEPATH, a.GetPathName());
	}
}
