# FileTransfer
## 구조체에 정보 넣어서 보내기 
## CreateFile 주의사항
```
// CreateFile 시 용도에 맞게 인자를 잘 설정해주어야 한다.

// Server
hDest = CreateFile(file_info.FileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ| FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    
// Client
hSrc = CreateFile(file_info.FileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    
// 파일크기를 받을 때,
// LARGE_INTEGER 는 구조체로서 사이즈를 받으려면 llFileSize.QuadPart; 쿼드파트에 값이 들어있음.
LARGE_INTEGER llFileSize = { 0 };
GetFileSizeEx(hDest,&llFileSize);
file_info.Size = llFileSize.QuadPart;
```
---
## ReadFile & send, WriteFile & recv 
**서버에서는 먼저 구조체 정보를 보낸다**  
**클라이언트에서는 먼저 구조체 정보를 받는다**  
서버에는 ReadFile & send, 클리이언트에는 WriteFile & recv를  
반복하여 서버에서는 파일을 (Read)읽고, 클라이언트에 보내고(send),  
클라이언트에서는 받고(recv) 파일을 쓴다(Write).  

서버측에서는 정확하게 읽은 값(dwByte)을 클라이언트에 보내야하고,  
클라이언트에서는 정확하게 읽은 값(rev)을 로컬에 Write 써야한다.  
```
//Server
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

// Client
for (int i = 0; i < file_info.Size; i+=dwByte)
	{
		// recv ,write (4K단위로)반복
		rev = recv(Socket, buf, sizeof(buf), 0);
		if (rev == SOCKET_ERROR || rev == 0)
			break;
		bSuccess= WriteFile(hSrc, buf, rev, &dwByte, 0);
		if ((bSuccess == FALSE) || (dwByte == 0)) break;
	}
```
