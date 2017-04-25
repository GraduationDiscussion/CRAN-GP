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
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 *         Nicola Baldo  <nbaldo@cttc.es>
 */


#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>

#include "lte-amc.h"
#include "lte-control-messages.h"
#include "lte-enb-net-device.h"
#include "lte-ue-net-device.h"

#include <ns3/lte-enb-mac.h>
#include <ns3/lte-radio-bearer-tag.h>
#include <ns3/lte-ue-phy.h>

#include "ns3/lte-mac-sap.h"
#include <ns3/lte-common.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteEnbMac");

NS_OBJECT_ENSURE_REGISTERED (LteEnbMac);



// //////////////////////////////////////
// member SAP forwarders
// //////////////////////////////////////

//----------------------------------------------done---------------------------
class EnbMacMemberLteEnbCmacSapProvider : public LteEnbCmacSapProvider
{
public:
  EnbMacMemberLteEnbCmacSapProvider (LteEnbMac* mac);

  // inherited from LteEnbCmacSapProvider
  virtual void ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth ) ;//,uint16_t  id);
  virtual void AddUe (uint16_t rnti,uint16_t  id);
  virtual void RemoveUe (uint16_t rnti,uint16_t  id);
  virtual void AddLc (LcInfo lcinfo, LteMacSapUser* msu);
  virtual void ReconfigureLc (LcInfo lcinfo);
  virtual void ReleaseLc (uint16_t rnti, uint8_t lcid, uint16_t id);
  virtual void UeUpdateConfigurationReq (UeConfig params);
  virtual RachConfig GetRachConfig ();
  virtual AllocateNcRaPreambleReturnValue AllocateNcRaPreamble (uint16_t rnti);


private:
  LteEnbMac* m_mac;
};


EnbMacMemberLteEnbCmacSapProvider::EnbMacMemberLteEnbCmacSapProvider (LteEnbMac* mac)
  : m_mac (mac)
{
}

void
EnbMacMemberLteEnbCmacSapProvider::ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  m_mac->DoConfigureMac (ulBandwidth, dlBandwidth);
}

void
EnbMacMemberLteEnbCmacSapProvider::AddUe (uint16_t rnti,uint16_t  id)
{
  m_mac->DoAddUe (rnti,id);
}

void
EnbMacMemberLteEnbCmacSapProvider::RemoveUe (uint16_t rnti,uint16_t  id)
{
  m_mac->DoRemoveUe (rnti,id);
}

void
EnbMacMemberLteEnbCmacSapProvider::AddLc (LcInfo lcinfo, LteMacSapUser* msu)
{
  m_mac->DoAddLc (lcinfo, msu);
}

void
EnbMacMemberLteEnbCmacSapProvider::ReconfigureLc (LcInfo lcinfo)
{
  m_mac->DoReconfigureLc (lcinfo);
}

void
EnbMacMemberLteEnbCmacSapProvider::ReleaseLc (uint16_t rnti, uint8_t lcid,uint16_t id)
{
  m_mac->DoReleaseLc (rnti, lcid,id);
}

void
EnbMacMemberLteEnbCmacSapProvider::UeUpdateConfigurationReq (UeConfig params)
{
  m_mac->DoUeUpdateConfigurationReq (params);
}

LteEnbCmacSapProvider::RachConfig 
EnbMacMemberLteEnbCmacSapProvider::GetRachConfig ()
{
  return m_mac->DoGetRachConfig ();
}
 
LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue 
EnbMacMemberLteEnbCmacSapProvider::AllocateNcRaPreamble (uint16_t rnti)
{
  return m_mac->DoAllocateNcRaPreamble (rnti);
}
//------------------------------------------------------------------------------

//--------------------------------------done-----------------------------------
class EnbMacMemberFfMacSchedSapUser : public FfMacSchedSapUser
{
public:
  EnbMacMemberFfMacSchedSapUser (LteEnbMac* mac);


  virtual void SchedDlConfigInd (const struct SchedDlConfigIndParameters& params);
  virtual void SchedUlConfigInd (const struct SchedUlConfigIndParameters& params);
private:
  LteEnbMac* m_mac;
};


EnbMacMemberFfMacSchedSapUser::EnbMacMemberFfMacSchedSapUser (LteEnbMac* mac)
  : m_mac (mac)
{
}


void
EnbMacMemberFfMacSchedSapUser::SchedDlConfigInd (const struct SchedDlConfigIndParameters& params)
{
  m_mac->DoSchedDlConfigInd (params);
}



void
EnbMacMemberFfMacSchedSapUser::SchedUlConfigInd (const struct SchedUlConfigIndParameters& params)
{
  m_mac->DoSchedUlConfigInd (params);
}

//------------------------------------------------------------------------------

class EnbMacMemberFfMacCschedSapUser : public FfMacCschedSapUser
{
public:
  EnbMacMemberFfMacCschedSapUser (LteEnbMac* mac);

  virtual void CschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params);
  virtual void CschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params);
  virtual void CschedLcConfigCnf (const struct CschedLcConfigCnfParameters& params);
  virtual void CschedLcReleaseCnf (const struct CschedLcReleaseCnfParameters& params);
  virtual void CschedUeReleaseCnf (const struct CschedUeReleaseCnfParameters& params);
  virtual void CschedUeConfigUpdateInd (const struct CschedUeConfigUpdateIndParameters& params);
  virtual void CschedCellConfigUpdateInd (const struct CschedCellConfigUpdateIndParameters& params);

private:
  LteEnbMac* m_mac;
};
//------------------------------------------------------------------//
EnbMacMemberFfMacCschedSapUser::EnbMacMemberFfMacCschedSapUser (LteEnbMac* mac)
  : m_mac (mac)
{
}

void
EnbMacMemberFfMacCschedSapUser::CschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params)
{
  m_mac->DoCschedCellConfigCnf (params);

}

void
EnbMacMemberFfMacCschedSapUser::CschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params)
{
  m_mac->DoCschedUeConfigCnf (params);

}

void
EnbMacMemberFfMacCschedSapUser::CschedLcConfigCnf (const struct CschedLcConfigCnfParameters& params)
{
  m_mac->DoCschedLcConfigCnf (params);

}

void
EnbMacMemberFfMacCschedSapUser::CschedLcReleaseCnf (const struct CschedLcReleaseCnfParameters& params)
{
  m_mac->DoCschedLcReleaseCnf (params);

}

void
EnbMacMemberFfMacCschedSapUser::CschedUeReleaseCnf (const struct CschedUeReleaseCnfParameters& params)
{
  m_mac->DoCschedUeReleaseCnf (params);

}

void
EnbMacMemberFfMacCschedSapUser::CschedUeConfigUpdateInd (const struct CschedUeConfigUpdateIndParameters& params)
{
  m_mac->DoCschedUeConfigUpdateInd (params);
}

void
EnbMacMemberFfMacCschedSapUser::CschedCellConfigUpdateInd (const struct CschedCellConfigUpdateIndParameters& params)
{
  m_mac->DoCschedCellConfigUpdateInd (params);

}

// ---------- PHY-SAP


class EnbMacMemberLteEnbPhySapUser:public LteEnbPhySapUser
{
public:
	uint16_t phyId;
  EnbMacMemberLteEnbPhySapUser (LteEnbMac* mac,uint16_t phyId);

  // inherited from LteEnbPhySapUser
  virtual void ReceivePhyPdu (Ptr<Packet> p);
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  virtual void ReceiveLteControlMessage (Ptr<LteControlMessage> msg);
  //------------need to take id-----------//
  //need to know which phy sent this rach :D
  virtual void ReceiveRachPreamble (uint32_t prachId);
  //-----------------------------------//
  virtual void UlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi);
  virtual void UlInfoListElementHarqFeeback (UlInfoListElement_s params);
  virtual void DlInfoListElementHarqFeeback (DlInfoListElement_s params);

private:
  LteEnbMac* m_mac;
};

EnbMacMemberLteEnbPhySapUser::EnbMacMemberLteEnbPhySapUser ( LteEnbMac* mac ,uint16_t Id):m_mac(mac)

{
//	m_mac(mac);
	phyId= Id;
}


void
EnbMacMemberLteEnbPhySapUser::ReceivePhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceivePhyPdu (p,phyId);
}

void
EnbMacMemberLteEnbPhySapUser::SubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  m_mac->DoSubframeIndication (frameNo, subframeNo,phyId);
}

void
EnbMacMemberLteEnbPhySapUser::ReceiveLteControlMessage (Ptr<LteControlMessage> msg)
{
  m_mac->DoReceiveLteControlMessage (msg,phyId);
}

void
EnbMacMemberLteEnbPhySapUser::ReceiveRachPreamble (uint32_t prachId)
{
  m_mac->DoReceiveRachPreamble (prachId , phyId);
}

void
EnbMacMemberLteEnbPhySapUser::UlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi)
{
  m_mac->DoUlCqiReport (ulcqi,phyId);
}

void
EnbMacMemberLteEnbPhySapUser::UlInfoListElementHarqFeeback (UlInfoListElement_s params)
{
	params.m_phyId = phyId;
  m_mac->DoUlInfoListElementHarqFeeback (params);
}

void
EnbMacMemberLteEnbPhySapUser::DlInfoListElementHarqFeeback (DlInfoListElement_s params)
{
	params.m_phyId = phyId;
  m_mac->DoDlInfoListElementHarqFeeback (params);
}


