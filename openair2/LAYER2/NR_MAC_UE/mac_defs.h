/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/* \file mac_defs.h
 * \brief MAC data structures and constants
 * \author R. Knopp, K.H. HSU
 * \date 2018
 * \version 0.1
 * \company Eurecom / NTUST
 * \email: knopp@eurecom.fr, kai-hsiang.hsu@eurecom.fr
 * \note
 * \warning
 */

#ifndef __LAYER2_NR_MAC_DEFS_H__
#define __LAYER2_NR_MAC_DEFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/platform_types.h"

/* IF */
#include "NR_IF_Module.h"
#include "fapi_nr_ue_interface.h"

/* MAC */
#include "LAYER2/NR_MAC_COMMON/nr_mac.h"
#include "LAYER2/NR_MAC_COMMON/nr_mac_common.h"
#include "NR_MAC_COMMON/nr_mac_extern.h"
#include "mac_defs_sl.h"

/* RRC */
#include "NR_DRX-Config.h"
#include "NR_SchedulingRequestConfig.h"
#include "NR_BSR-Config.h"
#include "NR_TAG-Config.h"
#include "NR_PHR-Config.h"
#include "NR_RNTI-Value.h"
#include "NR_MIB.h"
#include "NR_MAC-CellGroupConfig.h"
#include "NR_PhysicalCellGroupConfig.h"
#include "NR_CellGroupConfig.h"
#include "NR_ServingCellConfig.h"
#include "NR_MeasConfig.h"
#include "NR_ServingCellConfigCommonSIB.h"


// ==========
// NR UE defs
// ==========

#define NR_BSR_TRIGGER_NONE      (0) /* No BSR Trigger */
#define NR_BSR_TRIGGER_REGULAR   (1) /* For Regular and ReTxBSR Expiry Triggers */
#define NR_BSR_TRIGGER_PERIODIC (2) /* For BSR Periodic Timer Expiry Trigger */

#define NR_INVALID_LCGID (NR_MAX_NUM_LCGID)

#define MAX_NUM_BWP_UE 5
#define NR_MAX_SR_ID 8  // SchedulingRequestId ::= INTEGER (0..7)

/*!\brief value for indicating BSR Timer is not running */
#define NR_MAC_UE_BSR_TIMER_NOT_RUNNING   (0xFFFF)

// ================================================
// SSB to RO mapping private defines and structures
// ================================================

#define MAX_NB_PRACH_CONF_PERIOD_IN_ASSOCIATION_PERIOD (16) // Maximum association period is 16
#define MAX_NB_PRACH_CONF_PERIOD_IN_ASSOCIATION_PATTERN_PERIOD (16) // Max association pattern period is 160ms and minimum PRACH configuration period is 10ms
#define MAX_NB_ASSOCIATION_PERIOD_IN_ASSOCIATION_PATTERN_PERIOD (16) // Max nb of association periods in an association pattern period of 160ms
#define MAX_NB_FRAME_IN_PRACH_CONF_PERIOD (16) // Max PRACH configuration period is 160ms and frame is 10ms
#define MAX_NB_SLOT_IN_FRAME (160) // Max number of slots in a frame (@ SCS 240kHz = 160)
#define MAX_NB_FRAME_IN_ASSOCIATION_PATTERN_PERIOD (16) // Maximum number of frames in the maximum association pattern period
#define MAX_NB_SSB (64) // Maximum number of possible SSB indexes
#define MAX_RO_PER_SSB (8) // Maximum number of consecutive ROs that can be mapped to an SSB according to the ssb_per_RACH config

// Maximum number of ROs that can be mapped to an SSB in an association pattern
// This is to reserve enough elements in the SSBs list for each mapped ROs for a single SSB
// An arbitrary maximum number is chosen to be safe: maximum number of slots in an association pattern * maximum number of ROs in a slot
#define MAX_NB_RO_PER_SSB_IN_ASSOCIATION_PATTERN (MAX_TDM*MAX_FDM*MAX_NB_SLOT_IN_FRAME*MAX_NB_FRAME_IN_ASSOCIATION_PATTERN_PERIOD)

// ===============
// DCI fields defs
// ===============

#define NBR_NR_FORMATS                   8     // The number of formats is 8 (0_0, 0_1, 1_0, 1_1, 2_0, 2_1, 2_2, 2_3)
#define NBR_NR_DCI_FIELDS               56    // The number of different dci fields defined in TS 38.212 subclause 7.3.1

