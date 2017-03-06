/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Marco Miozzo  <marco.miozzo@cttc.es>
 *         Nicola Baldo  <nbaldo@cttc.es>
 */

#ifndef LTE_ENB_MAC_H
#define LTE_ENB_MAC_H


#include <map>
#include <vector>
#include <ns3/lte-common.h>
#include <ns3/lte-mac-sap.h>
#include <ns3/lte-enb-cmac-sap.h>
#include <ns3/ff-mac-csched-sap.h>
#include <ns3/ff-mac-sched-sap.h>
#include <ns3/lte-enb-phy-sap.h>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include <ns3/packet.h>
#include <ns3/packet-burst.h>

namespace ns3 {

class DlCqiLteControlMessage;
class UlCqiLteControlMessage;
class PdcchMapLteControlMessage;

typedef std::vector <std::vector < Ptr<PacketBurst> > > DlHarqProcessesBuffer_t;

/**
 * This class implements the MAC layer of the eNodeB device
 */
class LteEnbMac :   public Object
{
  friend class EnbMacMemberLteEnbCmacSapProvider;
  friend class EnbMacMemberLteMacSapProvider<LteEnbMac>;
  friend class EnbMacMemberFfMacSchedSapUser;
  friend class EnbMacMemberFfMacCschedSapUser;
  //this class inherited from LteEnbPhySap (ya3ny kol func ely gwah byt3mlha call mra fe phy de w mra fe phy de fmsh m7tag a3ml duplicate)???
  friend class EnbMacMemberLteEnbPhySapUser;

public:
  static TypeId GetTypeId (void);

  LteEnbMac (void);
  virtual ~LteEnbMac (void);
  virtual void DoDispose (void);


  /**
   * \brief Set the scheduler SAP provider
   * \param s a pointer SAP provider of the FF packet scheduler
   */
  void SetFfMacSchedSapProvider (FfMacSchedSapProvider* s,FfMacSchedSapProvider* s2);
  /**
   * \brief Get the scheduler SAP user
   * \return a pointer to the SAP user of the scheduler
   */
  FfMacSchedSapUser* GetFfMacSchedSapUser (uint8_t i);
  /**
   * \brief Set the control scheduler SAP provider
   * \param s a pointer to the control scheduler SAP provider
   */
  void SetFfMacCschedSapProvider (FfMacCschedSapProvider* s,FfMacCschedSapProvider* s2);
  /**
   * \brief Get the control scheduler SAP user
   * \return a pointer to the control scheduler SAP user
   */
  FfMacCschedSapUser* GetFfMacCschedSapUser (uint8_t i);

  /**
   * \brief Set the MAC SAP user
   * \param s a pointer to the MAC SAP user
   */
  void SetLteMacSapUser (LteMacSapUser* s);
  /**
   * \brief Get the MAC SAP provider
   * \return a pointer to the SAP provider of the MAC
   */
  LteMacSapProvider* GetLteMacSapProvider (void);
  /**
   * \brief Set the control MAC SAP user
   * \param s a pointer to the control MAC SAP user
   */
  void SetLteEnbCmacSapUser (LteEnbCmacSapUser* s);
  /**
   * \brief Get the control MAC SAP provider
   * \return a pointer to the control MAC SAP provider
   */
  LteEnbCmacSapProvider* GetLteEnbCmacSapProvider (void);


  /**
  * \brief Get the eNB-PHY SAP User
  * \return a pointer to the SAP User of the PHY
  */
  LteEnbPhySapUser* GetLteEnbPhySapUser (uint8_t i);



  /**
  * \brief Set the PHY SAP Provider
  * \param s a pointer to the PHY SAP provider
  */
  void SetLteEnbPhySapProvider (LteEnbPhySapProvider* s,LteEnbPhySapProvider* s2);


  /**
   * TracedCallback signature for DL scheduling events.
   *
   * \param [in] frame Frame number.
   * \param [in] subframe Subframe number.
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] mcs0 The MCS for transport block.. 
   * \param [in] tbs0Size
   * \param [in] mcs1 The MCS for transport block.
   * \param [in] tbs1Size
   */
  typedef void (* DlSchedulingTracedCallback)
    (uint32_t frame, uint32_t subframe,  uint16_t rnti,
     uint8_t mcs0, uint16_t tbs0Size,
     uint8_t mcs1, uint16_t tbs1Size);

