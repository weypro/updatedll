//�ļ�����main.cpp
//dll���ļ�
//���ߣ�wey
//ʱ�䣺2019.02
//�汾��1.0
#include<windows.h>
#include<wininet.h>
#include<cstdio>
#include<stdlib.h>
#pragma comment(lib,"wininet.lib")

//������ϢURL
char g_strTargetURL[100] = "https://wey.xsyds.cn/update.txt";
//dll����·��
char g_strMyPath[100] = { 0 };
//Ŀ���ļ�·����Ĭ��Ϊdll����·��+�ļ�����
char g_strNewTargetFileURL[100] = { 0 };
//������Ϣ������
char buffer[100] = { 0 };

//��ʼ����ָ��������ϢURL
extern "C" __declspec(dllexport) int __stdcall Init(char* strTargetURL);
//�õ�������Ϣ�еİ汾��
extern "C" __declspec(dllexport) int __stdcall GetNewVersion(char* strVersion);
//�õ�������Ϣ�е�Ŀ��URL
extern "C" __declspec(dllexport) int __stdcall GetNewURL(char* strURL);
//�õ�������Ϣ��Ŀ��URL��Ӧ���ļ�
extern "C" __declspec(dllexport) int __stdcall GetNewTargetFile();
//���������Ϣ��ʱ�ļ�
extern "C" __declspec(dllexport) void __stdcall CleanUp();

//ɾ��ָ���ַ���
char* strdel(char *src, const char *sub);
//�򵥵�json������
bool easyJsonParse(const char *src, const char *item, char *value);
//�����ļ�
bool getFileFromNet(char *target, char *path);


int __stdcall Init(char* strTargetURL)
{
	char temppath[100] = { 0 };
	HMODULE hModule = GetModuleHandle("downdll.dll");
	GetModuleFileName(hModule, g_strMyPath, sizeof(g_strMyPath));//���dll·��
	strcpy(g_strMyPath, strdel(g_strMyPath, "\\downdll.dll"));//���Ŀ¼

	strcpy(temppath, g_strMyPath);
	strcat(temppath, "\\record.txt");//��ϼ�¼�ļ�·��
	strcpy(g_strTargetURL, strTargetURL);
	
	//���ؼ�¼�ļ�
	if (getFileFromNet(g_strTargetURL, temppath))
	{
		GetNewURL(g_strNewTargetFileURL);
		return TRUE;
	}
	return FALSE;
}

int __stdcall GetNewVersion(char* strVersion)
{
	if(easyJsonParse(buffer, "version", strVersion))
		return TRUE;
	return FALSE;
}

int __stdcall GetNewURL(char* strURL)
{
	if (easyJsonParse(buffer, "url", strURL))
		return TRUE;
	return FALSE;
}

int __stdcall GetNewTargetFile()
{
	char temppath[100] = { 0 };
	strcpy(temppath, g_strMyPath);
	//MessageBox(NULL, g_strNewTargetFileURL, g_strNewTargetFileURL, 0);
	strcat(temppath, "\\");
	strcat(temppath, strrchr(g_strNewTargetFileURL, '\/')+1);//��ϳ���ǰĿ¼�¶�Ӧ���ļ�·��
	//MessageBox(NULL, temppath, temppath, 0);

	//����ָ���ļ�
	if (getFileFromNet(g_strNewTargetFileURL, temppath))
	{
		return TRUE;
	}
	return FALSE;
}

void __stdcall CleanUp()
{
	char temppath[100] = { 0 };
	strcpy(temppath, g_strMyPath);
	strcat(temppath, "\\record.txt");
	//MessageBox(NULL, temppath, temppath, 0);
	DeleteFile(temppath);//ɾ����ʱ�ļ�
}

bool getFileFromNet(char *target, char *path)
{
	DWORD byteread = 0;
	HINTERNET internetopen;

	internetopen = InternetOpen("MyUpdate", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (internetopen == NULL)
	{
		MessageBox(NULL, "�޷���������!", "Error", 0);
		return false;
	}

	HINTERNET internetopenurl;
	internetopenurl = InternetOpenUrl(internetopen, target, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (internetopenurl == NULL)
	{
		MessageBox(NULL, "�޷���URL!", "Error", 0);
		return false;
	}

	BOOL hwrite = 0;
	DWORD written = 0;
	HANDLE createfile;
	createfile = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (createfile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, "�ļ��޷�����!", "Error", 0);
		return false;
	}

	BOOL internetreadfile = FALSE;
	while (1)
	{
		internetreadfile = InternetReadFile(internetopenurl, buffer, sizeof(buffer), &byteread);
		if (byteread == 0)
			break;

		strcpy(buffer, strdel(buffer, "\\"));//������php echo�����json��ĳЩ����= =
		hwrite = WriteFile(createfile, buffer, strlen(buffer), &written, NULL);
		if (hwrite == 0)
		{
			MessageBox(NULL, "д���ļ�ʧ��!", "Error", 0);
			return false;
		}
	}

	CloseHandle(createfile);
	InternetCloseHandle(internetopenurl);
	InternetCloseHandle(internetopen);
	return true;
}

char* strdel(char *src, const char *sub)
{
	char *st = src, *s1 = NULL;
	const char *s2 = NULL;
	while (*st&& *sub)
	{
		s1 = st;
		s2 = sub;
		while (*s1 && *s2 &&!(*s1 - *s2))
		{
			s1++;
			s2++;
		}
		if (!*s2)
		{
			while (*st++ = *s1++);
			st = src;
		}
		st++;
	}
	return (src);
}

bool easyJsonParse(const char *src, const char *item, char *value)
{
	char temp[100] = { 0 };
	strcpy(temp, strstr(src, item));
	strcpy(temp, temp + strlen(item) + 3);//+3��Ϊ��Ҫ�����㣬��ȷ��key��value֮��û�пո�
	temp[unsigned int(strchr(temp, '\"') - temp)] = 0;
	strcpy(value, temp);
	return true;
}
