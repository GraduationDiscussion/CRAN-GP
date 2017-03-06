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
//badria
//register an object subclass with the same type of LteEnbMac i.e type elegwa el () y3ni.
// fe 7ga esmha registeration class dayman by create an object meno we type bt3o 3la 7sab elobject hna kan lteEnbMac
//we yebtedi ye4t33'al bel object dah
//end



// //////////////////////////////////////
// ------------member SAP forwarders
// //////////////////////////////////////


class EnbMacMemberLteEnbCmacSapProvider : public LteEnbCmacSapProvider
{
//badria
//inherited from CMac sap
//the part of the SAP that contains the MAC methods called by the RRC 
//y3ni dah elSAP ele ben elMac wel RRC
//eh ele5lnah 3refna enha lel RRC m4 lel fu2eha ele heya RLC
//34an feha C abl mac i.e. ay 7ga ablha C zay Cphy masln Wasla lel RRC 3latol
//end
public:
  EnbMacMemberLteEnbCmacSapProvider (LteEnbMac* mac);
//badria
//constructor
//LteEnbMac dah class by implement elMac layer 
//hna 5ad meno pointer we pass it lel constructer 
//gwa elconstructor m_mac(mac) 
//end
  // inherited from LteEnbCmacSapProvider
  virtual void ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth);
//by5ud el uplink wel downlink bw we ye configure beh el mac ele kan mepasseha lel constructor
  virtual void AddUe (uint16_t rnti);
//Radio Network Temporary Identifier. RNTIs are used to differentiate/identify a connected mode UE in the cell
//elfn deh bt5ud el rnti dah we  ted5ul fn tanya gwa elmac : m_mac->DoAddUe (rnti); we tezwed el ue dah 
  virtual void RemoveUe (uint16_t rnti);
//nfs Addue bs btmasa7 ele ue ele leha rnti dah:m_mac->DoRemoveUe (rnti);
  virtual void AddLc (LcInfo lcinfo,LcInfo lcinfo2, LteMacSapUser* msu);
//by add logical channel fa by5ud el 2 saps as a parameters:"LteEnbCmacSapProvider::LcInfo  	lcinfo","LteMacSapUser *  msu"
//eh huma fe word esmo logical ch. 
  virtual void ReconfigureLc (LcInfo lcinfo,LcInfo lcinfo);
//by reconfigure of existing Lc fa bypass leh el sap provider bs
// dah ele gwaha :m_mac->DoReconfigureLc (lcinfo);we gwa dorecon:NS_FATAL_ERROR ("not implemented");
//hwa 3'lban kda byd5ul el fn yetked enha implemented wla l2 we m4 fahmaha awi
  virtual void ReleaseLc (uint16_t rnti, uint8_t lcid,uint16_t rnti2, uint8_t lcid2);
//Find user based on rnti and then erase lcid stored against the same
// kman byru7 ye3rad el cscheed provider sap en el rnti dah bel lcid dah 7aslhum relesase we bysgelo 3ndo
  virtual void UeUpdateConfigurationReq (UeConfig params,UeConfig params2);
//propagates to scheduler bm3na by5ud mn el params deh 1.rnti wel 2.transmission mode we by5ali el 3.reconfiguration flag true
//we yeru7 ye3rfha lel Cshed  3n tre2 elsatr dah: m_cschedSapProvider->CschedUeConfigReq (req);
//note req dah object 4yel el info ele fu2
  virtual RachConfig GetRachConfig ();
//gwaha:return m_mac->DoGetRachConfig ()
//dogetconf fn  bt return object esmo rc "RachConf" ele by carry 3 info: 1.number of preambles 2.preamble trans. max 3.ra response widow size
  virtual AllocateNcRaPreambleReturnValue AllocateNcRaPreamble (uint16_t rnti);
//Allocate a random access preamble for non-contention based random access (e.g., for handover)
//btd5ul te3mel for loop 3al 64 preambles btu3ha te4uf ya2ema elfadi aw w7ed yekun el expire time bta3o 5les 
//lw la2et bt5li var esmo found true we te5rug te check found dah true wla l2 lw true bt return object fe:
//valid=true .. ret.raPreambleId = preambleId (btba3t elpreamble ele la2eto ..ret.raPrachMaskIndex = 0(m4 3rfa eh dah : D)
// lw false hyb2a valid = false  raPreambleId = 0, we 25er w7ed zay ma hwa

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
EnbMacMemberLteEnbCmacSapProvider::AddUe (uint16_t rnti)
{
  m_mac->DoAddUe (rnti);
}

void
EnbMacMemberLteEnbCmacSapProvider::RemoveUe (uint16_t rnti)
{
  m_mac->DoRemoveUe (rnti);
}

void
EnbMacMemberLteEnbCmacSapProvider::AddLc (LcInfo lcinfo,LcInfo lcinfo2, LteMacSapUser* msu)
{
  m_mac->DoAddLc (lcinfo,lcinfo, msu);
}

void
EnbMacMemberLteEnbCmacSapProvider::ReconfigureLc (LcInfo lcinfo,LcInfo lcinfo2)
{
  m_mac->DoReconfigureLc (lcinfo, lcinfo2);
}

void
EnbMacMemberLteEnbCmacSapProvider::ReleaseLc (uint16_t rnti, uint8_t lcid,uint16_t rnti2, uint8_t lcid2)
{
  m_mac->DoReleaseLc (rnti, lcid,rnti2, lcid2);
}