// //////////////////////////////////////
// generic LteEnbMac methods
// //////////////////////////////////////


TypeId
LteEnbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteEnbMac")
    .SetParent<Object> ()
    .SetGroupName("Lte")
    .AddConstructor<LteEnbMac> ()
    .AddAttribute ("NumberOfRaPreambles",
                   "how many random access preambles are available for the contention based RACH process",
                   UintegerValue (50),
                   MakeUintegerAccessor (&LteEnbMac::m_numberOfRaPreambles),
                   MakeUintegerChecker<uint8_t> (4, 64))
    .AddAttribute ("PreambleTransMax",
                   "Maximum number of random access preamble transmissions",
                   UintegerValue (50),
                   MakeUintegerAccessor (&LteEnbMac::m_preambleTransMax),
                   MakeUintegerChecker<uint8_t> (3, 200))
    .AddAttribute ("RaResponseWindowSize",
                   "length of the window (in TTIs) for the reception of the random access response (RAR); the resulting RAR timeout is this value + 3 ms",
                   UintegerValue (3),
                   MakeUintegerAccessor (&LteEnbMac::m_raResponseWindowSize),
                   MakeUintegerChecker<uint8_t> (2, 10))
    .AddTraceSource ("DlScheduling",
                     "Information regarding DL scheduling.",
                     MakeTraceSourceAccessor (&LteEnbMac::m_dlScheduling),
                     "ns3::LteEnbMac::DlSchedulingTracedCallback")
    .AddTraceSource ("UlScheduling",
                     "Information regarding UL scheduling.",
                     MakeTraceSourceAccessor (&LteEnbMac::m_ulScheduling),
                     "ns3::LteEnbMac::UlSchedulingTracedCallback")
  ;

  return tid;
}

//---------------------------------done--------------------------------------------
LteEnbMac::LteEnbMac ()
{
  NS_LOG_FUNCTION (this);
  m_macSapProvider = new EnbMacMemberLteMacSapProvider<LteEnbMac> (this);
  m_cmacSapProvider = new EnbMacMemberLteEnbCmacSapProvider (this);
  m_schedSapUser = new EnbMacMemberFfMacSchedSapUser (this);
  m_schedSapUser2 = new EnbMacMemberFfMacSchedSapUser (this);//added
  m_cschedSapUser = new EnbMacMemberFfMacCschedSapUser (this);
  m_cschedSapUser2 = new EnbMacMemberFfMacCschedSapUser (this);//added
  m_enbPhySapUser = new EnbMacMemberLteEnbPhySapUser (this,1);
  m_enbPhySapUser2 = new EnbMacMemberLteEnbPhySapUser (this,2);//added
}
//---------------------------------------------------------------------------------

LteEnbMac::~LteEnbMac ()
{
  NS_LOG_FUNCTION (this);
}
//-----------------------------------------done------------------------------------
void
LteEnbMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_dlCqiReceived.clear ();
  m_dlCqiReceived2.clear ();//added
  m_ulCqiReceived.clear ();
  m_ulCqiReceived2.clear ();//added
  m_ulCeReceived.clear ();
  m_ulCeReceived2.clear ();//added
  m_dlInfoListReceived.clear ();
  m_dlInfoListReceived2.clear ();//added
  m_ulInfoListReceived.clear ();
  m_ulInfoListReceived2.clear ();//added
  m_miDlHarqProcessesPackets.clear ();
  m_miDlHarqProcessesPackets2.clear ();//added
  delete m_macSapProvider;
  delete m_cmacSapProvider;
  delete m_schedSapUser;
  delete m_cschedSapUser;
  delete m_enbPhySapUser;
  delete m_schedSapUser2;
  delete m_cschedSapUser2;
  delete m_enbPhySapUser2;
}
//------------------------------------------------------------------------------------

//------------------------------------done--------------------------------------------
void
LteEnbMac::SetFfMacSchedSapProvider (FfMacSchedSapProvider* s,FfMacSchedSapProvider* s2)
{
  m_schedSapProvider = s;
  m_schedSapProvider2 = s2;
}

FfMacSchedSapUser*
LteEnbMac::GetFfMacSchedSapUser (uint16_t id)
{
	switch(id)
	{
	case 1 :
	    return m_schedSapUser;
	    break;
	case 2 :
		return m_schedSapUser2;
		break;
	default :
		return m_schedSapUser;
	}
}

void
LteEnbMac::SetFfMacCschedSapProvider (FfMacCschedSapProvider* s,FfMacCschedSapProvider* s2)
{
  m_cschedSapProvider = s;
  m_cschedSapProvider2 = s2;
}

FfMacCschedSapUser*
LteEnbMac::GetFfMacCschedSapUser (uint16_t id)
{
	switch(id)
		{
		case 1 :
			return m_cschedSapUser;
		    break;
		case 2 :
			return m_cschedSapUser2;
			break;
		default :
			return m_cschedSapUser;
		}
}
//---------------------------------------------------------------------

//-------------------------not modified--------------------------------
void
LteEnbMac::SetLteMacSapUser (LteMacSapUser* s)
{
  m_macSapUser = s;
}

LteMacSapProvider*
LteEnbMac::GetLteMacSapProvider (void)
{
  return m_macSapProvider;
}

void
LteEnbMac::SetLteEnbCmacSapUser (LteEnbCmacSapUser* s)
{
  m_cmacSapUser = s;
}

LteEnbCmacSapProvider*
LteEnbMac::GetLteEnbCmacSapProvider (void)
{
  return m_cmacSapProvider;
}
//-------------------------------------------------------------------------

//----------------------------------done-----------------------------------

void
LteEnbMac::SetLteEnbPhySapProvider (LteEnbPhySapProvider* s,LteEnbPhySapProvider* s2)
{
  m_enbPhySapProvider = s;
  m_enbPhySapProvider2 = s2;
}


LteEnbPhySapUser*
LteEnbMac::GetLteEnbPhySapUser (uint16_t id)
{
	switch(id)
			{
			case 1 :
				 return m_enbPhySapUser;
			    break;
			case 2 :
				 return m_enbPhySapUser2;
				break;
			default :
				 return m_enbPhySapUser;
			}
}
//-----------------------------------------------------------------------------


