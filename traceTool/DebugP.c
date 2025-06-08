/**
 * @file DebugP.c 
 * @brief For control servo, Debug protocol
 * @author      XiaoyuHe     
 * @version     V0.0.0
 * @attention
 *  This material is the exclusive property of KUKA Robot Group                
 *  Except as expressly permitted by separate agreement, this material may only
 *  be used by members of the development department of KUKA Robot Group for   
 *  internal development purposes of KUKA Robot Group.
 *  Copyright (C) 2021                                                         
 *  KUKA Robot Group. All Rights Reserved.                                     
 * @htmlonly 
 * <span style="font-weight: bold">History</span> 
 * @endhtmlonly 
 * Version|Auther|Date|Describe
 * -------|-------|-------|-------- 
 * V0.0.0|XiaoyuHe     |12/29/2021      |Module Initial                
 */

/******* I N C L U D E - F I L E S *******************************************/
#include "DebugP_Extern.h"
#include "DebugP_Config.h"
#include "CRC_CRC.h"
#include "trace.h"

/*______ L O C A L - T Y P E S _______________________________________________*/
typedef enum
{
    DebugP_ClientId_RamRead,
    DebugP_ClientId_RamWrite,
    Unused1,
    Unused2,
    Unused3,
    Unused4,
    Unused5,
    DebugP_ClientId_Trace,
} DebugP_ClientId_t;

typedef enum
{
    DebugP_TraceCmd_Clr,
    DebugP_TraceCmd_SetChannel,
    DebugP_TraceCmd_SetTrigger,
    DebugP_TraceCmd_SetSampleConfig,
    DebugP_TraceCmd_Start,
    DebugP_TraceCmd_StartPolling,
    DebugP_TraceCmd_GetStatus,
    DebugP_TraceCmd_GetTriggerData,
} DebugP_TraceCmd_t;

typedef enum
{
    sci_scan_Init,
    sci_scan_head1,
    sci_scan_head2,
    sci_scan_len,
    sci_scan_body,
    sci_scan_finish,
} sci_ScanStatus_t;

typedef struct
{
    uint8_t cmdBuff[32];
    uint16_t curIndex;
    sci_ScanStatus_t status;
} sci_scan_t;


/******* L O C A L - D E F I N E S *******************************************/
#define TRACE_CANMSG_ID 0x3a8
#define DebugP_txId_Client 0x3a1
#define DebugP_TRACE_SEND_UNIT 4

/******* L O C A L - D A T A *************************************************/
uint8_t uart_rx_buff[32];
sci_scan_t Usbc_CmdScan;
static uint8_t DebugP_USBSendBuff[32];
static DebugP_Frame_t DebugP_RxFrame;
static DebugP_Frame_t DebugP_TxFrame;
static uint8_t DebugP_TriggerOp = 0;

/******* L O C A L - F U N C T I O N S - P R O T O T Y P E S ***************/

int8_t DebugP_ReceiveFrame(DebugP_Frame_t* frame);
int16_t DebugP_HandleFrame(DebugP_Frame_t* RxFrame, DebugP_Frame_t* TxFrame);
int8_t DebugP_SendFrame(DebugP_Frame_t* frame);

static inline uint32_t DebugP_MakeRamAddr(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3);
static inline uint16_t DebugP_MakeU16(uint8_t a0, uint8_t a1);

int8_t DebugP_SendTracePollingFrame(uint8_t* buff, uint16_t len);
int8_t DebugP_SendTraceTriggerBuff(uint8_t* buff, uint16_t len, uint16_t* pleft);

/******* G L O B A L - F U N C T I O N S *************************************/
void DebugP_Init(void)
{
    ;
}

void DebugP_Main(void)
{
    if (DebugP_ReceiveFrame(&DebugP_RxFrame)) //1濡炪伇鍐炬eceive a Frame. bugp_RxFrame ,0 means a frame received */
    {
        if (DebugP_HandleFrame(&DebugP_RxFrame, &DebugP_TxFrame) >= 0) //2Handle a frame
        {
            DebugP_SendFrame(&DebugP_TxFrame); //3 Send a frame
        }
    }
}