  /**
   *  TracedCallback signature for UL scheduling events.
   *
   * \param [in] frame Frame number.
   * \param [in] subframe Subframe number.
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] mcs  The MCS for transport block
   * \param [in] tbsSize
   */
  typedef void (* UlSchedulingTracedCallback)
    (uint32_t frame, uint32_t subframe, uint16_t rnti,
     uint8_t mcs, uint16_t tbsSize);
  
private:

  /**
  * \brief Receive a DL CQI ideal control message
  * \param msg the DL CQI message
  */

  // not modified as they're related to ue
  void ReceiveDlCqiLteControlMessage  (Ptr<DlCqiLteControlMessage> msg);

  void DoReceiveLteControlMessage (Ptr<LteControlMessage> msg,Ptr<LteControlMessage> msg2);

  /**
  * \brief Receive a CE element containing the buffer status report
  * \param bsr the BSR message
  */
  void ReceiveBsrMessage  (MacCeListElement_s bsr,MacCeListElement_s bsr2);

 
  void DoUlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi,FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi2);



  // forwarded from LteEnbCmacSapProvider
  void DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth);
  void DoAddUe (uint16_t rnti);
  void DoRemoveUe (uint16_t rnti);
  void DoAddLc (LteEnbCmacSapProvider::LcInfo lcinfo,LteEnbCmacSapProvider::LcInfo lcinfo2, LteMacSapUser* msu);
  void DoReconfigureLc (LteEnbCmacSapProvider::LcInfo lcinfo,LteEnbCmacSapProvider::LcInfo lcinfo2);
  void DoReleaseLc (uint16_t  rnti, uint8_t lcid,uint16_t  rnti2, uint8_t lcid2);
  void DoUeUpdateConfigurationReq (LteEnbCmacSapProvider::UeConfig params,LteEnbCmacSapProvider::UeConfig params2);
  LteEnbCmacSapProvider::RachConfig DoGetRachConfig ();
  LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue DoAllocateNcRaPreamble (uint16_t rnti);

  // forwarded from LteMacSapProvider
  void DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params,LteMacSapProvider::TransmitPduParameters params2);
  void DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params,LteMacSapProvider::ReportBufferStatusParameters params2);


  // forwarded from FfMacCchedSapUser
  void DoCschedCellConfigCnf (FfMacCschedSapUser::CschedCellConfigCnfParameters params,FfMacCschedSapUser::CschedCellConfigCnfParameters params2);
  void DoCschedUeConfigCnf (FfMacCschedSapUser::CschedUeConfigCnfParameters params,FfMacCschedSapUser::CschedUeConfigCnfParameters params2);
  void DoCschedLcConfigCnf (FfMacCschedSapUser::CschedLcConfigCnfParameters params,FfMacCschedSapUser::CschedLcConfigCnfParameters params2);
  void DoCschedLcReleaseCnf (FfMacCschedSapUser::CschedLcReleaseCnfParameters params,FfMacCschedSapUser::CschedLcReleaseCnfParameters params2);
  void DoCschedUeReleaseCnf (FfMacCschedSapUser::CschedUeReleaseCnfParameters params,FfMacCschedSapUser::CschedUeReleaseCnfParameters params2);
  void DoCschedUeConfigUpdateInd (FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params,FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params2);
  void DoCschedCellConfigUpdateInd (FfMacCschedSapUser::CschedCellConfigUpdateIndParameters params,FfMacCschedSapUser::CschedCellConfigUpdateIndParameters params2);

  // forwarded from FfMacSchedSapUser
  void DoSchedDlConfigInd (FfMacSchedSapUser::SchedDlConfigIndParameters ind,FfMacSchedSapUser::SchedDlConfigIndParameters ind2);
  void DoSchedUlConfigInd (FfMacSchedSapUser::SchedUlConfigIndParameters params,FfMacSchedSapUser::SchedUlConfigIndParameters params2);

  // forwarded from LteEnbPhySapUser
  void DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  void DoReceiveRachPreamble (uint8_t prachId,uint8_t prachId2);

public:
  // legacy public for use the Phy callback
  void DoReceivePhyPdu (Ptr<Packet> p,Ptr<Packet> p2);

