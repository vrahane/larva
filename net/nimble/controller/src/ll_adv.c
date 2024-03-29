/**
 * Copyright (c) 2015 Runtime Inc.
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
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "os/os.h"
#include "nimble/ble.h"
#include "nimble/hci_common.h"
#include "controller/phy.h"
#include "controller/ll.h"
#include "controller/ll_adv.h"
#include "controller/ll_sched.h"
#include "controller/ll_scan.h"
#include "hal/hal_cputime.h"

/* 
 * Advertising configuration parameters. These are parameters that I have
 * fixed for now but could be considered "configuration" parameters for either
 * the device or the stack.
 */
#define BLE_LL_CFG_ADV_PDU_ITVL_HD_USECS    (5000)  /* usecs */
#define BLE_LL_CFG_ADV_PDU_ITVL_LD_USECS    (10000) /* usecs */
#define BLE_LL_CFG_ADV_TXPWR                (0)     /* dBm */

/* XXX: TODO
 * 1) Need to look at advertising and scan request PDUs. Do I allocate these
 * once? Do I use a different pool for smaller ones? Do I statically declare
 * them?
 * 2) The random address and initiator address (if doing directed adv) is not
 * set yet. Determine how that is to be done.
 * 3) How do features get supported? What happens if device does not support
 * advertising? (for example)
 * 4) Correct calculation of schedule start and end times for the various
 * scheduled advertising activities.
 * 5) How to determine the advertising interval we will actually use. As of
 * now, we set it to max.
 * 6) Currently, when we set scheduling events, we dont take into account
 * processor overhead/delays. We will want to do that.
 * 7) Need to implement the whole HCI command done stuff.
 * 8) For set adv enable command: if we get a connection, or we time out,
 *    we need to send a CONNECTION COMPLETE event. Do this.
 * 9) How does the advertising channel tx power get set? I dont implement
 * that currently.
 * 10) Add whitelist! Do this to the code.
 */

/* 
 * Advertising state machine 
 * 
 * The advertising state machine data structure.
 * 
 *  adv_pdu_len
 *      The length of the advertising PDU that will be sent. This does not
 *      include the preamble, access address and CRC.
 */
struct ll_adv_sm
{
    uint8_t enabled;
    uint8_t adv_type;
    uint8_t adv_len;
    uint8_t adv_chanmask;
    uint8_t adv_filter_policy;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    uint8_t adv_chan;
    uint8_t scan_rsp_len;
    uint8_t adv_pdu_len;
    uint16_t adv_itvl_min;
    uint16_t adv_itvl_max;
    uint32_t adv_itvl_usecs;
    uint32_t adv_event_start_time;
    uint32_t adv_pdu_start_time;
    uint8_t initiator_addr[BLE_DEV_ADDR_LEN];
    uint8_t adv_data[BLE_ADV_DATA_MAX_LEN];
    uint8_t scan_rsp_data[BLE_SCAN_RSP_DATA_MAX_LEN];
    struct os_mbuf *adv_pdu;
    struct os_mbuf *scan_rsp_pdu;
    struct os_event adv_txdone_ev;
};

/* The advertising state machine global object */
struct ll_adv_sm g_ll_adv_sm;

struct ll_adv_stats
{
    uint32_t late_tx_done;
    uint32_t cant_set_sched;
    /* XXX: add to these */
};

struct ll_adv_stats g_ll_adv_stats;

/* XXX: We can calculate scan response time as well. */
/* 
 * Worst case time needed for scheduled advertising item. This is the longest
 * possible time to receive a scan request and send a scan response (with the
 * appropriate IFS time between them). This number is calculated using the
 * following formula: IFS + SCAN_REQ + IFS + SCAN_RSP = 150 + 176 + 150 + 376.
 * 
 * NOTE: The advertising PDU transmit time is NOT included here since we know
 * how long that will take.
 */
#define BLE_LL_ADV_SCHED_MAX_USECS  (852)

/**
 * ble ll adv first chan 
 *  
 * Calculate the first channel that we should advertise upon when we start 
 * an advertising event. 
 * 
 * @param advsm 
 * 
 * @return uint8_t The number of the first channel usable for advertising.
 */