void
EnbMacMemberLteEnbCmacSapProvider::UeUpdateConfigurationReq (UeConfig params,UeConfig params2)
{
  m_mac->DoUeUpdateConfigurationReq (params,params2);
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

// --------------schedular sap user------------

class EnbMacMemberFfMacSchedSapUser : public FfMacSchedSapUser
{
public:
  EnbMacMemberFfMacSchedSapUser (LteEnbMac* mac);//constructor


  virtual void SchedDlConfigInd (const struct SchedDlConfigIndParameters& params,const struct SchedDlConfigIndParameters& params2);
//m7mad hay5las we ye2ulha
  virtual void SchedUlConfigInd (const struct SchedUlConfigIndParameters& params,const struct SchedUlConfigIndParameters& params2);
//by loop 3la dci list size we ye set elvalues bta3thum we yeb3atha lel phy sap (btb2a control msg)
//we by loop tani 3al size  dah we byb3at lel ul scheduling frame and sub frame numbers wel rnti w carrierComponentId we Tb_size
// w mcs(modulation and coding scheme)
private:
  LteEnbMac* m_mac;
};


EnbMacMemberFfMacSchedSapUser::EnbMacMemberFfMacSchedSapUser (LteEnbMac* mac)
  : m_mac (mac)
{
}


void
EnbMacMemberFfMacSchedSapUser::SchedDlConfigInd (const struct SchedDlConfigIndParameters& params,const struct SchedDlConfigIndParameters& params2)
{
  m_mac->DoSchedDlConfigInd (params,params2);
}



void
EnbMacMemberFfMacSchedSapUser::SchedUlConfigInd (const struct SchedUlConfigIndParameters& params,const struct SchedUlConfigIndParameters& params2)
{
  m_mac->DoSchedUlConfigInd (params,params2);
}

/*--------------Csched sap user*/

class EnbMacMemberFfMacCschedSapUser : public FfMacCschedSapUser
{
public:
  EnbMacMemberFfMacCschedSapUser (LteEnbMac* mac);

  virtual void CschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params,const struct CschedCellConfigCnfParameters& params2);
  virtual void CschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params,const struct CschedUeConfigCnfParameters& params2);
  virtual void CschedLcConfigCnf (const struct CschedLcConfigCnfParameters& params,const struct CschedLcConfigCnfParameters& params2);
  virtual void CschedLcReleaseCnf (const struct CschedLcReleaseCnfParameters& params,const struct CschedLcReleaseCnfParameters& params);
  virtual void CschedUeReleaseCnf (const struct CschedUeReleaseCnfParameters& params,const struct CschedUeReleaseCnfParameters& params2);
  virtual void CschedUeConfigUpdateInd (const struct CschedUeConfigUpdateIndParameters& params,const struct CschedUeConfigUpdateIndParameters& params2);
  virtual void CschedCellConfigUpdateInd (const struct CschedCellConfigUpdateIndParameters& params,const struct CschedCellConfigUpdateIndParameters& params2);

private:
  LteEnbMac* m_mac;
};


EnbMacMemberFfMacCschedSapUser::EnbMacMemberFfMacCschedSapUser (LteEnbMac* mac)
  : m_mac (mac)
{
}

void
EnbMacMemberFfMacCschedSapUser::CschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params,const struct CschedCellConfigCnfParameters& params2)
{
//ele by7sal 72e2i fel program m4 eno byb3at lel cshed hwa by5ud el params deh we ye print-ha
  m_mac->DoCschedCellConfigCnf (params,params2);
}

void
EnbMacMemberFfMacCschedSapUser::CschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params,const struct CschedUeConfigCnfParameters& params2)
{
//nfs elklam byprint elparams bs hna bta3t el UE
  m_mac->DoCschedUeConfigCnf (params,params2);
}

void
EnbMacMemberFfMacCschedSapUser::CschedLcConfigCnf (const struct CschedLcConfigCnfParameters& params,const struct CschedLcConfigCnfParameters& params2)
{
//by print bs brdo bs hna fel site 3mel coomment le cmd :m_cschedSap->LcConfigCompleted()
//by call beha csched primitive
  m_mac->DoCschedLcConfigCnf (params,params2);
}

void
EnbMacMemberFfMacCschedSapUser::CschedLcReleaseCnf (const struct CschedLcReleaseCnfParameters& params,const struct CschedLcReleaseCnfParameters& params2)
{
//print only
  m_mac->DoCschedLcReleaseCnf (params,params2);
}

void
EnbMacMemberFfMacCschedSapUser::CschedUeReleaseCnf (const struct CschedUeReleaseCnfParameters& params,const struct CschedUeReleaseCnfParameters& params2)
{
//print only
  m_mac->DoCschedUeReleaseCnf (params,params2);
}

void
EnbMacMemberFfMacCschedSapUser::CschedUeConfigUpdateInd (const struct CschedUeConfigUpdateIndParameters& params,const struct CschedUeConfigUpdateIndParameters& params2)
{
//hna b2a by print brdo plus by propagates this info to RRC by doing the following
//create object (ueConfigUpdate) its type is:LteEnbCmacSapUser::UeConfig
//ya5ud el rnti wel transmission mode we yeb3ato lel fn deh:>RrcConfigurationUpdateInd (ele bt notify the rrc of a ue config updated
// requested by the mac << we b3den te5li el cmac sap user ye4wer 3la elfn deh 
  m_mac->DoCschedUeConfigUpdateInd (params,params2);
}

void
EnbMacMemberFfMacCschedSapUser::CschedCellConfigUpdateInd (const struct CschedCellConfigUpdateIndParameters& params,const struct CschedCellConfigUpdateIndParameters& params2)
{
//print only
  m_mac->DoCschedCellConfigUpdateInd (params,params2);
}

// ---------- PHY-SAP------------------

//no changes


class EnbMacMemberLteEnbPhySapUser : public LteEnbPhySapUser
{
public:
  EnbMacMemberLteEnbPhySapUser (LteEnbMac* mac);

  // inherited from LteEnbPhySapUser
  virtual void ReceivePhyPdu (Ptr<Packet> p,Ptr<Packet> p2);
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  virtual void ReceiveLteControlMessage (Ptr<LteControlMessage> msg,Ptr<LteControlMessage> msg2);
  virtual void ReceiveRachPreamble (uint32_t prachId,uint32_t prachId2);
  virtual void UlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi,FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi2);//cqi fkstlha
  virtual void UlInfoListElementHarqFeeback (UlInfoListElement_s params,UlInfoListElement_s params2);
  virtual void DlInfoListElementHarqFeeback (DlInfoListElement_s params,DlInfoListElement_s params2);

private:
  LteEnbMac* m_mac;
};

EnbMacMemberLteEnbPhySapUser::EnbMacMemberLteEnbPhySapUser (LteEnbMac* mac)
: m_mac (mac)
{
}


void
EnbMacMemberLteEnbPhySapUser::ReceivePhyPdu (Ptr<Packet> p,Ptr<Packet> p2)
{
//called by PHY to notify mac the reception of a new pdu
  m_mac->DoReceivePhyPdu (p,p2);
}

void
EnbMacMemberLteEnbPhySapUser::SubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
//1.byb3at lel scheduler cqi we byzwedha fel cqi list ul & dl
//2.bt4uf eltrigger (34an teb2a synchronised) we bt4uf elpreamble (DL)
//3. Send BSR reports to the scheduler (BSR:buffer status report0 UL
//4.Get uplink transmission opportunities (by3rfak el subframes we frame ele ue hayb3a 3lehum)
//we kman by3rfo TTI delay mac ch delay
//5.Forward DL HARQ feebacks collected during last TTI (y3ni by2ra harq feedbaks we ye7tha fe ul param ele byb3tha lel scheduler 
  m_mac->DoSubframeIndication (frameNo, subframeNo);
}

