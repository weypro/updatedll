//文件名：main.cpp
//dll主文件
//作者：wey
//时间：2019.02
//版本：1.0
#include<windows.h>
#include<wininet.h>
#include<cstdio>
#include<stdlib.h>
#pragma comment(lib,"wininet.lib")

//更新信息URL
char g_strTargetURL[100] = "https://wey.xsyds.cn/update.txt";
//dll所在路径
char g_strMyPath[100] = { 0 };
//目标文件路径（默认为dll所在路径+文件名）
char g_strNewTargetFileURL[100] = { 0 };
//更新信息缓冲区
char buffer[100] = { 0 };

//初始化，指定更新信息URL
extern "C" __declspec(dllexport) int __stdcall Init(char* strTargetURL);
//得到更新信息中的版本号
extern "C" __declspec(dllexport) int __stdcall GetNewVersion(char* strVersion);
//得到更新信息中的目标URL
extern "C" __declspec(dllexport) int __stdcall GetNewURL(char* strURL);
//得到更新信息中目标URL对应的文件
extern "C" __declspec(dllexport) int __stdcall GetNewTargetFile();
//清理更新信息临时文件
extern "C" __declspec(dllexport) void __stdcall CleanUp();

//删除指定字符串
char* strdel(char *src, const char *sub);
//简单的json解析器
bool easyJsonParse(const char *src, const char *item, char *value);
//下载文件
bool getFileFromNet(char *target, char *path);


int __stdcall Init(char* strTargetURL)
{
	char temppath[100] = { 0 };
	HMODULE hModule = GetModuleHandle("downdll.dll");
	GetModuleFileName(hModule, g_strMyPath, sizeof(g_strMyPath));//获得dll路径
	strcpy(g_strMyPath, strdel(g_strMyPath, "\\downdll.dll"));//获得目录

	strcpy(temppath, g_strMyPath);
	strcat(temppath, "\\record.txt");//组合记录文件路径
	strcpy(g_strTargetURL, strTargetURL);
	
	//下载记录文件
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
	strcat(temppath, strrchr(g_strNewTargetFileURL, '\/')+1);//组合出当前目录下对应的文件路径
	//MessageBox(NULL, temppath, temppath, 0);

	//下载指定文件
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
	DeleteFile(temppath);//删除临时文件
}

bool getFileFromNet(char *target, char *path)
{
	DWORD byteread = 0;
	HINTERNET internetopen;

	internetopen = InternetOpen("MyUpdate", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (internetopen == NULL)
	{
		MessageBox(NULL, "无法连接网络!", "Error", 0);
		return false;
	}

	HINTERNET internetopenurl;
	internetopenurl = InternetOpenUrl(internetopen, target, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (internetopenurl == NULL)
	{
		MessageBox(NULL, "无法打开URL!", "Error", 0);
		return false;
	}

	BOOL hwrite = 0;
	DWORD written = 0;
	HANDLE createfile;
	createfile = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (createfile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, "文件无法下载!", "Error", 0);
		return false;
	}

	BOOL internetreadfile = FALSE;
	while (1)
	{
		internetreadfile = InternetReadFile(internetopenurl, buffer, sizeof(buffer), &byteread);
		if (byteread == 0)
			break;

		strcpy(buffer, strdel(buffer, "\\"));//处理下php echo输出的json的某些问题= =
		hwrite = WriteFile(createfile, buffer, strlen(buffer), &written, NULL);
		if (hwrite == 0)
		{
			MessageBox(NULL, "写入文件失败!", "Error", 0);
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
	strcpy(temp, temp + strlen(item) + 3);//+3因为还要算入标点，请确保key和value之间没有空格
	temp[unsigned int(strchr(temp, '\"') - temp)] = 0;
	strcpy(value, temp);
	return true;
}
