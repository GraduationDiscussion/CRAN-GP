/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef RR_FF_MAC_SCHEDULER_H
#define RR_FF_MAC_SCHEDULER_H

#include <ns3/ff-mac-csched-sap.h>
#include <ns3/ff-mac-sched-sap.h>
#include <ns3/ff-mac-scheduler.h>
#include <vector>
#include <map>
#include <ns3/lte-common.h>
#include <ns3/lte-amc.h>
#include <ns3/lte-ffr-sap.h>

#define HARQ_PROC_NUM 8
#define HARQ_DL_TIMEOUT 11

namespace ns3 {


typedef std::vector < uint8_t > DlHarqProcessesStatus_t;
typedef std::vector < uint8_t > DlHarqProcessesTimer_t;
typedef std::vector < DlDciListElement_s > DlHarqProcessesDciBuffer_t;
typedef std::vector < std::vector <struct RlcPduListElement_s> > RlcPduList_t; // vector of the LCs and layers per UE
typedef std::vector < RlcPduList_t > DlHarqRlcPduListBuffer_t; // vector of the 8 HARQ processes per UE

typedef std::vector < UlDciListElement_s > UlHarqProcessesDciBuffer_t;
typedef std::vector < uint8_t > UlHarqProcessesStatus_t;




/**
 * \ingroup ff-api
 * \brief Implements the SCHED SAP and CSCHED SAP for a Round Robin scheduler
 *
 * This class implements the interface defined by the FfMacScheduler abstract class
 */

class RrFfMacScheduler : public FfMacScheduler
{
public:
  /**
   * \brief Constructor
   *
   * Creates the MAC Scheduler interface implementation
   */
  RrFfMacScheduler ();

  /**
   * Destructor
   */
  virtual ~RrFfMacScheduler ();

  // inherited from Object
  virtual void DoDispose (void);     //clearing buffer,status, etc..
  static TypeId GetTypeId (void);

  // inherited from FfMacScheduler


 //--------------old implementation ----------------------------------
 // virtual void SetFfMacCschedSapUser (FfMacCschedSapUser* s);       // C for configuration
 //  virtual void SetFfMacSchedSapUser (FfMacSchedSapUser* s);         // Sap user is from the layer below  (Sched) to this layer (mac)
 //  virtual FfMacCschedSapProvider* GetFfMacCschedSapProvider ();     // C for configuration
 //  virtual FfMacSchedSapProvider* GetFfMacSchedSapProvider ();       // Sap user is from this layer (mac)  to the layer below  (Schedular)
//=======================================================================
  //----------------Modified <Abdelrahman>-----------------------------//
  //-----------Creation of 2 saps (user/provider) for two phys -----------------//
    virtual void SetFfMacCschedSapUser (FfMacCschedSapUser* s , FfMacCschedSapUser* s2);
    virtual void SetFfMacSchedSapUser (FfMacSchedSapUser* s , FfMacSchedSapUser* s2);
    virtual FfMacCschedSapProvider* GetFfMacCschedSapProvider (uint16_t i);       //@TOD0  Connect it to the corresponding sap of mac
    virtual FfMacSchedSapProvider* GetFfMacSchedSapProvider (uint16_t i);         //@TOD0  Connect it to the corresponding sap of mac

 //-------------------------------
//    virtual void SetFfMacCschedSapUser (FfMacCschedSapUser* s);       // C for configuration
//    virtual void SetFfMacSchedSapUser (FfMacSchedSapUser* s);         // Sap user is from the layer below  (Sched) to this layer (mac)
//    virtual FfMacCschedSapProvider* GetFfMacCschedSapProvider ();     // C for configuration
//    virtual FfMacSchedSapProvider* GetFfMacSchedSapProvider ();
//  //--------------'
//    virtual void SetFfMacCschedSapUser2 (FfMacCschedSapUser* s);       // C for configuration
//     virtual void SetFfMacSchedSapUser2 (FfMacSchedSapUser* s);
//     virtual FfMacCschedSapProvider* GetFfMacCschedSapProvider2 ();       //if we have to different functions rather than switch case
//     virtual FfMacSchedSapProvider* GetFfMacSchedSapProvider2();         //if we have to different functions rather than switch case
  //-------------

  //--------------------Mod-end-----------------------------------------------//
  //==========================================================================//

  // FFR SAPs   Fractional frequency reuse
  virtual void SetLteFfrSapProvider (LteFfrSapProvider* s , LteFfrSapProvider* s2);
  virtual LteFfrSapUser* GetLteFfrSapUser (uint16_t i);

