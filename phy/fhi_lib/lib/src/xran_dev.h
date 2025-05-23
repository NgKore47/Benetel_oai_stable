/******************************************************************************
*
*   Copyright (c) 2020 Intel.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*******************************************************************************/

/**
 * @brief XRAN layer O-DU|O-RU device context
 * @file xran_dev.h
 * @ingroup group_source_xran
 * @author Intel Corporation
 **/

#ifndef _XRAN_DEV_H_
#define _XRAN_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/queue.h>

#include <rte_common.h>
#include <rte_mbuf.h>
#include <rte_timer.h>

#include "xran_fh_o_du.h"
#include "xran_prach_cfg.h"
#include "xran_up_api.h"
#include "xran_cp_api.h"

#define DIV_ROUND_OFFSET(X,Y) ( X/Y + ((X%Y)?1:0) )

#define MAX_NUM_OF_XRAN_CTX          (2)
#define MAX_CB_TIMER_CTX             (10*MAX_NUM_OF_XRAN_CTX)
#define MAX_TTI_TO_PHY_TIMER         (10)
#define XranIncrementCtx(ctx)                             ((ctx >= (MAX_NUM_OF_XRAN_CTX-1)) ? 0 : (ctx+1))
#define XranDecrementCtx(ctx)                             ((ctx == 0) ? (MAX_NUM_OF_XRAN_CTX-1) : (ctx-1))

#define MAX_NUM_OF_DPDK_TIMERS       (10)
#define DpdkTimerIncrementCtx(ctx)           ((ctx >= (MAX_NUM_OF_DPDK_TIMERS-1)) ? 0 : (ctx+1))
#define DpdkTimerDecrementCtx(ctx)           ((ctx == 0) ? (MAX_NUM_OF_DPDK_TIMERS-1) : (ctx-1))

enum xran_job_type_id {
    XRAN_JOB_TYPE_OTA_CB   = 0,
    XRAN_JOB_TYPE_CP_DL    = 1,
    XRAN_JOB_TYPE_CP_UL    = 2,
    XRAN_JOB_TYPE_DEADLINE = 3,
    XRAN_JOB_TYPE_SYM_CB   = 4,
    XRAN_JOB_TYPE_MAX
};

struct xran_timer_ctx {
    uint32_t    tti_to_process;
    uint32_t    ota_sym_idx;
    uint16_t    xran_sfn_at_sec_start;
    uint64_t    current_second;
};

#define XRAN_MAX_POOLS_PER_SECTOR_NR 8 /**< 2x(TX_OUT, RX_IN, PRACH_IN, SRS_IN) with C-plane */

typedef struct sectorHandleInfo
{
    /**< Structure that contains the information to describe the
     * instance i.e service type, virtual function, package Id etc..*/
    uint16_t nIndex;
    uint16_t nXranPort;
    /* Unique ID of an handle shared between phy layer and library */
    /**< number of antennas supported per link*/
    uint32_t nBufferPoolIndex;
    /**< Buffer poolIndex*/
    struct rte_mempool * p_bufferPool[XRAN_MAX_POOLS_PER_SECTOR_NR];
    uint32_t bufferPoolElmSz[XRAN_MAX_POOLS_PER_SECTOR_NR];
    uint32_t bufferPoolNumElm[XRAN_MAX_POOLS_PER_SECTOR_NR];

}XranSectorHandleInfo, *PXranSectorHandleInfo;

typedef void (*XranSymCallbackFn)(struct rte_timer *tim, void* arg, void *p_dev_ctx);
typedef int32_t (*tx_sym_gen_fn)(void* pHandle, uint8_t ctx_id, uint32_t tti, int32_t num_cc, int32_t num_ant, uint32_t frame_id,
    uint32_t subframe_id, uint32_t slot_id, uint32_t sym_id, enum xran_comp_hdr_type compType, enum xran_pkt_dir direction,
    uint16_t xran_port_id, PSECTION_DB_TYPE p_sec_db);

struct cb_elem_entry{
    XranSymCallbackFn pSymCallback;
    void *pSymCallbackTag;
    void *p_dev_ctx;
    LIST_ENTRY(cb_elem_entry) pointers;
};

