#ifndef GWSDK_H
#define GWSDK_H
#include <windows.h>

#define SSLVPNAPI __stdcall
#define SSLVPNLIB __declspec(dllimport)

//////////////////////////////////////////////////////////
//事件通知ID定义
//用户已下线
#define EVENTID_LOGOUT                    202

//逐条上报代理服务的状态
//#define EVENTID_PROXYSERVICESTATUS        205

//NC状态（NC全局的虚拟IP、流量等信息）
#define EVENTID_NCSERVICESTATUS			  206

//服务（业务）列表
#define EVENTID_SERVICELIST               208

//代理服务已启动
#define EVENTID_PROXY_STARTED			  232

//NC服务已启动
#define EVENTID_NC_STARTED				  233

//用户已登录成功
#define EVENTID_USER_LOGINED			  234

// 服务信息:名称、ip
#define EVENTID_GroupListEnd              237

//////////////////////////////////////////////////////////
//登录相关错误码
enum LoginStatus
{
        ERROR_LOGIN_SUCCESS = 0,				// 登录成功
		ERROR_LOGIN_FAILED,					// 登录失败
		ERROR_LOGIN_TIMEOUT,				// 登录超时
		ERROR_COMPONET_NOTOK,				// 控件未正确安装
		ERROR_USER_ALREADY_LOGIN,			// 用户已经登录
		ERROR_INVALID_SSLVPN_ADDRESS,		// svpn地址不合法
		ERROR_INVALID_SSLVPN_PORT,			// svpn端口不合法
		ERROR_INIT_FAILED,					// 初始化SDK失败
		ERROR_INVALIED_PARAMETER,			// 参数错误
		ERROR_SAVE_INFO_FAILED,				// 保存信息失败
		ERROR_UNKNOW_ERR
};
//////////////////////////////////////////////////////////
//登出（注销）相关错误码，一般来说不用处理此错误
enum LogoutStatus
{
	LOGOUT_SUCCESS = 0,	//注销成功
		LOGOUT_FAIL,		   	//注销失败		
};

//////////////////////////////////////////////////////////
//代理转发服务相关状态
enum ConnectStatus
{
	CONNECTED = 0,   //已连接
	NOT_CONNECT	     //未连接
};

//////////////////////////////////////////////////////////
//NC相关的状态码
enum NCStatus
{
	NC_NOTCONNECTED = 1010000,		//未连接
		NC_CONNECTING,				//正在连接
		NC_CONNECTED,				//已连接
		NC_DISCONNECTING,			//正在断开
		NC_SERVICE_ERROR,			//服务错误
		NC_SYSTEM_ERROR,			//系统错误
		NC_CONFIGURING,				//正在配置
		NC_UNKNOWN					//未知错误
};

//////////////////////////////////////////////////////////
/*
FUNCEventCB: LoginSslVpnEx函数使用的回调函数定义
参数说明：
dwContext
[in]unsigned int，LoginSslVpnEx调用时传入的上下文参数
evtID
[in]unsigned int，事件的ID，参照EVENTID_*的定义
szParam
[in]char*，事件相关参数，各事件参数不同，JSON格式
*/
typedef void (__stdcall *FUNCEventCB)(
									  unsigned int dwContext,
									  unsigned int evtID,
									  const char* szParam
									  );

/*
函数说明：用户名密码登录SSLVPN接口
参数说明：
szLoginUrl
[in]char*，VPN地址，格式必须为 https://IP:端口 ,最长不超过1024字节。
szUserName
[in]char*，用户名，不超过64字节。
szPassword
[in]char*，密码，不超过64字节。 
uTimeout
[in]unsigned int类型，目前未使用，只是为了兼容而留的参数，填0即可
szErrorMsg
[in][out]char* 类型，返回登录错误信息
nErrorMsgLen
[in]unsigned int类型，登录错误信息长度
EventCB,
[in]FUNCEventCB类型，指定事件回调函数，可NULL，当客户端发生事件、通知时，SDK调用此回调函数通知调用者
iCBContext
[in]unsigned int类型，指定事件回调函数的回传参数，调用回调函数时，此参数将原样回传
szAuthServer
[in]char*类型，指定认证服务器名称，若为NULL，将使用第一个认证服务器
szExtraInfo
[in]char*类型，指定外部认证服务器自定义的额外信息，比如蓝信认证方式，传入extra_token_info，可为NULL或""
*/
typedef LoginStatus (SSLVPNAPI *FUNCLoginSslVpnEx)(
			  const char* szLoginUrl,
			  const char* szUserName,
			  const char* szPassword,
			  unsigned int uTimeout,
			  char* szErrorMsg,
			  unsigned int nErrorMsgLen,
			  FUNCEventCB EventCB,
			  unsigned int iCBContext,
			  const char* szAuthServer,
			  const char* szExtraInfo
			  );
LoginStatus SSLVPNAPI LoginSslVpnEx(
			  const char* szLoginUrl,
			  const char* szUserName,
			  const char* szPassword,
			  unsigned int uTimeout,
			  char* szErrorMsg,
			  unsigned int nErrorMsgLen,
			  FUNCEventCB EventCB,
			  unsigned int iCBContext,
			  const char* szAuthServer,
			  const char* szExtraInfo
			  );