// modified nourhan
void
LteEnbMac::DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo,uint16_t id)
{
  NS_LOG_FUNCTION (this << " EnbMac - frame " << frameNo << " subframe " << subframeNo);

  // Store current frame / subframe number
  m_frameNo = frameNo;
  m_subframeNo = subframeNo;

if(id==1)
{


	NS_LOG_FUNCTION(this << "<Abdelrhman>-------======> Phyid" << id );
	 // --- DOWNLINK ---
	  // Send Dl-CQI info to the scheduler
	  if (m_dlCqiReceived.size () > 0)
	    {
	      FfMacSchedSapProvider::SchedDlCqiInfoReqParameters dlcqiInfoReq;
	      dlcqiInfoReq.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);
	      dlcqiInfoReq.m_phyId =id;
	      int cqiNum = m_dlCqiReceived.size ();
	      if (cqiNum > MAX_CQI_LIST)
	        {
	          cqiNum = MAX_CQI_LIST;
	        }
	      dlcqiInfoReq.m_cqiList.insert (dlcqiInfoReq.m_cqiList.begin (), m_dlCqiReceived.begin (), m_dlCqiReceived.end ());
	      m_dlCqiReceived.erase (m_dlCqiReceived.begin (), m_dlCqiReceived.end ());
	      m_schedSapProvider->SchedDlCqiInfoReq (dlcqiInfoReq);
	    }

	  if (!m_receivedRachPreambleCount.empty ())
	    {
	      // process received RACH preambles and notify the scheduler
	      FfMacSchedSapProvider::SchedDlRachInfoReqParameters rachInfoReqParams;

	      //--------------mac compare------------//
	      rachInfoReqParams.m_phyId = id;
	      NS_ASSERT (subframeNo > 0 && subframeNo <= 10); // subframe in 1..10
	      for (std::map<uint8_t, uint32_t>::const_iterator it = m_receivedRachPreambleCount.begin ();
	           it != m_receivedRachPreambleCount.end ();
	           ++it)
	        {
	          NS_LOG_INFO (this << " preambleId " << (uint32_t) it->first << ": " << it->second << " received");
	          NS_ASSERT (it->second != 0);
	          if (it->second > 1)
	            {
	              NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": collision");
	              // in case of collision we assume that no preamble is
	              // successfully received, hence no RAR is sent
	            }
	          else
	            {
	              uint16_t rnti;
	              std::map<uint8_t, NcRaPreambleInfo>::iterator jt = m_allocatedNcRaPreambleMap.find (it->first);
	              if (jt != m_allocatedNcRaPreambleMap.end ())
	                {
	                  rnti = jt->second.rnti;
	                  NS_LOG_INFO ("preambleId previously allocated for NC based RA, RNTI =" << (uint32_t) rnti << ", sending RAR");

	                }
	              else
	                {
	                  rnti = m_cmacSapUser->AllocateTemporaryCellRnti (id);
	                  NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": allocated T-C-RNTI " << (uint32_t) rnti << ", sending RAR");
	                }

	              RachListElement_s rachLe;
	              rachLe.m_rnti = rnti;
	              rachLe.m_estimatedSize = 144; // to be confirmed
	              rachInfoReqParams.m_rachList.push_back (rachLe);
	              m_rapIdRntiMap.insert (std::pair <uint16_t, uint32_t> (rnti, it->first));
	            }
	        }
	      m_schedSapProvider->SchedDlRachInfoReq (rachInfoReqParams);
	      m_receivedRachPreambleCount.clear ();
	    }
	  // Get downlink transmission opportunities
	  uint32_t dlSchedFrameNo = m_frameNo;
	  uint32_t dlSchedSubframeNo = m_subframeNo;
	  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
	  if (dlSchedSubframeNo + m_macChTtiDelay > 10)
	    {
	      dlSchedFrameNo++;
	      dlSchedSubframeNo = (dlSchedSubframeNo + m_macChTtiDelay) % 10;
	    }
	  else
	    {
	      dlSchedSubframeNo = dlSchedSubframeNo + m_macChTtiDelay;
	    }
	  FfMacSchedSapProvider::SchedDlTriggerReqParameters dlparams;
	  dlparams.m_sfnSf = ((0x3FF & dlSchedFrameNo) << 4) | (0xF & dlSchedSubframeNo);
	  dlparams.m_phyId = id;
	  // Forward DL HARQ feebacks collected during last TTI
	  NS_LOG_FUNCTION("<mai> --------- line 602 ---------- <mai>" << m_dlInfoListReceived.size());

	  if (m_dlInfoListReceived.size () > 0)
	    {
		  NS_LOG_FUNCTION("<mai> --------- line 606 ---------- <mai>" << m_dlInfoListReceived.size());
	      dlparams.m_dlInfoList = m_dlInfoListReceived;
	      // empty local buffer
	      m_dlInfoListReceived.clear ();
	    }

	  m_schedSapProvider->SchedDlTriggerReq (dlparams);


	  // --- UPLINK ---
	  // Send UL-CQI info to the scheduler
	  std::vector <FfMacSchedSapProvider::SchedUlCqiInfoReqParameters>::iterator itCqi;
	  for (uint16_t i = 0; i < m_ulCqiReceived.size (); i++)
	    {
	      if (subframeNo > 1)
	        {
	          m_ulCqiReceived.at (i).m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & (subframeNo - 1));
	        }
	      else
	        {
	          m_ulCqiReceived.at (i).m_sfnSf = ((0x3FF & (frameNo - 1)) << 4) | (0xF & 10);
	        }
	      m_schedSapProvider->SchedUlCqiInfoReq (m_ulCqiReceived.at (i));
	    }
	    m_ulCqiReceived.clear ();

	  // Send BSR reports to the scheduler
	  if (m_ulCeReceived.size () > 0)
	    {
	      FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters ulMacReq;

	      //--------mac compare-------------//

	      ulMacReq.m_phyId = id;
	      ulMacReq.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);
	      ulMacReq.m_macCeList.insert (ulMacReq.m_macCeList.begin (), m_ulCeReceived.begin (), m_ulCeReceived.end ());
	      m_ulCeReceived.erase (m_ulCeReceived.begin (), m_ulCeReceived.end ());
	      m_schedSapProvider->SchedUlMacCtrlInfoReq (ulMacReq);
	    }


	  // Get uplink transmission opportunities
	  uint32_t ulSchedFrameNo = m_frameNo;
	  uint32_t ulSchedSubframeNo = m_subframeNo;
	  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
	  if (ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY) > 10)
	    {
	      ulSchedFrameNo++;
	      ulSchedSubframeNo = (ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY)) % 10;
	    }
	  else
	    {
	      ulSchedSubframeNo = ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY);
	    }
	  FfMacSchedSapProvider::SchedUlTriggerReqParameters ulparams;

	  ulparams.m_phyId = id;
	  ulparams.m_sfnSf = ((0x3FF & ulSchedFrameNo) << 4) | (0xF & ulSchedSubframeNo);

	  // Forward DL HARQ feebacks collected during last TTI
	  if (m_ulInfoListReceived.size () > 0)
	    {
	     ulparams.m_ulInfoList = m_ulInfoListReceived;
	      // empty local buffer
	      m_ulInfoListReceived.clear ();
	    }

	  m_schedSapProvider->SchedUlTriggerReq (ulparams);

}
if(id==2)
{
	NS_LOG_FUNCTION(this << "<Abdelrhman>-------======> Phyid " << id );
	 // --- DOWNLINK ---
	  // Send Dl-CQI info to the scheduler
	  if (m_dlCqiReceived2.size () > 0)
	    {
	      FfMacSchedSapProvider::SchedDlCqiInfoReqParameters dlcqiInfoReq2;
	      dlcqiInfoReq2.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);
	      dlcqiInfoReq2.m_phyId = id;
	      int cqiNum2 = m_dlCqiReceived2.size ();
	      if (cqiNum2 > MAX_CQI_LIST)
	        {
	          cqiNum2 = MAX_CQI_LIST;
	        }
	      dlcqiInfoReq2.m_cqiList.insert (dlcqiInfoReq2.m_cqiList.begin (), m_dlCqiReceived2.begin (), m_dlCqiReceived2.end ());
	      m_dlCqiReceived2.erase (m_dlCqiReceived2.begin (), m_dlCqiReceived2.end ());
	      m_schedSapProvider2->SchedDlCqiInfoReq (dlcqiInfoReq2);
	    }

	  if (!m_receivedRachPreambleCount2.empty ())
	    {
	      // process received RACH preambles and notify the scheduler
	      FfMacSchedSapProvider::SchedDlRachInfoReqParameters rachInfoReqParams2;
	      rachInfoReqParams2.m_phyId = id;
	      NS_ASSERT (subframeNo > 0 && subframeNo <= 10); // subframe in 1..10
	      for (std::map<uint8_t, uint32_t>::const_iterator it = m_receivedRachPreambleCount2.begin ();
	           it != m_receivedRachPreambleCount2.end ();
	           ++it)
	        {
	          NS_LOG_INFO (this << " preambleId " << (uint32_t) it->first << ": " << it->second << " received");
	          NS_ASSERT (it->second != 0);
	          if (it->second > 1)
	            {
	              NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": collision");
	              // in case of collision we assume that no preamble is
	              // successfully received, hence no RAR is sent
	            }
	          else
	            {
	              uint16_t rnti;
	              std::map<uint8_t, NcRaPreambleInfo>::iterator jt = m_allocatedNcRaPreambleMap2.find (it->first);
	              if (jt != m_allocatedNcRaPreambleMap2.end ())
	                {
	                  rnti = jt->second.rnti;
	                  NS_LOG_INFO ("preambleId previously allocated for NC based RA, RNTI =" << (uint32_t) rnti << ", sending RAR");

	                }
	              else
	                {
	                  rnti = m_cmacSapUser->AllocateTemporaryCellRnti (id);
	                  NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": allocated T-C-RNTI " << (uint32_t) rnti << ", sending RAR");
	                }

	              RachListElement_s rachLe;
	              rachLe.m_rnti = rnti;
	              rachLe.m_estimatedSize = 144; // to be confirmed
	              rachInfoReqParams2.m_rachList.push_back (rachLe);
	              m_rapIdRntiMap2.insert (std::pair <uint16_t, uint32_t> (rnti, it->first));
	            }
	        }
	      m_schedSapProvider2->SchedDlRachInfoReq (rachInfoReqParams2);
	      m_receivedRachPreambleCount2.clear ();
	    }
	  // Get downlink transmission opportunities
	  uint32_t dlSchedFrameNo2 = m_frameNo;
	  uint32_t dlSchedSubframeNo2 = m_subframeNo;
	  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
	  if (dlSchedSubframeNo2 + m_macChTtiDelay2 > 10)
	    {
	      dlSchedFrameNo2++;
	      dlSchedSubframeNo2 = (dlSchedSubframeNo2 + m_macChTtiDelay2) % 10;
	    }
	  else
	    {
	      dlSchedSubframeNo2 = dlSchedSubframeNo2 + m_macChTtiDelay2;
	    }
	  FfMacSchedSapProvider::SchedDlTriggerReqParameters dlparams2;
	  dlparams2.m_sfnSf = ((0x3FF & dlSchedFrameNo2) << 4) | (0xF & dlSchedSubframeNo2);
	  dlparams2.m_phyId = id ;
	  // Forward DL HARQ feebacks collected during last TTI
	  if (m_dlInfoListReceived2.size () > 0)
	    {
	      dlparams2.m_dlInfoList = m_dlInfoListReceived2;
	      // empty local buffer
	      m_dlInfoListReceived2.clear ();
	    }

	  m_schedSapProvider2->SchedDlTriggerReq (dlparams2);


	  // --- UPLINK ---
	  // Send UL-CQI info to the scheduler
	  std::vector <FfMacSchedSapProvider::SchedUlCqiInfoReqParameters>::iterator itCqi;
	  for (uint16_t i = 0; i < m_ulCqiReceived2.size (); i++)
	    {
	      if (subframeNo > 1)
	        {
	          m_ulCqiReceived2.at (i).m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & (subframeNo - 1));
	        }
	      else
	        {
	          m_ulCqiReceived2.at (i).m_sfnSf = ((0x3FF & (frameNo - 1)) << 4) | (0xF & 10);
	        }
	      m_schedSapProvider2->SchedUlCqiInfoReq (m_ulCqiReceived2.at (i));
	    }
	    m_ulCqiReceived2.clear ();

	  // Send BSR reports to the scheduler
	  if (m_ulCeReceived2.size () > 0)
	    {
	      FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters ulMacReq2;
	      ulMacReq2.m_phyId = id;
	      ulMacReq2.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);
	      ulMacReq2.m_macCeList.insert (ulMacReq2.m_macCeList.begin (), m_ulCeReceived2.begin (), m_ulCeReceived2.end ());
	      m_ulCeReceived2.erase (m_ulCeReceived2.begin (), m_ulCeReceived2.end ());
	      m_schedSapProvider2->SchedUlMacCtrlInfoReq (ulMacReq2);
	    }


	  // Get uplink transmission opportunities
	  uint32_t ulSchedFrameNo2 = m_frameNo;
	  uint32_t ulSchedSubframeNo2 = m_subframeNo;
	  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
	  if (ulSchedSubframeNo2 + (m_macChTtiDelay2 + UL_PUSCH_TTIS_DELAY) > 10)
	    {
	      ulSchedFrameNo2++;
	      ulSchedSubframeNo2 = (ulSchedSubframeNo2 + (m_macChTtiDelay2 + UL_PUSCH_TTIS_DELAY)) % 10;
	    }
	  else
	    {
	      ulSchedSubframeNo2 = ulSchedSubframeNo2 + (m_macChTtiDelay2 + UL_PUSCH_TTIS_DELAY);
	    }
	  FfMacSchedSapProvider::SchedUlTriggerReqParameters ulparams2;
	  ulparams2.m_phyId=id;
	  ulparams2.m_sfnSf = ((0x3FF & ulSchedFrameNo2) << 4) | (0xF & ulSchedSubframeNo2);

	  // Forward DL HARQ feebacks collected during last TTI
	  if (m_ulInfoListReceived2.size () > 0)
	    {
	     ulparams2.m_ulInfoList = m_ulInfoListReceived2;
	      // empty local buffer
	      m_ulInfoListReceived2.clear ();
	    }

	  m_schedSapProvider2->SchedUlTriggerReq (ulparams2);

}

}


