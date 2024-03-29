/**
 * Copyright (c) 2015 Stack Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef H_BLE_HCI_COMMON_
#define H_BLE_HCI_COMMON_

#include "ble.h"

/* 
 * HCI Command Header
 * 
 * Comprised of the following fields
 *  -> Opcode command field (1)
 *  -> Opcode group field   (1)
 *  -> Parameter Length     (1)
 *      Length of all the parameters (does not include any part of the hci
 *      command header
 */
#define BLE_HCI_CMD_HDR_LEN                 (3)

/* Get the OGF and OCF from the opcode in the command */
#define BLE_HCI_OGF(opcode)                 (((opcode) >> 10) & 0x003F)
#define BLE_HCI_OCF(opcode)                 ((opcode) & 0x03FF)

/* Opcode Group */
#define BLE_HCI_OGF_LINK_CTRL               (0x01)
#define BLE_HCI_OGF_LINK_POLICY             (0x02)
#define BLE_HCI_OGF_CTLR_BASEBAND           (0x03)
#define BLE_HCI_OGF_INFO_PARAMS             (0x04)
#define BLE_HCI_OGF_STATUS_PARAMS           (0x05)
#define BLE_HCI_OGF_TESTING                 (0x06)
/* NOTE: 0x07 not defined in specification  */
#define BLE_HCI_OGF_LE                      (0x08)

/* List of OCF for Controller and Baseband commands (OGF=0x03) */
#define BLE_HCI_OCF_CB_SET_EVENT_MASK       (0x0001)
#define BLE_HCI_OCF_CB_RESET                (0x0003)
#define BLE_HCI_OCF_CB_SET_EV_FILT          (0x0005)

/* Command specific definitions */
/* Set event mask */
#define BLE_HCI_SET_EVENT_MASK_LEN          (8)

/* List of OCF for LE commands (OGF = 0x08) */
#define BLE_HCI_OCF_LE_SET_EVENT_MASK       (0x0001)
#define BLE_HCI_OCF_LE_RD_BUF_SIZE          (0x0002)
#define BLE_HCI_OCF_LE_RD_SUPP_FEAT         (0x0003)
/* NOTE: 0x0004 is intentionally left undefined */
#define BLE_HCI_OCF_LE_SET_RAND_ADDR        (0x0005)
#define BLE_HCI_OCF_LE_SET_ADV_PARAMS       (0x0006)
#define BLE_HCI_OCF_LE_RD_ADV_CHAN_TXPWR    (0x0007)
#define BLE_HCI_OCF_LE_SET_ADV_DATA         (0x0008)
#define BLE_HCI_OCF_LE_SET_SCAN_RSP_DATA    (0x0009)
#define BLE_HCI_OCF_LE_SET_ADV_ENABLE       (0x000A)
#define BLE_HCI_OCF_LE_SET_SCAN_PARAMS      (0x000B)
#define BLE_HCI_OCF_LE_SET_SCAN_ENABLE      (0x000C)
#define BLE_HCI_OCF_LE_CREATE_CNXN          (0x000D)
#define BLE_HCI_OCF_LE_CREATE_CNXN_CANCEL   (0x000E)
#define BLE_HCI_OCF_LE_RD_WHITE_LIST_SIZE   (0x000F)
#define BLE_HCI_OCF_LE_CLEAR_WHITE_LIST     (0x0010)
#define BLE_HCI_OCF_LE_ADD_WHITE_LIST       (0x0011)
#define BLE_HCI_OCF_LE_RMV_WHITE_LIST       (0x0012)
#define BLE_HCI_OCF_LE_CNXN_UPDATE          (0x0013)
#define BLE_HCI_OCF_LE_SET_HOST_CHAN_CLASS  (0x0014)
#define BLE_HCI_OCF_LE_RD_CHAN_MAP          (0x0015)
#define BLE_HCI_OCF_LE_RD_REM_FEAT          (0x0016)
#define BLE_HCI_OCF_LE_ENCRYPT              (0x0017)
#define BLE_HCI_OCF_LE_RAND                 (0x0018)
#define BLE_HCI_OCF_LE_START_ENCRYPT        (0x0019)
#define BLE_HCI_OCF_LE_LT_KEY_REQ_REPLY     (0x001A)
#define BLE_HCI_OCF_LE_LT_KEY_REQ_NEG_REPLY (0x001B)
#define BLE_HCI_OCF_LE_RD_SUPP_STATES       (0x001C)
#define BLE_HCI_OCF_LE_RX_TEST              (0x001D)
#define BLE_HCI_OCF_LE_TX_TEST              (0x001E)
#define BLE_HCI_OCF_LE_TEST_END             (0x001F)
#define BLE_HCI_OCF_LE_REM_CNXN_PARAM_RR    (0x0020)
#define BLE_HCI_OCF_LE_REM_CNXN_PARAM_NRR   (0x0021)
#define BLE_HCI_OCF_LE_SET_DATA_LEN         (0x0022)
#define BLE_HCI_OCF_LE_RD_SUGG_DEF_DATA_LEN (0x0023)
#define BLE_HCI_OCF_LE_WR_SUGG_DEF_DATA_LEN (0x0024)
#define BLE_HCI_OCF_LE_RD_P256_PUBKEY       (0x0025)
#define BLE_HCI_OCF_LE_GEN_DHKEY            (0x0026)
#define BLE_HCI_OCF_LE_ADD_RESOLV_LIST      (0x0027)
#define BLE_HCI_OCF_LE_RMV_RESOLV_LIST      (0x0028)
#define BLE_HCI_OCF_LE_CLR_RESOLV_LIST      (0x0029)
#define BLE_HCI_OCF_LE_RD_RESOLV_LIST_SIZE  (0x002A)
#define BLE_HCI_OCF_LE_RD_PEER_RESOLV_ADDR  (0x002B)
#define BLE_HCI_OCF_LE_RD_LOCAL_RESOLV_ADDR (0x002C)
#define BLE_HCI_OCF_LE_SET_ADDR_RES_EN      (0x002D)
#define BLE_HCI_OCF_LE_SET_RESOLV_PRIV_ADDR (0x002E)
#define BLE_HCI_OCF_LE_RD_MAX_DATA_LEN      (0x002F)