//////////////////////////////////////////////////////////
/*
函数说明：登出VPN，一般来说，不用处理返回值
*/
typedef LogoutStatus (SSLVPNAPI *FUNCLogoutSslVpn)(
			 void
			 );
LogoutStatus SSLVPNAPI LogoutSslVpn(
			 void
			 );


//////////////////////////////////////////////////////////
/*
函数说明：设置自动重连标志，若设置为TRUE，客户端超时、用户被踢出后会自动重新登录
默认是设置为TRUE的，只有需要关闭此特性时才需要调用。
注意，在登录前调用此函数才能生效。
*/

typedef void
(SSLVPNAPI
*FUNCSetRenewFlag)(BOOL bAutoRenew);

void
SSLVPNAPI
SetRenewFlag(BOOL bAutoRenew);

/*
函数说明：设置VPN网关证书Finger
一旦设置这个值，客户端所有SSL连接将验证网关证书是否匹配

例："7C:E1:2B:29:EE:6F:B9:AC:8B:3F:CB:7E:20:54:8F:0A:80:09:7C:03|AD:B4:4F:0A:F4:D0:D4:F0:B9:E6:22:53:BA:38:DB:21:A2:F2:45:17|AD:B4:4F:0A:F4:D0:D4:F0:B9:E6:22:53:BA:38:DB:21:A2:F2:45:18"
*/

typedef void
(SSLVPNAPI
*FUNCSetGWCertFinger)(const char* szCertFinger);

void
SSLVPNAPI
SetGWCertFinger(const char* szCertFinger);
//////////////////////////////////////////////////////////
//
//如下为一些可选接口，主要是针对一些特定客户的定制接口，正常情况无需关注
//
//////////////////////////////////////////////////////////
/*
函数说明：简化的登录接口，参数同LoginSslVpnEx。
不建议使用此函数，而是使用LoginSslVpnEx
*/
typedef LoginStatus (SSLVPNAPI *FUNCLoginSslVpn)(
			const char* szLoginUrl,
			const char* szUserName,
			const char* szPassword,
			unsigned int uTimeout,
			char* szErrorMsg,
			unsigned int nErrorMsgLen
			);
LoginStatus SSLVPNAPI LoginSslVpn(
			const char* szLoginUrl,
			const char* szUserName,
			const char* szPassword,
			unsigned int uTimeout,
			char* szErrorMsg,
			unsigned int nErrorMsgLen
			);

//////////////////////////////////////////////////////////
/*
函数说明：国密SM2证书专用登录接口，只针对一些特定的key
szPin：ukey的pin码
其他参数同LoginSslVpnEx
*/
typedef LoginStatus (SSLVPNAPI *FUNCLoginSslVpnByGMKey)(
				   const char*		szLoginUrl, 
				   const char*		szPin,        
				   unsigned int		uTimeout,  
				   char*			szErrorMsg,
				   unsigned int		nErrorMsgLen
				   );
typedef LoginStatus (SSLVPNAPI *FUNCLoginSslVpnByGMKeyEx)(
					 const char*	szLoginUrl, 
					 const char*	szPin,        
					 unsigned int	uTimeout,  
					 char*			szErrorMsg,
					 unsigned int	nErrorMsgLen,
					 FUNCEventCB	EventCB,
					 unsigned int	iCBContext,
					 const char*	szAuthServer
					 );

LoginStatus
SSLVPNAPI
LoginSslVpnByGMKey(
				   const char*		szLoginUrl, 
				   const char*		szPin,        
				   unsigned int		uTimeout,  
				   char*			szErrorMsg,
				   unsigned int		nErrorMsgLen
				   );

LoginStatus
SSLVPNAPI
LoginSslVpnByGMKeyEx(
					 const char*	szLoginUrl, 
					 const char*    szPin,        
					 unsigned int	uTimeout,  
					 char*			szErrorMsg,
					 unsigned int	nErrorMsgLen,
					 FUNCEventCB	EventCB,
					 unsigned int	iCBContext,
					 const char*	szAuthServer
					 );



//////////////////////////////////////////////////////////
/*
函数说明：查询代理服务启动状态
不建议使用此方式，应该使用事件回调函数实时获取状态
*/
typedef ConnectStatus (SSLVPNAPI *FUNCQueryTcpServiceStatus)(
			 void
			 );
ConnectStatus SSLVPNAPI QueryTcpServiceStatus(
			 void
			 );

//////////////////////////////////////////////////////////
/*
多因子认证结构体
调用LoginSslVpnEx接口时，在szExtraInfo传入子认证信息
如OTP认证：
	SUBAUTH_CONTENT content;
	content.typeSubAuth = TypeOTP;
	memcpy_s(content.szOTP, sizeof(content.szOTP), "OTP值", strlen("OTP值"));
	LoginSslVpnEx(..., (LPCTSTR)&content);
*/

typedef enum {
	TypeOTP								// 动态口令认证

} SUBAUTH_TYPE;							// 子认证类型

typedef struct {
	SUBAUTH_TYPE typeSubAuth;	

	char szOTP[16];						// 动态口令值

} SUBAUTH_CONTENT, *PSUBAUTH_CONTENT;	// 子认证信息结构体




#endif