//------------------------will be changed -mai--------------------------
void
LteEnbMac::DoReceiveLteControlMessage  (Ptr<LteControlMessage> msg,uint16_t id)
{
  NS_LOG_FUNCTION (this << msg);
  NS_LOG_FUNCTION (this  << "Abdelrhman ==--- >  phyid"  << id << msg->GetMessageType ()) ;

  if (msg->GetMessageType () == LteControlMessage::DL_CQI)
    {
      Ptr<DlCqiLteControlMessage> dlcqi = DynamicCast<DlCqiLteControlMessage> (msg);
      ReceiveDlCqiLteControlMessage (dlcqi,id);
    }
  else if (msg->GetMessageType () == LteControlMessage::BSR)
    {
	  Ptr<BsrLteControlMessage> bsr = DynamicCast<BsrLteControlMessage> (msg);
      ReceiveBsrMessage (bsr->GetBsr (),id);
    }
  else if (msg->GetMessageType () == LteControlMessage::DL_HARQ)
    {
      Ptr<DlHarqFeedbackLteControlMessage> dlharq = DynamicCast<DlHarqFeedbackLteControlMessage> (msg);
      DlInfoListElement_s params = dlharq->GetDlHarqFeedback ();
      params.m_phyId = id;
      DoDlInfoListElementHarqFeeback (params);
    }
  else
    {
      NS_LOG_LOGIC (this << " LteControlMessage type " << msg->GetMessageType () << " not recognized");
    }
}
//----------------------changed -nourhan--------------------------------
void
LteEnbMac::DoReceiveRachPreamble  (uint8_t rapId, uint16_t id)
{
  NS_LOG_FUNCTION (this << "<abdlrhman> ==--> phyid" << id << (uint32_t) rapId);
  // just record that the preamble has been received; it will be processed later
  if(id==1)
  ++m_receivedRachPreambleCount[rapId]; // will create entry if not exists
  if(id==2)
    ++m_receivedRachPreambleCount2[rapId]; // will create entry if not exists
}
//---------------------nourhan--------------------------
void
LteEnbMac::DoUlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi,uint16_t id)
{ 
  if (ulcqi.m_ulCqi.m_type == UlCqi_s::PUSCH)
    {
      NS_LOG_DEBUG (this << "<abdlrhman> phyid " << id << " eNB rxed an PUSCH UL-CQI");
    }
  else if (ulcqi.m_ulCqi.m_type == UlCqi_s::SRS)
    {
      NS_LOG_DEBUG (this << "<abdlrhman> phyid " << id  << " eNB rxed an SRS UL-CQI");
    }
  if(id==1)
    {
	  m_ulCqiReceived.push_back (ulcqi);
    }
   if(id==2)
   {
	   m_ulCqiReceived2.push_back (ulcqi);
   }

}
//---------------------------nourhan-------------------------
void
LteEnbMac::ReceiveDlCqiLteControlMessage  (Ptr<DlCqiLteControlMessage> msg,uint16_t id)
{
  NS_LOG_FUNCTION (this << msg << "<abdlrhman> ==--> phyid" << id);

  CqiListElement_s dlcqi = msg->GetDlCqi ();
  NS_LOG_LOGIC (this << "Enb Received DL-CQI rnti" << dlcqi.m_rnti);
  NS_ASSERT (dlcqi.m_rnti != 0);
  if(id==1)
  {
	  m_dlCqiReceived.push_back (dlcqi);
  }
 if(id==2)
 {
	 m_dlCqiReceived2.push_back (dlcqi);
 }


}

//----------------------------------nourhan--------------------------------------
void
LteEnbMac::ReceiveBsrMessage  (MacCeListElement_s bsr,uint16_t id)
{
  NS_LOG_FUNCTION (this << "<abdlrhman> ==--> phyid" << id);
  if(id==1)
   {
	  m_ulCeReceived.push_back (bsr);
   }
  if(id==2)
  {
	  m_ulCeReceived2.push_back (bsr);
  }

}

//----------------------------------done nourhan------------------------------
// id is send but it will not use here
void
LteEnbMac::DoReceivePhyPdu (Ptr<Packet> p,uint16_t id)
{
  NS_LOG_FUNCTION (this << "Abdelrhman --=== > phyid"  << id);
  LteRadioBearerTag tag;
  p->RemovePacketTag (tag);

  // store info of the packet received

//   std::map <uint16_t,UlInfoListElement_s>::iterator it;
//   u_int rnti = tag.GetRnti ();
//  u_int lcid = tag.GetLcid ();
//   it = m_ulInfoListElements.find (tag.GetRnti ());
//   if (it == m_ulInfoListElements.end ())
//     {
//       // new RNTI
//       UlInfoListElement_s ulinfonew;
//       ulinfonew.m_rnti = tag.GetRnti ();
//       // always allocate full size of ulReception vector, initializing all elements to 0
//       ulinfonew.m_ulReception.assign (MAX_LC_LIST+1, 0);
//       // set the element for the current LCID
//       ulinfonew.m_ulReception.at (tag.GetLcid ()) = p->GetSize ();
//       ulinfonew.m_receptionStatus = UlInfoListElement_s::Ok;
//       ulinfonew.m_tpc = 0; // Tx power control not implemented at this stage
//       m_ulInfoListElements.insert (std::pair<uint16_t, UlInfoListElement_s > (tag.GetRnti (), ulinfonew));
// 
//     }
//   else
//     {
//       // existing RNTI: we just set the value for the current
//       // LCID. Note that the corresponding element had already been
//       // allocated previously.
//       NS_ASSERT_MSG ((*it).second.m_ulReception.at (tag.GetLcid ()) == 0, "would overwrite previously written ulReception element");
//       (*it).second.m_ulReception.at (tag.GetLcid ()) = p->GetSize ();
//       (*it).second.m_receptionStatus = UlInfoListElement_s::Ok;
//     }



  // forward the packet to the correspondent RLC
  uint16_t rnti = tag.GetRnti ();
  uint8_t lcid = tag.GetLcid ();
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
  //NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);

  //Receive PDU only if LCID is found
  if (lcidIt != rntiIt->second.end ())
    {
      (*lcidIt).second->ReceivePdu (p);
    }
}

// ////////////////////////////////////////////
// CMAC SAP
// ////////////////////////////////////////////