static uint8_t
ll_adv_first_chan(struct ll_adv_sm *advsm)
{
    uint8_t adv_chan;

    /* Set first advertising channel */
    if (advsm->adv_chanmask & 0x01) {
        adv_chan = BLE_PHY_ADV_CHAN_START;
    } else if (advsm->adv_chanmask & 0x02) {
        adv_chan = BLE_PHY_ADV_CHAN_START + 1;
    } else {
        adv_chan = BLE_PHY_ADV_CHAN_START + 2;
    }

    return adv_chan;
}

/**
 * ll adv pdu make 
 *  
 * Create the advertising PDU 
 * 
 * 
 * @param advsm Pointer to advertisement state machine
 */
static void
ll_adv_pdu_make(struct ll_adv_sm *advsm)
{
    uint8_t     adv_data_len;
    uint8_t     *dptr;
    uint8_t     pdulen;
    uint8_t     pdu_type;
    uint8_t     *addr;
    struct os_mbuf *m;

    /* assume this is not a direct ind */
    adv_data_len = advsm->adv_len;
    pdulen = BLE_DEV_ADDR_LEN + adv_data_len;

    /* Must be an advertising type! */
    switch (advsm->adv_type) {
    case BLE_HCI_ADV_TYPE_ADV_IND:
        pdu_type = BLE_ADV_PDU_TYPE_ADV_IND;
        break;

    case BLE_HCI_ADV_TYPE_ADV_NONCONN_IND:
        pdu_type = BLE_ADV_PDU_TYPE_ADV_NONCONN_IND;
        break;

    case BLE_HCI_ADV_TYPE_ADV_SCAN_IND:
        pdu_type = BLE_ADV_PDU_TYPE_ADV_SCAN_IND;
        break;

    case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD:
    case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD:
        pdu_type = BLE_ADV_PDU_TYPE_ADV_DIRECT_IND;
        adv_data_len = 0;
        pdulen = BLE_ADV_DIRECT_IND_LEN;
        break;

        /* Set these to avoid compiler warnings */
    default:
        pdulen = 0;
        pdu_type = 0;
        adv_data_len = 0xFF;
        break;
    }

    /* An invalid advertising data length indicates a memory overwrite */
    assert(adv_data_len <= BLE_ADV_DATA_MAX_LEN);

    /* Set the PDU length in the state machine */
    advsm->adv_pdu_len = pdulen + BLE_LL_PDU_HDR_LEN;

    /* Get the advertising PDU */
    m = advsm->adv_pdu;
    assert(m != NULL);
    m->om_len = advsm->adv_pdu_len;
    OS_MBUF_PKTHDR(m)->omp_len = m->om_len;

    /* Construct scan response */
    if (advsm->own_addr_type == BLE_HCI_ADV_OWN_ADDR_PUBLIC) {
        addr = g_dev_addr;
    } else if (advsm->own_addr_type == BLE_HCI_ADV_OWN_ADDR_RANDOM) {
        pdu_type |= BLE_ADV_PDU_HDR_TXADD_RAND;
        addr = g_random_addr;
    } else {
        /* XXX: unsupported for now  */
        addr = NULL;
        assert(0);
    }

    /* Construct advertisement */
    dptr = m->om_data;
    dptr[0] = pdu_type;
    dptr[1] = (uint8_t)pdulen;
    memcpy(dptr + BLE_LL_PDU_HDR_LEN, addr, BLE_DEV_ADDR_LEN);
    dptr += BLE_LL_PDU_HDR_LEN + BLE_DEV_ADDR_LEN;

    /* For ADV_DIRECT_IND, we need to put initiators address in there */
    if (pdu_type == BLE_ADV_PDU_TYPE_ADV_DIRECT_IND) {
        memcpy(dptr, advsm->initiator_addr, BLE_DEV_ADDR_LEN);
    }

    /* Copy in advertising data, if any */
    if (adv_data_len != 0) {
        memcpy(dptr, advsm->adv_data, adv_data_len);
    }
}

/**
 * ll adv scan rsp pdu make
 *  
 * Create a scan response PDU 
 * 
 * @param advsm 
 */