/* Command Specific Definitions */
/* Set event mask */
#define BLE_HCI_SET_LE_EVENT_MASK_LEN   (8)

/* Read buffer size */
#define BLE_HCI_RD_BUF_SIZE_LEN         (0)

/* Set scan response data */
#define BLE_HCI_MAX_SCAN_RSP_DATA_LEN   (31)

/* Set advertising data */
#define BLE_HCI_MAX_ADV_DATA_LEN        (31)

/* Set advertising enable */
#define BLE_HCI_SET_ADV_ENABLE_LEN      (1)

/* Set scan enable */
#define BLE_HCI_SET_SCAN_ENABLE_LEN     (2)

/* Set advertising parameters */
#define BLE_HCI_SET_ADV_PARAM_LEN       (15)

/* Advertising types */
#define BLE_HCI_ADV_TYPE_ADV_IND            (0)
#define BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD  (1)
#define BLE_HCI_ADV_TYPE_ADV_SCAN_IND       (2)
#define BLE_HCI_ADV_TYPE_ADV_NONCONN_IND    (3)
#define BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD  (4)
#define BLE_HCI_ADV_TYPE_MAX                (4)

/* Own address types */
#define BLE_HCI_ADV_OWN_ADDR_PUBLIC     (0)
#define BLE_HCI_ADV_OWN_ADDR_RANDOM     (1)
#define BLE_HCI_ADV_OWN_ADDR_PRIV_PUB   (2)
#define BLE_HCI_ADV_OWN_ADDR_PRIV_RAND  (3)
#define BLE_HCI_ADV_OWN_ADDR_MAX        (3)

/* Peer Address Type */
#define BLE_HCI_ADV_PEER_ADDR_PUBLIC    (0)
#define BLE_HCI_ADV_PEER_ADDR_RANDOM    (1)
#define BLE_HCI_ADV_PEER_ADDR_MAX       (1)

/* 
 * Advertising filter policy
 * 
 * Determines how an advertiser filters scan and connection requests.
 * 
 *  NONE: no filtering (default value). No whitelist used.
 *  SCAN: process all connection requests but only scans from white list.
 *  CONN: process all scan request but only connection requests from white list
 *  BOTH: ignore all scan and connection requests unless in white list.
 */
#define BLE_HCI_ADV_FILT_NONE               (0x01)
#define BLE_HCI_ADV_FILT_SCAN               (0x02)
#define BLE_HCI_ADV_FILT_CONN               (0x03)
#define BLE_HCI_ADV_FILT_BOTH               (0x04)
#define BLE_HCI_ADV_FILT_MAX                (0x04)

/* Advertising interval */
#define BLE_HCI_ADV_ITVL                    (625)           /* usecs */
#define BLE_HCI_ADV_ITVL_MIN                (32)            /* units */
#define BLE_HCI_ADV_ITVL_MAX                (16384)         /* units */
#define BLE_HCI_ADV_ITVL_NONCONN_MIN        (160)           /* units */

#define BLE_HCI_ADV_ITVL_DEF                (0x800)         /* 1.28 seconds */
#define BLE_HCI_ADV_CHANMASK_DEF            (0x7)           /* all channels */

/* Set scan parameters */
#define BLE_HCI_SET_SCAN_PARAM_LEN          (7)
#define BLE_HCI_SCAN_TYPE_PASSIVE           (0)
#define BLE_HCI_SCAN_TYPE_ACTIVE            (1)

/* Scan interval and scan window timing */
#define BLE_HCI_SCAN_ITVL                   (625)           /* usecs */
#define BLE_HCI_SCAN_ITVL_MIN               (4)             /* units */
#define BLE_HCI_SCAN_ITVL_MAX               (16384)         /* units */
#define BLE_HCI_SCAN_ITVL_DEF               (16)            /* units */
#define BLE_HCI_SCAN_WINDOW_MIN             (4)             /* units */
#define BLE_HCI_SCAN_WINDOW_MAX             (16384)         /* units */
#define BLE_HCI_SCAN_WINDOW_DEF             (16)            /* units */