void
EnbMacMemberLteEnbPhySapUser::ReceiveLteControlMessage (Ptr<LteControlMessage> msg,Ptr<LteControlMessage> msg2)
{
// check the type of control msg
//if >> cqi then >> hay3mel pointer ye4wer 3la elmsg deh we b3den yeb3tha le: ReceiveDlCqiLteControlMessage 
//if >>BSR then >>hay3mel pointer ye4wer 3la elmsg deh we b3den yeb3tha le ReceiveBsrMessage
//if >>dl harq >>hay3mel pointer ye4wer 3la elmsg deh we b3den yeb3tha le:DoDlInfoListElementHarqFeeback
//note: Dynamic cast by2nel pointer mn elclass elkber lel inherited mno

			 m_mac->DoReceiveLteControlMessage (msg);
		     m_mac->DoReceiveLteControlMessage (msg2);
}

void
EnbMacMemberLteEnbPhySapUser::ReceiveRachPreamble (uint32_t prachId,uint32_t prachId2)
{
//just record that the preamble has been received we byzwed elcounter f 7alet n l preamble da msh mwgood abl kda  ( 3shn s3at bykonn mwgood
//abl kda bs not used dlw2ti) 
  m_mac->DoReceiveRachPreamble (prachId, prachId2);
}

void
EnbMacMemberLteEnbPhySapUser::UlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi,FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi2)
{
//i7na hanb3at el ulcqi lel:m_ulCqiReceived
  m_mac->DoUlCqiReport (ulcqi,ulcqi2);
}

void
EnbMacMemberLteEnbPhySapUser::UlInfoListElementHarqFeeback (UlInfoListElement_s params,UlInfoListElement_s params2)
{

 m_mac->DoUlInfoListElementHarqFeeback (params,params2);
}

void
EnbMacMemberLteEnbPhySapUser::DlInfoListElementHarqFeeback (DlInfoListElement_s params,DlInfoListElement_s params2)
{
 m_mac->DoDlInfoListElementHarqFeeback (params,params2);
}


// //////////////////////////////////////
// generic LteEnbMac methods
// //////////////////////////////////////
//----------------------- not modified--------------------------------

TypeId
LteEnbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteEnbMac")
    .SetParent<Object> ()
    .SetGroupName("Lte")
    .AddConstructor<LteEnbMac> ()
	//addAttribute deh y3ni:see also struct typeid
	/**
	   * Record in this TypeId the fact that a new attribute exists.
	   *
	   * \param [in] name The name of the new attribute
	   * \param [in] help Some help text which describes the purpose of this
	   *             attribute.
	   * \param [in] initialValue The initial value for this attribute.
	   * \param [in] accessor An instance of the associated AttributeAccessor
	   *             subclass.
	   * \param [in] checker An instance of the associated AttributeChecker
	   *             subclass.
	   * \param [in] supportLevel Support/deprecation status of the attribute.
	   * \param [in] supportMsg Upgrade hint if this attribute is no longer
	   *             supported.  If the attribute is \c DEPRECATED the attribute
	   *             behavior still exists, but user code should be updated
	   *             following guidance in the hint..
	   *             If the attribute is \c OBSOLETE, the hint should indicate
	   *             which class the attribute functional has been moved to,
	   *             or that the functionality is no longer supported.
	   *             See test file \file type-id-test-suite.cc for examples.
	   * \returns This TypeId instance
	   */
	//accessor allow setting and getting the value of an attribute
	//checker: represent the type of the attribute
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
//------------------------------------done---------------------------------

LteEnbMac::LteEnbMac ()
{
  NS_LOG_FUNCTION (this);
  m_macSapProvider = new EnbMacMemberLteMacSapProvider<LteEnbMac> (this);
  m_cmacSapProvider = new EnbMacMemberLteEnbCmacSapProvider (this);
  m_schedSapUser = new EnbMacMemberFfMacSchedSapUser (this);
  m_schedSapUser2 = new EnbMacMemberFfMacSchedSapUser (this);//added
  m_cschedSapUser = new EnbMacMemberFfMacCschedSapUser (this);
  m_cschedSapUser2 = new EnbMacMemberFfMacCschedSapUser (this);//added
  m_enbPhySapUser = new EnbMacMemberLteEnbPhySapUser (this);
  m_enbPhySapUser2 = new EnbMacMemberLteEnbPhySapUser (this);//added here and in .h file line 247
}


LteEnbMac::~LteEnbMac ()
{
  NS_LOG_FUNCTION (this);
}
//-------------------------------done--------------------
void
LteEnbMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_dlCqiReceived.clear ();
  m_ulCqiReceived.clear ();
  m_ulCeReceived.clear ();
  m_dlInfoListReceived.clear ();
  m_ulInfoListReceived.clear ();
  m_miDlHarqProcessesPackets.clear ();
  delete m_macSapProvider;
  delete m_cmacSapProvider;
  delete m_schedSapUser;
  delete m_cschedSapUser;
  delete m_enbPhySapUser;
  delete m_schedSapUser2;//added
  delete m_cschedSapUser2;//added
  delete m_enbPhySapUser2; //added
}

//set the value sent to sap provider and get value to sap user
void
LteEnbMac::SetFfMacSchedSapProvider (FfMacSchedSapProvider* s,FfMacSchedSapProvider* s2)
{
  NS_LOG_FUNCTION(this << "LteEnbMac::SetFfMacSchecdSapProvider, Connecting FFMAC Scehd Sap");
  m_schedSapProvider = s;
  m_schedSapProvider2 = s2;
}

FfMacSchedSapUser*
LteEnbMac::GetFfMacSchedSapUser (uint8_t i)
{
	switch(i)
	{
	case 1 :
        return m_schedSapUser;
        break;
	case 2 :
		return m_schedSapUser2;
		        break;
	default:
		return m_schedSapUser;
	}

}



void
LteEnbMac::SetFfMacCschedSapProvider (FfMacCschedSapProvider* s,FfMacCschedSapProvider* s2)
{
  NS_LOG_FUNCTION(this << "<mohamed>L493,LteEnbMac::SetFfMacSchecdSapProvider, Connecting FFMAC CScehd Sap<mohamed>");
  m_cschedSapProvider = s;
  m_cschedSapProvider2 = s2;
}