#define IDENTIFIER_DCI_FORMATS           0
#define CARRIER_IND                      1
#define SUL_IND_0_1                      2
#define SLOT_FORMAT_IND                  3
#define PRE_EMPTION_IND                  4
#define BLOCK_NUMBER                     5
#define CLOSE_LOOP_IND                   6
#define BANDWIDTH_PART_IND               7
#define SHORT_MESSAGE_IND                8
#define SHORT_MESSAGES                   9
#define FREQ_DOM_RESOURCE_ASSIGNMENT_UL 10
#define FREQ_DOM_RESOURCE_ASSIGNMENT_DL 11
#define TIME_DOM_RESOURCE_ASSIGNMENT    12
#define VRB_TO_PRB_MAPPING              13
#define PRB_BUNDLING_SIZE_IND           14
#define RATE_MATCHING_IND               15
#define ZP_CSI_RS_TRIGGER               16
#define FREQ_HOPPING_FLAG               17
#define TB1_MCS                         18
#define TB1_NDI                         19
#define TB1_RV                          20
#define TB2_MCS                         21
#define TB2_NDI                         22
#define TB2_RV                          23
#define MCS                             24
#define NDI                             25
#define RV                              26
#define HARQ_PROCESS_NUMBER             27
#define DAI_                            28
#define FIRST_DAI                       29
#define SECOND_DAI                      30
#define TB_SCALING                      31
#define TPC_PUSCH                       32
#define TPC_PUCCH                       33
#define PUCCH_RESOURCE_IND              34
#define PDSCH_TO_HARQ_FEEDBACK_TIME_IND 35
#define SRS_RESOURCE_IND                36
#define PRECOD_NBR_LAYERS               37
#define ANTENNA_PORTS                   38
#define TCI                             39
#define SRS_REQUEST                     40
#define TPC_CMD                         41
#define CSI_REQUEST                     42
#define CBGTI                           43
#define CBGFI                           44
#define PTRS_DMRS                       45
#define BETA_OFFSET_IND                 46
#define DMRS_SEQ_INI                    47
#define UL_SCH_IND                      48
#define PADDING_NR_DCI                  49
#define SUL_IND_0_0                     50
#define RA_PREAMBLE_INDEX               51
#define SUL_IND_1_0                     52
#define SS_PBCH_INDEX                   53
#define PRACH_MASK_INDEX                54
#define RESERVED_NR_DCI                 55

// Define the UE L2 states with X-Macro
#define NR_UE_L2_STATES \
  UE_STATE(UE_NOT_SYNC) \
  UE_STATE(UE_SYNC) \
  UE_STATE(UE_PERFORMING_RA) \
  UE_STATE(UE_CONNECTED) \
  UE_STATE(UE_DETACHING)

typedef enum {
  phr_cause_prohibit_timer = 0,
  phr_cause_periodic_timer,
  phr_cause_phr_config,
} NR_UE_PHR_Reporting_cause_t;

/*!\brief UE layer 2 status */
typedef enum {
#define UE_STATE(state) state,
  NR_UE_L2_STATES
#undef UE_STATE
} NR_UE_L2_STATE_t;

typedef struct {
  pucch_format_nr_t format;
  uint8_t startingSymbolIndex;
  uint8_t nrofSymbols;
  uint16_t PRB_offset;
  uint8_t nb_CS_indexes;
  uint8_t initial_CS_indexes[MAX_NB_CYCLIC_SHIFT];
} initial_pucch_resource_t;

typedef enum {
  GO_TO_IDLE,
  DETACH,
  T300_EXPIRY,
  RE_ESTABLISHMENT
} NR_UE_MAC_reset_cause_t;

typedef struct {
  // after multiplexing buffer remain for each lcid
  int32_t LCID_buffer_remain;
  // logical channel group id of this LCID
  long LCGID;
  // Bj bucket usage per lcid
  int32_t Bj;
  NR_timer_t Bj_timer;
} NR_LC_SCHEDULING_INFO;

typedef struct {
  bool active_SR_ID;
  /// SR pending as defined in 38.321
  bool pending;
  /// SR_COUNTER as defined in 38.321
  uint32_t counter;
  /// sr ProhibitTimer
  NR_timer_t prohibitTimer;
  // Maximum number of SR transmissions
  uint32_t maxTransmissions;
} nr_sr_info_t;