//--------------------------------------done--------------------------------------
void
LteEnbMac::DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{

  FfMacCschedSapProvider::CschedCellConfigReqParameters params;
  // Configure the subset of parameters used by FfMacScheduler
  params.m_ulBandwidth = ulBandwidth;
  params.m_dlBandwidth = dlBandwidth;
 /* params.m_phyId=id;
  if(id==1)
  {
	 NS_LOG_FUNCTION (this << " ulBandwidth=" << (uint16_t) ulBandwidth << " dlBandwidth=" << (uint16_t) dlBandwidth);
     m_macChTtiDelay = m_enbPhySapProvider->GetMacChTtiDelay ();
     // ...more parameters can be configured
     m_cschedSapProvider->CschedCellConfigReq (params);
  }
  if(id==2)
    {
	NS_LOG_FUNCTION (this << " ulBandwidth2=" << (uint16_t) ulBandwidth << " dlBandwidth2=" << (uint16_t) dlBandwidth);
    m_macChTtiDelay2 = m_enbPhySapProvider2->GetMacChTtiDelay ();
    // ...more parameters can be configured
    m_cschedSapProvider2->CschedCellConfigReq (params);
    }*/
  NS_LOG_FUNCTION (this << " ulBandwidth=" << (uint16_t) ulBandwidth << " dlBandwidth=" << (uint16_t) dlBandwidth);
  m_macChTtiDelay = m_enbPhySapProvider->GetMacChTtiDelay ();
  // ...more parameters can be configured
  params.m_phyId = 1;
  m_cschedSapProvider->CschedCellConfigReq (params);

  NS_LOG_FUNCTION (this << " ulBandwidth2=" << (uint16_t) ulBandwidth << " dlBandwidth2=" << (uint16_t) dlBandwidth);
  m_macChTtiDelay2 = m_enbPhySapProvider2->GetMacChTtiDelay ();
  // ...more parameters can be configured
  params.m_phyId = 2;
  m_cschedSapProvider2->CschedCellConfigReq (params);
}
//--------------------------------------------------------------------------------

//-----------------------------------------------done-----------------------------
void
LteEnbMac::DoAddUe (uint16_t rnti,uint16_t id)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  NS_LOG_FUNCTION (this << " Abdelrhman ----====> phyId " << id);
  std::map<uint8_t, LteMacSapUser*> empty;
  std::pair <std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator, bool> 
    ret = m_rlcAttached.insert (std::pair <uint16_t,  std::map<uint8_t, LteMacSapUser*> > 
                                (rnti, empty));
  NS_ASSERT_MSG (ret.second, "element already present, RNTI already existed");

  FfMacCschedSapProvider::CschedUeConfigReqParameters params;
  params.m_rnti = rnti;
  params.m_phyId = id;
  params.m_transmissionMode = 0; // set to default value (SISO) for avoiding random initialization (valgrind error)
//------------------------------------------added-------------------------
  if(id==1)
  {
  m_cschedSapProvider->CschedUeConfigReq (params);
  NS_LOG_FUNCTION (this << " Abdelrhman ----====> <first if > phyId " << id);

  }
  if(id==2)
  {
	  NS_LOG_FUNCTION (this << " Abdelrhman ----====> <second if > phyId " << id);
  m_cschedSapProvider2->CschedUeConfigReq (params);
  }
//-------------------------------------------------------------------------
  // Create DL trasmission HARQ buffers
  std::vector < Ptr<PacketBurst> > dlHarqLayer0pkt;
  dlHarqLayer0pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer0pkt.at (i) = pb;
    }
  std::vector < Ptr<PacketBurst> > dlHarqLayer1pkt;
  dlHarqLayer1pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer1pkt.at (i) = pb;
    }
  DlHarqProcessesBuffer_t buf;
  buf.push_back (dlHarqLayer0pkt);
  buf.push_back (dlHarqLayer1pkt); // multiple layers for mimo ???
  //-------------------------------------added---------------------------
  if(id==1)
    {NS_LOG_FUNCTION (this << " Abdelrhman ----====> <2nd if > phyId " << id);
	  m_miDlHarqProcessesPackets.insert (std::pair <uint16_t, DlHarqProcessesBuffer_t> (rnti, buf));
    }
   if(id==2)
    {
      m_miDlHarqProcessesPackets2.insert (std::pair <uint16_t, DlHarqProcessesBuffer_t> (rnti, buf));
    }

}
//-------------------------------------------------------------------------

//-----------------------------------------------done---------------------------
void
LteEnbMac::DoRemoveUe (uint16_t rnti,uint16_t id)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  NS_LOG_FUNCTION (this << " PhyId" << id);
  FfMacCschedSapProvider::CschedUeReleaseReqParameters params;
  params.m_rnti = rnti;
  params.m_phyId = id;
             //-----------------------------added--------------------------
  if(id==1)
  {
	  m_cschedSapProvider->CschedUeReleaseReq (params);
	  m_rlcAttached.erase (rnti);
	  m_miDlHarqProcessesPackets.erase (rnti);
  }
  if(id==2)
  {
  m_cschedSapProvider2->CschedUeReleaseReq (params);
  m_rlcAttached.erase (rnti);
  m_miDlHarqProcessesPackets2.erase (rnti);
  }
}
//-------------------------------------------------------------------------

//----------------------------done-----------------------------------------
void
LteEnbMac::DoAddLc (LteEnbCmacSapProvider::LcInfo lcinfo, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this );
  NS_LOG_FUNCTION (this <<"Abdelrhman -----=====>phyId" <<lcinfo.phyId);

  std::map <LteFlowId_t, LteMacSapUser* >::iterator it;
  
  LteFlowId_t flow (lcinfo.rnti,lcinfo.lcId);
  
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (lcinfo.rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "RNTI not found");
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcinfo.lcId);
  if (lcidIt == rntiIt->second.end ())
    {
      rntiIt->second.insert (std::pair<uint8_t, LteMacSapUser*> (lcinfo.lcId, msu));
    }
  else
    {
      NS_LOG_ERROR ("LC already exists");
    }

  // CCCH (LCID 0) is pre-configured 
  // see FF LTE MAC Scheduler
  // Interface Specification v1.11, 
  // 4.3.4 logicalChannelConfigListElement
  if (lcinfo.lcId != 0)
    {
	  NS_LOG_FUNCTION (this <<"Abdelrhman -----=====>LcId" <<lcinfo.lcId);
      struct FfMacCschedSapProvider::CschedLcConfigReqParameters params;
      params.m_rnti = lcinfo.rnti;
      params.m_phyId = lcinfo.phyId;
      params.m_reconfigureFlag = false;

      struct LogicalChannelConfigListElement_s lccle;
      lccle.m_logicalChannelIdentity = lcinfo.lcId;
      lccle.m_logicalChannelGroup = lcinfo.lcGroup;
      lccle.m_direction = LogicalChannelConfigListElement_s::DIR_BOTH;
      lccle.m_qosBearerType = lcinfo.isGbr ? LogicalChannelConfigListElement_s::QBT_GBR : LogicalChannelConfigListElement_s::QBT_NON_GBR;
      lccle.m_qci = lcinfo.qci;
      lccle.m_eRabMaximulBitrateUl = lcinfo.mbrUl;
      lccle.m_eRabMaximulBitrateDl = lcinfo.mbrDl;
      lccle.m_eRabGuaranteedBitrateUl = lcinfo.gbrUl;
      lccle.m_eRabGuaranteedBitrateDl = lcinfo.gbrDl;
      params.m_logicalChannelConfigList.push_back (lccle);

      //-----------------------------added---------------------------
      if(lcinfo.phyId==1)
       {
          m_cschedSapProvider->CschedLcConfigReq (params);
       }
      if(lcinfo.phyId==2)
       {
           m_cschedSapProvider2->CschedLcConfigReq (params);
       }
      //--------------------------------------------------------------
    }
}
//--------------------------------------------------------------------

//----------------------------------not implemented-----------------------
void
LteEnbMac::DoReconfigureLc (LteEnbCmacSapProvider::LcInfo lcinfo)
{
  NS_FATAL_ERROR ("not implemented");
}
//----------------------------------------------------------------------

//-------------------------------------------done------------------------
void
LteEnbMac::DoReleaseLc (uint16_t rnti, uint8_t lcid,uint16_t id)
{
  NS_LOG_FUNCTION (this);

  //Find user based on rnti and then erase lcid stored against the same
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  rntiIt->second.erase (lcid);

  struct FfMacCschedSapProvider::CschedLcReleaseReqParameters params;
  params.m_rnti = rnti;
  params.m_phyId = id;
  params.m_logicalChannelIdentity.push_back (lcid);
     //----------------------------added-------------------------------
  if(id==1)
  {
  m_cschedSapProvider->CschedLcReleaseReq (params);
  }
  if(id==2)
    {
    m_cschedSapProvider2->CschedLcReleaseReq (params);
    }
     //------------------------------------------------------------------
}
//------------------------------------------------------------------------

//--------------------------------------done----------------------------
void
LteEnbMac::DoUeUpdateConfigurationReq (LteEnbCmacSapProvider::UeConfig params)
{
  NS_LOG_FUNCTION (this);

  // propagates to scheduler
  FfMacCschedSapProvider::CschedUeConfigReqParameters req;
  req.m_rnti = params.m_rnti;
  req.m_phyId=params.m_phyId;
  req.m_transmissionMode = params.m_transmissionMode;
  req.m_reconfigureFlag = true;
  //--------------------------------added--------------------------
  if(params.m_phyId==1)
    {
	  m_cschedSapProvider->CschedUeConfigReq (req);
    }
    if(params.m_phyId==2)
    {
    	m_cschedSapProvider2->CschedUeConfigReq (req);
    }
  //---------------------------------------------------------------
}
//--------------------------------------------------------------------

//-------------------------------------not modified-------------------------
LteEnbCmacSapProvider::RachConfig 
LteEnbMac::DoGetRachConfig ()
{
  struct LteEnbCmacSapProvider::RachConfig rc;
  rc.numberOfRaPreambles = m_numberOfRaPreambles;
  rc.preambleTransMax = m_preambleTransMax;
  rc.raResponseWindowSize = m_raResponseWindowSize;
  return rc;
}
//---------------------------------------------------------------------------