FfMacCschedSapUser*
LteEnbMac::GetFfMacCschedSapUser (uint8_t i)
{
	switch(i)
		{
		case 1 :
			return m_cschedSapUser;
	        break;
		case 2 :
			return m_cschedSapUser2;
			        break;
		default:
			return m_cschedSapUser;
		}
}



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
//------------------------------done----------------------------
void
LteEnbMac::SetLteEnbPhySapProvider (LteEnbPhySapProvider* s,LteEnbPhySapProvider* s2)
{
  NS_LOG_FUNCTION(this << s << "<mohamed> L:532,LteEnbMac::SetLteEnbPhySapUser, Connecting connecting PHY to MAC <mohamed>");
  m_enbPhySapProvider = s;
  NS_LOG_FUNCTION(this << s2 << "LteEnbMac::SetLteEnbPhySapUser, Connecting connecting PHY to MAC");
  m_enbPhySapProvider2 = s2;
}
//---------------------------done--------------------------------
LteEnbPhySapUser*
LteEnbMac::GetLteEnbPhySapUser (uint8_t i)
{
  NS_LOG_FUNCTION(this << "<mohamed> L545,LteEnbMac::GetLteEnbPhySapUser, Connecting connecting PHY to MAC <mohamed>");
  switch(i)
  		{
  		case 1 :
  			 return m_enbPhySapUser;
  	        break;
  		case 2 :
  			 return m_enbPhySapUser2;
  		    break;
  		default:
  			 return m_enbPhySapUser;
  		}
}
//----------------------------------not modified-----------------------------------
void
LteEnbMac::DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << " EnbMac - frame " << frameNo << " subframe " << subframeNo);

  // Store current frame / subframe number
  m_frameNo = frameNo;
  m_subframeNo = subframeNo;


  // --- DOWNLINK ---
  // Send Dl-CQI info to the scheduler
  if (m_dlCqiReceived.size () > 0)
    {
      FfMacSchedSapProvider::SchedDlCqiInfoReqParameters dlcqiInfoReq;
      dlcqiInfoReq.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);

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
                  rnti = m_cmacSapUser->AllocateTemporaryCellRnti ();
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

  // Forward DL HARQ feebacks collected during last TTI
  if (m_dlInfoListReceived.size () > 0)
    {
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
//------------------------------------------------------------------------


//-------------------------done------------------------------------------
void
LteEnbMac::DoReceiveLteControlMessage  (Ptr<LteControlMessage> msg,Ptr<LteControlMessage> msg2)
{
  NS_LOG_FUNCTION (this << msg);
  Ptr<DlCqiLteControlMessage> dlcqi,dlcqi2;
  Ptr<BsrLteControlMessage> bsr,bsr2;
  Ptr<DlHarqFeedbackLteControlMessage> dlharq,dlharq2;

  if (msg->GetMessageType () == LteControlMessage::DL_CQI)
    {
       dlcqi = DynamicCast<DlCqiLteControlMessage> (msg);

    }
  else if (msg->GetMessageType () == LteControlMessage::BSR)
    {
      bsr = DynamicCast<BsrLteControlMessage> (msg);

    }
  else if (msg->GetMessageType () == LteControlMessage::DL_HARQ)
    {
       dlharq = DynamicCast<DlHarqFeedbackLteControlMessage> (msg);
    }
  else
    {
      NS_LOG_LOGIC (this << " LteControlMessage type " << msg->GetMessageType () << " not recognized");
   }
//--------------------------modified-------------------------------------

  NS_LOG_FUNCTION (this << msg2);
   if (msg2->GetMessageType () == LteControlMessage::DL_CQI)
     {
       dlcqi2 = DynamicCast<DlCqiLteControlMessage> (msg2);

     }
   else if (msg2->GetMessageType () == LteControlMessage::BSR)
     {
       bsr2 = DynamicCast<BsrLteControlMessage> (msg2);

     }
   else if (msg2->GetMessageType () == LteControlMessage::DL_HARQ)
     {
       dlharq2 = DynamicCast<DlHarqFeedbackLteControlMessage> (msg2);
     }
   else
     {
       NS_LOG_LOGIC (this << " LteControlMessage type 2 " << msg2->GetMessageType () << " not recognized");
     }

   if(dlharq || dlharq2)
   {
	   DoDlInfoListElementHarqFeeback (dlharq->GetDlHarqFeedback (),dlharq2->GetDlHarqFeedback ()) ;
   }
   if(bsr || bsr2)
      {
	   ReceiveBsrMessage (bsr->GetBsr (),bsr2->GetBsr ());
      }
   if(bsr || bsr2)
         {
	   ReceiveDlCqiLteControlMessage (dlcqi,dlcqi2);
         }

}
//-----------------------------------------------------------------------------

//----------------------------------done--------------------------------------
void
LteEnbMac::DoReceiveRachPreamble  (uint8_t rapId,uint8_t rapId2)
{
  NS_LOG_FUNCTION (this << (uint32_t) rapId);
  // just record that the preamble has been received; it will be processed later
  if(rapId != 0)
  ++m_receivedRachPreambleCount[rapId];
  // will create entry if not exists
  NS_LOG_FUNCTION (this << (uint32_t) rapId2);
  if(rapId2 !=0)
  ++m_receivedRachPreambleCount2[rapId2];
}
//-----------------------done--------------------------------------
void
LteEnbMac::DoUlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi,FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi2)
{ 
  if (ulcqi.m_ulCqi.m_type == UlCqi_s::PUSCH)
    {
      NS_LOG_DEBUG (this << " eNB rxed an PUSCH UL-CQI");
    }
  else if (ulcqi.m_ulCqi.m_type == UlCqi_s::SRS)
    {
      NS_LOG_DEBUG (this << " eNB rxed an SRS UL-CQI");
    }
  m_ulCqiReceived.push_back (ulcqi);
  m_ulCqiReceived.push_back (ulcqi2);
}

//------------------------------------done---------------------------
void
LteEnbMac::ReceiveDlCqiLteControlMessage  (Ptr<DlCqiLteControlMessage> msg,Ptr<DlCqiLteControlMessage> msg2)
{
  NS_LOG_FUNCTION (this << msg);

  CqiListElement_s dlcqi = msg->GetDlCqi ();
  NS_LOG_LOGIC (this << "Enb Received DL-CQI rnti" << dlcqi.m_rnti);
  NS_ASSERT (dlcqi.m_rnti != 0);
  m_dlCqiReceived.push_back (dlcqi);

  NS_LOG_FUNCTION (this << msg2);
    CqiListElement_s dlcqi2 = msg2->GetDlCqi ();
    NS_LOG_LOGIC (this << "Enb Received DL-CQI rnti 2" << dlcqi2.m_rnti);
    NS_ASSERT (dlcqi2.m_rnti != 0);
  m_dlCqiReceived2.push_back (dlcqi2);

}

//--------------------------------done-------------------------------
void
LteEnbMac::ReceiveBsrMessage  (MacCeListElement_s bsr,MacCeListElement_s bsr2)
{
m_ulCeReceived.push_back (bsr);
m_ulCeReceived2.push_back (bsr2);
}

//------------------------------done------------------------------
void
LteEnbMac::DoReceivePhyPdu (Ptr<Packet> p,Ptr<Packet> p2)
{
  NS_LOG_FUNCTION (this);
  //Tag used to define the RNTI and LC id for each MAC packet trasmitted
  LteRadioBearerTag tag;
  LteRadioBearerTag tag2;
  // bydwar 3la tag elawl lma yl2eh y3ml remove lltag da mn list  ely feha tags w kda yb2a elphy estlmo
  p->RemovePacketTag (tag);
  p2->RemovePacketTag (tag2);

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
  uint16_t rnti2 = tag2.GetRnti ();
    uint8_t lcid2= tag2.GetLcid ();
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt2 = m_rlcAttached.find (rnti2);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
  NS_ASSERT_MSG (rntiIt2 != m_rlcAttached.end (), "could not find RNTI" << rnti2);
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt2 = rntiIt2->second.find (lcid2);

  //Receive PDU only if LCID is found
  if (lcidIt != rntiIt->second.end ())
    {
      (*lcidIt).second->ReceivePdu (p);
    }
  if (lcidIt2 != rntiIt2->second.end ())
      {
        (*lcidIt2).second->ReceivePdu (p2);
      }
}