     //--------------------------------------------
//     virtual void SetLteFfrSapProvider (LteFfrSapProvider* s );
//     virtual LteFfrSapUser* GetLteFfrSapUser ();
//     //--------------------------------
//     virtual void SetLteFfrSapProvider2 (LteFfrSapProvider* s );
//     virtual LteFfrSapUser* GetLteFfrSapUser2 ();
     //============================================



  friend class  RrSchedulerMemberCschedSapProvider;
  friend class  RrSchedulerMemberSchedSapProvider;

  void TransmissionModeConfigurationUpdate (uint16_t rnti, uint8_t txMode , uint16_t id);
 // void TransmissionModeConfigurationUpdate2 (uint16_t rnti, uint8_t txMode);

private:
  //
  // Implementation of the CSCHED API primitives
  // (See 4.1 for description of the primitives)
  //
//-------Modified <Abdelrhman>-------------------
//----- add parameters of second phy

/*  ------(re-)configure MAC scheduler with cell configuration and scheduler configuration.
 * The cell configuration will also setup the BCH, BCCH, PCCH and CCCH LC configuration*/

  void DoCschedCellConfigReq (const struct FfMacCschedSapProvider::CschedCellConfigReqParameters& params);
//-------------------------------------------------------------------
  /*(re-)configure MAC scheduler with UE specific parameters.
   * A UE can only be configured when a cell configuration has been received  */

  void DoCschedUeConfigReq (const struct FfMacCschedSapProvider::CschedUeConfigReqParameters& params);
//------------------------------------------------------------------------
  /*(re-)configure MAC scheduler with logical channel configuration.
   *  A logical channel can only be configured when a UE configuration has been received.*/

  void DoCschedLcConfigReq (const struct FfMacCschedSapProvider::CschedLcConfigReqParameters& params);
//------------------------------------------------------------------------
  /*release a logical channel in the MAC scheduler.
   * A logical channel can only be released if it has been configured previously.*/

  void DoCschedLcReleaseReq (const struct FfMacCschedSapProvider::CschedLcReleaseReqParameters& params);
//----------------------------------------------------------------------------
  /*release a UE in the MAC scheduler. The release of the UE configuration implies the release of LCs,
   * which are still active. A UE can only be released if it has been configured previously*/

  void DoCschedUeReleaseReq (const struct FfMacCschedSapProvider::CschedUeReleaseReqParameters& params);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--------Mod.  end
//=================================================================================================================







///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //
  // Implementation of the SCHED API primitives
  // (See 4.2 for description of the primitives)
  //


//  //-------Modified <Abdelrhman> ---------------------
//  void DoSchedDlRlcBufferReq (const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params ,
//		    std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>& m_rlcBufferReqx,
//			std::map <uint16_t,uint8_t>& m_p10CqiRxedx ,  std::map <uint16_t,uint32_t>& m_p10CqiTimersx);
 // ====================================================
   //-------Modified <Abdelrhman> ---------------------
  void DoSchedDlRlcBufferReq (const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);
   // ====================================================

  //--not implemented

  void DoSchedDlPagingBufferReq (const struct FfMacSchedSapProvider::SchedDlPagingBufferReqParameters& params);

  void DoSchedDlMacBufferReq (const struct FfMacSchedSapProvider::SchedDlMacBufferReqParameters& params);
//=======================================================================================================

  //-------Modified <Abdelrhman> --------------------------
  void DoSchedDlTriggerReq (const struct FfMacSchedSapProvider::SchedDlTriggerReqParameters& params);
  // ======================================================
  //-------Modified <Abdelrhman> --------------------------
  void DoSchedDlRachInfoReq (const struct FfMacSchedSapProvider::SchedDlRachInfoReqParameters& params);

  //=======================================================


  void DoSchedDlCqiInfoReq (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params );
  //----------------------------------------------------------------------------------------
  /*Starts the UL MAC scheduler for this subframe*/

  //---Modified <Doha> --------------
  void DoSchedUlTriggerReq (const struct FfMacSchedSapProvider::SchedUlTriggerReqParameters& params);
 //----------------------------------------------------------------------------------------
  //============================================================
  //----no implement.
  void DoSchedUlNoiseInterferenceReq (const struct FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters& params);

  void DoSchedUlSrInfoReq (const struct FfMacSchedSapProvider::SchedUlSrInfoReqParameters& params);
  //==============================

//----Modified <Doha>------------------------
  void DoSchedUlMacCtrlInfoReq (const struct FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params);
//=============================================

  //----Modified <Doha>------------------------
  void DoSchedUlCqiInfoReq (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);
 //=============================================

  int GetRbgSize (int dlbandwidth);    //Rbg  <=--=> Resource block group

