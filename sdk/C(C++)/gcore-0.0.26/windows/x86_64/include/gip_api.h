/*
 * gip.h
 *
 *  Created on: 2021年4月23日
 *      Author: 36192
 */

#ifndef GIP_API_H_
#define GIP_API_H_


#include <stdint.h>

#ifdef _WIN32
#define DLL_API __declspec(dllexport)
#define DLL_PRIVATE __declspec(dllimport)
#else
#define DLL_API __attribute__((visibility("default")))
#define DLL_PRIVATE __attribute__ ((visibility("hidden")))
#define __stdcall
#endif


typedef unsigned char boolean;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 启动结点的配置信息
 */
typedef struct S_GCONFIG_PARAM {
    int gid;
    int gsubid;
    char pk_filename[256];
    char pk_pwd[64];
    char listen_ip[32];
    int listen_port;
    int use_upnp;
    char mapping_ip[32];
    int mapping_port;
    char pubkey_cache_dir[256];
} gconfig_param_t;

typedef enum E_TRANSFER_MODE {
    DIRECT_MODE = 0, //直连模式，传输更快
    JUMP_MODE = 1, //转发模式，传输较慢
} transfer_mode_t;

typedef enum E_MSG_ACTION {
    REQUEST = 0, //
    RESPONSE = 1, //
} msg_action_t;

/**
 * Gip SDK 初始化
 */
DLL_API void __stdcall Gip_Initialize();

/**
 * 当 GNode/GTunnel/GConnection 创建失败，可以通过 Gip_GetErrorStr 查看错误码
 *  10001: "GID NOT EXIST"
 *  10002: "PUB KEY NOT FOUND"
 *  10003: "REMOTE NOT ONLINE"
 *  10004: "REMOTE NOT AUTHORIZED"
 *  10005: "TUNNEL NOT EXIST"
 *  10006: "TUNNEL INVALID"
 *  10007: "SERVER ERROR"
 *
 * @retun errorCode
 */
DLL_API int __stdcall Gip_GetErrorCode();

/**
 * 当 GNode/GTunnel/GConnection 创建失败，可以通过 Gip_GetErrorStr 找到出错原因
 * @retun errorStr , 调用方不需要释放
 */
DLL_API const char *__stdcall Gip_GetErrorStr(char *buffer, int buff_size);

/**
 * Gip SDK 反初始化，资源释放
 */
DLL_API void __stdcall Gip_UnInitialize();

typedef int gstate;
typedef void *ghandle;

//连接(Connection)
DLL_API gstate __stdcall GConnection_Destroy(ghandle node, uint32_t conn_id);

DLL_API ghandle __stdcall GConnection_Get(ghandle node, uint32_t conn_id);

DLL_API const char *__stdcall GConnection_Get_Remote_IP(ghandle node, uint32_t conn_id);

DLL_API uint16_t __stdcall GConnection_Get_Remote_Port(ghandle node, uint32_t conn_id);

DLL_API uint32_t __stdcall GConnection_Get_Remote_Gid(ghandle node, uint32_t conn_id);

DLL_API uint32_t __stdcall GConnection_Get_Remote_SubId(ghandle node, uint32_t conn_id);

DLL_API uint32_t __stdcall GConnection_Get_Remote_ConId(ghandle node, uint32_t conn_id);

DLL_API gstate __stdcall GConnection_IsOpen(ghandle node, uint32_t conn_id);

DLL_API gstate __stdcall GConnection_Close(ghandle node, uint32_t conn_id);

DLL_API gstate __stdcall
GConnection_Receive(ghandle node, uint32_t conn_id, unsigned char **data, int *size, int timeout_ms);

DLL_API gstate __stdcall
GConnection_Send(ghandle node, uint32_t conn_id, msg_action_t action, gstate is_compress, unsigned char *data, int size,int timeout_ms);

DLL_API gstate __stdcall GConnection_Reset(ghandle node, uint32_t conn_id);