static void
ll_adv_scan_rsp_pdu_make(struct ll_adv_sm *advsm)
{
    uint8_t     scan_rsp_len;
    uint8_t     *dptr;
    uint8_t     *addr;
    uint8_t     pdulen;
    uint8_t     hdr;
    struct os_mbuf *m;

    /* Make sure that the length is valid */
    scan_rsp_len = advsm->scan_rsp_len;
    assert(scan_rsp_len <= BLE_SCAN_RSP_DATA_MAX_LEN);

    /* Set PDU payload length */
    pdulen = BLE_DEV_ADDR_LEN + scan_rsp_len;

    /* Get the advertising PDU */
    m = advsm->scan_rsp_pdu;
    assert(m != NULL);
    m->om_len = BLE_LL_PDU_HDR_LEN + pdulen;
    OS_MBUF_PKTHDR(m)->omp_len = m->om_len;

    /* Construct scan response */
    if (advsm->own_addr_type == BLE_HCI_ADV_OWN_ADDR_PUBLIC) {
        hdr = BLE_ADV_PDU_TYPE_SCAN_RSP;
        addr = g_dev_addr;
    } else if (advsm->own_addr_type == BLE_HCI_ADV_OWN_ADDR_RANDOM) {
        hdr = BLE_ADV_PDU_TYPE_SCAN_RSP | BLE_ADV_PDU_HDR_TXADD_RAND;
        addr = g_random_addr;
    } else {
        /* XXX: unsupported for now  */
        hdr = 0;
        addr = NULL;
        assert(0);
    }

    /* Write into the pdu buffer */
    dptr = m->om_data;
    dptr[0] = hdr;
    dptr[1] = pdulen;
    memcpy(dptr + BLE_LL_PDU_HDR_LEN, addr, BLE_DEV_ADDR_LEN);

    /* Copy in scan response data, if any */
    if (scan_rsp_len != 0) {
        dptr += BLE_LL_PDU_HDR_LEN + BLE_DEV_ADDR_LEN;  
        memcpy(dptr, advsm->scan_rsp_data, scan_rsp_len);
    }
}

/**
 * ble ll adv rx cb 
 *  
 * Scheduler callback used after advertising PDU sent. 
 * 
 * @param arg 
 */
static int
ll_adv_rx_cb(struct ll_sched_item *sch)
{
    /* Disable the PHY as we might be receiving */
    ble_phy_disable();
    os_eventq_put(&g_ll_data.ll_evq, &g_ll_adv_sm.adv_txdone_ev);
    return BLE_LL_SCHED_STATE_DONE;
}


/**
 * ble ll adv tx done cb 
 *  
 * Scheduler callback when an advertising PDU has been sent. 
 * 
 * @param arg 
 */
static int
ll_adv_tx_done_cb(struct ll_sched_item *sch)
{
    os_eventq_put(&g_ll_data.ll_evq, &g_ll_adv_sm.adv_txdone_ev);
    return BLE_LL_SCHED_STATE_DONE;
}

/**
 * ll adv tx start cb
 *  
 * This is the scheduler callback (called from interrupt context) which 
 * transmits an advertisement. 
 *  
 * Context: Interrupt (scheduler) 
 *  
 * @param sch 
 * 
 * @return int 
 */
static int
ll_adv_tx_start_cb(struct ll_sched_item *sch)
{
    int rc;
    uint8_t end_trans;
    struct ll_adv_sm *advsm;

    /* Get the state machine for the event */
    advsm = (struct ll_adv_sm *)sch->cb_arg;

    /* Set channel */
    rc = ble_phy_setchan(advsm->adv_chan);
    assert(rc == 0);

    /* Set phy mode based on type of advertisement */
    if (advsm->adv_type == BLE_HCI_ADV_TYPE_ADV_NONCONN_IND) {
        end_trans = BLE_PHY_TRANSITION_NONE;
    } else {
        end_trans = BLE_PHY_TRANSITION_TX_RX;
    }

    /* Transmit advertisement */
    rc = ble_phy_tx(advsm->adv_pdu, BLE_PHY_TRANSITION_NONE, end_trans);
    if (rc) {
        /* Transmit failed. */
        rc = ll_adv_tx_done_cb(sch);
    } else {
        /* Set link layer state to advertising */
        ble_ll_state_set(BLE_LL_STATE_ADV);

        /* Set schedule item next wakeup time */
        if (advsm->adv_type == BLE_HCI_ADV_TYPE_ADV_NONCONN_IND) {
            sch->next_wakeup = sch->end_time;
            sch->sched_cb = ll_adv_tx_done_cb;
        } else {
            /* XXX: set next wakeup time. We have to look for either a
             * connect request or scan request or both, depending on
             * advertising type. For now, we will just wait until the
             * end of the scheduled event, which was set to the worst case
             * time to send a scan response PDU. Note that I use the time
             * now to insure the callback occurs after we are dont transmitting
             * the scan response, as we may have been late starting the tx.
             */
            sch->next_wakeup = cputime_get32() +
                (sch->end_time - sch->start_time);
            sch->sched_cb = ll_adv_rx_cb;
        }

        rc = BLE_LL_SCHED_STATE_RUNNING;
    }

    return rc;
}