  static bool SortRlcBufferReq (FfMacSchedSapProvider::SchedDlRlcBufferReqParameters i,FfMacSchedSapProvider::SchedDlRlcBufferReqParameters j);

//  void RefreshDlCqiMaps (void);
//  void RefreshUlCqiMaps (void);
  void RefreshDlCqiMaps (uint16_t id);
  void RefreshUlCqiMaps (uint16_t id);
//--- Id Added
  void UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size , uint16_t id);
  void UpdateUlRlcBufferInfo (uint16_t rnti, uint16_t size , uint16_t id);

  //--------Added <Abdelrhman> --------------------------
//  void UpdateDlRlcBufferInfo2 (uint16_t rnti, uint8_t lcid, uint16_t size );
//  void UpdateUlRlcBufferInfo2 (uint16_t rnti, uint16_t size);

  //=====================================================

  /**
  * \brief Update and return a new process Id for the RNTI specified
  *
  * \param rnti the RNTI of the UE to be updated
  * \return the process id  value
  */
  uint8_t UpdateHarqProcessId (uint16_t rnti , uint16_t id);

  /**
  * \brief Return the availability of free process for the RNTI specified
  *
  * \param rnti the RNTI of the UE to be updated
  * \return the process id  value
  */
  uint8_t HarqProcessAvailability (uint16_t rnti);

  /**
  * \brief Refresh HARQ processes according to the timers
  *
  */
  void RefreshHarqProcesses (uint16_t id);



  uint16_t m_phyId = 2;

  Ptr<LteAmc> m_amc,m_amc2 ;      //Adaptive modulation and coding


  //----------modified <Abdelrhman>

  /*
   * Vectors of UE's RLC info
  */
//------------------
  std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters> m_rlcBufferReq,m_rlcBufferReq2;
//-----------------
  /*
  * Map of UE's DL CQI P01 received
  */


  std::map <uint16_t,uint8_t> m_p10CqiRxed;
  //----------------------
  std::map <uint16_t,uint8_t> m_p10CqiRxed2;
  //------------------------
  /*
  * Map of UE's timers on DL CQI P01 received
  */
  std::map <uint16_t,uint32_t> m_p10CqiTimers;
  //----------------------
  std::map <uint16_t,uint32_t> m_p10CqiTimers2;
  //----------------------
  /*
  * Map of previous allocated UE per RBG
  * (used to retrieve info from UL-CQI)
  */
  //----------------------
  std::map <uint16_t, std::vector <uint16_t> > m_allocationMaps,m_allocationMaps2;
  //----------------------

  /*
  * Map of UEs' UL-CQI per RBG
  */
  //----------------------
  std::map <uint16_t, std::vector <double> > m_ueCqi , m_ueCqi2;
  //----------------------

  /*
  * Map of UEs' timers on UL-CQI per RBG
  */
  //--------------------------
  std::map <uint16_t, uint32_t> m_ueCqiTimers,m_ueCqiTimers2;
  //--------------------------



  /*
  * Map of UE's buffer status reports received
  */

  //-----------------------
  std::map <uint16_t,uint32_t> m_ceBsrRxed,m_ceBsrRxed2;
  //-----------------------

  //----------Mod end----------------------
  //=============================================================================



  //-- MAC SAPs -----

  //----------------Modified <Abdelrahman>-----------------------------//
  //-----------Creation of 2 saps (user/provider) between (mac. and schedular)for two phys -----------------//
  //-- SapUser

  FfMacCschedSapUser* m_cschedSapUser;
  FfMacSchedSapUser* m_schedSapUser;

  FfMacCschedSapUser* m_cschedSapUser2;               // Config. SapUser for phy2
  FfMacSchedSapUser* m_schedSapUser2;                 // Execution SapUser for phy2
  //------------------------------------------------------------------
  //-- SapProvider
  FfMacCschedSapProvider* m_cschedSapProvider;
  FfMacSchedSapProvider* m_schedSapProvider;

  FfMacCschedSapProvider* m_cschedSapProvider2;        // Config. SapProvider for phy2
  FfMacSchedSapProvider* m_schedSapProvider2;          // Execution. SapProvider for phy2

  //--------------Mod-end---------------------------------------------//
  //==================================================================//

  //---FFR SAPs ---------

  //----------------Modified <Abdelrahman>-----------------------------//
  //-----------Creation of 2 saps (user/provider) for two phys -----------------//
  //-- SapUser
  LteFfrSapUser* m_ffrSapUser;                 // for UEs of phy1
  LteFfrSapUser* m_ffrSapUser2;                // for UEs of phy2

  //--sapProvider

  LteFfrSapProvider* m_ffrSapProvider;
  LteFfrSapProvider* m_ffrSapProvider2;