typedef struct {
  bool is_configured;
  ///timer before triggering a periodic PHR
  NR_timer_t periodicPHR_Timer;
  ///timer before triggering a prohibit PHR
  NR_timer_t prohibitPHR_Timer;
  ///DL Pathloss change value
  uint16_t PathlossLastValue;
  ///number of subframe before triggering a periodic PHR
  int16_t periodicPHR_SF;
  ///number of subframe before triggering a prohibit PHR
  int16_t prohibitPHR_SF;
  ///DL Pathloss Change in db
  uint16_t PathlossChange_db;
  int phr_reporting;
  bool was_mac_reset;
} nr_phr_info_t;

// LTE structure, might need to be adapted for NR
typedef struct {
  // lcs scheduling info
  NR_LC_SCHEDULING_INFO lc_sched_info[NR_MAX_NUM_LCID];
  // SR INFO
  nr_sr_info_t sr_info[NR_MAX_SR_ID];
  /// BSR report flag management
  uint8_t BSR_reporting_active;
  // LCID triggering BSR
  NR_LogicalChannelIdentity_t regularBSR_trigger_lcid;
  // logicalChannelSR-DelayTimer
  NR_timer_t sr_DelayTimer;
  /// retxBSR-Timer
  NR_timer_t retxBSR_Timer;
  /// periodicBSR-Timer
  NR_timer_t periodicBSR_Timer;

  nr_phr_info_t phr_info;
} NR_UE_SCHEDULING_INFO;

typedef enum {
  nrRA_UE_IDLE,
  nrRA_GENERATE_PREAMBLE,
  nrRA_WAIT_RAR,
  nrRA_WAIT_MSGB,
  nrRA_WAIT_CONTENTION_RESOLUTION,
  nrRA_SUCCEEDED,
  nrRA_FAILED,
} nrRA_UE_state_t;

static const char *const nrra_ue_text[] =
    {"UE_IDLE", "GENERATE_PREAMBLE", "WAIT_RAR", "WAIT_MSGB", "WAIT_CONTENTION_RESOLUTION", "RA_SUCCEEDED", "RA_FAILED"};

typedef struct {
  /// PRACH format retrieved from prach_ConfigIndex
  uint16_t prach_format;
  /// Preamble Tx Counter
  uint8_t RA_PREAMBLE_TRANSMISSION_COUNTER;
  /// Preamble Power Ramping Counter
  uint8_t RA_PREAMBLE_POWER_RAMPING_COUNTER;
  /// 2-step RA power offset
  int POWER_OFFSET_2STEP_RA;
  /// Target received power at gNB. Baseline is range -202..-60 dBm. Depends on delta preamble, power ramping counter and step.
  int ra_PREAMBLE_RECEIVED_TARGET_POWER;
  /// PRACH index for TDD (0 ... 6) depending on TDD configuration and prachConfigIndex
  uint8_t ra_TDD_map_index;
  /// RA Preamble Power Ramping Step in dB
  uint32_t RA_PREAMBLE_POWER_RAMPING_STEP;
  ///
  uint8_t RA_PREAMBLE_BACKOFF;
  ///
  uint8_t RA_SCALING_FACTOR_BI;
  /// Indicating whether it is 2-step or 4-step RA
  nr_ra_type_t RA_TYPE;
  /// UE configured maximum output power
  int RA_PCMAX;
} NR_PRACH_RESOURCES_t;