//--------------------------------------------not modified---------------------------
LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue 
LteEnbMac::DoAllocateNcRaPreamble (uint16_t rnti)
{
  bool found = false;
  uint8_t preambleId;
  for (preambleId = m_numberOfRaPreambles; preambleId < 64; ++preambleId)
    {
      std::map<uint8_t, NcRaPreambleInfo>::iterator it = m_allocatedNcRaPreambleMap.find (preambleId);
      if ((it ==  m_allocatedNcRaPreambleMap.end ())
          || (it->second.expiryTime < Simulator::Now ()))
        {
          found = true;
          NcRaPreambleInfo preambleInfo;
          uint32_t expiryIntervalMs = (uint32_t) m_preambleTransMax * ((uint32_t) m_raResponseWindowSize + 5); 
          
          preambleInfo.expiryTime = Simulator::Now () + MilliSeconds (expiryIntervalMs);
          preambleInfo.rnti = rnti;
          NS_LOG_INFO ("allocated preamble for NC based RA: preamble " << preambleId << ", RNTI " << preambleInfo.rnti << ", exiryTime " << preambleInfo.expiryTime);
          m_allocatedNcRaPreambleMap[preambleId] = preambleInfo; // create if not exist, update otherwise
          break;
        }
    }
  LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue ret;
  if (found)
    {
      ret.valid = true;
      ret.raPreambleId = preambleId;
      ret.raPrachMaskIndex = 0;
    }
  else
    {
      ret.valid = false;
      ret.raPreambleId = 0;
      ret.raPrachMaskIndex = 0;
    }
  return ret;
}
//------------------------------------------------------------------------------


// ////////////////////////////////////////////
// MAC SAP
// ////////////////////////////////////////////

//----------------------------------------done-----------------------------------------
void
LteEnbMac::DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params)
{
  NS_LOG_FUNCTION (this);
  LteRadioBearerTag tag (params.rnti, params.lcid, params.layer); //,params.phyId);
  params.pdu->AddPacketTag (tag);
  if(params.phyId==1)
  {
	  // Store pkt in HARQ buffer
	    std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.rnti);
	    NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
	    NS_LOG_DEBUG (this << " LAYER " << (uint16_t)tag.GetLayer () << " HARQ ID " << (uint16_t)params.harqProcessId);

	    //(*it).second.at (params.layer).at (params.harqProcessId) = params.pdu;//->Copy ();
	    (*it).second.at (params.layer).at (params.harqProcessId)->AddPacket (params.pdu);
	    m_enbPhySapProvider->SendMacPdu (params.pdu);
  }
  if(params.phyId==2)
    {
  	  // Store pkt in HARQ buffer
  	    std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets2.find (params.rnti);
  	    NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
  	    NS_LOG_DEBUG (this << " LAYER 2" << (uint16_t)tag.GetLayer () << " HARQ ID " << (uint16_t)params.harqProcessId);

  	    //(*it).second.at (params.layer).at (params.harqProcessId) = params.pdu;//->Copy ();
  	    (*it).second.at (params.layer).at (params.harqProcessId)->AddPacket (params.pdu);
  	    m_enbPhySapProvider2->SendMacPdu (params.pdu);
    }
  
}
//--------------------------------------------------------------------------------------

//------------------------------------------done---------------------------------------
void
LteEnbMac::DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params)
{
  NS_LOG_FUNCTION (this);
  FfMacSchedSapProvider::SchedDlRlcBufferReqParameters req;
  if(params.phyId==1)
  {
  req.m_rnti = params.rnti;
  req.m_phyId = params.phyId;
  req.m_logicalChannelIdentity = params.lcid;
  req.m_rlcTransmissionQueueSize = params.txQueueSize;
  req.m_rlcTransmissionQueueHolDelay = params.txQueueHolDelay;
  req.m_rlcRetransmissionQueueSize = params.retxQueueSize;
  req.m_rlcRetransmissionHolDelay = params.retxQueueHolDelay;
  req.m_rlcStatusPduSize = params.statusPduSize;
  m_schedSapProvider->SchedDlRlcBufferReq (req);
  }
  if(params.phyId==2)
  {
  req.m_rnti = params.rnti;
  req.m_phyId = params.phyId;
  req.m_logicalChannelIdentity = params.lcid;
  req.m_rlcTransmissionQueueSize = params.txQueueSize;
  req.m_rlcTransmissionQueueHolDelay = params.txQueueHolDelay;
  req.m_rlcRetransmissionQueueSize = params.retxQueueSize;
  req.m_rlcRetransmissionHolDelay = params.retxQueueHolDelay;
  req.m_rlcStatusPduSize = params.statusPduSize;
  m_schedSapProvider2->SchedDlRlcBufferReq (req);
  }
}
//-----------------------------------------------------------------------------------


// ////////////////////////////////////////////
// SCHED SAP
// ////////////////////////////////////////////

//------------------------------------------------done----------------------------------------
//
//void
//LteEnbMac::DoSchedDlConfigInd (FfMacSchedSapUser::SchedDlConfigIndParameters ind)
//{
//  NS_LOG_FUNCTION (this  << " <Abdelrhman> -------======>  phyId" << ind.m_phyId);
//  // Create DL PHY PDU
//  Ptr<PacketBurst> pb = CreateObject<PacketBurst> ();
//  std::map <LteFlowId_t, LteMacSapUser* >::iterator it;
//
//  for (unsigned int i = 0; i < ind.m_buildDataList.size (); i++)
//    {
//      for (uint16_t layer = 0; layer < ind.m_buildDataList.at (i).m_dci.m_ndi.size (); layer++)
//        {
//          if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (layer) == 1)
//            {
//        	  if(ind.m_phyId==1)
//        	  {
//        		  // new data -> force emptying correspondent harq pkt buffer
//                  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
//                  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
//                  for (uint16_t lcId = 0; lcId < (*it).second.size (); lcId++)
//                    {
//                      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
//                      (*it).second.at (lcId).at (ind.m_buildDataList.at (i).m_dci.m_harqProcess) = pb;
//                    }
//        	  }
//                if(ind.m_phyId==2)
//                     {
//                      // new data -> force emptying correspondent harq pkt buffer
//                       std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets2.find (ind.m_buildDataList.at (i).m_rnti);
//                       NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
//                       for (uint16_t lcId = 0; lcId < (*it).second.size (); lcId++)
//                           {
//                             Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
//                             (*it).second.at (lcId).at (ind.m_buildDataList.at (i).m_dci.m_harqProcess) = pb;
//                           }
//        	          }
//        }
//      for (unsigned int j = 0; j < ind.m_buildDataList.at (i).m_rlcPduList.size (); j++)
//        {
//          for (uint16_t k = 0; k < ind.m_buildDataList.at (i).m_rlcPduList.at (j).size (); k++)
//            {
//              if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (k) == 1)
//                {
//                  // New Data -> retrieve it from RLC
//                  uint16_t rnti = ind.m_buildDataList.at (i).m_rnti;
//                  uint8_t lcid = ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_logicalChannelIdentity;
//                  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
//                  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
//                  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
//                  NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);
//                  NS_LOG_DEBUG (this << " rnti= " << rnti << " lcid= " << (uint32_t) lcid << " layer= " << k);
//                  (*lcidIt).second->NotifyTxOpportunity (ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_size, k, ind.m_buildDataList.at (i).m_dci.m_harqProcess);
//                }
//              else
//                {
//                  if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (k) > 0)
//                    {
//                	  if(ind.m_phyId==1)
//                	  {
//                		  // HARQ retransmission -> retrieve TB from HARQ buffer
//                        std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
//                        NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
//                        Ptr<PacketBurst> pb = (*it).second.at (k).at ( ind.m_buildDataList.at (i).m_dci.m_harqProcess);
//                        for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
//                          {
//                            Ptr<Packet> pkt = (*j)->Copy ();
//                            m_enbPhySapProvider->SendMacPdu (pkt);
//                          }
//                	  }
//                	  if(ind.m_phyId==2)
//                	     {
//                	       // HARQ retransmission -> retrieve TB from HARQ buffer
//                	      std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets2.find (ind.m_buildDataList.at (i).m_rnti);
//                	      NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
//                	      Ptr<PacketBurst> pb = (*it).second.at (k).at ( ind.m_buildDataList.at (i).m_dci.m_harqProcess);
//                	        for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
//                	              {
//                	                 Ptr<Packet> pkt = (*j)->Copy ();
//                	                    m_enbPhySapProvider2->SendMacPdu (pkt);
//                	               }
//                	           }
//
//                    }
//                }
//            }
//        }
//      if(ind.m_phyId==1)
//      {// send the relative DCI
//          Ptr<DlDciLteControlMessage> msg = Create<DlDciLteControlMessage> ();
//          msg->SetDci (ind.m_buildDataList.at (i).m_dci);
//          m_enbPhySapProvider->SendLteControlMessage (msg);
//      }
//      if(ind.m_phyId==2)
//            {// send the relative DCI
//                Ptr<DlDciLteControlMessage> msg2 = Create<DlDciLteControlMessage> ();
//                msg2->SetDci (ind.m_buildDataList.at (i).m_dci);
//                m_enbPhySapProvider2->SendLteControlMessage (msg2);
//            }
//
//    }
//
//  // Fire the trace with the DL information
//  for (  uint32_t i  = 0; i < ind.m_buildDataList.size (); i++ )
//    {
//      // Only one TB used
//      if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 1)
//        {
//          m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
//                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
//                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
//                          0, 0
//                          );
//
//        }
//      // Two TBs used
//      else if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 2)
//        {
//          m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
//                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
//                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
//                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (1),
//                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (1)
//                          );
//        }
//      else
//        {
//          NS_FATAL_ERROR ("Found element with more than two transport blocks");
//        }
//    }
//
//  // Random Access procedure: send RARs
//  Ptr<RarLteControlMessage> rarMsg = Create<RarLteControlMessage> ();
//  // see TS 36.321 5.1.4;  preambles were sent two frames ago
//  // (plus 3GPP counts subframes from 0, not 1)
//  uint16_t raRnti;
//  if (m_subframeNo < 3)
//    {
//      raRnti = m_subframeNo + 7; // equivalent to +10-3
//    }
//  else
//    {
//      raRnti = m_subframeNo - 3;
//    }
//  rarMsg->SetRaRnti (raRnti);
//  for (unsigned int i = 0; i < ind.m_buildRarList.size (); i++)
//  {
//	  if(ind.m_phyId==1)
//	  {
//		  std::map <uint8_t, uint32_t>::iterator itRapId = m_rapIdRntiMap.find (ind.m_buildRarList.at (i).m_rnti);
//       if (itRapId == m_rapIdRntiMap.end ())
//         {
//           NS_FATAL_ERROR ("Unable to find rapId of RNTI " << ind.m_buildRarList.at (i).m_rnti);
//         }
//     RarLteControlMessage::Rar rar;
//     rar.rapId = itRapId->second;
//     rar.rarPayload = ind.m_buildRarList.at (i);
//     rarMsg->AddRar (rar);
//     NS_LOG_INFO (this << " Send RAR message to RNTI " << ind.m_buildRarList.at (i).m_rnti << " rapId " << itRapId->second);
//	  }
//	  if(ind.m_phyId==2)
//	 	  {
//	 		  std::map <uint8_t, uint32_t>::iterator itRapId = m_rapIdRntiMap2.find (ind.m_buildRarList.at (i).m_rnti);
//	        if (itRapId == m_rapIdRntiMap2.end ())
//	          {
//	            NS_FATAL_ERROR ("Unable to find rapId of RNTI " << ind.m_buildRarList.at (i).m_rnti);
//	          }
//	      RarLteControlMessage::Rar rar;
//	      rar.rapId = itRapId->second;
//	      rar.rarPayload = ind.m_buildRarList.at (i);
//	      rarMsg->AddRar (rar);
//	      NS_LOG_INFO (this << " Send RAR message to RNTI " << ind.m_buildRarList.at (i).m_rnti << " rapId " << itRapId->second);
//	 	  }
//    }
//  if (ind.m_buildRarList.size () > 0 && ind.m_phyId==1)
//    {
//      m_enbPhySapProvider->SendLteControlMessage (rarMsg);
//    }
//  if (ind.m_buildRarList.size () > 0 && ind.m_phyId==2)
//      {
//        m_enbPhySapProvider2->SendLteControlMessage (rarMsg);
//      }
//
//}
//  if(ind.m_phyId==1)
//    {
//    m_rapIdRntiMap.clear ();
//    }
//  if(ind.m_phyId==2)
//    {
//    m_rapIdRntiMap2.clear ();
//    }
//}
//-------------------------------------------done------------------------------