/* Callback function to send mbuf to the ring */
typedef int (*xran_ethdi_mbuf_send_fn)(struct rte_mbuf *mb, uint16_t ethertype, uint16_t vf_id);

/*
 * manage one cell's all Ethernet frames for one DL or UL LTE subframe
 */
typedef struct {
    /* -1-this subframe is not used in current frame format
         0-this subframe can be transmitted, i.e., data is ready
          1-this subframe is waiting transmission, i.e., data is not ready
         10 - DL transmission missing deadline. When FE needs this subframe data but bValid is still 1,
        set bValid to 10.
    */
    int32_t bValid ; // when UL rx, it is subframe index.
    int32_t nSegToBeGen;
    int32_t nSegGenerated; // how many date segment are generated by DL LTE processing or received from FE
                       // -1 means that DL packet to be transmitted is not ready in BS
    int32_t nSegTransferred; // number of data segments has been transmitted or received
    struct rte_mbuf *pData[XRAN_N_MAX_BUFFER_SEGMENT]; // point to DPDK allocated memory pool
    struct xran_buffer_list sBufferList;
} BbuIoBufCtrlStruct;


#define XranIncrementJob(i)                  ((i >= (XRAN_SYM_JOB_SIZE-1)) ? 0 : (i+1))

#define XRAN_MAX_PKT_BURST_PER_SYM 96 /**< 16 layers with 6 sections each */
#define XRAN_MAX_PACKET_FRAG 9

#define MBUF_TABLE_SIZE  (2 * MAX(XRAN_MAX_PKT_BURST_PER_SYM, XRAN_MAX_PACKET_FRAG))

#define XRAN_IQ_FLOW_MAX 512 /**< Maximum flow IQ flows per XRAN port */

struct mbuf_table {
	uint16_t len;
	struct rte_mbuf *m_table[MBUF_TABLE_SIZE];
};

/** Symbols CB structure defining context of exection */
struct cb_user_per_sym_ctx {
    int32_t status;       /** status of CB - free, used */
    int32_t symb_num_req; /**< requested Symb for CB */
    int32_t sym_diff; /**< delay/advace as measured against OTA "-" - delay(later OTA) "+" - advace (earlier OTA) */
    int32_t symb_num_ota; /**< coresponding "execution time" for Symb according to type of CB */
    int32_t cb_type_id;   /**< type of CB */

    /** DPDK timer specific variables */
    int32_t user_timer_put;  /**< put index (producer)*/
    int32_t user_timer_get;  /**< get index  (consumer)*/
    struct xran_timer_ctx user_cb_timer_ctx[MAX_CB_TIMER_CTX]; /**< DPDK timer context */

    xran_callback_sym_fn       symCb;        /**< call back for Symb event */
    void                      *symCbParam;  /**< parameters of call back function */
    struct xran_sense_of_time *symCbTimeInfo;  /**< Time related infomation to this CB */
    void  *p_dev;       /**< poiter back to coresponding Device context */
};

/** Shared data at the end of an external buffer for C-plane and U-plane*/
struct xran_shared_data_ucp_t {
    struct rte_mbuf_ext_shared_info sh_data[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_MAX_SECTIONS_PER_SLOT];
};

/** Shared data at the end of an external buffer for Beam forming weights */
struct xran_shared_data_bfw_t {
    struct rte_mbuf_ext_shared_info sh_data[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_MAX_SECTIONS_PER_SLOT];
};

/** Shared data at the end of an external buffer for SRS */
struct xran_shared_data_srs_t {
    struct rte_mbuf_ext_shared_info sh_data[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANT_ARRAY_ELM_NR];
};

struct __rte_cache_aligned xran_device_ctx
{
    uint8_t sector_id;
    uint8_t xran_port_id;
    struct xran_eaxcid_config    eAxc_id_cfg;
    struct xran_fh_init          fh_init;
    struct xran_fh_config        fh_cfg;
    struct xran_prach_cp_config  PrachCPConfig;

    uint32_t enablePrach;
    uint32_t enableCP;

    int32_t DynamicSectionEna;
    int64_t offset_sec;
    int64_t offset_nsec;    //offset to GPS time calcuated based on alpha and beta
    uint32_t interval_us_local;

