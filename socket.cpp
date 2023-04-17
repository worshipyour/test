#include<iostream>
#include<string>
//#include<Windows.h>
#include<tchar.h>
#include<queue>
#include<winsock2.h>
#include<vector>
#include<regex>  //д������ʽ
#pragma comment(lib ,"ws2_32.lib")
using namespace std;
string g_host;      //����
string g_object;   //��Դ·��
SOCKET g_socket;   //�ͻ����׽���
//ץȡurl
bool StartCatch(string url);
//����url
bool AnalyseURL(string url);
//��������
bool Connect();
//������ҳ
bool GetHtml(string url,string &html);
int main()
{
    //�����ļ���
    CreateDirectoryW(L"./file",NULL);
    cout<<"������Ҫץȡ����ʼURL"<<endl;
    string url;
    cin>>url;
    //��ʼץȡ
    StartCatch(url);
  return 0;
}

//ץȡuRL
bool StartCatch(string url)
{
    queue<string>q;//��ʼurl�����
    q.push(url);
    while(!q.empty())
    {
        string currenturl=q.front(); //ȡ��һ��
        q.pop();
        //������ҳ
        string html;   //�����ҳ��Դ����
        if (GetHtml(currenturl,html) == false)
            continue;
       // cout << html;//�����ҳ������
        //�ҳ�����http��URL
        vector<string> o_url;
        //������ʽƥ��URL
        smatch mat;
        regex reg;
        reg.assign("http://[^\\s'\"<>()]+");
        string::const_iterator start = html.begin();
        string::const_iterator end = html.end();
        while (regex_search(start, end, mat, reg))
        {
            string per(mat[0].first, mat[0].second);
            o_url.push_back(per);
            start = mat[0].second;
        }
        for (int i = 0; i < o_url.size(); i++)
        {
            cout  << o_url[i] << endl;
            q.push(o_url[i]);//����ʵ���Լ���ȡhttp��վ
        }
    }
    return true;
}

    //����URL
bool AnalyseURL(string url)
{
    //ͳһתΪСд
    string newurl;
    for (int i = 0; i < url.size(); i++)
    {
        newurl += tolower(url[i]);
    }
    //�ж��Ƿ���������
    if (string::npos == newurl.find("http://"))
        return false;
    if (newurl.length() <= 7)
        return false;
    int pos = newurl.find('/', 7);
    if (string::npos == pos)   //http:// www.baidu.com
    {
        g_host = newurl.substr(7);    //�ӵ�/����ʼ����� 
        g_object = '/';
    }
    else
    {
        g_host = newurl.substr(7, pos - 7);   //posλ��'/'��ȡ���ŵ�g_object��
        g_object = newurl.substr(pos);
    }
    if (g_host.empty())
        return false;
    return true;
}

//������ַ
bool Connect()
{
    WSADATA wd;
    //ʹ��socket֮ǰ�����ʼ������
    WSAStartup(MAKEWORD(2,2),&wd);
    //�����׽���
    g_socket=socket(AF_INET,SOCK_STREAM,0);
    if(g_socket==INVALID_SOCKET)
    return false;
    //����������ȡ������Ϣ
    hostent *host=gethostbyname(g_host.c_str());      //c������û��string ��.c_str();ת��Ϊ����c���Ե�
    if(host==NULL)return false;
    //��������
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    memcpy(&sa.sin_addr, host->h_addr, 4);
    sa.sin_port = htons(80);
    if (SOCKET_ERROR == connect(g_socket, (sockaddr*)&sa, sizeof(sa)))
        return false;
    return true;
}

//��ȡ��ҳ
bool GetHtml(string url, string& html)
{
    //����url
    if (false == AnalyseURL(url))
        return false;
   // cout << "����:" << g_host << endl;
   // cout << "��Դ·��:" << g_object << endl;
    //��������
    if (Connect() == false)
        return false;
    //����HTTP����
    string request = "GET " + g_object + " HTTP/1.1\r\n";
    request += "Host: " + g_host + "\r\n";
    request += "Connection: close\r\n\r\n";
    if (send(g_socket, request.c_str(), request.length(), 0) == SOCKET_ERROR)
        return false;
    //����HTTP��Ӧ
    char ch;
    int realsize = 0;
    while (recv(g_socket, &ch, sizeof(ch), 0))
    {
        html += ch;
    }
    cout << html << endl;
    return true;
    closesocket(g_socket);
}