static struct ll_sched_item *
ll_adv_sched_set(struct ll_adv_sm *advsm)
{
    int rc;
    uint32_t max_usecs;
    struct ll_sched_item *sch;

    sch = ll_sched_get_item();
    if (sch) {
        /* Set sched type */
        sch->sched_type = BLE_LL_SCHED_TYPE_ADV;

        /* XXX: HW output compare to trigger tx start? Look into this */
        /* Set the start time of the event */
        sch->start_time = advsm->adv_pdu_start_time - 
            cputime_usecs_to_ticks(XCVR_TX_SCHED_DELAY_USECS);

        /* Set the callback and argument */
        sch->cb_arg = advsm;
        sch->sched_cb = ll_adv_tx_start_cb;

        /* Set end time to maximum time this schedule item may take */
        max_usecs = ll_pdu_tx_time_get(advsm->adv_pdu_len);
        if (advsm->adv_type != BLE_HCI_ADV_TYPE_ADV_NONCONN_IND) {
            max_usecs += BLE_LL_ADV_SCHED_MAX_USECS;
        }
        sch->end_time = advsm->adv_pdu_start_time +
            cputime_usecs_to_ticks(max_usecs);

        /* XXX: for now, we cant get an overlap so assert on error. */
        /* Add the item to the scheduler */
        rc = ll_sched_add(sch);
        assert(rc == 0);
    } else {
        ++g_ll_adv_stats.cant_set_sched;
    }

    return sch;
}

/**
 * ll adv set adv params 
 *  
 * Called by the HCI command parser when a set advertising parameters command 
 * has been received. 
 *  
 * Context: Link Layer task (HCI command parser) 
 * 
 * @param cmd 
 * 
 * @return int 
 */