    uint32_t enableSrs;
    uint8_t puschMaskEnable;
    uint8_t puschMaskSlot;
    struct xran_srs_config srs_cfg; /** configuration of SRS */

    BbuIoBufCtrlStruct sFrontHaulTxBbuIoBufCtrl[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR];
    BbuIoBufCtrlStruct sFrontHaulTxPrbMapBbuIoBufCtrl[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR];
    BbuIoBufCtrlStruct sFrontHaulRxBbuIoBufCtrl[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR];
    BbuIoBufCtrlStruct sFrontHaulRxPrbMapBbuIoBufCtrl[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR];
    BbuIoBufCtrlStruct sFHPrachRxBbuIoBufCtrl[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR];
    BbuIoBufCtrlStruct sFHPrachRxBbuIoBufCtrlDecomp[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR];
    
    BbuIoBufCtrlStruct sFHSrsRxBbuIoBufCtrl[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANT_ARRAY_ELM_NR];
    BbuIoBufCtrlStruct sFHSrsRxPrbMapBbuIoBufCtrl[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANT_ARRAY_ELM_NR];

    /* buffers lists */
    struct xran_flat_buffer sFrontHaulTxBuffers[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    struct xran_flat_buffer sFrontHaulTxPrbMapBuffers[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    struct xran_flat_buffer sFrontHaulRxBuffers[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    struct xran_flat_buffer sFrontHaulRxPrbMapBuffers[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    struct xran_flat_buffer sFHPrachRxBuffers[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    struct xran_flat_buffer sFHPrachRxBuffersDecomp[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    
    struct xran_flat_buffer sFHSrsRxBuffers[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANT_ARRAY_ELM_NR][XRAN_MAX_NUM_OF_SRS_SYMBOL_PER_SLOT];
    struct xran_flat_buffer sFHSrsRxPrbMapBuffers[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANT_ARRAY_ELM_NR];

    xran_transport_callback_fn pCallback[XRAN_MAX_SECTOR_NR];
    void *pCallbackTag[XRAN_MAX_SECTOR_NR];

    xran_transport_callback_fn pPrachCallback[XRAN_MAX_SECTOR_NR];
    void *pPrachCallbackTag[XRAN_MAX_SECTOR_NR];

    xran_transport_callback_fn pSrsCallback[XRAN_MAX_SECTOR_NR];
    void *pSrsCallbackTag[XRAN_MAX_SECTOR_NR];

    LIST_HEAD(sym_cb_elem_list, cb_elem_entry) sym_cb_list_head[XRAN_NUM_OF_SYMBOL_PER_SLOT];

    int32_t sym_up; /**< when we start sym 0 of up with respect to OTA time as measured in symbols */
    int32_t sym_up_ul;

    xran_fh_tti_callback_fn ttiCb[XRAN_CB_MAX];
    void *TtiCbParam[XRAN_CB_MAX];
    uint32_t SkipTti[XRAN_CB_MAX];

    int xran2phy_mem_ready;

    int rx_packet_symb_tracker[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    int rx_packet_prach_tracker[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
    int rx_packet_callback_tracker[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR];
    int rx_packet_prach_callback_tracker[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR];
    int prach_start_symbol[XRAN_MAX_SECTOR_NR];
    int prach_last_symbol[XRAN_MAX_SECTOR_NR];

    int phy_tti_cb_done;

    struct rte_mempool *direct_pool;
    struct rte_mempool *indirect_pool;

    struct xran_common_counters fh_counters;

    xran_ethdi_mbuf_send_fn send_cpmbuf2ring;   /**< callback to send mbufs of C-Plane packets to the ring */
    xran_ethdi_mbuf_send_fn send_upmbuf2ring;   /**< callback to send mbufs of U-Plane packets to the ring */

    struct xran_timer_ctx timer_ctx[MAX_NUM_OF_XRAN_CTX];
    struct xran_timer_ctx cb_timer_ctx[MAX_CB_TIMER_CTX];

    struct rte_timer tti_to_phy_timer[MAX_TTI_TO_PHY_TIMER];
    struct rte_timer sym_timer;
    struct rte_timer dpdk_timer[MAX_NUM_OF_DPDK_TIMERS];

    uint16_t map2vf[2][XRAN_COMPONENT_CARRIERS_MAX][XRAN_MAX_ANTENNA_NR*2 + XRAN_MAX_ANT_ARRAY_ELM_NR][XRAN_VF_MAX];

    int32_t ctx;

    int timer_put;

    struct cb_user_per_sym_ctx symCbCtx[XRAN_NUM_OF_SYMBOL_PER_SLOT][XRAN_CB_SYM_MAX];

    volatile int32_t timing_source_thread_running;

    struct rte_mbuf *to_free_mbuf[XRAN_N_FE_BUF_LEN][XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT][XRAN_MAX_SECTIONS_PER_SLOT];

    tx_sym_gen_fn tx_sym_gen_func;

    int32_t job2wrk_id[XRAN_JOB_TYPE_MAX]; /** mapping of HI prio Job to worker core */

    struct xran_shared_data_ucp_t share_data;
    struct xran_shared_data_ucp_t cp_share_data;
    struct xran_shared_data_bfw_t bfw_share_data;
    struct xran_shared_data_srs_t srs_share_data;

    struct rte_flow *p_iq_flow[XRAN_IQ_FLOW_MAX];
    uint32_t iq_flow_cnt;  /**< number of IQ flows configured */
};

struct xran_eaxcid_config *xran_get_conf_eAxC(void *pHandle);
uint8_t xran_get_conf_prach_scs(void *pHandle);
uint8_t xran_get_conf_fftsize(void *pHandle);
uint8_t xran_get_conf_numerology(void *pHandle);
uint8_t xran_get_conf_iqwidth_prach(void *pHandle);
uint8_t xran_get_conf_compmethod_prach(void *pHandle);
uint8_t xran_get_conf_num_bfweights(void *pHandle);
uint8_t xran_get_num_cc(void *pHandle);
uint8_t xran_get_num_eAxc(void *pHandle);
uint8_t xran_get_num_eAxcUl(void *pHandle);
uint8_t xran_get_num_ant_elm(void *pHandle);
enum xran_category xran_get_ru_category(void *pHandle);
uint16_t xran_get_beamid(void *pHandle, uint8_t dir, uint8_t cc_id, uint8_t ant_id, uint8_t slot_id);

int32_t xran_dev_create_ctx(uint32_t xran_ports_num);
int32_t xran_dev_destroy_ctx();
struct xran_device_ctx *xran_dev_get_ctx(void);
struct xran_device_ctx *xran_dev_get_ctx_by_id(uint32_t xran_port_id);
struct xran_device_ctx **xran_dev_get_ctx_addr(void);

struct cb_elem_entry *xran_create_cb(XranSymCallbackFn cb_fn, void *cb_data, void* p_dev_ctx);
int32_t xran_destroy_cb(struct cb_elem_entry * cb_elm);

uint16_t xran_map_ecpriRtcid_to_vf(struct xran_device_ctx *p_dev_ctx, int32_t dir, int32_t cc_id, int32_t ru_port_id);
uint16_t xran_map_ecpriPcid_to_vf(struct xran_device_ctx *p_dev_ctx,  int32_t dir, int32_t cc_id, int32_t ru_port_id);

uint16_t xran_set_map_ecpriRtcid_to_vf(struct xran_device_ctx *p_dev_ctx, int32_t dir, int32_t cc_id, int32_t ru_port_id, uint16_t vf_id);
uint16_t xran_set_map_ecpriPcid_to_vf(struct xran_device_ctx *p_dev_ctx,  int32_t dir, int32_t cc_id, int32_t ru_port_id, uint16_t vf_id);

int32_t xran_init_vfs_mapping(void *pHandle);
int32_t xran_init_vf_rxq_to_pcid_mapping(void *pHandle);

static inline int8_t xran_dev_ctx_get_port_id(void * handle)
{
    struct xran_device_ctx * p_dev_ctx  = (struct xran_device_ctx *)handle;
    if(p_dev_ctx)
        return (int8_t)p_dev_ctx->xran_port_id;
    else
        return -1;
};

#ifdef __cplusplus
}
#endif

#endif