typedef struct {

  // pointer to RACH config dedicated
  NR_RACH_ConfigDedicated_t *rach_ConfigDedicated;
  /// state of RA procedure
  nrRA_UE_state_t ra_state;
  /// RA contention type
  uint8_t cfra;
  /// RA type
  nr_ra_type_t ra_type;
  /// RA rx frame offset: compensate RA rx offset introduced by OAI gNB.
  uint8_t RA_offset;
  /// MsgB SuccessRAR MAC subheader
  int8_t MsgB_R;
  int8_t MsgB_CH_ACESS_CPEXT;
  uint8_t MsgB_TPC;
  int8_t MsgB_HARQ_FTI;
  uint16_t timing_advance_command;
  int8_t PUCCH_RI;
  /// RA-rnti
  uint16_t ra_rnti;
  /// MsgB RNTI
  uint16_t MsgB_rnti;
  /// Temporary CRNTI
  uint16_t t_crnti;
  /// number of attempt for rach
  uint8_t RA_attempt_number;
  /// Random-access procedure flag
  bool RA_active;
  /// Random-access preamble index
  int ra_PreambleIndex;
  // When multiple SSBs per RO is configured, this indicates which one is selected in this RO -> this is used to properly compute the PRACH preamble
  uint8_t ssb_nb_in_ro;

  /// Random-access window counter
  int16_t RA_window_cnt;
  /// Flag to monitor if matching RAPID was received in RAR
  uint8_t RA_RAPID_found;
  /// Flag to monitor if BI was received in RAR
  uint8_t RA_BI_found;
  /// Random-access backoff counter
  int16_t RA_backoff_indicator;
  /// Flag to indicate whether preambles Group A was used
  uint8_t RA_usedGroupA;
  /// RA backoff counter
  int16_t RA_backoff_cnt;
  /// RA max number of preamble transmissions
  int preambleTransMax;
  /// Nb of preambles per SSB
  long cb_preambles_per_ssb;
  int starting_preamble_nb;

  /// Received TPC command (in dB) from RAR
  int8_t Msg3_TPC;
  /// Flag to indicate whether it is the first Msg3 to be transmitted
  bool first_Msg3;
  /// RA Msg3 size in bytes
  uint8_t Msg3_size;
  /// Msg3 buffer
  uint8_t *Msg3_buffer;

  bool msg3_C_RNTI;

  /// Random-access Contention Resolution Timer
  NR_timer_t contention_resolution_timer;
  /// Transmitted UE Contention Resolution Identifier
  uint8_t cont_res_id[6];

  /// BeamfailurerecoveryConfig
  NR_BeamFailureRecoveryConfig_t RA_BeamFailureRecoveryConfig;

  NR_PRACH_RESOURCES_t prach_resources;
} RA_config_t;

typedef struct {
  bool active;
  bool ack_received;
  uint8_t  pucch_resource_indicator;
  frame_t ul_frame;
  int ul_slot;
  uint8_t ack;
  int n_CCE;
  int N_CCE;
  int dai_cumul;
  int8_t delta_pucch;
  uint32_t R;
  uint32_t TBS;
  int last_ndi;
  int round;
} NR_UE_DL_HARQ_STATUS_t;

typedef struct {
  uint32_t R;
  uint32_t TBS;
  int last_ndi;
  int round;
} NR_UE_UL_HARQ_INFO_t;

typedef struct {
  uint8_t freq_hopping;
  uint8_t mcs;
  uint8_t Msg3_t_alloc;
  uint16_t Msg3_f_alloc;
} RAR_grant_t;

typedef struct {
  NR_PUCCH_Resource_t *pucch_resource;
  uint32_t ack_payload;
  uint8_t sr_payload;
  uint32_t csi_part1_payload;
  uint32_t csi_part2_payload;
  int n_sr;
  int n_csi;
  int n_harq;
  int n_CCE;
  int N_CCE;
  int initial_pucch_id;
} PUCCH_sched_t;

typedef struct {
  uint32_t ssb_index;
  /// SSB RSRP in dBm
  short ssb_rsrp_dBm;
} NR_SSB_meas_t;

typedef enum ta_type {
  no_ta = 0,
  adjustment_ta,
  rar_ta,
} ta_type_t;

typedef struct NR_UL_TIME_ALIGNMENT {
  /// TA command received from the gNB
  ta_type_t ta_apply;
  int ta_command;
  int frame;
  int slot;
} NR_UL_TIME_ALIGNMENT_t;

// The PRACH Config period is a series of selected slots in one or multiple frames
typedef struct prach_conf_period {
  prach_occasion_slot_t prach_occasion_slot_map[MAX_NB_FRAME_IN_PRACH_CONF_PERIOD][MAX_NB_SLOT_IN_FRAME];
  uint16_t nb_of_prach_occasion; // Total number of PRACH occasions in the PRACH Config period
  uint8_t nb_of_frame; // Size of the PRACH Config period in number of 10ms frames
  uint8_t nb_of_slot; // Nb of slots in each frame
} prach_conf_period_t;

// The association period is a series of PRACH Config periods
typedef struct prach_association_period {
  prach_conf_period_t *prach_conf_period_list[MAX_NB_PRACH_CONF_PERIOD_IN_ASSOCIATION_PERIOD];
  uint8_t nb_of_prach_conf_period; // Nb of PRACH configuration periods within the association period
  uint8_t nb_of_frame; // Total number of frames included in the association period
} prach_association_period_t;