int
ll_adv_set_adv_params(uint8_t *cmd)
{
    uint8_t adv_type;
    uint8_t adv_filter_policy;
    uint8_t adv_chanmask;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    uint16_t adv_itvl_min;
    uint16_t adv_itvl_max;
    uint16_t min_itvl;
    struct ll_adv_sm *advsm;

    /* If already enabled, we return an error */
    advsm = &g_ll_adv_sm;
    if (advsm->enabled) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /* Make sure intervals are OK (along with advertising type */
    adv_itvl_min = le16toh(cmd);
    adv_itvl_max = le16toh(cmd + 2);
    adv_type = cmd[4];

    /* Min has to be less than max and cannot equal max */
    if ((adv_itvl_min > adv_itvl_max) || (adv_itvl_min == adv_itvl_max)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    switch (adv_type) {
    case BLE_HCI_ADV_TYPE_ADV_IND:
    case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD:
    case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD:
        min_itvl = BLE_LL_ADV_ITVL_MIN;
        break;
    case BLE_HCI_ADV_TYPE_ADV_NONCONN_IND:
    case BLE_HCI_ADV_TYPE_ADV_SCAN_IND:
        min_itvl = BLE_LL_ADV_ITVL_NONCONN_MIN;
        break;
    default:
        min_itvl = 0xFFFF;
        break;
    }

    /* XXX: isnt there a maximum that we need to check? */
    /* Make sure interval minimum is valid for the advertising type */
    if ((adv_itvl_min < min_itvl) || (adv_itvl_min > BLE_HCI_ADV_ITVL_MAX)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Check own and peer address type */
    own_addr_type =  cmd[5];
    peer_addr_type = cmd[6];

    if ((own_addr_type > BLE_HCI_ADV_OWN_ADDR_MAX) ||
        (peer_addr_type > BLE_HCI_ADV_PEER_ADDR_MAX)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* There are only three adv channels, so check for any outside the range */
    adv_chanmask = cmd[13];
    if (((adv_chanmask & 0xF8) != 0) || (adv_chanmask == 0)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Check for valid filter policy */
    adv_filter_policy = cmd[14];
    if (adv_filter_policy > BLE_HCI_ADV_FILT_MAX) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* XXX: determine if there is anything that needs to be done for
       own address type or peer address type */
    advsm->own_addr_type = own_addr_type;
    advsm->peer_addr_type = peer_addr_type;

    advsm->adv_filter_policy = adv_filter_policy;
    advsm->adv_chanmask = adv_chanmask;
    advsm->adv_itvl_min = adv_itvl_min;
    advsm->adv_itvl_max = adv_itvl_max;
    advsm->adv_type = adv_type;

    return 0;
}

/* Stop the advertising state machine */
static void
ll_adv_sm_stop(struct ll_adv_sm *advsm)
{
    /* XXX: Stop any timers we may have started */

    /* Remove any scheduled advertising items */
    ll_sched_rmv(BLE_LL_SCHED_TYPE_ADV);

    /* Disable advertising */
    advsm->enabled = 0;
}

/**
 * ll adv sm start 
 *  
 * Start the advertising state machine. 
 *  
 * Context: Link-layer task. 
 * 
 * @param advsm Pointer to advertising state machine
 * 
 * @return int 
 */
static int
ll_adv_sm_start(struct ll_adv_sm *advsm)
{
    uint8_t adv_chan;
    struct ll_sched_item *sch;

    /* 
     * XXX: not sure if I should do this or just report whatever random
     * address the host sent. For now, I will reject the command with a
     * command disallowed error. All the parameter errors refer to the command
     * parameter (which in this case is just enable or disable).
     */ 
    if (advsm->own_addr_type != BLE_HCI_ADV_OWN_ADDR_PUBLIC) {
        if (!ll_is_valid_rand_addr(g_random_addr)) {
            return BLE_ERR_CMD_DISALLOWED;
        }

        /* XXX: support these other types */
        if (advsm->own_addr_type != BLE_HCI_ADV_OWN_ADDR_RANDOM) {
            assert(0);
        }
    }

    /* Set flag telling us that advertising is enabled */
    advsm->enabled = 1;

    /* Determine the advertising interval we will use */
    advsm->adv_itvl_usecs = (uint32_t)advsm->adv_itvl_max * BLE_LL_ADV_ITVL;

    /* Create the advertising PDU */
    ll_adv_pdu_make(advsm);

    /* Create scan response PDU (if needed) */
    if (advsm->adv_type != BLE_HCI_ADV_TYPE_ADV_NONCONN_IND) {
        ll_adv_scan_rsp_pdu_make(advsm);
    }

    /* Set first advertising channel */
    adv_chan = ll_adv_first_chan(advsm);
    advsm->adv_chan = adv_chan;

    /* 
     * Set start time for the advertising event. This time is the same
     * as the time we will send the first PDU. Since there does not seem
     * to be any requirements as to when we start, we just set the time to
     * now.
     */ 
    advsm->adv_event_start_time = cputime_get32();
    advsm->adv_pdu_start_time = advsm->adv_event_start_time;

    /* Set packet in schedule */
    sch = ll_adv_sched_set(advsm);
    if (!sch) {
        /* XXX: set a wakeup timer to deal with this. For now, assert */
        assert(0);
    }

    return 0;
}

/**
 * ll adv read txpwr
 *  
 * Called when the LE HCI command read advertising channel tx power command 
 * has been received. Returns the current advertising transmit power. 
 *  
 * Context: Link Layer task (HCI command parser) 
 * 
 * @return int 
 */
int
ll_adv_read_txpwr(uint8_t *rspbuf)
{
    rspbuf[0] = BLE_LL_CFG_ADV_TXPWR;
    return BLE_ERR_SUCCESS;
}

int
ll_adv_set_enable(uint8_t *cmd)
{
    int rc;
    uint8_t enable;
    struct ll_adv_sm *advsm;

    advsm = &g_ll_adv_sm;

    rc = 0;
    enable = cmd[0];
    if (enable == 1) {
        /* If already enabled, do nothing */
        if (!advsm->enabled) {
            /* Start the advertising state machine */
            rc = ll_adv_sm_start(advsm);
        }
    } else if (enable == 0) {
        if (advsm->enabled) {
            ll_adv_sm_stop(advsm);
        }
    } else {
        rc = BLE_ERR_INV_HCI_CMD_PARMS;
    }

    return rc;
}

int
ll_adv_set_scan_rsp_data(uint8_t *cmd, uint8_t len)
{
    uint8_t datalen;
    os_sr_t sr;
    struct ll_adv_sm *advsm;

    /* Check for valid scan response data length */
    datalen = cmd[0];
    if ((datalen > BLE_SCAN_RSP_DATA_MAX_LEN) || (datalen != len)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Copy the new data into the advertising structure. */
    advsm = &g_ll_adv_sm;
    advsm->scan_rsp_len = datalen;
    memcpy(advsm->scan_rsp_data, cmd + 1, datalen);

    /* Re-make the scan response PDU since data may have changed */
    OS_ENTER_CRITICAL(sr);
    /* 
     * XXX: there is a chance, even with interrupts disabled, that
     * we are transmitting the scan response PDU while writing to it.
     */ 
    ll_adv_scan_rsp_pdu_make(advsm);
    OS_EXIT_CRITICAL(sr);

    return 0;
}

/**
 * ble ll adv set adv data 
 *  
 * Called by the LL HCI command parser when a set advertising 
 * data command has been sent from the host to the controller. 
 * 
 * @param cmd Pointer to command data
 * @param len Length of command data
 * 
 * @return int 0: success; BLE_ERR_INV_HCI_CMD_PARMS otherwise.
 */
int
ll_adv_set_adv_data(uint8_t *cmd, uint8_t len)
{
    uint8_t datalen;
    struct ll_adv_sm *advsm;
    os_sr_t sr;

    /* Check for valid advertising data length */
    datalen = cmd[0];
    if ((datalen > BLE_ADV_DATA_MAX_LEN) || (datalen != len)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Copy the new data into the advertising structure. */
    advsm = &g_ll_adv_sm;
    advsm->adv_len = datalen;
    memcpy(advsm->adv_data, cmd + 1, datalen);

    /* If the state machine is enabled, we need to re-make the adv PDU */
    if (advsm->enabled) {
        /* 
         * XXX: currently, even with interrupts disabled, there is a chance
         * that we are transmitting the advertising PDU while writing into
         * it.
         */
        OS_ENTER_CRITICAL(sr);
        ll_adv_pdu_make(advsm);
        OS_EXIT_CRITICAL(sr);
    }

    return 0;
}

/* XXX: this might be used for both scanning and advertising. */
/**
 * ble ll adv set rand addr 
 *  
 * Called from the HCI command parser when the set random address command 
 * is received. 
 *  
 * Context: Link Layer task (HCI command parser) 
 * 
 * @param addr Pointer to address
 * 
 * @return int 0: success
 */
int
ll_adv_set_rand_addr(uint8_t *addr)
{
    int rc;

    rc = BLE_ERR_INV_HCI_CMD_PARMS;
    if (ll_is_valid_rand_addr(addr)) {
        memcpy(g_random_addr, addr, BLE_DEV_ADDR_LEN);
        rc = BLE_ERR_SUCCESS;
    }

    return rc;
}

/**
 * ll adv rx scan req 
 *  
 * Called when the LL receives a scan request.  
 *  
 * NOTE: Called from interrupt context. 
 * 
 * @param rxbuf 
 * 
 * @return -1: scan request not for us. 
 *          0: Scan request is for us and we successfully went from rx to tx.
 *        > 0: PHY error attempting to go from rx to tx.
 */
int
ll_adv_rx_scan_req(uint8_t *rxbuf)
{
    int rc;
    uint8_t rxaddr_type;
    uint8_t *our_addr;
    uint8_t *adva;

    /* Determine if this is addressed to us */
    rxaddr_type = rxbuf[0] & BLE_ADV_PDU_HDR_RXADD_MASK;
    if (rxaddr_type) {
        our_addr = g_random_addr;
    } else {
        our_addr = g_dev_addr;
    }

    rc = -1;
    adva = rxbuf + BLE_LL_PDU_HDR_LEN + BLE_DEV_ADDR_LEN;
    if (!memcmp(our_addr, adva, BLE_DEV_ADDR_LEN)) {
        /* Setup to transmit the scan response */
        rc = ble_phy_tx(g_ll_adv_sm.scan_rsp_pdu, BLE_PHY_TRANSITION_RX_TX, 
                        BLE_PHY_TRANSITION_NONE);
    }

    return rc;
}

/**
 * ll adv tx done proc
 *  
 * Process advertistement tx done event. 
 *  
 * Context: Link Layer task. 
 * 
 * @param arg Pointer to advertising state machine.
 */
void
ll_adv_tx_done_proc(void *arg)
{
    uint8_t mask;
    uint8_t final_adv_chan;
    int32_t delta_t;
    uint32_t itvl;
    struct ll_adv_sm *advsm;

    /* Free the advertising packet */
    advsm = (struct ll_adv_sm *)arg;
    ble_ll_state_set(BLE_LL_STATE_STANDBY);

    /* 
     * Check if we have ended our advertising event. If our last advertising
     * packet was sent on the last channel, it means we are done with this
     * event.
     */
    if (advsm->adv_chanmask & 0x04) {
        final_adv_chan = BLE_PHY_ADV_CHAN_START + 2;
    } else if (advsm->adv_chanmask & 0x02) {
        final_adv_chan = BLE_PHY_ADV_CHAN_START + 1;
    } else {
        final_adv_chan = BLE_PHY_ADV_CHAN_START;
    }

    if (advsm->adv_chan == final_adv_chan) {
        /* This event is over. Set adv channel to first one */
        advsm->adv_chan = ll_adv_first_chan(advsm);

        /* Calculate start time of next advertising event */
        itvl = advsm->adv_itvl_usecs;
        itvl += rand() % (BLE_LL_ADV_DELAY_MS_MAX * 1000);
        advsm->adv_event_start_time += cputime_usecs_to_ticks(itvl);
        advsm->adv_pdu_start_time = advsm->adv_event_start_time;
    } else {
        /* 
         * Move to next advertising channel. If not in the mask, just
         * increment by 1. We can do this because we already checked if we
         * just transmitted on the last advertising channel
         */
        ++advsm->adv_chan;
        mask = 1 << (advsm->adv_chan - BLE_PHY_ADV_CHAN_START);
        if ((mask & advsm->adv_chanmask) == 0) {
            ++advsm->adv_chan;
        }

        /* Set next start time to next pdu transmit time */
        if (advsm->adv_type == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD) {
            itvl = BLE_LL_CFG_ADV_PDU_ITVL_HD_USECS;
        } else {
            itvl = BLE_LL_CFG_ADV_PDU_ITVL_LD_USECS;
        }
        itvl = cputime_usecs_to_ticks(itvl);
        advsm->adv_pdu_start_time += itvl;
    }

    /* 
     * The scheduled time better be in the future! If it is not, we will
     * count a statistic and close the current advertising event. We will
     * then setup the next advertising event.
     */
    delta_t = (int32_t)(advsm->adv_pdu_start_time - cputime_get32());
    if (delta_t < 0) {
        /* Count times we were late */
        ++g_ll_adv_stats.late_tx_done;

        /* Set back to first adv channel */
        advsm->adv_chan = ll_adv_first_chan(advsm);

        /* Calculate start time of next advertising event */
        while (delta_t < 0) {
            itvl = advsm->adv_itvl_usecs;
            itvl += rand() % (BLE_LL_ADV_DELAY_MS_MAX * 1000);
            itvl = cputime_usecs_to_ticks(itvl);
            advsm->adv_event_start_time += itvl;
            advsm->adv_pdu_start_time = advsm->adv_event_start_time;
            delta_t += (int32_t)itvl;
        }
    }

    if (!ll_adv_sched_set(advsm)) {
        /* XXX: we will need to set a timer here to wake us up */
        assert(0);
    }
}

/**
 * ll adv init 
 *  
 * Initialize the advertising functionality of a BLE device. This should 
 * be called once on initialization
 */
void
ll_adv_init(void)
{
    struct ll_adv_sm *advsm;

    /* Set default advertising parameters */
    advsm = &g_ll_adv_sm;
    memset(advsm, 0, sizeof(struct ll_adv_sm));

    advsm->adv_itvl_min = BLE_HCI_ADV_ITVL_DEF;
    advsm->adv_itvl_max = BLE_HCI_ADV_ITVL_DEF;
    advsm->adv_chanmask = BLE_HCI_ADV_CHANMASK_DEF;

    /* Initialize advertising tx done event */
    advsm->adv_txdone_ev.ev_type = BLE_LL_EVENT_ADV_TXDONE;
    advsm->adv_txdone_ev.ev_arg = advsm;

    /* Get an advertising mbuf (packet header) and attach to state machine */
    advsm->adv_pdu = os_mbuf_get_pkthdr(&g_mbuf_pool);
    assert(advsm->adv_pdu != NULL);

    /* Get a scan response mbuf (packet header) and attach to state machine */
    advsm->scan_rsp_pdu = os_mbuf_get_pkthdr(&g_mbuf_pool);
    assert(advsm->scan_rsp_pdu != NULL);
}