void
LteEnbMac::DoSchedDlConfigInd (FfMacSchedSapUser::SchedDlConfigIndParameters ind) //ind comes from Round Robin
{
  NS_LOG_FUNCTION (this << " PhyId " << ind.m_phyId);
  // Create DL PHY PDU
  Ptr<PacketBurst> pb = CreateObject<PacketBurst> ();
  std::map <LteFlowId_t, LteMacSapUser* >::iterator it; 	//LteMacSapUser connection with RLC

  if(ind.m_phyId == 1)
  {
	  NS_LOG_FUNCTION("<mohamed> Phy 1 <mohamed>");
  for (unsigned int i = 0; i < ind.m_buildDataList.size (); i++)
      {
	    for (uint16_t layer = 0; layer < ind.m_buildDataList.at (i).m_dci.m_ndi.size (); layer++)
          {
            if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (layer) == 1)
              {
                // new data -> force emptying correspondent harq pkt buffer
          	  // <mohamed> it da ele hwa bymlah fe el 2a5r f mmkn n7tag mno etnen
                std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
                NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
                for (uint16_t lcId = 0; lcId < (*it).second.size (); lcId++)
                  {
                    Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
                    (*it).second.at (lcId).at (ind.m_buildDataList.at (i).m_dci.m_harqProcess) = pb;
                  }
              }
          }
        for (unsigned int j = 0; j < ind.m_buildDataList.at (i).m_rlcPduList.size (); j++)
          {
            for (uint16_t k = 0; k < ind.m_buildDataList.at (i).m_rlcPduList.at (j).size (); k++)
              {
                if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (k) == 1)
                  {
                    // New Data -> retrieve it from RLC
                    uint16_t rnti = ind.m_buildDataList.at (i).m_rnti;
                    uint8_t lcid = ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_logicalChannelIdentity;
                    std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
                    NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
                    std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
                    NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);
                    NS_LOG_DEBUG (this << " rnti= " << rnti << " lcid= " << (uint32_t) lcid << " layer= " << k);
                    (*lcidIt).second->NotifyTxOpportunity (ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_size, k, ind.m_buildDataList.at (i).m_dci.m_harqProcess);
                  }
                else
                  {
                    if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (k) > 0)
                      {
                        // HARQ retransmission -> retrieve TB from HARQ buffer
                        std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
                        NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
                        Ptr<PacketBurst> pb = (*it).second.at (k).at ( ind.m_buildDataList.at (i).m_dci.m_harqProcess);
                        for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
                          {
                            Ptr<Packet> pkt = (*j)->Copy ();
                            m_enbPhySapProvider->SendMacPdu (pkt);
                          }
                      }
                  }
              }
          }
        // send the relative DCI
        Ptr<DlDciLteControlMessage> msg = Create<DlDciLteControlMessage> ();
        msg->SetDci (ind.m_buildDataList.at (i).m_dci);
        m_enbPhySapProvider->SendLteControlMessage (msg);
      }

    // Fire the trace with the DL information
    for (  uint32_t i  = 0; i < ind.m_buildDataList.size (); i++ )
      {
    	NS_LOG_FUNCTION("<mohamed> ------------- 5 ------------- <mohamed>");
        // Only one TB used
        if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 1)
          {
            m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
                            ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                            ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                            0, 0
                            );

          }
        // Two TBs used
        else if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 2)
          {
            m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
                            ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                            ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                            ind.m_buildDataList.at (i).m_dci.m_mcs.at (1),
                            ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (1)
                            );
          }
        else
          {
            NS_FATAL_ERROR ("Found element with more than two transport blocks");
          }
      }
    // Random Access procedure: send RARs
    Ptr<RarLteControlMessage> rarMsg = Create<RarLteControlMessage> ();
    // see TS 36.321 5.1.4;  preambles were sent two frames ago
    // (plus 3GPP counts subframes from 0, not 1)
    uint16_t raRnti;
    if (m_subframeNo < 3)
      {
        raRnti = m_subframeNo + 7; // equivalent to +10-3
      }
    else
      {
        raRnti = m_subframeNo - 3;
      }
    rarMsg->SetRaRnti (raRnti);
    for (unsigned int i = 0; i < ind.m_buildRarList.size (); i++)
      {
        std::map <uint8_t, uint32_t>::iterator itRapId = m_rapIdRntiMap.find (ind.m_buildRarList.at (i).m_rnti);
        if (itRapId == m_rapIdRntiMap.end ())
          {
            NS_FATAL_ERROR ("Unable to find rapId of RNTI " << ind.m_buildRarList.at (i).m_rnti);
          }
        RarLteControlMessage::Rar rar;
        rar.rapId = itRapId->second;
        rar.rarPayload = ind.m_buildRarList.at (i);
        rarMsg->AddRar (rar);
        NS_LOG_INFO (this << " Send RAR message to RNTI " << ind.m_buildRarList.at (i).m_rnti << " rapId " << itRapId->second);
      }
    if (ind.m_buildRarList.size () > 0)
      {
        m_enbPhySapProvider->SendLteControlMessage (rarMsg);
      }
    m_rapIdRntiMap.clear ();
  }
  else if(ind.m_phyId == 2)
  {
	  for (unsigned int i = 0; i < ind.m_buildDataList.size (); i++)
	      {
	        for (uint16_t layer = 0; layer < ind.m_buildDataList.at (i).m_dci.m_ndi.size (); layer++)
	          {
	            if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (layer) == 1)
	              {
	                // new data -> force emptying correspondent harq pkt buffer
	          	  // <mohamed> it da ele hwa bymlah fe el 2a5r f mmkn n7tag mno etnen
	                std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets2.find (ind.m_buildDataList.at (i).m_rnti);
	                NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
	                for (uint16_t lcId = 0; lcId < (*it).second.size (); lcId++)
	                  {
	                    Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
	                    (*it).second.at (lcId).at (ind.m_buildDataList.at (i).m_dci.m_harqProcess) = pb;
	                  }
	              }
	          }
	        for (unsigned int j = 0; j < ind.m_buildDataList.at (i).m_rlcPduList.size (); j++)
	          {
	            for (uint16_t k = 0; k < ind.m_buildDataList.at (i).m_rlcPduList.at (j).size (); k++)
	              {
	                if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (k) == 1)
	                  {
	                    // New Data -> retrieve it from RLC
	                    uint16_t rnti = ind.m_buildDataList.at (i).m_rnti;
	                    uint8_t lcid = ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_logicalChannelIdentity;
	                    std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
	                    NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
	                    std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
	                    NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);
	                    NS_LOG_DEBUG (this << " rnti= " << rnti << " lcid= " << (uint32_t) lcid << " layer= " << k);
	                    (*lcidIt).second->NotifyTxOpportunity (ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_size, k, ind.m_buildDataList.at (i).m_dci.m_harqProcess);
	                  }
	                else
	                  {
	                    if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (k) > 0)
	                      {
	                        // HARQ retransmission -> retrieve TB from HARQ buffer
	                        std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets2.find (ind.m_buildDataList.at (i).m_rnti);
	                        NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
	                        Ptr<PacketBurst> pb = (*it).second.at (k).at ( ind.m_buildDataList.at (i).m_dci.m_harqProcess);
	                        for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
	                          {
	                            Ptr<Packet> pkt = (*j)->Copy ();
	                            m_enbPhySapProvider2->SendMacPdu (pkt);
	                          }
	                      }
	                  }
	              }
	          }
	        // send the relative DCI
	        Ptr<DlDciLteControlMessage> msg = Create<DlDciLteControlMessage> ();
	        msg->SetDci (ind.m_buildDataList.at (i).m_dci);
	        m_enbPhySapProvider2->SendLteControlMessage (msg);
	      }

	    // Fire the trace with the DL information
	    for (  uint32_t i  = 0; i < ind.m_buildDataList.size (); i++ )
	      {
	        // Only one TB used
	        if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 1)
	          {
	            m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
	                            ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
	                            ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
	                            0, 0
	                            );

	          }
	        // Two TBs used
	        else if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 2)
	          {
	            m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
	                            ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
	                            ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
	                            ind.m_buildDataList.at (i).m_dci.m_mcs.at (1),
	                            ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (1)
	                            );
	          }
	        else
	          {
	            NS_FATAL_ERROR ("Found element with more than two transport blocks");
	          }
	      }

	    // Random Access procedure: send RARs
	    Ptr<RarLteControlMessage> rarMsg = Create<RarLteControlMessage> ();
	    // see TS 36.321 5.1.4;  preambles were sent two frames ago
	    // (plus 3GPP counts subframes from 0, not 1)
	    uint16_t raRnti;
	    if (m_subframeNo < 3)
	      {
	        raRnti = m_subframeNo + 7; // equivalent to +10-3
	      }
	    else
	      {
	        raRnti = m_subframeNo - 3;
	      }
	    rarMsg->SetRaRnti (raRnti);
	    for (unsigned int i = 0; i < ind.m_buildRarList.size (); i++)
	      {
	        std::map <uint8_t, uint32_t>::iterator itRapId = m_rapIdRntiMap2.find (ind.m_buildRarList.at (i).m_rnti);
	        if (itRapId == m_rapIdRntiMap2.end ())
	          {
	            NS_FATAL_ERROR ("Unable to find rapId of RNTI " << ind.m_buildRarList.at (i).m_rnti);
	          }
	        RarLteControlMessage::Rar rar;
	        rar.rapId = itRapId->second;
	        rar.rarPayload = ind.m_buildRarList.at (i);
	        rarMsg->AddRar (rar);
	        NS_LOG_INFO (this << " Send RAR message to RNTI " << ind.m_buildRarList.at (i).m_rnti << " rapId " << itRapId->second);
	      }
	    if (ind.m_buildRarList.size () > 0)
	      {
	        m_enbPhySapProvider2->SendLteControlMessage (rarMsg);
	      }
	    m_rapIdRntiMap2.clear ();
  }
  else
  {
	  std::cerr << this << "LteEnbMac::DoSchedDlConfigInd --- phy Id isn't set correctly -> phyId = " << ind.m_phyId << std::endl;
  }

}