// The association pattern is a series of Association periods
typedef struct prach_association_pattern {
  prach_association_period_t prach_association_period_list[MAX_NB_ASSOCIATION_PERIOD_IN_ASSOCIATION_PATTERN_PERIOD];
  prach_conf_period_t prach_conf_period_list[MAX_NB_PRACH_CONF_PERIOD_IN_ASSOCIATION_PATTERN_PERIOD];
  uint8_t nb_of_assoc_period; // Nb of association periods within the association pattern
  uint8_t nb_of_prach_conf_period_in_max_period; // Nb of PRACH configuration periods within the maximum association pattern period (according to the size of the configured PRACH
  uint8_t nb_of_frame; // Total number of frames included in the association pattern period (after mapping the SSBs and determining the real association pattern length)
} prach_association_pattern_t;

// SSB details
typedef struct ssb_info {
  prach_occasion_info_t *mapped_ro[MAX_NB_RO_PER_SSB_IN_ASSOCIATION_PATTERN]; // List of mapped RACH Occasions to this SSB index
  uint32_t nb_mapped_ro; // Total number of mapped ROs to this SSB index
} ssb_info_t;

// List of all the possible SSBs and their details
typedef struct ssb_list_info {
  ssb_info_t *tx_ssb;
  int nb_tx_ssb;
  int nb_ssb_per_index[MAX_NB_SSB];
} ssb_list_info_t;

typedef struct nr_lcordered_info_s {
  // logical channels ids ordered as per priority
  NR_LogicalChannelIdentity_t lcid;
  int sr_id;
  long priority;
  uint32_t pbr; // in B/s (UINT_MAX = infinite)
  // Bucket size per lcid
  uint32_t bucket_size;
  bool sr_DelayTimerApplied;
  bool lc_SRMask;
} nr_lcordered_info_t;


typedef struct {
  uint8_t payload[NR_CCCH_PAYLOAD_SIZE_MAX];
} __attribute__ ((__packed__)) NR_CCCH_PDU;

typedef struct {
  NR_SearchSpace_t *otherSI_SS;
  NR_SearchSpace_t *ra_SS;
  NR_SearchSpace_t *paging_SS;
  NR_ControlResourceSet_t *commonControlResourceSet;
  A_SEQUENCE_OF(NR_ControlResourceSet_t) list_Coreset;
  A_SEQUENCE_OF(NR_SearchSpace_t) list_SS;
} NR_BWP_PDCCH_t;

typedef struct csi_payload {
  uint32_t part1_payload;
  uint32_t part2_payload;
  int p1_bits;
  int p2_bits;
} csi_payload_t;

typedef enum {
  WIDEBAND_ON_PUCCH,
  SUBBAND_ON_PUCCH,
  ON_PUSCH
} CSI_mapping_t;

typedef struct {
  uint64_t rounds[NR_MAX_HARQ_ROUNDS_FOR_STATS];
  uint64_t total_bits;
  uint64_t total_symbols;
  uint64_t target_code_rate;
  uint64_t qam_mod_order;
  uint64_t rb_size;
  uint64_t nr_of_symbols;
} ue_mac_dir_stats_t;

typedef struct {
  ue_mac_dir_stats_t dl;
  ue_mac_dir_stats_t ul;
  uint32_t bad_dci;
  uint32_t ulsch_DTX;
  uint64_t ulsch_total_bytes_scheduled;
  uint32_t pucch0_DTX;
  int cumul_rsrp;
  uint8_t num_rsrp_meas;
  char srs_stats[50]; // Statistics may differ depending on SRS usage
  int pusch_snrx10;
  int deltaMCS;
  int NPRB;
} ue_mac_stats_t;

typedef enum {
  NR_SI_INFO,
  NR_SI_INFO_v1700
} nr_si_info_type;

typedef struct {
  nr_si_info_type type;
  long si_Periodicity;
  long si_WindowPosition;
} si_schedinfo_config_t;

typedef struct {
  int si_window_start;
  int si_WindowLength;
  A_SEQUENCE_OF(si_schedinfo_config_t) si_SchedInfo_list;
} si_schedInfo_t;