/*______ L O C A L - F U N C T I O N S _______________________________________*/

/* 关键流程说明：
状态机设计：

sci_scan_head1 → sci_scan_head2 → sci_scan_len → sci_scan_body

帧格式：0xAA 0x55 [长度] [ID低] [ID高] [数据...]

帧结构：

plaintext
| 0xAA | 0x55 | 长度N | ID低 | ID高 | 数据1 | ... | 数据N-3 |
长度N包含自身及后续所有字节（ID+数据）

CRC处理：

代码中强制跳过校验 (if (1U))，实际应使用 sci_CalcCrc() 验证

输出帧：

this->len = 原始长度 - 3（去除非数据部分）

this->id = 组合两个字节的ID

this->buff = 原始数据中的有效载荷

错误处理：

任何阶段出错都会重置状态机到 sci_scan_head1

长度无效/CRC失败会清空缓冲区 */
int8_t DebugP_ReceiveFrame(DebugP_Frame_t* this)
{
    /* 
     * 功能：逐字节解析UART接收的数据，保存到 Usbc_CmdScan_t 结构体，
     *       其他模块可通过 USBC_GetLastFrame 获取完整帧
     */
    uint8_t i;          // 循环计数器
    uint8_t ret = 0;    // 返回值（0=未完成帧，1=成功接收帧）

    /* 持续尝试从UART读取1字节数据（非阻塞模式）*/
    while ((Uart_AsyncReceive(0, uart_rx_buff, 1) > 0))
    {
        // 获取当前接收的字节
        uint8_t OneByte = uart_rx_buff[0];
        
        // 根据当前解析状态处理字节
        switch (Usbc_CmdScan.status)
        {
        case sci_scan_Init:  // 初始状态
            Usbc_CmdScan.status = sci_scan_head1;  // 直接跳转到帧头1检测
            break;  // 注意：此处缺少break会导致穿透，但状态立即被覆盖，实际影响不大

        case sci_scan_head1:  // 检测帧头第一个字节
            if (OneByte == 0xaa) {       // 匹配到帧头1 (0xAA)
                Usbc_CmdScan.status = sci_scan_head2;  // 进入帧头2检测状态
            }
            break;  // 未匹配则保持sci_scan_head1状态

        case sci_scan_head2:  // 检测帧头第二个字节
            if (OneByte == 0x55) {       // 匹配到帧头2 (0x55)
                Usbc_CmdScan.status = sci_scan_len;   // 进入长度检测状态
                Usbc_CmdScan.cmdBuff[0] = 0;          // 重置长度字段
                Usbc_CmdScan.curIndex = 0;            // 重置缓冲区索引
            } else {
                Usbc_CmdScan.status = sci_scan_head1;  // 不匹配则退回帧头1检测
            }
            break;

        case sci_scan_len:  // 检测长度字节
            if (OneByte > 2 && OneByte < 200) {  // 长度需在有效范围(3~199)
                Usbc_CmdScan.cmdBuff[0] = OneByte;  // 存储长度到缓冲区[0]
                Usbc_CmdScan.status = sci_scan_body; // 进入数据体接收状态
                Usbc_CmdScan.curIndex++;             // 索引递增（指向下一存储位）
            } else {
                Usbc_CmdScan.status = sci_scan_head1;  // 无效长度则重置
            }
            break;

        case sci_scan_body:  // 接收数据体
            // 将字节存入缓冲区当前索引位置
            Usbc_CmdScan.cmdBuff[Usbc_CmdScan.curIndex] = OneByte;
            
            if (Usbc_CmdScan.curIndex < Usbc_CmdScan.cmdBuff[0]) {
                // 未达到长度要求：继续接收
                Usbc_CmdScan.curIndex++;
            } else {
                // 已接收完整帧（跳过CRC校验）
                if (1U) {  // 此处强制跳过CRC校验（实际应调用sci_CalcCrc）
                    /* 解析有效帧 */
                    // 1. 设置输出帧长度（总长-3字节头）
                    this->len = Usbc_CmdScan.cmdBuff[0] - 3;
                    
                    // 2. 组合帧ID（低字节+高字节<<8）
                    this->id = Usbc_CmdScan.cmdBuff[1] | (Usbc_CmdScan.cmdBuff[2] << 8);
                    
                    // 3. 复制有效数据（跳过3字节头）
                    for (i = 0; i < this->len; i++) {
                        this->buff[i] = Usbc_CmdScan.cmdBuff[i + 3];
                    }
                    
                    // 4. 重置解析状态
                    Usbc_CmdScan.cmdBuff[0] = 0;  // 清除长度标记
                    Usbc_CmdScan.status = sci_scan_head1;  // 返回初始状态
                    
                    ret = 1U;  // 标记成功接收
                } else {
                    // CRC校验失败处理
                    Usbc_CmdScan.status = sci_scan_head1;  // 重置状态
                    Usbc_CmdScan.cmdBuff[0] = 0;           // 清除长度
                    Usbc_CmdScan.curIndex = 0;              // 重置索引
                    ret = 0U;  // 返回失败
                }
            }
            break;  // 注意：此break属于else块外的switch分支

        default:  // 未定义状态处理
            break;
        }  // switch结束
    }  // while结束
    
    return ret;  // 返回帧接收状态
}
/* 关键功能说明：
命令处理机制：

RAM读取：读取指定内存区域数据并返回

RAM写入：向指定内存区域写入数据

跟踪控制：包含7个子命令的复杂操作

跟踪子系统功能：

子命令	功能	参数
Clr	清除跟踪数据	无
SetChannel	设置跟踪通道	地址+通道号
SetSampleConfig	配置采样参数	前后采样点数+采样率
SetTrigger	设置触发器	地址+掩码+电平
Start	开始跟踪	无
StartPolling	启动轮询模式	无
GetStatus	获取状态	返回状态码+缓冲区信息
GetTriggerData	获取触发数据	需要后续发送
特殊处理标志：

buffSendFlag：用于区分立即响应和需要后续发送数据的命令

0：立即发送响应帧（大多数命令）

1：需要后续处理（如GetTriggerData） */
int16_t DebugP_HandleFrame(DebugP_Frame_t* RxFrame, DebugP_Frame_t* TxFrame)
{
    int8_t rst = -1;  // 默认返回错误状态（-1表示未处理或失败）

    // 根据接收帧的第一个字节（命令ID）进行分发
    switch (RxFrame->buff[0])
    {
    case DebugP_ClientId_RamRead:  // RAM读取命令
    {
        // 组合32位内存地址（从buff[1]到buff[4]共4字节）
        uint32_t addr = DebugP_MakeRamAddr(
            (uint32_t)RxFrame->buff[1], 
            (uint32_t)RxFrame->buff[2],
            (uint32_t)RxFrame->buff[3], 
            (uint32_t)RxFrame->buff[4]
        );
        uint8_t index;  // 循环索引

        // 构造响应帧
        TxFrame->id = RxFrame->id;  // 保持ID一致（用于请求-响应匹配）
        TxFrame->len = RxFrame->buff[5] + 2;  // 响应长度 = 请求长度 + 2（命令码+长度）
        TxFrame->buff[0] = RxFrame->buff[0];  // 原样返回命令码
        TxFrame->buff[1] = RxFrame->buff[5];  // 返回请求的读取字节数
        
        // 从指定地址读取数据
        for (index = 0; index < RxFrame->buff[5]; index++) {
            // __byte(addr, index): 从地址(addr + index)读取1字节
            TxFrame->buff[index + 2] = __byte(addr, index);  // 数据存入响应帧
        }
        
        rst = 0;  // 标记操作成功
        break;
    }
    case DebugP_ClientId_RamWrite:  // RAM写入命令
    {
        // 组合32位内存地址（同上）
        uint32_t addr = DebugP_MakeRamAddr(
            (uint32_t)RxFrame->buff[1],
            (uint32_t)RxFrame->buff[2],
            (uint32_t)RxFrame->buff[3],
            (uint32_t)RxFrame->buff[4]
        );
        uint8_t index;

        // 构造响应帧
        TxFrame->id = RxFrame->id;
        TxFrame->len = 2;  // 固定响应长度（命令码+状态）
        TxFrame->buff[0] = RxFrame->buff[0];  // 原样返回命令码
        TxFrame->buff[1] = 0x01;  // 写入成功标志（0x01成功，0x00失败）
        
        // 向指定地址写入数据
        for (index = 0; index < RxFrame->buff[5]; index++) {
            // __byte(addr, index) = ...：向地址(addr + index)写入1字节
            __byte(addr, index) = RxFrame->buff[6 + index];  // 数据从接收帧获取
        }
        
        rst = 0;  // 标记操作成功
        break;
    }
    case DebugP_ClientId_Trace:  // 跟踪命令
    {
        // 基础响应设置
        TxFrame->id = RxFrame->id;
        TxFrame->buff[0] = RxFrame->buff[0];  // 原样返回命令码
        
        uint8_t buffSendFlag = 0;  // 数据发送标志（0=立即响应，1=需要后续发送）
        
        // 根据跟踪子命令处理（第二个字节为子命令类型）
        switch (RxFrame->buff[1])
        {
        case DebugP_TraceCmd_Clr:  // 清除跟踪数据
            TRACE_Clearall();  // 调用清除函数
            TxFrame->buff[2] = 255;  // 操作成功标志（任意非0值）
            TxFrame->len = 3;         // 响应长度（命令码+子命令+状态）
            buffSendFlag = 0;         // 无额外数据发送
            break;
        
        case DebugP_TraceCmd_SetChannel:  // 设置跟踪通道
        {
            // 组合通道地址（从buff[2]到buff[5]共4字节）
            uint32_t addr = DebugP_MakeRamAddr(
                RxFrame->buff[2], RxFrame->buff[3], 
                RxFrame->buff[4], RxFrame->buff[5]
            );
            // 设置通道（地址+通道号）
            TRACE_SetChannel((uint8_t*)addr, RxFrame->buff[6]);
            TxFrame->buff[2] = 255;  // 操作成功标志
            TxFrame->len = 3;        // 响应长度
            buffSendFlag = 0;
            break;
        }
        case DebugP_TraceCmd_SetSampleConfig:  // 设置采样配置
        {
            // 组合预触发计数（2字节）和后触发计数（2字节）
            uint16_t preCount = DebugP_MakeU16(RxFrame->buff[2], RxFrame->buff[3]);
            uint16_t postCount = DebugP_MakeU16(RxFrame->buff[4], RxFrame->buff[5]);
            
            // 配置跟踪参数
            TRACE_SetTraceCount(preCount, postCount);  // 设置触发前后采样点数
            TRACE_SetSampleRate(RxFrame->buff[6]);    // 设置采样率
            DebugP_TriggerOp = RxFrame->buff[7];      // 保存触发操作类型
            
            TxFrame->buff[2] = 255;  // 操作成功标志
            TxFrame->len = 3;        // 响应长度
            buffSendFlag = 0;
            break;
        }
        case DebugP_TraceCmd_SetTrigger:  // 设置触发器
        {
            // 组合触发地址（4字节）
            uint32_t addr = DebugP_MakeRamAddr(
                RxFrame->buff[2], RxFrame->buff[3],
                RxFrame->buff[4], RxFrame->buff[5]
            );
            // 组合触发掩码（2字节）和触发电平（2字节）
            uint16_t mask = DebugP_MakeU16(RxFrame->buff[6], RxFrame->buff[7]);
            int16_t level = (int16_t)DebugP_MakeU16(RxFrame->buff[8], RxFrame->buff[9]);
            
            // 设置触发器参数
            TRACE_SetTrigger((int16_t*)addr, mask, level, DebugP_TriggerOp);
            TxFrame->buff[2] = 255;  // 操作成功标志
            TxFrame->len = 3;        // 响应长度
            buffSendFlag = 0;
            break;
        }
        case DebugP_TraceCmd_Start:  // 开始跟踪
            TRACE_Start();           // 启动跟踪
            TxFrame->buff[2] = 255;  // 操作成功标志
            TxFrame->len = 3;        // 响应长度
            buffSendFlag = 0;
            break;
        
        case DebugP_TraceCmd_StartPolling:  // 开始轮询模式
            TRACE_AutoPolling();            // 启动自动轮询
            TxFrame->buff[2] = 255;         // 操作成功标志
            TxFrame->len = 3;               // 响应长度
            buffSendFlag = 0;
            break;
        
        case DebugP_TraceCmd_GetStatus:  // 获取跟踪状态
        {
            uint8_t* p1, *p2;  // 数据缓冲区指针
            uint16_t l1, l2;   // 数据长度
            
            TxFrame->buff[2] = TRACE_GetStatus();  // 获取状态码
            
            // 获取两个数据扇区信息
            TRACE_GetDataSector(&p1, &l1, &p2, &l2);
            
            // 填充响应数据（小端序）
            TxFrame->buff[3] = l1 & 0xff;  // 长度1低字节
            TxFrame->buff[4] = l1 >> 8;    // 长度1高字节
            TxFrame->buff[5] = l2 & 0xff;  // 长度2低字节
            TxFrame->buff[6] = l2 >> 8;    // 长度2高字节
            
            TxFrame->len = 7;  // 响应长度
            buffSendFlag = 0;
            break;
        }
        case DebugP_TraceCmd_GetTriggerData:  // 获取触发数据
            TRACE_GetAllBuff();  // 准备所有缓冲区数据
            buffSendFlag = 1;    // 标记需要后续发送（非立即响应）
            break;
        }  // 子命令switch结束

        // 根据buffSendFlag决定是否立即发送响应
        if (buffSendFlag == 0)
        {
            // 填充响应帧头部
            TxFrame->id = RxFrame->id;            // 保持ID一致
            TxFrame->buff[0] = RxFrame->buff[0];  // 主命令码
            TxFrame->buff[1] = RxFrame->buff[1];  // 子命令码
            rst = 0;  // 标记操作成功
        }
        else
        {
            // 需要后续发送数据（如GetTriggerData）
            rst = -1;  // 标记不立即发送响应
        }
        break;
    }  // DebugP_ClientId_Trace结束
    
    default:  // 未识别的命令
        break;
    }  // 主命令switch结束

    return rst;  // 返回操作结果（0=成功并发送响应，-1=未处理或需要后续发送）
}
/* 关键结构解析：
帧结构：

text
| 0xAA | 0x55 | 长度 | ID低 | ID高 | 数据... | CRC |
|------|------|------|------|------|----------|-----|
| 2字节| 1字节| 1字节| 1字节| n字节| 1字节    |
长度计算：

长度字段 = ID(2字节) + 数据长度(n) + CRC(1字节) = n+3

总帧长 = SOF(2) + 长度(1) + (n+3) = 6+n

CRC范围：

从索引2（长度字段）开始，连续len+3个字节（包含长度、ID和数据）

设计特点：
双字节SOF：0xAA55 提高帧同步可靠性

小端ID：兼容小端序处理器

动态长度：支持可变长度数据帧

CRC校验：替代原EOF设计，增强数据完整性检查

异步发送：非阻塞式传输提高系统效率

注意：实际应用中需确保 DebugP_USBSendBuff 缓冲区大小足够容纳 6 + frame->len 的数据，否则可能引发内存越界错误。 */
// 发送调试帧函数
// 参数：frame - 指向调试帧结构体的指针
// 返回值：int8_t类型，表示异步发送操作的状态（通常0=成功，负值=错误）
int8_t DebugP_SendFrame(DebugP_Frame_t* frame)
{
    uint8_t i;  // 声明循环计数器
    
    // 帧起始标志（SOF）
    DebugP_USBSendBuff[0] = 0xaa;             // 帧头第一个字节（固定值0xAA）
    DebugP_USBSendBuff[1] = 0x55;             // 帧头第二个字节（固定值0x55）
    
    // 长度字段 = 数据长度 + 3（包含从设备ID低8位到CRC的所有字段）
    DebugP_USBSendBuff[2] = frame->len + 3;   // 总长度字段（ID占2字节 + CRC占1字节）
    
    // 从设备ID（小端序存储）
    DebugP_USBSendBuff[3] = frame->id & 0xff; // ID低字节（位与操作取低8位）
    DebugP_USBSendBuff[4] = frame->id >> 8;   // ID高字节（右移8位取高8位）

    // 数据载荷复制
    for (i = 0; i < frame->len; i++)          // 遍历数据缓冲区
    {
        DebugP_USBSendBuff[5 + i] = frame->buff[i];  // 将源数据复制到发送缓冲区偏移位置5
    }
    
    // CRC校验码计算与填充
    // 注释掉的EOF（0x50）被CRC取代
    DebugP_USBSendBuff[5 + i] = sci_CalcCrc(&DebugP_USBSendBuff[2], frame->len + 3); 
    // 参数1：从长度字段(索引2)开始的指针
    // 参数2：计算长度 = 长度字段(1) + ID(2) + 数据(n) = len+3
    
    // 异步发送完整帧
    return Uart_AsyncSend(0, DebugP_USBSendBuff, 6 + frame->len);
    // 参数1：UART端口号（0表示第一个UART）
    // 参数2：发送缓冲区起始地址
    // 参数3：总帧长 = SOF(2) + 长度(1) + ID(2) + 数据(n) + CRC(1) = 6+n
}