  //--------------Mod-end---------------------------------------------//
  //==================================================================//


  // Internal parameters

  //----------------Modified <Abdelrahman>-----------------------------//
  //------------ setting of parameters for each phy -------------------//
  FfMacCschedSapProvider::CschedCellConfigReqParameters m_cschedCellConfig;      // for UEs of phy1
  //--------

    uint16_t m_nextRntiDl; // RNTI of the next user to be served next scheduling in DL   ------->>>>> phy1
    uint16_t m_nextRntiUl; // RNTI of the next user to be served next scheduling in UL   ------->>>>> phy1
    //--------------------------------------
    uint32_t m_cqiTimersThreshold; // # of TTIs for which a CQI canbe considered valid
    //---------------------------------------
    //--------
      std::map <uint16_t,uint8_t> m_uesTxMode; // txMode of the UEs
    //------------------------------------------------


  //--------
  FfMacCschedSapProvider::CschedCellConfigReqParameters m_cschedCellConfig2;     // for UEs of phy2

  uint16_t m_nextRntiDl2; // RNTI of the next user to be served next scheduling in DL   ------->>>>> phy2
  uint16_t m_nextRntiUl2; // RNTI of the next user to be served next scheduling in UL   ------->>>>> phy2
  //--------
  uint32_t m_cqiTimersThreshold2; // # of TTIs for which a CQI canbe considered valid

  std::map <uint16_t,uint8_t> m_uesTxMode2; // txMode of the UEs
  //--------
  
  //--------------Mod-end---------------------------------------------//
  //==================================================================//



  //-----HARQ attributes -------



   /**
  * m_harqOn when false inhibit te HARQ mechanisms (by default active)
  */
  bool m_harqOn;

  //----------------Modified <Abdelrahman>-----------------------------//
  //------------ setting of HARQ parameters,buffers,timers,listInfo,etc ... for each phy. UES -------------------//

  std::map <uint16_t, uint8_t> m_dlHarqCurrentProcessId;
  //----------
  std::map <uint16_t, uint8_t> m_dlHarqCurrentProcessId2;
  //---------
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` transmission count
  std::map <uint16_t, DlHarqProcessesStatus_t> m_dlHarqProcessesStatus;
  std::map <uint16_t, DlHarqProcessesTimer_t> m_dlHarqProcessesTimer;
  std::map <uint16_t, DlHarqProcessesDciBuffer_t> m_dlHarqProcessesDciBuffer;
  std::map <uint16_t, DlHarqRlcPduListBuffer_t> m_dlHarqProcessesRlcPduListBuffer;
  std::vector <DlInfoListElement_s> m_dlInfoListBuffered; // HARQ retx buffered
//-------------------
  std::map <uint16_t, DlHarqProcessesStatus_t> m_dlHarqProcessesStatus2;
  std::map <uint16_t, DlHarqProcessesTimer_t> m_dlHarqProcessesTimer2;
  std::map <uint16_t, DlHarqProcessesDciBuffer_t> m_dlHarqProcessesDciBuffer2;
  std::map <uint16_t, DlHarqRlcPduListBuffer_t> m_dlHarqProcessesRlcPduListBuffer2;
  std::vector <DlInfoListElement_s> m_dlInfoListBuffered2; // HARQ retx buffered
//-------------------

  std::map <uint16_t, uint8_t> m_ulHarqCurrentProcessId;
  //-----------
  std::map <uint16_t, uint8_t> m_ulHarqCurrentProcessId2;
  //-----------
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, UlHarqProcessesStatus_t> m_ulHarqProcessesStatus;
  std::map <uint16_t, UlHarqProcessesDciBuffer_t> m_ulHarqProcessesDciBuffer;
 //------------
  std::map <uint16_t, UlHarqProcessesStatus_t> m_ulHarqProcessesStatus2;
  std::map <uint16_t, UlHarqProcessesDciBuffer_t> m_ulHarqProcessesDciBuffer2;
 //------------


  // ---RACH attributes--------

  std::vector <struct RachListElement_s> m_rachList;
  std::vector <uint16_t> m_rachAllocationMap;
  uint8_t m_ulGrantMcs; // MCS for UL grant (default 0)        ??????????????

  //-------------
  std::vector <struct RachListElement_s> m_rachList2;
  std::vector <uint16_t> m_rachAllocationMap2;
  //uint8_t m_ulGrantMcs2; // MCS for UL grant (default 0)             // Ue. dependent
  //-------------
  //--------------Mod-end---------------------------------------------//
  //==================================================================//





};

} // namespace ns3

#endif /* RR_FF_MAC_SCHEDULER_H */