// -----------------------not modified as it's related to RCC----------------

// ////////////////////////////////////////////
// CMAC SAP
// ////////////////////////////////////////////

void
LteEnbMac::DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  NS_LOG_FUNCTION (this << " ulBandwidth=" << (uint16_t) ulBandwidth << " dlBandwidth=" << (uint16_t) dlBandwidth);
  FfMacCschedSapProvider::CschedCellConfigReqParameters params;
  // Configure the subset of parameters used by FfMacScheduler
  params.m_ulBandwidth = ulBandwidth;
  params.m_dlBandwidth = dlBandwidth;
  m_macChTtiDelay = m_enbPhySapProvider->GetMacChTtiDelay ();
  m_macChTtiDelay2 = m_enbPhySapProvider2->GetMacChTtiDelay (); //added
  // ...more parameters can be configured
  m_cschedSapProvider->CschedCellConfigReq (params);
}


void
LteEnbMac::DoAddUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  std::map<uint8_t, LteMacSapUser*> empty;
  std::pair <std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator, bool> 
    ret = m_rlcAttached.insert (std::pair <uint16_t,  std::map<uint8_t, LteMacSapUser*> > 
                                (rnti, empty));
  NS_ASSERT_MSG (ret.second, "element already present, RNTI already existed");

  FfMacCschedSapProvider::CschedUeConfigReqParameters params;
  params.m_rnti = rnti;
  params.m_transmissionMode = 0; // set to default value (SISO) for avoiding random initialization (valgrind error)

  m_cschedSapProvider->CschedUeConfigReq (params);
  m_cschedSapProvider2->CschedUeConfigReq (params);

  // Create DL trasmission HARQ buffers
  std::vector < Ptr<PacketBurst> > dlHarqLayer0pkt;
  std::vector < Ptr<PacketBurst> > dlHarqLayer0pkt2;
  dlHarqLayer0pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer0pkt.at (i) = pb;
    }
  dlHarqLayer0pkt2.resize (8);
    for (uint8_t j = 0; j < 8; j++)
      {
        Ptr<PacketBurst> pb2 = CreateObject <PacketBurst> ();
        dlHarqLayer0pkt2.at (j) = pb2;
      }
  std::vector < Ptr<PacketBurst> > dlHarqLayer1pkt;
  dlHarqLayer1pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer1pkt.at (i) = pb;
    }
  std::vector < Ptr<PacketBurst> > dlHarqLayer1pkt2;
    dlHarqLayer1pkt2.resize (8);
    for (uint8_t j = 0; j < 8; j++)
      {
        Ptr<PacketBurst> pb2 = CreateObject <PacketBurst> ();
        dlHarqLayer1pkt2.at (j) = pb2;
      }
  DlHarqProcessesBuffer_t buf,buf2;
  buf.push_back (dlHarqLayer0pkt);
  buf.push_back (dlHarqLayer1pkt);
  buf2.push_back (dlHarqLayer0pkt);
  buf2.push_back (dlHarqLayer1pkt);
  m_miDlHarqProcessesPackets.insert (std::pair <uint16_t, DlHarqProcessesBuffer_t> (rnti, buf));
  m_miDlHarqProcessesPackets2.insert (std::pair <uint16_t, DlHarqProcessesBuffer_t> (rnti, buf2));