static inline uint32_t DebugP_MakeRamAddr(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3)
{
    uint32_t addr = (uint32_t) addr0 | (((uint32_t) addr1) << 8) | (((uint32_t) addr2) << 16) | (((uint32_t) addr3) << 24);
    return addr;
}

static inline uint16_t DebugP_MakeU16(uint8_t a0, uint8_t a1)
{
    return (uint16_t) a0 | (((uint16_t) a1) << 8);
}


int8_t DebugP_SendTraceTriggerBuff(uint8_t* buff, uint16_t len, uint16_t* pLeft)
{
    uint8_t* pwork = &buff[len - *pLeft];
    int8_t status = 0;

    if (*pLeft <= DebugP_TRACE_SEND_UNIT)
    {
        DebugP_SendTracePollingFrame(pwork, *pLeft);
        *pLeft = 0;
    }
    else
    {
        DebugP_SendTracePollingFrame(pwork, DebugP_TRACE_SEND_UNIT);
        *pLeft = *pLeft - DebugP_TRACE_SEND_UNIT;
    }

    return status;
}


int8_t DebugP_SendTracePollingFrame(uint8_t* buff, uint16_t len)
{
    uint8_t i;
    uint16_t buffLen = len;
    DebugP_USBSendBuff[2] = buffLen + 3;
    DebugP_USBSendBuff[3] = TRACE_CANMSG_ID & 0xff;
    DebugP_USBSendBuff[4] = TRACE_CANMSG_ID >> 8;
    for (i = 0; i < buffLen; i++)
    {
        DebugP_USBSendBuff[5 + i] = buff[i];
    }
    // DebugP_USBSendBuff[5 + i] = 0x50;
    // DebugP_USBSendBuff[5 + i] = sci_CalcCrc(&DebugP_USBSendBuff[3], buffLen + 2);
    DebugP_USBSendBuff[5 + i] = sci_CalcCrc(&DebugP_USBSendBuff[2], buffLen + 3);

    return Uart_AsyncSend(0, DebugP_USBSendBuff, 6 + buffLen);
}