/* 
 * Scanning filter policy
 *  NO_WL:
 *      Scanner processes all advertising packets (white list not used) except
 *      directed, connectable advertising packets not sent to the scanner.
 *  USE_WL:
 *      Scanner processes advertisements from white list only. A connectable,
 *      directed advertisment is ignored unless it contains scanners address.
 *  NO_WL_INITA:
 *      Scanner process all advertising packets (white list not used). A
 *      connectable, directed advertisement shall not be ignored if the InitA
 *      is a resolvable private address.
 *  USE_WL_INITA:
 *      Scanner process advertisements from white list ony. A connectable,
 *      directed advertisement shall not be ignored if the InitA is a
 *      resolvable private address.
 */
#define BLE_HCI_SCAN_FILT_NO_WL             (0)
#define BLE_HCI_SCAN_FILT_USE_WL            (1)
#define BLE_HCI_SCAN_FILT_NO_WL_INITA       (2)
#define BLE_HCI_SCAN_FILT_USE_WL_INITA      (3)
#define BLE_HCI_SCAN_FILT_MAX               (3)

/* Event Codes */
#define BLE_HCI_EVCODE_INQUIRY_CMP          (0x01)
#define BLE_HCI_EVCODE_INQUIRY_RESULT       (0x02)
#define BLE_HCI_EVCODE_CNXN_DONE            (0x03)
#define BLE_HCI_EVCODE_CNXN_REQUEST         (0x04)
#define BLE_HCI_EVCODE_DISCNXN_CMP          (0x05)
#define BLE_HCI_EVCODE_AUTH_CMP             (0x06)
#define BLE_HCI_EVCODE_REM_NAME_REQ_CM P    (0x07)
#define BLE_HCI_EVCODE_ENCRYPT_CHG          (0x08)
#define BLE_HCI_EVCODE_CHG_LINK_KEY_CMP     (0x09)
#define BLE_HCI_EVCODE_MASTER_LINK_KEY_CMP  (0x0A)
#define BLE_HCI_EVCODE_RD_REM_SUPP_FEAT_CMP (0x0B)
#define BLE_HCI_EVCODE_RD_REM_VER_INFO_CMP  (0x0C)
#define BLE_HCI_EVCODE_QOS_SETUP_CMP        (0x0D)
#define BLE_HCI_EVCODE_COMMAND_COMPLETE     (0x0E)
#define BLE_HCI_EVCODE_COMMAND_STATE        (0x0F)
#define BLE_HCI_EVCODE_HW_ERROR             (0x10)
#define BLE_HCI_EVCODE_LE_META              (0x3E)
/* XXX: Define them all... */

/* LE sub-event codes */
#define BLE_HCI_LE_SUBEV_CONN_COMPLETE      (0x01)
#define BLE_HCI_LE_SUBEV_ADV_RPT            (0x02)
#define BLE_HCI_LE_SUBEV_CONN_UPD_COMPLETE  (0x03)
#define BLE_HCI_LE_SUBEV_RD_REM_USED_FEAT   (0x04)
#define BLE_HCI_LE_SUBEV_LT_KEY_REQ         (0x05)
#define BLE_HCI_LE_SUBEV_REM_CONN_PARM_REQ  (0x06)
#define BLE_HCI_LE_SUBEV_DATA_LEN_CHG       (0x07)
#define BLE_HCI_LE_SUBEV_RD_LOC_P256_PUBKEY (0x08)
#define BLE_HCI_LE_SUBEV_GEN_DHKEY_COMPLETE (0x09)
#define BLE_HCI_LE_SUBEV_ENH_CONN_COMPLETE  (0x0A)
#define BLE_HCI_LE_SUBEV_DIRECT_ADV_RPT     (0x0B)

/* Event specific definitions */
/* Event command complete */
#define BLE_HCI_EVENT_CMD_COMPLETE_HDR_LEN  (6)

/* Advertising report */
#define BLE_HCI_ADV_RPT_EVTYPE_ADV_IND      (0)
#define BLE_HCI_ADV_RPT_EVTYPE_DIR_IND      (1)
#define BLE_HCI_ADV_RPT_EVTYPE_SCAN_IND     (2)
#define BLE_HCI_ADV_RPT_EVTYPE_NONCONN_IND  (3)
#define BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP     (5)

/*--- Shared data structures ---*/

/* set advertising parameters command (ocf = 0x0006) */
struct hci_adv_params
{
    uint8_t adv_type;
    uint8_t adv_channel_map;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    uint8_t adv_filter_policy;
    uint16_t adv_itvl_min; 
    uint16_t adv_itvl_max;
    uint8_t peer_addr[BLE_DEV_ADDR_LEN];
};

#endif /* H_BLE_HCI_COMMON_ */