//last line may be changed
}
//-------------------------------------done------------------------------
void
LteEnbMac::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  FfMacCschedSapProvider::CschedUeReleaseReqParameters params;
  params.m_rnti = rnti;
  m_cschedSapProvider->CschedUeReleaseReq (params);
  m_cschedSapProvider2->CschedUeReleaseReq (params);
  m_rlcAttached.erase (rnti);
  m_miDlHarqProcessesPackets.erase (rnti);
  m_miDlHarqProcessesPackets2.erase (rnti);
}
//-----------------------------done--------------------------------------
void
LteEnbMac::DoAddLc (LteEnbCmacSapProvider::LcInfo lcinfo,LteEnbCmacSapProvider::LcInfo lcinfo2, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this);

  std::map <LteFlowId_t, LteMacSapUser* >::iterator it;
  std::map <LteFlowId_t, LteMacSapUser* >::iterator it2;
  LteFlowId_t flow (lcinfo.rnti, lcinfo.lcId);
  LteFlowId_t flow2 (lcinfo2.rnti, lcinfo2.lcId);
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (lcinfo.rnti);
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt2 = m_rlcAttached.find (lcinfo2.rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "RNTI not found");
  NS_ASSERT_MSG (rntiIt2 != m_rlcAttached.end (), "RNTI not found");
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcinfo.lcId);
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt2 = rntiIt2->second.find (lcinfo2.lcId);
  if (lcidIt == rntiIt->second.end ())
    {
      rntiIt->second.insert (std::pair<uint8_t, LteMacSapUser*> (lcinfo.lcId, msu));
    }
  else
    {
      NS_LOG_ERROR ("LC already exists");
    }
  if (lcidIt2 == rntiIt2->second.end ())
      {
        rntiIt2->second.insert (std::pair<uint8_t, LteMacSapUser*> (lcinfo2.lcId, msu));
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
      struct FfMacCschedSapProvider::CschedLcConfigReqParameters params;
      params.m_rnti = lcinfo.rnti;
      params.m_reconfigureFlag = false;

      struct LogicalChannelConfigListElement_s lccle;
      lccle.m_logicalChannelIdentity = lcinfo.lcId;
      lccle.m_logicalChannelGroup = lcinfo.lcGroup;
      lccle.m_direction = LogicalChannelConfigListElement_s::DIR_DL;//kanet >>DIR_BOTH; //changed
      lccle.m_qosBearerType = lcinfo.isGbr ? LogicalChannelConfigListElement_s::QBT_GBR : LogicalChannelConfigListElement_s::QBT_NON_GBR;
      lccle.m_qci = lcinfo.qci;
      lccle.m_eRabMaximulBitrateUl = lcinfo.mbrUl;
      lccle.m_eRabMaximulBitrateDl = lcinfo.mbrDl;
      lccle.m_eRabGuaranteedBitrateUl = lcinfo.gbrUl;
      lccle.m_eRabGuaranteedBitrateDl = lcinfo.gbrDl;
      params.m_logicalChannelConfigList.push_back (lccle);
      m_cschedSapProvider->CschedLcConfigReq (params);
    }
      //-----------------------modified-------------------------------
  if(lcinfo2.lcId != 0)
  {
      struct FfMacCschedSapProvider::CschedLcConfigReqParameters params2;
      params2.m_rnti = lcinfo.rnti;
      params2.m_reconfigureFlag = false;
      struct LogicalChannelConfigListElement_s lccle2;
      lccle2.m_logicalChannelIdentity = lcinfo.lcId;
      lccle2.m_logicalChannelGroup = lcinfo.lcGroup;
      lccle2.m_direction = LogicalChannelConfigListElement_s::DIR_DL;//kanet >>DIR_BOTH; //changed
      lccle2.m_qosBearerType = lcinfo.isGbr ? LogicalChannelConfigListElement_s::QBT_GBR : LogicalChannelConfigListElement_s::QBT_NON_GBR;
      lccle2.m_qci = lcinfo.qci;
      lccle2.m_eRabMaximulBitrateUl = lcinfo.mbrUl;
      lccle2.m_eRabMaximulBitrateDl = lcinfo.mbrDl;
      lccle2.m_eRabGuaranteedBitrateUl = lcinfo.gbrUl;
      lccle2.m_eRabGuaranteedBitrateDl = lcinfo.gbrDl;
      params2.m_logicalChannelConfigList.push_back (lccle2);
      m_cschedSapProvider2->CschedLcConfigReq (params2)
    }
}
//-------------------------------not modified---------------------------
void
LteEnbMac::DoReconfigureLc (LteEnbCmacSapProvider::LcInfo lcinfo,LteEnbCmacSapProvider::LcInfo lcinfo2)
{
  NS_FATAL_ERROR ("not implemented");
}
//----------------------------done---------------------------------
void
LteEnbMac::DoReleaseLc (uint16_t rnti, uint8_t lcid,uint16_t rnti2, uint8_t lcid2)
{
  NS_LOG_FUNCTION (this);

  //Find user based on rnti and then erase lcid stored against the same
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt2 = m_rlcAttached.find (rnti2);
  rntiIt->second.erase (lcid);
  rntiIt2->second.erase (lcid2);
  struct FfMacCschedSapProvider::CschedLcReleaseReqParameters params;
  struct FfMacCschedSapProvider::CschedLcReleaseReqParameters params2;
  params.m_rnti = rnti;
  params.m_logicalChannelIdentity.push_back (lcid);
  params2.m_rnti = rnti2;
  params2.m_logicalChannelIdentity.push_back (lcid2);
  m_cschedSapProvider->CschedLcReleaseReq (params);
  m_cschedSapProvider2->CschedLcReleaseReq (params2);
}
//------------------------------done-------------------------------------
void
LteEnbMac::DoUeUpdateConfigurationReq (LteEnbCmacSapProvider::UeConfig params,LteEnbCmacSapProvider::UeConfig params2)
{
  NS_LOG_FUNCTION (this);

  // propagates to scheduler
  FfMacCschedSapProvider::CschedUeConfigReqParameters req;
  FfMacCschedSapProvider::CschedUeConfigReqParameters req2;
  req.m_rnti = params.m_rnti;
  req.m_transmissionMode = params.m_transmissionMode;
  req.m_reconfigureFlag = true;
  req2.m_rnti = params2.m_rnti;
  req2.m_transmissionMode = params2.m_transmissionMode;
  req2.m_reconfigureFlag = true;
  m_cschedSapProvider->CschedUeConfigReq (req);
  m_cschedSapProvider2->CschedUeConfigReq (req2);
}
//--------------------------------not modified------------------------------
LteEnbCmacSapProvider::RachConfig 
LteEnbMac::DoGetRachConfig ()
{
  struct LteEnbCmacSapProvider::RachConfig rc;
  rc.numberOfRaPreambles = m_numberOfRaPreambles;
  rc.preambleTransMax = m_preambleTransMax;
  rc.raResponseWindowSize = m_raResponseWindowSize;
  return rc;
}
//---------------------not modified----------------------------------
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

//------------------------------------------------------------------------

// ////////////////////////////////////////////
// MAC SAP
// ////////////////////////////////////////////

//-----------------------------------done-----------------------------
void
LteEnbMac::DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params,LteMacSapProvider::TransmitPduParameters params2)
{
  NS_LOG_FUNCTION (this);
  LteRadioBearerTag tag (params.rnti, params.lcid, params.layer);
  LteRadioBearerTag tag2 (params2.rnti, params2.lcid, params2.layer);
  params.pdu->AddPacketTag (tag);
  params2.pdu->AddPacketTag (tag2);
  // Store pkt in HARQ buffer
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.rnti);
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it2 =  m_miDlHarqProcessesPackets2.find (params2.rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
  NS_ASSERT (it2 != m_miDlHarqProcessesPackets2.end ());
  NS_LOG_DEBUG (this << " LAYER " << (uint16_t)tag.GetLayer () << " HARQ ID " << (uint16_t)params.harqProcessId);
  NS_LOG_DEBUG (this << " LAYER 2 " << (uint16_t)tag2.GetLayer () << " HARQ ID 2" << (uint16_t)params2.harqProcessId);
  
  //(*it).second.at (params.layer).at (params.harqProcessId) = params.pdu;//->Copy ();
  (*it).second.at (params.layer).at (params.harqProcessId)->AddPacket (params.pdu);
  (*it2).second.at (params2.layer).at (params2.harqProcessId)->AddPacket (params2.pdu);
  m_enbPhySapProvider->SendMacPdu (params.pdu);
  m_enbPhySapProvider2->SendMacPdu (params2.pdu); //added

}
//-------------------------------done-------------------------------
void
LteEnbMac::DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params,LteMacSapProvider::ReportBufferStatusParameters params2)
{
  NS_LOG_FUNCTION (this);
  FfMacSchedSapProvider::SchedDlRlcBufferReqParameters req;
  FfMacSchedSapProvider::SchedDlRlcBufferReqParameters req2;
  req.m_rnti = params.rnti;
  req.m_logicalChannelIdentity = params.lcid;
  req.m_rlcTransmissionQueueSize = params.txQueueSize;
  req.m_rlcTransmissionQueueHolDelay = params.txQueueHolDelay;
  req.m_rlcRetransmissionQueueSize = params.retxQueueSize;
  req.m_rlcRetransmissionHolDelay = params.retxQueueHolDelay;
  req.m_rlcStatusPduSize = params.statusPduSize;
  m_schedSapProvider->SchedDlRlcBufferReq (req);
  //-------------------------------added----------------------------------
  req2.m_rnti = params2.rnti;
  req2.m_logicalChannelIdentity = params2.lcid;
  req2.m_rlcTransmissionQueueSize = params2.txQueueSize;
  req2.m_rlcTransmissionQueueHolDelay = params2.txQueueHolDelay;
  req2.m_rlcRetransmissionQueueSize = params2.retxQueueSize;
  req2.m_rlcRetransmissionHolDelay = params2.retxQueueHolDelay;
  req2.m_rlcStatusPduSize = params2.statusPduSize;
  m_schedSapProvider2->SchedDlRlcBufferReq (req2);
}
//-----------------------------------------------------------------------------