/*!\brief Top level UE MAC structure */
typedef struct NR_UE_MAC_INST_s {
  module_id_t ue_id;
  NR_UE_L2_STATE_t state;
  int servCellIndex;
  long physCellId;
  int first_sync_frame;
  bool get_sib1;
  bool get_otherSI;
  NR_MIB_t *mib;

  si_schedInfo_t si_SchedInfo;
  ssb_list_info_t ssb_list[MAX_NUM_BWP_UE];
  prach_association_pattern_t prach_assoc_pattern[MAX_NUM_BWP_UE];

  NR_UE_ServingCell_Info_t sc_info;
  A_SEQUENCE_OF(NR_UE_DL_BWP_t) dl_BWPs;
  A_SEQUENCE_OF(NR_UE_UL_BWP_t) ul_BWPs;
  NR_BWP_PDCCH_t config_BWP_PDCCH[MAX_NUM_BWP_UE];
  NR_ControlResourceSet_t *coreset0;
  NR_SearchSpace_t *search_space_zero;
  NR_UE_DL_BWP_t *current_DL_BWP;
  NR_UE_UL_BWP_t *current_UL_BWP;

  bool harq_ACK_SpatialBundlingPUCCH;
  bool harq_ACK_SpatialBundlingPUSCH;

  uint32_t uecap_maxMIMO_PDSCH_layers;
  uint32_t uecap_maxMIMO_PUSCH_layers_cb;
  uint32_t uecap_maxMIMO_PUSCH_layers_nocb;

  NR_UL_TIME_ALIGNMENT_t ul_time_alignment;
  NR_TDD_UL_DL_ConfigCommon_t *tdd_UL_DL_ConfigurationCommon;
  frame_type_t frame_type;

  /* Random Access */
  /// CRNTI
  uint16_t crnti;
  /// RA configuration
  RA_config_t ra;
  /// SSB index from MIB decoding
  uint8_t mib_ssb;
  uint32_t mib_additional_bits;
  int mib_frame;

  nr_csi_report_t csi_report_template[MAX_CSI_REPORTCONFIG];

  /// measurements from CSI-RS
  fapi_nr_csirs_measurements_t csirs_measurements;

  ////	FAPI-like interface message
  fapi_nr_ul_config_request_t *ul_config_request;
  fapi_nr_dl_config_request_t *dl_config_request;

  ///     Interface module instances
  nr_ue_if_module_t       *if_module;
  nr_phy_config_t         phy_config;
  nr_synch_request_t      synch_request;

  // order lc info
  A_SEQUENCE_OF(nr_lcordered_info_t) lc_ordered_list;
  NR_UE_SCHEDULING_INFO scheduling_info;
  NR_timer_t *data_inactivity_timer;

  int dmrs_TypeA_Position;
  int p_Max;
  int p_Max_alt;
  int n_ta_offset; // -1 not present, otherwise value to be applied

  long pdsch_HARQ_ACK_Codebook;

  NR_Type0_PDCCH_CSS_config_t type0_PDCCH_CSS_config;
  frequency_range_t frequency_range;
  uint16_t nr_band;
  uint8_t ssb_subcarrier_offset;
  int ssb_start_subcarrier;

  NR_SSB_meas_t ssb_measurements;

  dci_pdu_rel15_t def_dci_pdu_rel15[NR_MAX_SLOTS_PER_FRAME][8];

  // Defined for abstracted mode
  nr_downlink_indication_t dl_info;
  NR_UE_DL_HARQ_STATUS_t dl_harq_info[NR_MAX_HARQ_PROCESSES];
  NR_UE_UL_HARQ_INFO_t ul_harq_info[NR_MAX_HARQ_PROCESSES];

  NR_TAG_Id_t tag_Id;
  A_SEQUENCE_OF(NR_TAG_t) TAG_list;
  NR_TimeAlignmentTimer_t timeAlignmentTimerCommon;
  NR_timer_t time_alignment_timer;

  nr_emulated_l1_t nr_ue_emul_l1;

  pthread_mutex_t mutex_dl_info;

  //SIDELINK MAC PARAMETERS
  sl_nr_ue_mac_params_t *SL_MAC_PARAMS;
  // PUCCH closed loop power control state
  int G_b_f_c;
  bool pucch_power_control_initialized;
  int f_b_f_c;
  bool pusch_power_control_initialized;
  int delta_msg2;
  pthread_mutex_t if_mutex;
  ue_mac_stats_t stats;
} NR_UE_MAC_INST_t;

/*@}*/
#endif /*__LAYER2_MAC_DEFS_H__ */