DLL_API gstate __stdcall
GConnection_RemoteCall(ghandle node, uint32_t conn_id, gstate is_compress, unsigned char *data, int size,
                       unsigned char **result, int *result_size, int timeout_ms);

//通道（Tunnel）
DLL_API gstate __stdcall GTunnel_Destroy(ghandle node, int tunnel_id);

DLL_API ghandle __stdcall GTunnel_Get(ghandle node, int tunnel_id);

DLL_API const char *__stdcall GTunnel_Get_Remote_IP(ghandle node, int tunnel_id);

DLL_API uint16_t __stdcall GTunnel_Get_Remote_Port(ghandle node, int tunnel_id);

DLL_API uint32_t __stdcall GTunnel_Get_Remote_Gid(ghandle node, int tunnel_id);

DLL_API uint32_t __stdcall GTunnel_Get_Remote_SubId(ghandle node, int tunnel_id);

DLL_API gstate __stdcall GTunnel_IsOpen(ghandle node, int tunnel_id);

DLL_API gstate __stdcall GTunnel_Close(ghandle node, int tunnel_id);

DLL_API int __stdcall GTunnel_OpenGConnection(ghandle node, int tunnel_id);

DLL_API gstate __stdcall GTunnel_CloseGConnection(ghandle node, uint32_t conn_id);

DLL_API gstate __stdcall GTunnel_ResetGConnection(ghandle node, uint32_t conn_id);

//回调事件

/**
 * 事件：收到一个新连接
 * @return ture:接受连接; false:拒绝连接
 */
typedef boolean (*Fun_On_NewConnection)(ghandle node, uint32_t conn_id);

/**
 * 事件：数据到达
 */
typedef void (*Fun_OnDataArrive)(ghandle node, uint32_t conn_id, unsigned char *data, int size);

/**
 * 事件：连接已关闭
 */
typedef void (*Fun_OnCloseConnection)(ghandle node, uint32_t conn_id);

//节点（Node）
DLL_API ghandle __stdcall GNode_Create(gconfig_param_t *config,
                                       Fun_On_NewConnection onnewconn,
                                       Fun_OnDataArrive ondata,
                                       Fun_OnCloseConnection oncloseconn);

DLL_API gstate __stdcall GNode_Destroy(ghandle node);


/**
 *
 * @param node          Node(结点)句柄
 * @param remote_gid    对端Gid
 * @param remote_sid    对端Sid
 * @param transfer_mode 传输模式：直传或转发
 * @return Tunnel(通道)句柄
 */
DLL_API int __stdcall
GNode_OpenGTunnel(ghandle node, uint32_t remote_gid, uint32_t remote_sid, transfer_mode_t transfer_mode);


DLL_API gstate __stdcall GNode_CloseGTunnel(ghandle node, int tunnel_id);

/**
 * @return 当前时间戳 （ms）
 */
DLL_API uint64_t __stdcall Gip_GetTimestamp();

/**
 * 加载配置文件中信息到 gnode_config_t 结构体对象中
 * 配置文件示例:
 * #结点GID
 * gid = 22
 * #结点SID
 * gsubid = 0
 * #结点私钥
 * private_key_path = ca/22_0.key
 * #私钥密码
 * private_key_pwd = gip100
 * #结点监听的网卡IP地址
 * listen_ip=0.0.0.0
 * #结点监听的端口号
 * listen_port=2222
 * #是否使用upnp, 0代表禁用
 * upnp_enable = 0
 * #手动映射ip地址
 * mapping_ip=10.240.18.17
 * #手动映射端口地址
 * mapping_port=2222
 *
 * @param path 配置文件路径
 * @param param
 * @return
 */
DLL_API int __stdcall GConfig_Load_Param(const char *path, gconfig_param_t *param);


#ifdef __cplusplus
}
#endif

#endif /* GIP_API_H_ */