// ////////////////////////////////////////////
// SCHED SAP
// ////////////////////////////////////////////


//----------------------------------done---------------------------------
void
LteEnbMac::DoSchedDlConfigInd (FfMacSchedSapUser::SchedDlConfigIndParameters ind,FfMacSchedSapUser::SchedDlConfigIndParameters ind2)
{
  NS_LOG_FUNCTION (this);
  // Create DL PHY PDU
  Ptr<PacketBurst> pb = CreateObject<PacketBurst> ();
  Ptr<PacketBurst> pb2 = CreateObject<PacketBurst> ();
  std::map <LteFlowId_t, LteMacSapUser* >::iterator it;
  std::map <LteFlowId_t, LteMacSapUser* >::iterator it2;

  for (unsigned int i = 0; i < ind.m_buildDataList.size (); i++)
    {
      for (uint16_t layer = 0; layer < ind.m_buildDataList.at (i).m_dci.m_ndi.size (); layer++)
        {
          if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (layer) == 1)
            {
              // new data -> force emptying correspondent harq pkt buffer
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
  for (unsigned int i = 0; i < ind2.m_buildDataList.size (); i++)
      {
        for (uint16_t layer = 0; layer < ind2.m_buildDataList.at (i).m_dci.m_ndi.size (); layer++)
          {
            if (ind2.m_buildDataList.at (i).m_dci.m_ndi.at (layer) == 1)
              {
                // new data -> force emptying correspondent harq pkt buffer
                std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it2 = m_miDlHarqProcessesPackets2.find (ind.m_buildDataList.at (i).m_rnti);
                NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
                for (uint16_t lcId2 = 0; lcId2 < (*it2).second.size (); lcId2++)
                  {
                    Ptr<PacketBurst> pb2 = CreateObject <PacketBurst> ();
                    (*it2).second.at (lcId2).at (ind2.m_buildDataList.at (i).m_dci.m_harqProcess) = pb;
                  }
              }
          }
        for (unsigned int j = 0; j < ind2.m_buildDataList.at (i).m_rlcPduList.size (); j++)
          {
            for (uint16_t k = 0; k < ind2.m_buildDataList.at (i).m_rlcPduList.at (j).size (); k++)
              {
                if (ind2.m_buildDataList.at (i).m_dci.m_ndi.at (k) == 1)
                  {
                    // New Data -> retrieve it from RLC
                    uint16_t rnti2 = ind2.m_buildDataList.at (i).m_rnti;
                    uint8_t lcid2 = ind2.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_logicalChannelIdentity;
                    std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt2 = m_rlcAttached.find (rnti2);
                    NS_ASSERT_MSG (rntiIt2 != m_rlcAttached.end (), "could not find RNTI" << rnti2);
                    std::map<uint8_t, LteMacSapUser*>::iterator lcidIt2 = rntiIt2->second.find (lcid2);
                    NS_ASSERT_MSG (lcidIt2 != rntiIt2->second.end (), "could not find LCID" << lcid2);
                    NS_LOG_DEBUG (this << " rnti= " << rnti2 << " lcid= " << (uint32_t) lcid2 << " layer= " << k);
                    (*lcidIt2).second->NotifyTxOpportunity (ind2.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_size, k, ind2.m_buildDataList.at (i).m_dci.m_harqProcess);
                  }
                else
                  {
                    if (ind2.m_buildDataList.at (i).m_dci.m_tbsSize.at (k) > 0)
                      {
                        // HARQ retransmission -> retrieve TB from HARQ buffer
                        std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it2 = m_miDlHarqProcessesPackets2.find (ind2.m_buildDataList.at (i).m_rnti);
                        NS_ASSERT (it != m_miDlHarqProcessesPackets2.end ());
                        Ptr<PacketBurst> pb2 = (*it2).second.at (k).at ( ind2.m_buildDataList.at (i).m_dci.m_harqProcess);
                        for (std::list<Ptr<Packet> >::const_iterator j = pb2->Begin (); j != pb2->End (); ++j)
                          {
                            Ptr<Packet> pkt2 = (*j)->Copy ();
                            m_enbPhySapProvider2->SendMacPdu (pkt2);

                          }
                      }
                  }
              }
          }
        // send the relative DCI
        Ptr<DlDciLteControlMessage> msg2 = Create<DlDciLteControlMessage> ();
        msg2->SetDci (ind2.m_buildDataList.at (i).m_dci);
        m_enbPhySapProvider2->SendLteControlMessage (msg2);

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

  for (  uint32_t i  = 0; i < ind2.m_buildDataList.size (); i++ )
      {
        // Only one TB used
        if (ind2.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 1)
          {
            m_dlScheduling (m_frameNo, m_subframeNo, ind2.m_buildDataList.at (i).m_dci.m_rnti,
                            ind2.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                            ind2.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                            0, 0
                            );
          }
        // Two TBs used
        else if (ind2.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 2)
          {
            m_dlScheduling (m_frameNo, m_subframeNo, ind2.m_buildDataList.at (i).m_dci.m_rnti,
                            ind2.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                            ind2.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                            ind2.m_buildDataList.at (i).m_dci.m_mcs.at (1),
                            ind2.m_buildDataList.at (i).m_dci.m_tbsSize.at (1)
                            );
          }
        else
          {
            NS_FATAL_ERROR ("Found element with more than two transport blocks");
          }
      }
  // Random Access procedure: send RARs
  Ptr<RarLteControlMessage> rarMsg = Create<RarLteControlMessage> ();
  Ptr<RarLteControlMessage> rarMsg2 = Create<RarLteControlMessage> ();
  // see TS 36.321 5.1.4;  preambles were sent two frames ago
  // (plus 3GPP counts subframes from 0, not 1)
  uint16_t raRnti;
  uint16_t raRnti2;
  //control subframes are the first 3 subframes in each frame
  if (m_subframeNo < 3)
    {
      raRnti = m_subframeNo + 7; // equivalent to +10-3
      raRnti2 = m_subframeNo + 7;
    }
  else
    {
      raRnti = m_subframeNo - 3;
      raRnti2 = m_subframeNo - 3;
    }

  rarMsg->SetRaRnti (raRnti);
  rarMsg2->SetRaRnti (raRnti2);
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
  //------------------------added-----------------------------------
  for (unsigned int i = 0; i < ind2.m_buildRarList.size (); i++)
      {
        std::map <uint8_t, uint32_t>::iterator itRapId2 = m_rapIdRntiMap.find (ind2.m_buildRarList.at (i).m_rnti);
        if (itRapId2 == m_rapIdRntiMap2.end ())
          {
            NS_FATAL_ERROR ("Unable to find rapId of RNTI " << ind2.m_buildRarList.at (i).m_rnti);
          }
        RarLteControlMessage::Rar rar2;
        rar2.rapId = itRapId2->second;
        rar2.rarPayload = ind.m_buildRarList.at (i);
        rarMsg2->AddRar (rar2);
        NS_LOG_INFO (this << " Send RAR message to RNTI 2" << ind2.m_buildRarList.at (i).m_rnti << " rapId " << itRapId2->second);
      }
  //---------------------------------------------------------------------------------


  //---------------------------added-------------------------------------------------
  if (ind.m_buildRarList.size () > 0)
    {
      m_enbPhySapProvider->SendLteControlMessage (rarMsg);
    }
  m_rapIdRntiMap.clear ();
  //---------------------------added-------------------------------------------------
  if (ind2.m_buildRarList.size () > 0)
     {
       m_enbPhySapProvider2->SendLteControlMessage (rarMsg2);
     }
  m_rapIdRntiMap2.clear ();
  //---------------------------------------------------------------------------------

}

//----------------------------------------------------------------

//--------------------------------done----------------------------
void
LteEnbMac::DoSchedUlConfigInd (FfMacSchedSapUser::SchedUlConfigIndParameters ind,FfMacSchedSapUser::SchedUlConfigIndParameters ind2)
{
  NS_LOG_FUNCTION (this);

  for (unsigned int i = 0; i < ind.m_dciList.size (); i++)
    {
      // send the correspondent ul dci
      Ptr<UlDciLteControlMessage> msg = Create<UlDciLteControlMessage> ();
      msg->SetDci (ind.m_dciList.at (i));
      m_enbPhySapProvider->SendLteControlMessage (msg);
    }

  // Fire the trace with the UL information
  for (  uint32_t i  = 0; i < ind.m_dciList.size (); i++ )
    {
      m_ulScheduling (m_frameNo, m_subframeNo, ind.m_dciList.at (i).m_rnti,
                      ind.m_dciList.at (i).m_mcs, ind.m_dciList.at (i).m_tbSize);
    }
  for (unsigned int j = 0; j < ind2.m_dciList.size (); j++)
      {
        // send the correspondent ul dci
        Ptr<UlDciLteControlMessage> msg2 = Create<UlDciLteControlMessage> ();
        msg2->SetDci (ind2.m_dciList.at (j));
        m_enbPhySapProvider2->SendLteControlMessage (msg2);
      }

    // Fire the trace with the UL information
    for (  uint32_t j  = 0; j < ind2.m_dciList.size (); j++ )
      {
        m_ulScheduling (m_frameNo, m_subframeNo, ind2.m_dciList.at (j).m_rnti,
                        ind2.m_dciList.at (j).m_mcs, ind2.m_dciList.at (j).m_tbSize);
      }


}




// ////////////////////////////////////////////
// CSCHED SAP
// ////////////////////////////////////////////

//-----------------------modified---------------//
void
LteEnbMac::DoCschedCellConfigCnf (FfMacCschedSapUser::CschedCellConfigCnfParameters params,FfMacCschedSapUser::CschedCellConfigCnfParameters params2)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedUeConfigCnf (FfMacCschedSapUser::CschedUeConfigCnfParameters params,FfMacCschedSapUser::CschedUeConfigCnfParameters params2)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedLcConfigCnf (FfMacCschedSapUser::CschedLcConfigCnfParameters params,FfMacCschedSapUser::CschedLcConfigCnfParameters params2)
{
  NS_LOG_FUNCTION (this);
  // Call the CSCHED primitive
  // m_cschedSap->LcConfigCompleted();
}

void
LteEnbMac::DoCschedLcReleaseCnf (FfMacCschedSapUser::CschedLcReleaseCnfParameters params,FfMacCschedSapUser::CschedLcReleaseCnfParameters params2)
{
  NS_LOG_FUNCTION (this);
}
//-----------------------------not modified---------------------
void
LteEnbMac::DoCschedUeReleaseCnf (FfMacCschedSapUser::CschedUeReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}
//----------------------- not modified ---------------------------------------
void
LteEnbMac::DoCschedUeConfigUpdateInd(FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
  // propagates to RRC
  LteEnbCmacSapUser::UeConfig ueConfigUpdate;
  ueConfigUpdate.m_rnti = params.m_rnti;
  ueConfigUpdate.m_transmissionMode = params.m_transmissionMode;
  m_cmacSapUser->RrcConfigurationUpdateInd (ueConfigUpdate);
}
//------------------------done-------------------------------
void
LteEnbMac::DoCschedCellConfigUpdateInd (FfMacCschedSapUser::CschedCellConfigUpdateIndParameters params,FfMacCschedSapUser::CschedCellConfigUpdateIndParameters params2)
{
  NS_LOG_FUNCTION (this);
}
//----------------------------------done-----------------------------
void
LteEnbMac::DoUlInfoListElementHarqFeeback (UlInfoListElement_s params,UlInfoListElement_s params2)
{
  NS_LOG_FUNCTION (this);
  m_ulInfoListReceived.push_back (params);
  m_ulInfoListReceived2.push_back (params2);
}
//------------------------done---------------------------------
void
LteEnbMac::DoDlInfoListElementHarqFeeback (DlInfoListElement_s params,DlInfoListElement_s params2)
{
  NS_LOG_FUNCTION (this);
  // Update HARQ buffer
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.m_rnti);
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it2 =  m_miDlHarqProcessesPackets.find (params2.m_rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
  NS_ASSERT (it2 != m_miDlHarqProcessesPackets2.end ());
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
  for (uint8_t layer2 = 0; layer2 < params2.m_harqStatus.size (); layer2++)
      {
        if (params2.m_harqStatus.at (layer2) == DlInfoListElement_s::ACK)
          {
            // discard buffer
            Ptr<PacketBurst> emptyBuf2 = CreateObject <PacketBurst> ();
            (*it2).second.at (layer2).at (params2.m_harqProcessId) = emptyBuf2;
            NS_LOG_DEBUG (this << " HARQ-ACK UE " << params2.m_rnti << " harqId " << (uint16_t)params2.m_harqProcessId << " layer " << (uint16_t)layer2);
          }
        else if (params2.m_harqStatus.at (layer2) == DlInfoListElement_s::NACK)
          {
            NS_LOG_DEBUG (this << " HARQ-NACK UE " << params2.m_rnti << " harqId " << (uint16_t)params2.m_harqProcessId << " layer " << (uint16_t)layer2);
          }
        else
          {
            NS_FATAL_ERROR (" HARQ functionality not implemented");
          }
      }
  m_dlInfoListReceived.push_back (params);
}


} // namespace ns3