private:
  void DoUlInfoListElementHarqFeeback (UlInfoListElement_s params,UlInfoListElement_s params2);
  void DoDlInfoListElementHarqFeeback (DlInfoListElement_s params,DlInfoListElement_s params2);

  //            rnti,             lcid, SAP of the RLC instance
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> > m_rlcAttached;

  std::vector <CqiListElement_s> m_dlCqiReceived,m_dlCqiReceived2; // DL-CQI received
  std::vector <FfMacSchedSapProvider::SchedUlCqiInfoReqParameters> m_ulCqiReceived,m_ulCqiReceived2; // UL-CQI received
  std::vector <MacCeListElement_s> m_ulCeReceived,m_ulCeReceived2; // CE received (BSR up to now)

  std::vector <DlInfoListElement_s> m_dlInfoListReceived,m_dlInfoListReceived2; // DL HARQ feedback received

  std::vector <UlInfoListElement_s> m_ulInfoListReceived,m_ulInfoListReceived2; // UL HARQ feedback received


  /*
  * Map of UE's info element (see 4.3.12 of FF MAC Scheduler API)
  */
//   std::map <uint16_t,UlInfoListElement_s> m_ulInfoListElements; 



  LteMacSapProvider* m_macSapProvider;
  LteEnbCmacSapProvider* m_cmacSapProvider;
  LteMacSapUser* m_macSapUser;
  LteEnbCmacSapUser* m_cmacSapUser;


  FfMacSchedSapProvider* m_schedSapProvider;
  FfMacSchedSapProvider* m_schedSapProvider2;//added
  FfMacCschedSapProvider* m_cschedSapProvider;
  FfMacCschedSapProvider* m_cschedSapProvider2;//added
  FfMacSchedSapUser* m_schedSapUser;
  FfMacSchedSapUser* m_schedSapUser2;//added
  FfMacCschedSapUser* m_cschedSapUser;
  FfMacCschedSapUser* m_cschedSapUser2;//added

  // PHY-SAP
  LteEnbPhySapProvider* m_enbPhySapProvider;
  LteEnbPhySapProvider* m_enbPhySapProvider2; //added
  LteEnbPhySapUser* m_enbPhySapUser;
  LteEnbPhySapUser* m_enbPhySapUser2; //added

  uint32_t m_frameNo;
  uint32_t m_subframeNo;
  /**
   * Trace information regarding DL scheduling
   * Frame number, Subframe number, RNTI, MCS of TB1, size of TB1,
   * MCS of TB2 (0 if not present), size of TB2 (0 if not present)
   */
  TracedCallback<uint32_t, uint32_t, uint16_t,
                 uint8_t, uint16_t, uint8_t, uint16_t> m_dlScheduling;
  /**
   * Trace information regarding UL scheduling
   * Frame number, Subframe number, RNTI, MCS of TB, size of TB
   */
  TracedCallback<uint32_t, uint32_t, uint16_t,
                 uint8_t, uint16_t> m_ulScheduling;
  
  uint8_t m_macChTtiDelay; // delay of MAC, PHY and channel in terms of TTIs
  uint8_t m_macChTtiDelay2; // added

  std::map <uint16_t, DlHarqProcessesBuffer_t> m_miDlHarqProcessesPackets; // Packet under trasmission of the DL HARQ process
  std::map <uint16_t, DlHarqProcessesBuffer_t> m_miDlHarqProcessesPackets2;
  uint8_t m_numberOfRaPreambles;
  uint8_t m_preambleTransMax;
  uint8_t m_raResponseWindowSize;

  /**
   * info associated with a preamble allocated for non-contention based RA
   * 
   */
  struct NcRaPreambleInfo
  {   
    uint16_t rnti; ///< rnti previously allocated for this non-contention based RA procedure
    Time expiryTime; ///< value the expiration time of this allocation (so that stale preambles can be reused)
  };

  /**
   * map storing as key the random acccess preamble IDs allocated for
   * non-contention based access, and as value the associated info
   * 
   */
  std::map<uint8_t, NcRaPreambleInfo> m_allocatedNcRaPreambleMap;
 
  std::map<uint8_t, uint32_t> m_receivedRachPreambleCount,m_receivedRachPreambleCount2;

  std::map<uint8_t, uint32_t> m_rapIdRntiMap,m_rapIdRntiMap2;
};

} // end namespace ns3

#endif /* LTE_ENB_MAC_ENTITY_H */
