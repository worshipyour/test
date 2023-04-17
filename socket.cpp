#include<iostream>
#include<string>
//#include<Windows.h>
#include<tchar.h>
#include<queue>
#include<winsock2.h>
#include<vector>
#include<regex>  //写正则表达式
#pragma comment(lib ,"ws2_32.lib")
using namespace std;
string g_host;      //域名
string g_object;   //资源路径
SOCKET g_socket;   //客户端套接字
//抓取url
bool StartCatch(string url);
//解析url
bool AnalyseURL(string url);
//连接网络
bool Connect();
//下载网页
bool GetHtml(string url,string &html);
int main()
{
    //创建文件夹
    CreateDirectoryW(L"./file",NULL);
    cout<<"输入你要抓取的起始URL"<<endl;
    string url;
    cin>>url;
    //开始抓取
    StartCatch(url);
  return 0;
}

//抓取uRL
bool StartCatch(string url)
{
    queue<string>q;//初始url入队列
    q.push(url);
    while(!q.empty())
    {
        string currenturl=q.front(); //取出一个
        q.pop();
        //下载网页
        string html;   //存放网页的源代码
        if (GetHtml(currenturl,html) == false)
            continue;
       // cout << html;//输出网页的内容
        //找出所有http的URL
        vector<string> o_url;
        //正则表达式匹配URL
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
            q.push(o_url[i]);//可以实现自己爬取http网站
        }
    }
    return true;
}

    //解析URL
bool AnalyseURL(string url)
{
    //统一转为小写
    string newurl;
    for (int i = 0; i < url.size(); i++)
    {
        newurl += tolower(url[i]);
    }
    //判断是否满足条件
    if (string::npos == newurl.find("http://"))
        return false;
    if (newurl.length() <= 7)
        return false;
    int pos = newurl.find('/', 7);
    if (string::npos == pos)   //http:// www.baidu.com
    {
        g_host = newurl.substr(7);    //从第/个开始到最后， 
        g_object = '/';
    }
    else
    {
        g_host = newurl.substr(7, pos - 7);   //pos位的'/'不取，放到g_object中
        g_object = newurl.substr(pos);
    }
    if (g_host.empty())
        return false;
    return true;
}

//连接网址
bool Connect()
{
    WSADATA wd;
    //使用socket之前必须初始化网络
    WSAStartup(MAKEWORD(2,2),&wd);
    //创建套接字
    g_socket=socket(AF_INET,SOCK_STREAM,0);
    if(g_socket==INVALID_SOCKET)
    return false;
    //根据域名获取主机信息
    hostent *host=gethostbyname(g_host.c_str());      //c语言中没有string 用.c_str();转换为符合c语言的
    if(host==NULL)return false;
    //连接主机
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    memcpy(&sa.sin_addr, host->h_addr, 4);
    sa.sin_port = htons(80);
    if (SOCKET_ERROR == connect(g_socket, (sockaddr*)&sa, sizeof(sa)))
        return false;
    return true;
}

//爬取网页
bool GetHtml(string url, string& html)
{
    //解析url
    if (false == AnalyseURL(url))
        return false;
   // cout << "域名:" << g_host << endl;
   // cout << "资源路径:" << g_object << endl;
    //连接网络
    if (Connect() == false)
        return false;
    //发送HTTP请求
    string request = "GET " + g_object + " HTTP/1.1\r\n";
    request += "Host: " + g_host + "\r\n";
    request += "Connection: close\r\n\r\n";
    if (send(g_socket, request.c_str(), request.length(), 0) == SOCKET_ERROR)
        return false;
    //接收HTTP响应
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