void
LteEnbMac::DoSchedUlConfigInd (FfMacSchedSapUser::SchedUlConfigIndParameters ind)
{
  NS_LOG_FUNCTION (this << "<Abdelrhman ---=== phyId>" <<ind.m_phyId);

  for (unsigned int i = 0; i < ind.m_dciList.size (); i++)
    {
      // send the correspondent ul dci
      Ptr<UlDciLteControlMessage> msg = Create<UlDciLteControlMessage> ();
      msg->SetDci (ind.m_dciList.at (i));
      if(ind.m_phyId==1)
      {
      m_enbPhySapProvider->SendLteControlMessage (msg);
      }
      if(ind.m_phyId==2)
      {
      m_enbPhySapProvider2->SendLteControlMessage (msg);
      }
    }

  // Fire the trace with the UL information
  for (  uint32_t i  = 0; i < ind.m_dciList.size (); i++ )
    {
      m_ulScheduling (m_frameNo, m_subframeNo, ind.m_dciList.at (i).m_rnti,
                      ind.m_dciList.at (i).m_mcs, ind.m_dciList.at (i).m_tbSize);
    }
}
//-------------------------------------------------------------------------------------------



// ////////////////////////////////////////////
// CSCHED SAP
// ////////////////////////////////////////////


void
LteEnbMac::DoCschedCellConfigCnf (FfMacCschedSapUser::CschedCellConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedUeConfigCnf (FfMacCschedSapUser::CschedUeConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedLcConfigCnf (FfMacCschedSapUser::CschedLcConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
  // Call the CSCHED primitive
  // m_cschedSap->LcConfigCompleted();
}

void
LteEnbMac::DoCschedLcReleaseCnf (FfMacCschedSapUser::CschedLcReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedUeReleaseCnf (FfMacCschedSapUser::CschedUeReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedUeConfigUpdateInd (FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this << "<Abdelrhman>----=====> phyid" << params.m_phyId );
  // propagates to RRC
  LteEnbCmacSapUser::UeConfig ueConfigUpdate;
  ueConfigUpdate.m_rnti = params.m_rnti;
  ueConfigUpdate.m_phyId = params.m_phyId;
  ueConfigUpdate.m_transmissionMode = params.m_transmissionMode;
  m_cmacSapUser->RrcConfigurationUpdateInd (ueConfigUpdate);
}

void
LteEnbMac::DoCschedCellConfigUpdateInd (FfMacCschedSapUser::CschedCellConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
}

//-------------------------------------done------------------------------------------
void
LteEnbMac::DoUlInfoListElementHarqFeeback (UlInfoListElement_s params)
{
  NS_LOG_FUNCTION (this  << "Abdelrhman --=== > phyid"  << params.m_phyId);
  if(params.m_phyId==1)
  {
	  m_ulInfoListReceived.push_back (params);
  }
  if(params.m_phyId==2)
    {
  	  m_ulInfoListReceived2.push_back (params);
    }
}
//---------------------------------------------------------------------------------
//-------------------------------done-------------------------------
void
LteEnbMac::DoDlInfoListElementHarqFeeback (DlInfoListElement_s params)
{
  NS_LOG_FUNCTION (this   << "Abdelrhman --=== > phyid"  << params.m_phyId);
  if(params.m_phyId==1)
  {
  // Update HARQ buffer
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.m_rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
  for (uint8_t layer = 0; layer < params.m_harqStatus.size (); layer++)
    {
      if (params.m_harqStatus.at (layer) == DlInfoListElement_s::ACK)
        {
          // discard buffer
          Ptr<PacketBurst> emptyBuf = CreateObject <PacketBurst> ();
          (*it).second.at (layer).at (params.m_harqProcessId) = emptyBuf;
          NS_LOG_DEBUG (this << " HARQ-ACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
        }
      else if (params.m_harqStatus.at (layer) == DlInfoListElement_s::NACK)
        {
          NS_LOG_DEBUG (this << " HARQ-NACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
        }
      else
        {
          NS_FATAL_ERROR (" HARQ functionality not implemented");
        }
    }

  m_dlInfoListReceived.push_back (params);
  }
  if(params.m_phyId==2)
    {
    // Update HARQ buffer
    std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets2.find (params.m_rnti);
    NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
    for (uint8_t layer = 0; layer < params.m_harqStatus.size (); layer++)
      {
        if (params.m_harqStatus.at (layer) == DlInfoListElement_s::ACK)
          {
            // discard buffer
            Ptr<PacketBurst> emptyBuf = CreateObject <PacketBurst> ();
            (*it).second.at (layer).at (params.m_harqProcessId) = emptyBuf;
            NS_LOG_DEBUG (this << " HARQ-ACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
          }
        else if (params.m_harqStatus.at (layer) == DlInfoListElement_s::NACK)
          {
            NS_LOG_DEBUG (this << " HARQ-NACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
          }
        else
          {
            NS_FATAL_ERROR (" HARQ functionality not implemented");
          }
      }

    m_dlInfoListReceived2.push_back (params);
    }
}


} // namespace ns3..
