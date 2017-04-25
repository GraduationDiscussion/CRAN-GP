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
 */

#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/math.h>
#include <cfloat>
#include <set>
#include <climits>
#include <ns3/lte-amc.h>
#include <ns3/rr-ff-mac-scheduler.h>
#include <ns3/simulator.h>
#include <ns3/lte-common.h>
#include <ns3/lte-vendor-specific-parameters.h>
#include <ns3/boolean.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("RrFfMacScheduler");

static const int Type0AllocationRbg[4] = { 10,       // RGB size 1
		26,       // RGB size 2
		63,       // RGB size 3
		110       // RGB size 4
		};// see table 7.1.6.1-1 of 36.213

NS_OBJECT_ENSURE_REGISTERED(RrFfMacScheduler);

class RrSchedulerMemberCschedSapProvider: public FfMacCschedSapProvider {
public:
	RrSchedulerMemberCschedSapProvider(RrFfMacScheduler* scheduler);

	// inherited from FfMacCschedSapProvider

	//------Modified <Abdelrhman> ---------------------------------------------------
	//----Sending parameters of both phys. for configuration independently.

	virtual void CschedCellConfigReq(
			const struct CschedCellConfigReqParameters& params);
	virtual void CschedUeConfigReq(
			const struct CschedUeConfigReqParameters& params);
	virtual void CschedLcConfigReq(
			const struct CschedLcConfigReqParameters& params);
	virtual void CschedLcReleaseReq(
			const struct CschedLcReleaseReqParameters& params);
	virtual void CschedUeReleaseReq(
			const struct CschedUeReleaseReqParameters& params);
//--------Mod. end--------------------------------------
//=================================================================================

private:
	RrSchedulerMemberCschedSapProvider();
	RrFfMacScheduler* m_scheduler;
};



///////////////////////////////////////////////////////////////////////
RrSchedulerMemberCschedSapProvider::RrSchedulerMemberCschedSapProvider() {
}

RrSchedulerMemberCschedSapProvider::RrSchedulerMemberCschedSapProvider(RrFfMacScheduler* scheduler) :m_scheduler(scheduler) {
}

void RrSchedulerMemberCschedSapProvider::CschedCellConfigReq(
		const struct CschedCellConfigReqParameters& params) {
	m_scheduler->DoCschedCellConfigReq(params);
}

void RrSchedulerMemberCschedSapProvider::CschedUeConfigReq(
		const struct CschedUeConfigReqParameters& params) {
	m_scheduler->DoCschedUeConfigReq(params);
}

void RrSchedulerMemberCschedSapProvider::CschedLcConfigReq(
		const struct CschedLcConfigReqParameters& params) {
	m_scheduler->DoCschedLcConfigReq(params );
}

void RrSchedulerMemberCschedSapProvider::CschedLcReleaseReq(
		const struct CschedLcReleaseReqParameters& params) {
	m_scheduler->DoCschedLcReleaseReq(params);
}

void RrSchedulerMemberCschedSapProvider::CschedUeReleaseReq(
		const struct CschedUeReleaseReqParameters& params) {
	m_scheduler->DoCschedUeReleaseReq(params );
}

class RrSchedulerMemberSchedSapProvider: public FfMacSchedSapProvider {
public:
	RrSchedulerMemberSchedSapProvider(RrFfMacScheduler* scheduler);

	// inherited from FfMacSchedSapProvider

//----------Modified <Abdelrhman> ---------------------
//	virtual void SchedDlRlcBufferReq(const struct SchedDlRlcBufferReqParameters& params ,
//			std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>& m_rlcBufferReq,
//			std::map <uint16_t,uint8_t>& m_p10CqiRxed ,
//			std::map <uint16_t,uint32_t>& m_p10CqiTimers,
//			//----------parameters of second phy
//			const struct SchedDlRlcBufferReqParameters& params ,
//			std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>& m_rlcBufferReq,
//			std::map <uint16_t,uint8_t>& m_p10CqiRxed2 ,
//			std::map <uint16_t,uint32_t>& m_p10CqiTimers2);
//---------------------------------
//=========================================================

//----------Modified <Abdelrhman> -------------------------
virtual void SchedDlRlcBufferReq(const struct SchedDlRlcBufferReqParameters& params);


//--------------------------------------------------------
//=========================================================


	virtual void SchedDlPagingBufferReq(const struct SchedDlPagingBufferReqParameters& params); //not implemented  --->Simple plans hosted in the cloud

	virtual void SchedDlMacBufferReq(const struct SchedDlMacBufferReqParameters& params); //not implemented
//=====================================================================================================
	//------Modified ------------------
	virtual void SchedDlTriggerReq(const struct SchedDlTriggerReqParameters& params );

	virtual void SchedDlRachInfoReq(const struct SchedDlRachInfoReqParameters& params);

	virtual void SchedDlCqiInfoReq(const struct SchedDlCqiInfoReqParameters& params);

	virtual void SchedUlTriggerReq(const struct SchedUlTriggerReqParameters& params);
//=================================================================================
//-------Not implemented

	virtual void SchedUlNoiseInterferenceReq(const struct SchedUlNoiseInterferenceReqParameters& params);

	virtual void SchedUlSrInfoReq(const struct SchedUlSrInfoReqParameters& params);
//========================================
	//---------MOdified ----------------
	virtual void SchedUlMacCtrlInfoReq(const struct SchedUlMacCtrlInfoReqParameters& params );

	virtual void SchedUlCqiInfoReq(const struct SchedUlCqiInfoReqParameters& params );
//========================================
private:
	RrSchedulerMemberSchedSapProvider();
	RrFfMacScheduler* m_scheduler;
};


RrSchedulerMemberSchedSapProvider::RrSchedulerMemberSchedSapProvider() {
}

RrSchedulerMemberSchedSapProvider::RrSchedulerMemberSchedSapProvider(RrFfMacScheduler* scheduler):m_scheduler(scheduler) {
}


////---------Modified <Abdelrhman> ------------
//void RrSchedulerMemberSchedSapProvider::SchedDlRlcBufferReq (
//		const struct SchedDlRlcBufferReqParameters& params ,
//		std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>& m_rlcBufferReq,
//		std::map <uint16_t,uint8_t>& m_p10CqiRxed ,
//		std::map <uint16_t,uint32_t>& m_p10CqiTimers,
//		//----------parameters of second phy
//		const struct SchedDlRlcBufferReqParameters& params ,
//		std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>& m_rlcBufferReq2,
//		std::map <uint16_t,uint8_t>& m_p10CqiRxed2 ,
//		std::map <uint16_t,uint32_t>& m_p10CqiTimers2) {
//	m_scheduler->DoSchedDlRlcBufferReq(params ,m_rlcBufferReq ,m_p10CqiRxed , m_p10CqiTimers);	//Seek for Lc for the given rnti if found erase it if not re allocate and initialize
//	m_scheduler->DoSchedDlRlcBufferReq(params ,m_rlcBufferReq2 ,m_p10CqiRxed2 ,  m_p10CqiTimers2);
//}
//
//
////------MOd. end

//---------Modified <Abdelrhman> ------------
void RrSchedulerMemberSchedSapProvider::SchedDlRlcBufferReq (
 const struct SchedDlRlcBufferReqParameters& params){

	m_scheduler->DoSchedDlRlcBufferReq(params);

}




void RrSchedulerMemberSchedSapProvider::SchedDlPagingBufferReq(
		const struct SchedDlPagingBufferReqParameters& params) {
	m_scheduler->DoSchedDlPagingBufferReq(params);     //not implemented
}

void RrSchedulerMemberSchedSapProvider::SchedDlMacBufferReq(
		const struct SchedDlMacBufferReqParameters& params) {
	m_scheduler->DoSchedDlMacBufferReq(params);         //not implemented
}

void RrSchedulerMemberSchedSapProvider::SchedDlTriggerReq(
		const struct SchedDlTriggerReqParameters& params) {
	m_scheduler->DoSchedDlTriggerReq(params);
}

void RrSchedulerMemberSchedSapProvider::SchedDlRachInfoReq(
		const struct SchedDlRachInfoReqParameters& params ) {
	m_scheduler->DoSchedDlRachInfoReq(params );
}

void RrSchedulerMemberSchedSapProvider::SchedDlCqiInfoReq(
		const struct SchedDlCqiInfoReqParameters& params ) {
	m_scheduler->DoSchedDlCqiInfoReq(params );
}

void RrSchedulerMemberSchedSapProvider::SchedUlTriggerReq(
		const struct SchedUlTriggerReqParameters& params) {
	m_scheduler->DoSchedUlTriggerReq(params);
}

void RrSchedulerMemberSchedSapProvider::SchedUlNoiseInterferenceReq(
		const struct SchedUlNoiseInterferenceReqParameters& params) {
	m_scheduler->DoSchedUlNoiseInterferenceReq(params);
}

void RrSchedulerMemberSchedSapProvider::SchedUlSrInfoReq(
		const struct SchedUlSrInfoReqParameters& params) {
	m_scheduler->DoSchedUlSrInfoReq(params);
}

void RrSchedulerMemberSchedSapProvider::SchedUlMacCtrlInfoReq(
		const struct SchedUlMacCtrlInfoReqParameters& params) {
	m_scheduler->DoSchedUlMacCtrlInfoReq(params);
}

void RrSchedulerMemberSchedSapProvider::SchedUlCqiInfoReq(
		const struct SchedUlCqiInfoReqParameters& params)
{

	m_scheduler->DoSchedUlCqiInfoReq(params);
}


//-----MOd <Abdelrhman>---------------
//-------------------------------------

//RrFfMacScheduler::RrFfMacScheduler ()
//  :   m_cschedSapUser (0),
//    m_schedSapUser (0),
//    m_nextRntiDl (0),
//    m_nextRntiUl (0)
//{
//  m_amc = CreateObject <LteAmc> ();
//  m_cschedSapProvider = new RrSchedulerMemberCschedSapProvider (this);
//  m_schedSapProvider  = new RrSchedulerMemberSchedSapProvider (this);
//}
RrFfMacScheduler::RrFfMacScheduler() //:
//		m_cschedSapUser(0),m_schedSapUser(0),m_nextRntiDl(0),m_nextRntiUl(0),
//		m_cschedSapUser2(0),m_schedSapUser2(0),m_nextRntiDl2(0),m_nextRntiUl2(0)

{
	m_cschedSapUser=0; m_schedSapUser=0; m_nextRntiDl=0; m_nextRntiUl=0;
	m_cschedSapUser2=0; m_schedSapUser2=0; m_nextRntiDl2 = 0 ; m_nextRntiUl2 = 0;
	m_amc = CreateObject <LteAmc> ();
	m_amc2 = CreateObject <LteAmc> ();

 m_cschedSapProvider = new RrSchedulerMemberCschedSapProvider(this);
 m_schedSapProvider = new RrSchedulerMemberSchedSapProvider(this);
 m_cschedSapProvider2 = new RrSchedulerMemberCschedSapProvider(this);
 m_schedSapProvider2 = new RrSchedulerMemberSchedSapProvider(this);

}

//RrFfMacScheduler::RrFfMacScheduler() {
//	m_amc = CreateObject <LteAmc> ();
//	m_amc2 = CreateObject <LteAmc> ();
//
// m_cschedSapProvider = new RrSchedulerMemberCschedSapProvider(this);
// m_schedSapProvider = new RrSchedulerMemberSchedSapProvider(this);
// m_cschedSapProvider2 = new RrSchedulerMemberCschedSapProvider(this);
// m_schedSapProvider2 = new RrSchedulerMemberSchedSapProvider(this);
//
//}
//RrFfMacScheduler::RrFfMacScheduler() :
//		m_cschedSapUser(0),m_schedSapUser(0),m_nextRntiDl(0),m_nextRntiUl(0)
//		 {
// m_amc = CreateObject<LteAmc>();
// m_cschedSapProvider = new RrSchedulerMemberCschedSapProvider(this);
// m_schedSapProvider = new RrSchedulerMemberSchedSapProvider(this);
//
// //m_amc2 = CreateObject<LteAmc>();
//
// m_cschedSapProvider2 = new RrSchedulerMemberCschedSapProvider(this);
// m_schedSapProvider2 = new RrSchedulerMemberSchedSapProvider(this);

//}

//RrFfMacScheduler::RrFfMacScheduler() :
//		m_cschedSapUser(0),
//		m_schedSapUser(0),
//		m_nextRntiDl(0),
//		m_nextRntiUl(0) {
//     	m_amc = CreateObject<LteAmc>();
//        m_cschedSapProvider = new RrSchedulerMemberCschedSapProvider(this);
//        m_schedSapProvider = new RrSchedulerMemberSchedSapProvider(this);
//}
//
//      //----------------------------------
//        RrFfMacScheduler::RrFfMacScheduler() :
//        		m_cschedSapUser2(0),
//        		m_schedSapUser2(0),
//        		m_nextRntiDl2(0),
//        		m_nextRntiUl2(0) {
//
//             	m_amc2 = CreateObject<LteAmc>();
//                m_cschedSapProvider2 = new RrSchedulerMemberCschedSapProvider(this);
//                m_schedSapProvider2 = new RrSchedulerMemberSchedSapProvider(this);
//        }
//





//--------MOd.end----------------------
//=====================================

RrFfMacScheduler::~RrFfMacScheduler() {
	NS_LOG_FUNCTION(this);
}
//----modified <Abdelrhman>------
void RrFfMacScheduler::DoDispose() {
	NS_LOG_FUNCTION(this);
	m_dlHarqProcessesDciBuffer.clear();
	m_dlHarqProcessesTimer.clear();
	m_dlHarqProcessesRlcPduListBuffer.clear();
	m_dlInfoListBuffered.clear();
	m_ulHarqCurrentProcessId.clear();
	m_ulHarqProcessesStatus.clear();
	m_ulHarqProcessesDciBuffer.clear();
	delete m_cschedSapProvider;
	delete m_schedSapProvider;
	//----start of mod------------------------
	m_dlHarqProcessesDciBuffer2.clear();
	m_dlHarqProcessesTimer2.clear();
	m_dlHarqProcessesRlcPduListBuffer2.clear();
	m_dlInfoListBuffered2.clear();
	m_ulHarqCurrentProcessId2.clear();
	m_ulHarqProcessesStatus2.clear();
	m_ulHarqProcessesDciBuffer2.clear();
	delete m_cschedSapProvider2;
	delete m_schedSapProvider2;

	//-----mod end------------------------

}
//===========================================

TypeId RrFfMacScheduler::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::RrFfMacScheduler").SetParent<FfMacScheduler>().SetGroupName(
					"Lte").AddConstructor<RrFfMacScheduler>().AddAttribute(
					"CqiTimerThreshold",
					"The number of TTIs a CQI is valid (default 1000 - 1 sec.)",
					UintegerValue(1000),
					MakeUintegerAccessor(
							&RrFfMacScheduler::m_cqiTimersThreshold),
					MakeUintegerChecker<uint32_t>()).AddAttribute("HarqEnabled",
					"Activate/Deactivate the HARQ [by default is active].",
					BooleanValue(true),
					MakeBooleanAccessor(&RrFfMacScheduler::m_harqOn),
					MakeBooleanChecker()).AddAttribute("UlGrantMcs",
					"The MCS of the UL grant, must be [0..15] (default 0)",
					UintegerValue(0),
					MakeUintegerAccessor(&RrFfMacScheduler::m_ulGrantMcs),
					MakeUintegerChecker<uint8_t>());
	return tid;
}

//----- modified<Abdelrhman>-------------------------------
void RrFfMacScheduler::SetFfMacCschedSapUser(FfMacCschedSapUser* s,
		FfMacCschedSapUser* s2) {
	m_cschedSapUser = s;
	//------MODIFIED<Abdelrhman>-------//
	m_cschedSapUser2 = s2;
	NS_LOG_INFO(this << "##################### 2 Configs SAPUSERS ARE CREATED ");
	//------Mod end-------------------//
}

//--------------------------
//void RrFfMacScheduler::SetFfMacCschedSapUser(FfMacCschedSapUser* s){
//	m_cschedSapUser = s;
//}
//void RrFfMacScheduler::SetFfMacCschedSapUser2(FfMacCschedSapUser* s){
//	m_cschedSapUser2 = s;
//}

//=======================================================

//----- modified<Abdelrhman>-------------------------------
void RrFfMacScheduler::SetFfMacSchedSapUser(FfMacSchedSapUser* s,
		FfMacSchedSapUser* s2) {
	m_schedSapUser = s;
	//------MODIFIED<Abdelrhman>-------//
	m_schedSapUser2 = s2;			//should check It (problem)
	NS_LOG_INFO(this << "##################### 2  SAPUSERS ARE CREATED ");
	//------Mod end-------------------//
}

//---------------------------------
//void RrFfMacScheduler::SetFfMacSchedSapUser(FfMacSchedSapUser* s){
//	m_schedSapUser = s;
//}
//void RrFfMacScheduler::SetFfMacSchedSapUser2(FfMacSchedSapUser* s){
//	m_schedSapUser2 = s;
//}

//=======================================================

//----- modified<Abdelrhman>-------------------------------
FfMacCschedSapProvider*
RrFfMacScheduler::GetFfMacCschedSapProvider(uint16_t i) {
	switch (i) {
	case 1:
		return m_cschedSapProvider;
		break;
	case 2:
		return m_cschedSapProvider2;
		break;

	default:
		return m_cschedSapProvider;
		break;

	}

	//return m_cschedSapProvider;       // this is the old implementation for 1 phy case
}
//-------------------------------
//FfMacCschedSapProvider*
//RrFfMacScheduler::GetFfMacCschedSapProvider() {
//	return m_cschedSapProvider;
//}
////-----------------------
//FfMacCschedSapProvider*
//RrFfMacScheduler::GetFfMacCschedSapProvider2() {
//	return m_cschedSapProvider2;
//}

//=======================================================

//----- modified<Abdelrhman>-------------------------------
FfMacSchedSapProvider*
RrFfMacScheduler::GetFfMacSchedSapProvider(uint16_t i) {
	switch (i) {
	case 1:
		return m_schedSapProvider;
		break;
	case 2:
		return m_schedSapProvider2;
		break;

	default:
		return m_schedSapProvider;
		break;

	}

	//return m_schedSapProvider;     // old implememtation for 1 phy only
}

//FfMacSchedSapProvider*
//RrFfMacScheduler::GetFfMacSchedSapProvider() {
//	return m_schedSapProvider;
//}
////--------------------------------------
//FfMacSchedSapProvider*
//RrFfMacScheduler::GetFfMacSchedSapProvider2() {
//	return m_schedSapProvider2;
//}
//---------------------------------

//========================================================

//----- modified<Abdelrhman>-------------------------------
void RrFfMacScheduler::SetLteFfrSapProvider(LteFfrSapProvider* s,
		LteFfrSapProvider* s2) {
	m_ffrSapProvider = s;
	m_ffrSapProvider2 = s2;
}

//void RrFfMacScheduler::SetLteFfrSapProvider(LteFfrSapProvider* s) {
//	m_ffrSapProvider = s;
//
//}
////-----------------------------------
//void RrFfMacScheduler::SetLteFfrSapProvider2(LteFfrSapProvider* s) {
//	m_ffrSapProvider2 = s;
//
//}
//========================================================

LteFfrSapUser*
RrFfMacScheduler::GetLteFfrSapUser(uint16_t i) {
	switch (i) {
	case 1:
		return m_ffrSapUser;
		break;
	case 2:
		return m_ffrSapUser2;
		break;

	default:
		return m_ffrSapUser;
		break;

	}
	//return m_ffrSapUser;
}
//--------------------------------------------
//LteFfrSapUser*
//RrFfMacScheduler::GetLteFfrSapUser() {
//	return m_ffrSapUser;
//}
////--------------------------
//LteFfrSapUser*
//RrFfMacScheduler::GetLteFfrSapUser2() {
//	return m_ffrSapUser2;
//}

//===========================================




//--------Modified <Abdelrhman>---------------------------------------
//----getting parameters from each phy. and do Csched Cell Configuration request.
void RrFfMacScheduler::DoCschedCellConfigReq(
		const struct FfMacCschedSapProvider::CschedCellConfigReqParameters& params ) {
	NS_LOG_FUNCTION(this);
	// Read the subset of parameters used
	FfMacCschedSapUser::CschedUeConfigCnfParameters cnf, cnf2;
	std::clog << "<mohamed> ---------- 1 ------------- phy= "<<params.m_phyId << " <mohamed>" << std::endl;
	switch(params.m_phyId){
	case 1:

		{

		 NS_LOG_FUNCTION(this << "<phy1> DoCschedCellConfigReq" );
		m_cschedCellConfig = params;
		m_rachAllocationMap.resize(m_cschedCellConfig.m_ulBandwidth, 0);
		cnf.m_result = SUCCESS;
		m_cschedSapUser->CschedUeConfigCnf(cnf);
		}
		break;
		//---------------------------

		case 2:

		{
			std::clog << "<mohamed> phy 2 ---------- 2 ------------- <mohamed>" << std::endl;
			NS_LOG_FUNCTION(this << "<phy2> DoCschedCellConfigReq" );
		m_cschedCellConfig2 = params;
		m_rachAllocationMap2.resize(m_cschedCellConfig2.m_ulBandwidth, 0);
		cnf2.m_result = SUCCESS;
		m_cschedSapUser2->CschedUeConfigCnf(cnf2);
		}
		break;
	}

	return;
}

//-----------Mod. end------------------------------------------------
//===================================================================

//-------Modified <Abdelrhman>------------------------------------
void RrFfMacScheduler::DoCschedUeConfigReq(
		const struct FfMacCschedSapProvider::CschedUeConfigReqParameters& params ){

	NS_LOG_FUNCTION(this << "<mohamed> m_rnti= " << params.m_rnti << " <mohamed>");

	switch(params.m_phyId){
		case 1:
			{

		    NS_LOG_FUNCTION(this << "phy 1"<<" RNTI " << params.m_rnti << " txMode " << (uint16_t)params.m_transmissionMode);

			std::map<uint16_t, uint8_t>::iterator it = m_uesTxMode.find(params.m_rnti);
			if (it == m_uesTxMode.end()) {
				NS_LOG_FUNCTION("<mohamed> phy 1 ---------- 1 -------------- <mohamed>");
				m_uesTxMode.insert(std::pair<uint16_t, double>(params.m_rnti,params.m_transmissionMode));
				// generate HARQ buffers
				m_dlHarqCurrentProcessId.insert(std::pair<uint16_t, uint8_t>(params.m_rnti, 0));
				DlHarqProcessesStatus_t dlHarqPrcStatus;
				dlHarqPrcStatus.resize(8, 0);
				m_dlHarqProcessesStatus.insert(std::pair<uint16_t, DlHarqProcessesStatus_t>(params.m_rnti,dlHarqPrcStatus));
				DlHarqProcessesTimer_t dlHarqProcessesTimer;
				dlHarqProcessesTimer.resize(8, 0);
				m_dlHarqProcessesTimer.insert(std::pair<uint16_t, DlHarqProcessesTimer_t>(params.m_rnti,dlHarqProcessesTimer));
				DlHarqProcessesDciBuffer_t dlHarqdci;
				dlHarqdci.resize(8);
				m_dlHarqProcessesDciBuffer.insert(std::pair<uint16_t, DlHarqProcessesDciBuffer_t>(params.m_rnti,dlHarqdci));
				DlHarqRlcPduListBuffer_t dlHarqRlcPdu;
				dlHarqRlcPdu.resize(2);
				dlHarqRlcPdu.at(0).resize(8);
				dlHarqRlcPdu.at(1).resize(8);
				m_dlHarqProcessesRlcPduListBuffer.insert(std::pair<uint16_t, DlHarqRlcPduListBuffer_t>(params.m_rnti,dlHarqRlcPdu));
				m_ulHarqCurrentProcessId.insert(
						std::pair<uint16_t, uint8_t>(params.m_rnti, 0));
				UlHarqProcessesStatus_t ulHarqPrcStatus;
				ulHarqPrcStatus.resize(8, 0);
				m_ulHarqProcessesStatus.insert(
						std::pair<uint16_t, UlHarqProcessesStatus_t>(params.m_rnti,
								ulHarqPrcStatus));
				UlHarqProcessesDciBuffer_t ulHarqdci;
				ulHarqdci.resize(8);
				m_ulHarqProcessesDciBuffer.insert(
						std::pair<uint16_t, UlHarqProcessesDciBuffer_t>(params.m_rnti,
								ulHarqdci));
			} else {
				NS_LOG_FUNCTION("<mohamed> phy 1 ---------- 2 -------------- <mohamed>");
				(*it).second = params.m_transmissionMode;
			   }
			} //end of case1 block code
			break;

//	//-------UE configuration for the Ues of the second phy
		case 2:
		{

			NS_LOG_FUNCTION(this << "phy 2"<<" RNTI " << params.m_rnti << " txMode " << (uint16_t)params.m_transmissionMode);
			std::map<uint16_t, uint8_t>::iterator it2 = m_uesTxMode2.find(params.m_rnti);
			if (it2 == m_uesTxMode2.end()) {
				NS_LOG_FUNCTION("<mohamed> phy 2 ---------- 1 -------------- <mohamed>");
				m_uesTxMode2.insert(
						std::pair<uint16_t, double>(params.m_rnti,params.m_transmissionMode));

				// generate HARQ buffers
				m_dlHarqCurrentProcessId2.insert(std::pair<uint16_t, uint8_t>(params.m_rnti, 0));
				DlHarqProcessesStatus_t dlHarqPrcStatus2;
				dlHarqPrcStatus2.resize(8, 0);
				m_dlHarqProcessesStatus2.insert(
						std::pair<uint16_t, DlHarqProcessesStatus_t>(params.m_rnti,
								dlHarqPrcStatus2));
				DlHarqProcessesTimer_t dlHarqProcessesTimer2;
				dlHarqProcessesTimer2.resize(8, 0);
				m_dlHarqProcessesTimer2.insert(
						std::pair<uint16_t, DlHarqProcessesTimer_t>(params.m_rnti,
								dlHarqProcessesTimer2));
				DlHarqProcessesDciBuffer_t dlHarqdci2;
				dlHarqdci2.resize(8);
				m_dlHarqProcessesDciBuffer2.insert(
						std::pair<uint16_t, DlHarqProcessesDciBuffer_t>(params.m_rnti,
								dlHarqdci2));
				DlHarqRlcPduListBuffer_t dlHarqRlcPdu2;
				dlHarqRlcPdu2.resize(2);
				dlHarqRlcPdu2.at(0).resize(8);
				dlHarqRlcPdu2.at(1).resize(8);
				m_dlHarqProcessesRlcPduListBuffer2.insert(
						std::pair<uint16_t, DlHarqRlcPduListBuffer_t>(params.m_rnti,
								dlHarqRlcPdu2));
				m_ulHarqCurrentProcessId2.insert(
						std::pair<uint16_t, uint8_t>(params.m_rnti, 0));
				UlHarqProcessesStatus_t ulHarqPrcStatus2;
				ulHarqPrcStatus2.resize(8, 0);
				m_ulHarqProcessesStatus2.insert(
						std::pair<uint16_t, UlHarqProcessesStatus_t>(params.m_rnti,
								ulHarqPrcStatus2));
				UlHarqProcessesDciBuffer_t ulHarqdci2;
				ulHarqdci2.resize(8);
				m_ulHarqProcessesDciBuffer2.insert(
						std::pair<uint16_t, UlHarqProcessesDciBuffer_t>(params.m_rnti,ulHarqdci2));
			} else {
				NS_LOG_FUNCTION("<mohamed> phy 2 ---------- 2 -------------- <mohamed>");
				(*it2).second = params.m_transmissionMode;
		     	}
		  } // close of case2 block
		break ;


    } // end of switch case

	//-----------------------------------------
	///////////////////////////////////////////////////////
	return;
}
//----------Mod. end--------------------------------------------------------------------------------
//===================================================================================================
void RrFfMacScheduler::DoCschedLcConfigReq(
		const struct FfMacCschedSapProvider::CschedLcConfigReqParameters& params) {
	NS_LOG_FUNCTION(this);
	// Not used at this stage (LCs updated by DoSchedDlRlcBufferReq)
	return;
}

//-------Mod <Abdelrhman> --------------------

void RrFfMacScheduler::DoCschedLcReleaseReq(
				const struct FfMacCschedSapProvider::CschedLcReleaseReqParameters& params) {

			NS_LOG_FUNCTION(this  << "<phy>" << params.m_phyId << "...CschedLcReleaseReqParameters");
			switch (params.m_phyId)
			{
				case 1:
				{

					for (uint16_t i = 0; i < params.m_logicalChannelIdentity.size(); i++) {
						std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it =
								m_rlcBufferReq.begin();
						while (it != m_rlcBufferReq.end()) {
							if (((*it).m_rnti == params.m_rnti) && ((*it).m_logicalChannelIdentity	== params.m_logicalChannelIdentity.at(i))) {
								it = m_rlcBufferReq.erase(it);
							} else {
								it++;
							}
						}
					}
				}
				break;

			//--------------------------------------
				case 2:
				{
					for (uint16_t i = 0; i < params.m_logicalChannelIdentity.size(); i++) {
						std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq2.begin();
						while (it != m_rlcBufferReq2.end()) {
							if (((*it).m_rnti == params.m_rnti) && ((*it).m_logicalChannelIdentity == params.m_logicalChannelIdentity.at(i))) {
								it = m_rlcBufferReq2.erase(it);
							} else {
								it++;
							}
						}
					}
				}

				 break;
			}//end of switch cases
			//--------------------------------------
			return;
		}
		//-------Mod. end -----------------------------------
		//======================================================-
//---------Mod <Abdelrhman> --------------
void RrFfMacScheduler::DoCschedUeReleaseReq(
		const struct FfMacCschedSapProvider::CschedUeReleaseReqParameters& params) {
	NS_LOG_FUNCTION(this << "For phy" << params.m_phyId << "  Release RNTI " << params.m_rnti );

 switch(params.m_phyId)
 {
	case 1:
	{


		m_uesTxMode.erase(params.m_rnti);
		m_dlHarqCurrentProcessId.erase(params.m_rnti);
		m_dlHarqProcessesStatus.erase(params.m_rnti);
		m_dlHarqProcessesTimer.erase(params.m_rnti);
		m_dlHarqProcessesDciBuffer.erase(params.m_rnti);
		m_dlHarqProcessesRlcPduListBuffer.erase(params.m_rnti);
		m_ulHarqCurrentProcessId.erase(params.m_rnti);
		m_ulHarqProcessesStatus.erase(params.m_rnti);
		m_ulHarqProcessesDciBuffer.erase(params.m_rnti);
		m_ceBsrRxed.erase(params.m_rnti);

		std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq.begin();
			while (it != m_rlcBufferReq.end()) {
				if ((*it).m_rnti == params.m_rnti) {
					NS_LOG_INFO(
							this << "for phy1" <<" Erase RNTI " << (*it).m_rnti << " LC " << (uint16_t)(*it).m_logicalChannelIdentity);
					it = m_rlcBufferReq.erase(it);
				} else {
					it++;
				}
			}
			if (m_nextRntiUl == params.m_rnti) {
				m_nextRntiUl = 0;
			}

			if (m_nextRntiDl == params.m_rnti) {
				m_nextRntiDl = 0;
			}
	}  break;

	//----------------------------------------------

	case 2:
	{

		m_uesTxMode2.erase(params.m_rnti);
		m_dlHarqCurrentProcessId2.erase(params.m_rnti);
		m_dlHarqProcessesStatus2.erase(params.m_rnti);
		m_dlHarqProcessesTimer2.erase(params.m_rnti);
		m_dlHarqProcessesDciBuffer2.erase(params.m_rnti);
		m_dlHarqProcessesRlcPduListBuffer2.erase(params.m_rnti);
		m_ulHarqCurrentProcessId2.erase(params.m_rnti);
		m_ulHarqProcessesStatus2.erase(params.m_rnti);
		m_ulHarqProcessesDciBuffer2.erase(params.m_rnti);
		m_ceBsrRxed2.erase(params.m_rnti);
		//------------------------------



		std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq2.begin();
		while (it != m_rlcBufferReq2.end()) {
			if ((*it).m_rnti == params.m_rnti) {



				NS_LOG_INFO(this <<  "for phy2" << " Erase RNTI " << (*it).m_rnti << " LC " << (uint16_t)(*it).m_logicalChannelIdentity);
				it = m_rlcBufferReq2.erase(it);
			} else {
				it++;
			}
		}
		if (m_nextRntiUl2 == params.m_rnti) {
			m_nextRntiUl2 = 0;
		}

		if (m_nextRntiDl2 == params.m_rnti) {
			m_nextRntiDl2 = 0;
		}

	} break ;


	//----------------------------------------
 }
	return;
}

//==================================================================================
//-----Mod end ------------------------------

//==================================================================================

////------Mod <Abdelrhman> ----------------------
//
//void RrFfMacScheduler::DoSchedDlRlcBufferReq(
//		const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params ,
//		std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>& m_rlcBufferReq,
//		std::map <uint16_t,uint8_t>& m_p10CqiRxedx ,  std::map <uint16_t,uint32_t>& m_p10CqiTimersx ) {
void RrFfMacScheduler::DoSchedDlRlcBufferReq(
		const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params){

	NS_LOG_FUNCTION(this << "<phy>" << params.m_phyId << params.m_rnti << (uint32_t) params.m_logicalChannelIdentity);
	// API generated by RLC for updating RLC parameters on a LC (tx and retx queues)
	switch(params.m_phyId){

	case 1:
	{
	std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq.begin();
	bool newLc = true;
	while (it != m_rlcBufferReq.end()) {

		// remove old entries of this UE-LC
		if (((*it).m_rnti == params.m_rnti)
				&& ((*it).m_logicalChannelIdentity
						== params.m_logicalChannelIdentity)) {
			it = m_rlcBufferReq.erase(it);
			newLc = false;          // found
		} else {
			++it;
		}
	}
	// add the new parameters
	m_rlcBufferReq.insert(it, params);
	NS_LOG_INFO(this << " RNTI " << params.m_rnti << " LC " << (uint16_t)params.m_logicalChannelIdentity << " RLC tx size " << params.m_rlcTransmissionQueueHolDelay << " RLC retx size " << params.m_rlcRetransmissionQueueSize << " RLC stat size " << params.m_rlcStatusPduSize);
	// initialize statistics of the flow in case of new flows
	if (newLc == true) // this Ue with the given Rnti not found among the LCs so insert it new lc and initialize it
			{
		m_p10CqiRxed.insert(std::pair<uint16_t, uint8_t>(params.m_rnti, 1)); // only codeword 0 at this stage (SISO)
		// initialized to 1 (i.e., the lowest value for transmitting a signal)
		m_p10CqiTimers.insert(
				std::pair<uint16_t, uint32_t>(params.m_rnti,
						m_cqiTimersThreshold));
	}
	} break;
////////////////////////////////////////////////////////////////////////////////
	//-------------------------------------------------------

	case 2:
	{
		// API generated by RLC for updating RLC parameters on a LC (tx and retx queues)
		std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq2.begin();
		bool  newLc = true;
		while (it != m_rlcBufferReq2.end()) {

			// remove old entries of this UE-LC
			if (((*it).m_rnti == params.m_rnti)
					&& ((*it).m_logicalChannelIdentity
							== params.m_logicalChannelIdentity)) {
				it = m_rlcBufferReq2.erase(it);
				newLc = false;          // found
			} else {
				++it;
			}
		}
		// add the new parameters
		m_rlcBufferReq2.insert(it, params);
		NS_LOG_INFO(this << "phy2"<<" RNTI " << params.m_rnti << " LC " << (uint16_t)params.m_logicalChannelIdentity << " RLC tx size " << params.m_rlcTransmissionQueueHolDelay << " RLC retx size " << params.m_rlcRetransmissionQueueSize << " RLC stat size " << params.m_rlcStatusPduSize);
		// initialize statistics of the flow in case of new flows
		if (newLc == true) // this Ue with the given Rnti not found among the LCs so insert it new lc and initialize it
				{
			m_p10CqiRxed2.insert(std::pair<uint16_t, uint8_t>(params.m_rnti, 1)); // only codeword 0 at this stage (SISO)
			// initialized to 1 (i.e., the lowest value for transmitting a signal)
			m_p10CqiTimers.insert(std::pair<uint16_t, uint32_t>(params.m_rnti,m_cqiTimersThreshold));
		}

	//-------------------------------------------------------
	} break ;

	}

	return;
}
//-----Mod. end
//=================================================================================

//-----not impemented -------
void RrFfMacScheduler::DoSchedDlPagingBufferReq(
		const struct FfMacSchedSapProvider::SchedDlPagingBufferReqParameters& params) {
	NS_LOG_FUNCTION(this);
	NS_FATAL_ERROR("method not implemented");
	return;
}
//======================================

//-----not impemented -------
void RrFfMacScheduler::DoSchedDlMacBufferReq(
		const struct FfMacSchedSapProvider::SchedDlMacBufferReqParameters& params) {
	NS_LOG_FUNCTION(this);
	NS_FATAL_ERROR("method not implemented");
	return;
}
//=====================================

int RrFfMacScheduler::GetRbgSize(int dlbandwidth) {
	for (int i = 0; i < 4; i++) {
		if (dlbandwidth < Type0AllocationRbg[i]) {
			return (i + 1);
		}
	}

	return (-1);
}

bool RrFfMacScheduler::SortRlcBufferReq(
		FfMacSchedSapProvider::SchedDlRlcBufferReqParameters i,
		FfMacSchedSapProvider::SchedDlRlcBufferReqParameters j) {
	return (i.m_rnti < j.m_rnti);
}

uint8_t RrFfMacScheduler::HarqProcessAvailability(uint16_t rnti) {
	NS_LOG_FUNCTION(this << rnti);

	std::map<uint16_t, uint8_t>::iterator it = m_dlHarqCurrentProcessId.find(
			rnti);
	if (it == m_dlHarqCurrentProcessId.end()) {
		NS_FATAL_ERROR("No Process Id found for this RNTI " << rnti);
	}
	std::map<uint16_t, DlHarqProcessesStatus_t>::iterator itStat =
			m_dlHarqProcessesStatus.find(rnti);
	if (itStat == m_dlHarqProcessesStatus.end()) {
		NS_FATAL_ERROR("No Process Id Status found for this RNTI " << rnti);
	}
	uint8_t i = (*it).second;
	do {
		i = (i + 1) % HARQ_PROC_NUM;
	} while (((*itStat).second.at(i) != 0) && (i != (*it).second));
	if ((*itStat).second.at(i) == 0) {
		return (true);
	} else {
		return (false); // return a not valid harq proc id
	}
}

uint8_t RrFfMacScheduler::UpdateHarqProcessId(uint16_t rnti , uint16_t id) {
	NS_LOG_FUNCTION(this << id <<"   " <<rnti);

	if (m_harqOn == false) {
		return (0);
	}

	std::map<uint16_t, uint8_t>::iterator it = m_dlHarqCurrentProcessId.find(
			rnti);
	if (it == m_dlHarqCurrentProcessId.end()) {
		NS_FATAL_ERROR("No Process Id found for this RNTI " << rnti);
	}
	std::map<uint16_t, DlHarqProcessesStatus_t>::iterator itStat =
			m_dlHarqProcessesStatus.find(rnti);
	if (itStat == m_dlHarqProcessesStatus.end()) {
		NS_FATAL_ERROR("No Process Id Statusfound for this RNTI " << rnti);
	}
	uint8_t i = (*it).second;
	do {
		i = (i + 1) % HARQ_PROC_NUM;
	} while (((*itStat).second.at(i) != 0) && (i != (*it).second));
	if ((*itStat).second.at(i) == 0) {
		(*it).second = i;
		(*itStat).second.at(i) = 1;
	} else {
		return (9); // return a not valid harq proc id <as max valid no. is 8 for 8 processes>
	}

	return ((*it).second);
}


/*                        ^^^^^^^^^^^^^^^^FIZO^^^^^^^^^^^^^^^^^^^^^
 * l map byb2a fiha kol process w l timer bta3ha, l fun. bt3ml loop 3la kol l processes
 * kol mra for loop mn 0 to 8 (kol l processes)
 * kol process bnshoof l timer bta3ha 7salo timeout wla la ( note : HARQ_DL_TIMEOUT = 11 !!!!!!!!! )
 * lw true bn3ml reset bn-search 3la l status w nkhali  l status w l timer = 0
 *                        ^^^^^^^^^^^^^^^^FIZO^^^^^^^^^^^^^^^^^^^^^
 */
//----------Modified <Abdelrhman> ------------------//
//---------------------------------------------------
void RrFfMacScheduler::RefreshHarqProcesses(uint16_t id) {
	NS_LOG_FUNCTION(this << "PhyId= " << id);

	std::map<uint16_t, DlHarqProcessesTimer_t>::iterator itTimers;


	switch (id)
		{
		case 1:
		{
			//NS_LOG_FUNCTION(this << "<mohamed> phy 1 <mohamed>" << m_dlHarqProcessesTimer.begin() << m_dlHarqProcessesTimer.end());
		for (itTimers = m_dlHarqProcessesTimer.begin();
				itTimers != m_dlHarqProcessesTimer.end(); itTimers++) {
			for (uint16_t i = 0; i < HARQ_PROC_NUM; i++) {
				if ((*itTimers).second.at(i) == HARQ_DL_TIMEOUT) {
					NS_LOG_FUNCTION(this << "<mohamed> ------------ 11 --------");
					// reset HARQ process
					NS_LOG_INFO(
							this << " Reset HARQ proc " << i << " for RNTI " << (*itTimers).first);
					std::map<uint16_t, DlHarqProcessesStatus_t>::iterator itStat =
							m_dlHarqProcessesStatus.find((*itTimers).first);
					if (itStat == m_dlHarqProcessesStatus.end()) {
						NS_FATAL_ERROR(
								"No Process Id Status found for this RNTI " << (*itTimers).first);
					}
					(*itStat).second.at(i) = 0;
					(*itTimers).second.at(i) = 0;
				} else {
					(*itTimers).second.at(i)++;}
				}
			}
		}break;
		//-------------------------------------
		case 2:
		{
			NS_LOG_FUNCTION(this << "<mohamed> phy " << id << " <mohamed>");
		for (itTimers = m_dlHarqProcessesTimer2.begin();
							itTimers != m_dlHarqProcessesTimer2.end(); itTimers++) {
						for (uint16_t i = 0; i < HARQ_PROC_NUM; i++) {
							if ((*itTimers).second.at(i) == HARQ_DL_TIMEOUT) {
								// reset HARQ process
								NS_LOG_INFO(
										this << "<phy2>"<<" Reset HARQ proc " << i << " for RNTI " << (*itTimers).first);
								std::map<uint16_t, DlHarqProcessesStatus_t>::iterator itStat =
										m_dlHarqProcessesStatus2.find((*itTimers).first);
								if (itStat == m_dlHarqProcessesStatus2.end()) {
									NS_FATAL_ERROR(
											"<phy2> No Process Id Status found for this RNTI " << (*itTimers).first);
								}
								(*itStat).second.at(i) = 0;
								(*itTimers).second.at(i) = 0;
							} else {
								(*itTimers).second.at(i)++;}
							}
						}
		}break;
	//-------------------------------------

    //-------------------------------------
	  }
	NS_LOG_FUNCTION(this << "<mohamed> End of Refresh Harq <mohamed>");
	}
//------MOd. end ----------------------------------
//=================================================

//----------Modified <Abdelrhman> ------------------//
//---------------------------------------------------
void RrFfMacScheduler::DoSchedDlTriggerReq(const struct FfMacSchedSapProvider::SchedDlTriggerReqParameters& params )

{
	NS_LOG_FUNCTION(this << "phy" << params.m_phyId<< " DL Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf));
	// API generated by RLC for triggering the scheduling of a DL subframe

	RefreshDlCqiMaps(params.m_phyId);    // ---  does it need duplication ???

	//-----------------------------

	std::vector<bool> rbgMap;
	uint16_t rbgAllocatedNum = 0;
	std::set<uint16_t> rntiAllocated;
	FfMacSchedSapUser::SchedDlConfigIndParameters ret ;
	ret.m_phyId = params.m_phyId;
	std::vector<struct DlInfoListElement_s> dlInfoListUntxed;

	// Get the actual active flows (queue!=0)
	std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it, it2;


	m_rlcBufferReq.sort(SortRlcBufferReq);
	//-----------
	m_rlcBufferReq2.sort(SortRlcBufferReq);
	//-------------
	int nflows = 0;
	int nTbs = 0;
	//----------------
	int nflows2 = 0;
    int nTbs2 = 0;
	//-------------
	std::map<uint16_t, uint8_t> lcActivesPerRnti,lcActivesPerRnti2;// tracks how many active LCs per RNTI there are
	std::map<uint16_t, uint8_t>::iterator itLcRnti,itLcRnti2;

	std::map<uint16_t, uint8_t>::iterator itTxMode;

	//------------------------------
	switch (params.m_phyId)
	{
	case 1:

	{
	NS_LOG_FUNCTION(this << " <mohamed> Phy 1 <mohamed>");
	int rbgSize = GetRbgSize(m_cschedCellConfig.m_dlBandwidth);     //////////
	int rbgNum = m_cschedCellConfig.m_dlBandwidth / rbgSize;        //////////
	// Generate RBGs map

		rbgMap.resize(m_cschedCellConfig.m_dlBandwidth / rbgSize, false);       ////////


		//   update UL HARQ proc id
		std::map<uint16_t, uint8_t>::iterator itProcId;
		for (itProcId = m_ulHarqCurrentProcessId.begin();
				itProcId != m_ulHarqCurrentProcessId.end(); itProcId++) {
			(*itProcId).second = ((*itProcId).second + 1) % HARQ_PROC_NUM;
		}
   //----------------------------------------
		// RACH Allocation
			m_rachAllocationMap.resize(m_cschedCellConfig.m_ulBandwidth, 0);
			uint16_t rbStart = 0;
			std::vector<struct RachListElement_s>::iterator itRach;
			//------------------------------
			for (itRach = m_rachList.begin(); itRach != m_rachList.end(); itRach++) {

					///////////////////
					NS_ASSERT_MSG(
							m_amc->GetTbSizeFromMcs(m_ulGrantMcs,
									m_cschedCellConfig.m_ulBandwidth)
									> (*itRach).m_estimatedSize,
							" Default UL Grant MCS does not allow to send RACH messages");

					//////////////////////
					BuildRarListElement_s newRar;
					newRar.m_rnti = (*itRach).m_rnti;
					// DL-RACH Allocation
					// Ideal: no needs of configuring m_dci
					// UL-RACH Allocation
					newRar.m_grant.m_rnti = newRar.m_rnti;
					newRar.m_grant.m_mcs = m_ulGrantMcs;
					uint16_t rbLen = 1;
					uint16_t tbSizeBits = 0;
					// find lowest TB size that fits UL grant estimated size
					while ((tbSizeBits < (*itRach).m_estimatedSize)
							&& (rbStart + rbLen < m_cschedCellConfig.m_ulBandwidth)) {
						rbLen++;
						tbSizeBits = m_amc->GetTbSizeFromMcs(m_ulGrantMcs, rbLen);
					}
					if (tbSizeBits < (*itRach).m_estimatedSize) {
						// no more allocation space: finish allocation
						break;
					}
					newRar.m_grant.m_rbStart = rbStart;
					newRar.m_grant.m_rbLen = rbLen;
					newRar.m_grant.m_tbSize = tbSizeBits / 8;
					newRar.m_grant.m_hopping = false;
					newRar.m_grant.m_tpc = 0;
					newRar.m_grant.m_cqiRequest = false;
					newRar.m_grant.m_ulDelay = false;
					NS_LOG_INFO(
							this << " UL grant allocated to RNTI " << (*itRach).m_rnti << " rbStart " << rbStart << " rbLen " << rbLen << " MCS " << (uint16_t) m_ulGrantMcs << " tbSize " << newRar.m_grant.m_tbSize);
					for (uint16_t i = rbStart; i < rbStart + rbLen; i++) {
						m_rachAllocationMap.at(i) = (*itRach).m_rnti;
					}

					if (m_harqOn == true) {
						// generate UL-DCI for HARQ retransmissions
						UlDciListElement_s uldci;
						uldci.m_rnti = newRar.m_rnti;
						uldci.m_rbLen = rbLen;
						uldci.m_rbStart = rbStart;
						uldci.m_mcs = m_ulGrantMcs;
						uldci.m_tbSize = tbSizeBits / 8;
						uldci.m_ndi = 1;
						uldci.m_cceIndex = 0;
						uldci.m_aggrLevel = 1;
						uldci.m_ueTxAntennaSelection = 3; // antenna selection OFF
						uldci.m_hopping = false;
						uldci.m_n2Dmrs = 0;
						uldci.m_tpc = 0; // no power control
						uldci.m_cqiRequest = false; // only period CQI at this stage
						uldci.m_ulIndex = 0; // TDD parameter
						uldci.m_dai = 1; // TDD parameter
						uldci.m_freqHopping = 0;
						uldci.m_pdcchPowerOffset = 0; // not used

						uint8_t harqId = 0;
						std::map<uint16_t, uint8_t>::iterator itProcId;
						itProcId = m_ulHarqCurrentProcessId.find(uldci.m_rnti);
						if (itProcId == m_ulHarqCurrentProcessId.end()) {
							NS_FATAL_ERROR(
									"No info find in HARQ buffer for UE " << uldci.m_rnti);
						}
						harqId = (*itProcId).second;
						std::map<uint16_t, UlHarqProcessesDciBuffer_t>::iterator itDci =
								m_ulHarqProcessesDciBuffer.find(uldci.m_rnti);
						if (itDci == m_ulHarqProcessesDciBuffer.end()) {
							NS_FATAL_ERROR(
									"Unable to find RNTI entry in UL DCI HARQ buffer for RNTI " << uldci.m_rnti);
						}
						(*itDci).second.at(harqId) = uldci;
					}

					rbStart = rbStart + rbLen;
					ret.m_buildRarList.push_back(newRar);
				}
			//-------------------
			m_rachList.clear();
			//------------------
			//-------Process DL HARQ feedback
			RefreshHarqProcesses(params.m_phyId);
			NS_LOG_FUNCTION("<mohamed> ----------- 1 --------------<mohamed>" << m_dlInfoListBuffered.size());
		   //---------
			//---- retrieve past HARQ retx buffered

				if (m_dlInfoListBuffered.size() > 0) {
					if (params.m_dlInfoList.size() > 0) {
						NS_LOG_INFO(this << "<phy1> Received DL-HARQ feedback");
						m_dlInfoListBuffered.insert(m_dlInfoListBuffered.end(),
								params.m_dlInfoList.begin(), params.m_dlInfoList.end());
					}
				} else {
					if (params.m_dlInfoList.size() > 0) {
						NS_LOG_FUNCTION("<mohamed> ----------- 1* --------------<mohamed>");
						m_dlInfoListBuffered = params.m_dlInfoList;
					}
				}
	//----------------------------------
				if (m_harqOn == false) {
							// Ignore HARQ feedback
							m_dlInfoListBuffered.clear();

							//------------------------------
						}
	//----------------------------------
				for (uint16_t i = 0; i < m_dlInfoListBuffered.size(); i++) {
					NS_LOG_FUNCTION(this << "<mohamed> --------- 2 ---------------- <mohamed>");
					std::set<uint16_t>::iterator itRnti = rntiAllocated.find(
							m_dlInfoListBuffered.at(i).m_rnti);
					if (itRnti != rntiAllocated.end()) {
						// RNTI already allocated for retx
						NS_LOG_FUNCTION(this << "<mohamed> --------- 3 ---------------- <mohamed>");
						continue;
					}
					uint8_t nLayers = m_dlInfoListBuffered.at(i).m_harqStatus.size();
					std::vector<bool> retx;
					NS_LOG_FUNCTION(this << "<mohamed> --------- 4 ---------------- <mohamed>");
					NS_LOG_INFO(this << " Processing DLHARQ feedback");
					if (nLayers == 1) {
						retx.push_back(
								m_dlInfoListBuffered.at(i).m_harqStatus.at(0)
										== DlInfoListElement_s::NACK);
						retx.push_back(false);
					} else {
						retx.push_back(
								m_dlInfoListBuffered.at(i).m_harqStatus.at(0)
										== DlInfoListElement_s::NACK);
						retx.push_back(
								m_dlInfoListBuffered.at(i).m_harqStatus.at(1)
										== DlInfoListElement_s::NACK);
					}
					if (retx.at(0) || retx.at(1)) {
						// retrieve HARQ process information
						uint16_t rnti = m_dlInfoListBuffered.at(i).m_rnti;
						uint8_t harqId = m_dlInfoListBuffered.at(i).m_harqProcessId;
						NS_LOG_INFO(
								this << " HARQ retx RNTI " << rnti << " harqId " << (uint16_t)harqId);
						std::map<uint16_t, DlHarqProcessesDciBuffer_t>::iterator itHarq =
								m_dlHarqProcessesDciBuffer.find(rnti);
						if (itHarq == m_dlHarqProcessesDciBuffer.end()) {
							NS_FATAL_ERROR("No info find in HARQ buffer for UE " << rnti);
						}

						DlDciListElement_s dci = (*itHarq).second.at(harqId);
						int rv = 0;
						if (dci.m_rv.size() == 1) {
							rv = dci.m_rv.at(0);
						} else {
							rv = (dci.m_rv.at(0) > dci.m_rv.at(1) ?
									dci.m_rv.at(0) : dci.m_rv.at(1));
						}

						if (rv == 3) {
							// maximum number of retx reached -> drop process
							NS_LOG_INFO(
									"Max number of retransmissions reached -> drop process");
							std::map<uint16_t, DlHarqProcessesStatus_t>::iterator it =
									m_dlHarqProcessesStatus.find(rnti);
							if (it == m_dlHarqProcessesStatus.end()) {
								NS_LOG_ERROR(
										"No info find in HARQ buffer for UE (might change eNB) " << m_dlInfoListBuffered.at (i).m_rnti);
							}
							(*it).second.at(harqId) = 0;
							std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
									m_dlHarqProcessesRlcPduListBuffer.find(rnti);
							if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer.end()) {
								NS_FATAL_ERROR(
										"Unable to find RlcPdcList in HARQ buffer for RNTI " << m_dlInfoListBuffered.at (i).m_rnti);
							}
							for (uint16_t k = 0; k < (*itRlcPdu).second.size(); k++) {
								(*itRlcPdu).second.at(k).at(harqId).clear();
							}
							continue;
						}
						// check the feasibility of retransmitting on the same RBGs
						// translate the DCI to Spectrum framework
						std::vector<int> dciRbg;
						uint32_t mask = 0x1;
						NS_LOG_INFO(
								"Original RBGs " << dci.m_rbBitmap << " rnti " << dci.m_rnti);
						for (int j = 0; j < 32; j++) {
							if (((dci.m_rbBitmap & mask) >> j) == 1) {
								dciRbg.push_back(j);
								NS_LOG_INFO("\t" << j);
							}
							mask = (mask << 1);
						}
						bool free = true;
						for (uint8_t j = 0; j < dciRbg.size(); j++) {
							if (rbgMap.at(dciRbg.at(j)) == true) {
								free = false;
								break;
							}
						}
						if (free) {
							// use the same RBGs for the retx
							// reserve RBGs
							for (uint8_t j = 0; j < dciRbg.size(); j++) {
								rbgMap.at(dciRbg.at(j)) = true;
								NS_LOG_INFO("RBG " << dciRbg.at (j) << " assigned");
								rbgAllocatedNum++;
							}

							NS_LOG_INFO(this << " Send retx in the same RBGs");
						} else {
							// find RBGs for sending HARQ retx
							uint8_t j = 0;
							uint8_t rbgId = (dciRbg.at(dciRbg.size() - 1) + 1) % rbgNum;
							uint8_t startRbg = dciRbg.at(dciRbg.size() - 1);
							std::vector<bool> rbgMapCopy = rbgMap;
							while ((j < dciRbg.size()) && (startRbg != rbgId)) {
								if (rbgMapCopy.at(rbgId) == false) {
									rbgMapCopy.at(rbgId) = true;
									dciRbg.at(j) = rbgId;
									j++;
								}
								rbgId = (rbgId + 1) % rbgNum;
							}
							if (j == dciRbg.size()) {
								// find new RBGs -> update DCI map
								uint32_t rbgMask = 0;
								for (uint16_t k = 0; k < dciRbg.size(); k++) {
									rbgMask = rbgMask + (0x1 << dciRbg.at(k));
									NS_LOG_INFO(
											this << " New allocated RBG " << dciRbg.at (k));
									rbgAllocatedNum++;
								}
								dci.m_rbBitmap = rbgMask;
								rbgMap = rbgMapCopy;
							} else {
								// HARQ retx cannot be performed on this TTI -> store it
								dlInfoListUntxed.push_back(m_dlInfoListBuffered.at(i));
								NS_LOG_INFO(
										this << " No resource for this retx -> buffer it");
							}
						}
						// retrieve RLC PDU list for retx TBsize and update DCI
						BuildDataListElement_s newEl;
						std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
								m_dlHarqProcessesRlcPduListBuffer.find(rnti);
						if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer.end()) {
							NS_FATAL_ERROR(
									"Unable to find RlcPdcList in HARQ buffer for RNTI " << rnti);
						}
						for (uint8_t j = 0; j < nLayers; j++) {
							if (retx.at(j)) {
								if (j >= dci.m_ndi.size()) {
									// for avoiding errors in MIMO transient phases
									dci.m_ndi.push_back(0);
									dci.m_rv.push_back(0);
									dci.m_mcs.push_back(0);
									dci.m_tbsSize.push_back(0);
									NS_LOG_INFO(
											this << " layer " << (uint16_t)j << " no txed (MIMO transition)");

								} else {
									dci.m_ndi.at(j) = 0;
									dci.m_rv.at(j)++;(
			                        *itHarq).second.at (harqId).m_rv.at (j)++;
									NS_LOG_INFO(
											this << " layer " << (uint16_t)j << " RV " << (uint16_t)dci.m_rv.at (j));
								}
							} else {
								// empty TB of layer j
								dci.m_ndi.at(j) = 0;
								dci.m_rv.at(j) = 0;
								dci.m_mcs.at(j) = 0;
								dci.m_tbsSize.at(j) = 0;
								NS_LOG_INFO(this << " layer " << (uint16_t)j << " no retx");
							}
						}

						for (uint16_t k = 0;
								k < (*itRlcPdu).second.at(0).at(dci.m_harqProcess).size();
								k++) {
							std::vector<struct RlcPduListElement_s> rlcPduListPerLc;
							for (uint8_t j = 0; j < nLayers; j++) {
								if (retx.at(j)) {
									if (j < dci.m_ndi.size()) {
										rlcPduListPerLc.push_back(
												(*itRlcPdu).second.at(j).at(
														dci.m_harqProcess).at(k));
									}
								}
							}

							if (rlcPduListPerLc.size() > 0) {
								newEl.m_rlcPduList.push_back(rlcPduListPerLc);
							}
						}
						newEl.m_rnti = rnti;
						newEl.m_dci = dci;
						(*itHarq).second.at(harqId).m_rv = dci.m_rv;
						// refresh timer
						std::map<uint16_t, DlHarqProcessesTimer_t>::iterator itHarqTimer =
								m_dlHarqProcessesTimer.find(rnti);
						if (itHarqTimer == m_dlHarqProcessesTimer.end()) {
							NS_FATAL_ERROR(
									"Unable to find HARQ timer for RNTI " << (uint16_t)rnti);
						}
						(*itHarqTimer).second.at(harqId) = 0;
						ret.m_buildDataList.push_back(newEl);
						rntiAllocated.insert(rnti);
					} else {
						// update HARQ process status
						NS_LOG_INFO(
								this << " HARQ ACK UE " << m_dlInfoListBuffered.at (i).m_rnti);
						std::map<uint16_t, DlHarqProcessesStatus_t>::iterator it =
								m_dlHarqProcessesStatus.find(
										m_dlInfoListBuffered.at(i).m_rnti);
						if (it == m_dlHarqProcessesStatus.end()) {
							NS_FATAL_ERROR(
									"No info find in HARQ buffer for UE " << m_dlInfoListBuffered.at (i).m_rnti);
						}
						(*it).second.at(m_dlInfoListBuffered.at(i).m_harqProcessId) = 0;
						std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
								m_dlHarqProcessesRlcPduListBuffer.find(
										m_dlInfoListBuffered.at(i).m_rnti);
						if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer.end()) {
							NS_FATAL_ERROR(
									"Unable to find RlcPdcList in HARQ buffer for RNTI " << m_dlInfoListBuffered.at (i).m_rnti);
						}
						for (uint16_t k = 0; k < (*itRlcPdu).second.size(); k++) {
							(*itRlcPdu).second.at(k).at(
									m_dlInfoListBuffered.at(i).m_harqProcessId).clear();
						}
					}
				}
				m_dlInfoListBuffered.clear();
				m_dlInfoListBuffered = dlInfoListUntxed;
	//-------------------------------------------------------------------

				if (rbgAllocatedNum == rbgNum) {
					// all the RBGs are already allocated -> exit
					if ((ret.m_buildDataList.size() > 0)
							|| (ret.m_buildRarList.size() > 0)) {
						m_schedSapUser->SchedDlConfigInd(ret);
					}
					return;
				}
	//------------------------------------------
				for (it = m_rlcBufferReq.begin(); it != m_rlcBufferReq.end(); it++) {
						// remove old entries of this UE-LC
						std::set<uint16_t>::iterator itRnti = rntiAllocated.find((*it).m_rnti);
						if ((((*it).m_rlcTransmissionQueueSize > 0)
								|| ((*it).m_rlcRetransmissionQueueSize > 0)
								|| ((*it).m_rlcStatusPduSize > 0))
								&& (itRnti == rntiAllocated.end()) // UE must not be allocated for HARQ retx
								&& (HarqProcessAvailability((*it).m_rnti))) // UE needs HARQ proc free

								{
							NS_LOG_LOGIC(
									this << " User " << (*it).m_rnti << " LC " << (uint16_t)(*it).m_logicalChannelIdentity << " is active, status  " << (*it).m_rlcStatusPduSize << " retx " << (*it).m_rlcRetransmissionQueueSize << " tx " << (*it).m_rlcTransmissionQueueSize);
							std::map<uint16_t, uint8_t>::iterator itCqi = m_p10CqiRxed.find(
									(*it).m_rnti);
							uint8_t cqi = 0;
							if (itCqi != m_p10CqiRxed.end()) {
								cqi = (*itCqi).second;
							} else {
								cqi = 1; // lowest value fro trying a transmission
							}
							if (cqi != 0) {
								// CQI == 0 means "out of range" (see table 7.2.3-1 of 36.213)
								nflows++;
								itLcRnti = lcActivesPerRnti.find((*it).m_rnti);
								if (itLcRnti != lcActivesPerRnti.end()) {
									(*itLcRnti).second++;
								} else {
									lcActivesPerRnti.insert(
											std::pair<uint16_t, uint8_t>((*it).m_rnti, 1));
									nTbs++;
								}

							}
						}
					}
	//---------------------------------------------

				if (nflows == 0) {
					if ((ret.m_buildDataList.size() > 0)
							|| (ret.m_buildRarList.size() > 0)) {
						m_schedSapUser->SchedDlConfigInd(ret);
					}
					return;
				}
				//--------------------------------------------
				// Divide the resource equally among the active users according to
					// Resource allocation type 0 (see sec 7.1.6.1 of 36.213)

					int rbgPerTb = (nTbs > 0) ? ((rbgNum - rbgAllocatedNum) / nTbs) : INT_MAX;
					NS_LOG_INFO(this << " Flows to be transmitted " << nflows << " rbgPerTb " << rbgPerTb);
					//------------------------------

					if (rbgPerTb == 0) {
						rbgPerTb = 1;         // at least 1 rbg per TB (till available resource)
					}


					int rbgAllocated = 0;

	//---------------------------------------------

					// round robin assignment to all UEs registered starting from the subsequent of the one
					// served last scheduling trigger event
					if (m_nextRntiDl != 0) {
						NS_LOG_DEBUG(
								"Start from the successive of " << (uint16_t) m_nextRntiDl);
						for (it = m_rlcBufferReq.begin(); it != m_rlcBufferReq.end(); it++) {
							if ((*it).m_rnti == m_nextRntiDl) {
								// select the next RNTI to starting
								it++;
								if (it == m_rlcBufferReq.end()) {
									it = m_rlcBufferReq.begin();
								}
								m_nextRntiDl = (*it).m_rnti;
								break;
							}
						}

						if (it == m_rlcBufferReq.end()) {
							NS_LOG_ERROR(this << " no user found");
						}
					} else {
						it = m_rlcBufferReq.begin();
						m_nextRntiDl = (*it).m_rnti;
					}
	//---------------------------------------------
					do {
							itLcRnti = lcActivesPerRnti.find((*it).m_rnti);
							std::set<uint16_t>::iterator itRnti = rntiAllocated.find((*it).m_rnti);
							if ((itLcRnti == lcActivesPerRnti.end())
									|| (itRnti != rntiAllocated.end())) {
								// skip this RNTI (no active queue or yet allocated for HARQ)
								uint16_t rntiDiscared = (*it).m_rnti;
								while (it != m_rlcBufferReq.end()) {
									if ((*it).m_rnti != rntiDiscared) {
										break;
									}
									it++;
								}
								if (it == m_rlcBufferReq.end()) {
									// restart from the first
									it = m_rlcBufferReq.begin();
								}
								continue;
							}
							itTxMode = m_uesTxMode.find((*it).m_rnti);
							if (itTxMode == m_uesTxMode.end()) {
								NS_FATAL_ERROR(
										"No Transmission Mode info on user " << (*it).m_rnti);
							}
							int nLayer = TransmissionModesLayers::TxMode2LayerNum(
									(*itTxMode).second);
							int lcNum = (*itLcRnti).second;
							// create new BuildDataListElement_s for this RNTI
							BuildDataListElement_s newEl;
							newEl.m_rnti = (*it).m_rnti;
							// create the DlDciListElement_s
							DlDciListElement_s newDci;
							newDci.m_rnti = (*it).m_rnti;
							newDci.m_harqProcess = UpdateHarqProcessId((*it).m_rnti,params.m_phyId);
							newDci.m_resAlloc = 0;
							newDci.m_rbBitmap = 0;
							std::map<uint16_t, uint8_t>::iterator itCqi = m_p10CqiRxed.find(
									newEl.m_rnti);
							for (uint8_t i = 0; i < nLayer; i++) {
								if (itCqi == m_p10CqiRxed.end()) {
									newDci.m_mcs.push_back(0); // no info on this user -> lowest MCS
								} else {
									newDci.m_mcs.push_back(m_amc->GetMcsFromCqi((*itCqi).second));
								}
							}
							int tbSize = (m_amc->GetTbSizeFromMcs(newDci.m_mcs.at(0),
									rbgPerTb * rbgSize) / 8);
							uint16_t rlcPduSize = tbSize / lcNum;
							while ((*it).m_rnti == newEl.m_rnti) {
								if (((*it).m_rlcTransmissionQueueSize > 0)
										|| ((*it).m_rlcRetransmissionQueueSize > 0)
										|| ((*it).m_rlcStatusPduSize > 0)) {
									std::vector<struct RlcPduListElement_s> newRlcPduLe;
									for (uint8_t j = 0; j < nLayer; j++) {
										RlcPduListElement_s newRlcEl;
										newRlcEl.m_logicalChannelIdentity =
												(*it).m_logicalChannelIdentity;
										NS_LOG_INFO(
												this << "LCID " << (uint32_t) newRlcEl.m_logicalChannelIdentity << " size " << rlcPduSize << " ID " << (*it).m_rnti << " layer " << (uint16_t)j);
										newRlcEl.m_size = rlcPduSize;
										UpdateDlRlcBufferInfo((*it).m_rnti,	newRlcEl.m_logicalChannelIdentity, rlcPduSize , params.m_phyId);
										newRlcPduLe.push_back(newRlcEl);

										if (m_harqOn == true) {
											// store RLC PDU list for HARQ
											std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
													m_dlHarqProcessesRlcPduListBuffer.find(
															(*it).m_rnti);
											if (itRlcPdu
													== m_dlHarqProcessesRlcPduListBuffer.end()) {
												NS_FATAL_ERROR(
														"Unable to find RlcPdcList in HARQ buffer for RNTI " << (*it).m_rnti);
											}
											(*itRlcPdu).second.at(j).at(newDci.m_harqProcess).push_back(
													newRlcEl);
										}

									}
									newEl.m_rlcPduList.push_back(newRlcPduLe);
									lcNum--;
								}
								it++;
								if (it == m_rlcBufferReq.end()) {
									// restart from the first
									it = m_rlcBufferReq.begin();
									break;
								}
							}
							uint32_t rbgMask = 0;
							uint16_t i = 0;
							NS_LOG_INFO(
									this << " DL - Allocate user " << newEl.m_rnti << " LCs " << (uint16_t)(*itLcRnti).second << " bytes " << tbSize << " mcs " << (uint16_t) newDci.m_mcs.at (0) << " harqId " << (uint16_t)newDci.m_harqProcess << " layers " << nLayer);
							NS_LOG_INFO("RBG:");
							while (i < rbgPerTb) {
								if (rbgMap.at(rbgAllocated) == false) {
									rbgMask = rbgMask + (0x1 << rbgAllocated);
									NS_LOG_INFO("\t " << rbgAllocated);
									i++;
									rbgMap.at(rbgAllocated) = true;
									rbgAllocatedNum++;
								}
								rbgAllocated++;
							}
							newDci.m_rbBitmap = rbgMask; // (32 bit bitmap see 7.1.6 of 36.213)

							for (int i = 0; i < nLayer; i++) {
								newDci.m_tbsSize.push_back(tbSize);
								newDci.m_ndi.push_back(1);
								newDci.m_rv.push_back(0);
							}

							newDci.m_tpc = 1; //1 is mapped to 0 in Accumulated Mode and to -1 in Absolute Mode

							newEl.m_dci = newDci;
							if (m_harqOn == true) {
								// store DCI for HARQ
								std::map<uint16_t, DlHarqProcessesDciBuffer_t>::iterator itDci =
										m_dlHarqProcessesDciBuffer.find(newEl.m_rnti);
								if (itDci == m_dlHarqProcessesDciBuffer.end()) {
									NS_FATAL_ERROR(
											"Unable to find RNTI entry in DCI HARQ buffer for RNTI " << newEl.m_rnti);
								}
								(*itDci).second.at(newDci.m_harqProcess) = newDci;
								// refresh timer
								std::map<uint16_t, DlHarqProcessesTimer_t>::iterator itHarqTimer =
										m_dlHarqProcessesTimer.find(newEl.m_rnti);
								if (itHarqTimer == m_dlHarqProcessesTimer.end()) {
									NS_FATAL_ERROR(
											"Unable to find HARQ timer for RNTI " << (uint16_t)newEl.m_rnti);
								}
								(*itHarqTimer).second.at(newDci.m_harqProcess) = 0;
							}
							// ...more parameters -> ignored in this version

							ret.m_buildDataList.push_back(newEl);
							if (rbgAllocatedNum == rbgNum) {
								m_nextRntiDl = newEl.m_rnti; // store last RNTI served
								break;                       // no more RGB to be allocated
							}
						} while ((*it).m_rnti != m_nextRntiDl);

					//-------------------------------------------------------------------------------------------------
					//-----------------------------
						ret.m_nrOfPdcchOfdmSymbols = 1; /// \todo check correct value according the DCIs txed
						m_schedSapUser->SchedDlConfigInd(ret);
						//---------------------
    //---------------------------------------------

	} break;


//	=========------------------------------===============

	case 2:
	{
	NS_LOG_FUNCTION(this << "Phy 2 " << "<mohamed> ---------------- 1 ------------------ <mohamed>");

	//---------------------
	int rbgSize2 = GetRbgSize(m_cschedCellConfig2.m_dlBandwidth);
	int rbgNum2 = m_cschedCellConfig2.m_dlBandwidth / rbgSize2;

	// Generate RBGs map


	std::vector<bool> rbgMap2;
	uint16_t rbgAllocatedNum2 = 0;
	std::set<uint16_t> rntiAllocated2;
	rbgMap.resize(m_cschedCellConfig2.m_dlBandwidth / rbgSize2, false);       ////////

	//   update UL HARQ proc id

		std::map<uint16_t, uint8_t>::iterator itProcId2;
			for (itProcId2 = m_ulHarqCurrentProcessId2.begin();itProcId2 != m_ulHarqCurrentProcessId2.end(); itProcId2++) {
				(*itProcId2).second = ((*itProcId2).second + 1) % HARQ_PROC_NUM;        // mod 8 to get the current process
			}
	//-----------------------------------
	// RACH Allocation
	m_rachAllocationMap2.resize(m_cschedCellConfig2.m_ulBandwidth, 0);
	uint16_t rbStart = 0;

	std::vector<struct RachListElement_s>::iterator itRach;

	//------------------------------
	for (itRach = m_rachList2.begin(); itRach != m_rachList2.end(); itRach++){

			///////////////////
			NS_ASSERT_MSG(m_amc2->GetTbSizeFromMcs(m_ulGrantMcs,m_cschedCellConfig2.m_ulBandwidth)> (*itRach).m_estimatedSize,
					" Default UL Grant MCS does not allow to send RACH messages");
			//////////////////////
			BuildRarListElement_s newRar;             // It`s created and destroyed every iteration
			newRar.m_rnti = (*itRach).m_rnti;
			// DL-RACH Allocation
			// Ideal: no needs of configuring m_dci
			// UL-RACH Allocation
			newRar.m_grant.m_rnti = newRar.m_rnti;
			newRar.m_grant.m_mcs = m_ulGrantMcs;
			uint16_t rbLen = 1;
			uint16_t tbSizeBits = 0;
			// find lowest TB size that fits UL grant estimated size
			while ((tbSizeBits < (*itRach).m_estimatedSize)
					&& (rbStart + rbLen < m_cschedCellConfig2.m_ulBandwidth)) {
				rbLen++;
				tbSizeBits = m_amc2->GetTbSizeFromMcs(m_ulGrantMcs, rbLen);  // <mohamed> problem <mohamed>
			}
			std::clog << this << "RrFfMacScheduler::DoSchedDlTriggerReq <mohamed> ---------- 2 --------- <mohamed>" << std::endl;

			if (tbSizeBits < (*itRach).m_estimatedSize) {
				// no more allocation space: finish allocation
				std::clog << this << "RrFfMacScheduler::DoSchedDlTriggerReq <mohamed> ---------- 3 --------- <mohamed>" << std::endl;
				break;
			}
			newRar.m_grant.m_rbStart = rbStart;
			newRar.m_grant.m_rbLen = rbLen;
			newRar.m_grant.m_tbSize = tbSizeBits / 8;
			newRar.m_grant.m_hopping = false;
			newRar.m_grant.m_tpc = 0;
			newRar.m_grant.m_cqiRequest = false;
			newRar.m_grant.m_ulDelay = false;
			NS_LOG_INFO(
					this << "<phy2>" <<" UL grant allocated to RNTI " << (*itRach).m_rnti << " rbStart " << rbStart << " rbLen " << rbLen << " MCS " << (uint16_t) m_ulGrantMcs << " tbSize " << newRar.m_grant.m_tbSize);
			for (uint16_t i = rbStart; i < rbStart + rbLen; i++) {
				m_rachAllocationMap2.at(i) = (*itRach).m_rnti;
			}

			if (m_harqOn == true) {
				// generate UL-DCI for HARQ retransmissions
				UlDciListElement_s uldci;
				uldci.m_rnti = newRar.m_rnti;
				uldci.m_rbLen = rbLen;
				uldci.m_rbStart = rbStart;
				uldci.m_mcs = m_ulGrantMcs;
				uldci.m_tbSize = tbSizeBits / 8;
				uldci.m_ndi = 1;
				uldci.m_cceIndex = 0;
				uldci.m_aggrLevel = 1;
				uldci.m_ueTxAntennaSelection = 3; // antenna selection OFF
				uldci.m_hopping = false;
				uldci.m_n2Dmrs = 0;
				uldci.m_tpc = 0; // no power control
				uldci.m_cqiRequest = false; // only period CQI at this stage
				uldci.m_ulIndex = 0; // TDD parameter
				uldci.m_dai = 1; // TDD parameter
				uldci.m_freqHopping = 0;
				uldci.m_pdcchPowerOffset = 0; // not used

				uint8_t harqId = 0;
				std::map<uint16_t, uint8_t>::iterator itProcId;
				itProcId = m_ulHarqCurrentProcessId2.find(uldci.m_rnti);
				NS_LOG_FUNCTION(this << "<mohamed> rnti= " << itProcId->first << "actual rnti= " << uldci.m_rnti << " <mohamed>");
				if (itProcId == m_ulHarqCurrentProcessId2.end()) {
					NS_FATAL_ERROR(
							"<phy2> No info find in HARQ buffer for UE " << uldci.m_rnti);
				}
				harqId = (*itProcId).second;
				std::map<uint16_t, UlHarqProcessesDciBuffer_t>::iterator itDci =
						m_ulHarqProcessesDciBuffer2.find(uldci.m_rnti);
				if (itDci == m_ulHarqProcessesDciBuffer2.end()) {
					NS_FATAL_ERROR(
							" <phy2> Unable to find RNTI entry in UL DCI HARQ buffer for RNTI " << uldci.m_rnti);
				}
				(*itDci).second.at(harqId) = uldci;
			}

			rbStart = rbStart + rbLen;
		    ret.m_buildRarList.push_back(newRar);
		}
		//-------------------------------------------------

		m_rachList2.clear();
		//--------------------
		//-------Process DL HARQ feedback
		NS_LOG_INFO(this << "RrFfMacScheduler::DoSchedDlTriggerReq <mohamed> ---------- 4 --------- <mohamed>");
		RefreshHarqProcesses(params.m_phyId);
		NS_LOG_FUNCTION(this << "<mohamed> ---------- 5 --------- <mohamed>");
	   //--------------------------------
		//-------------------------
			if (m_dlInfoListBuffered2.size() > 0) {
					if (params.m_dlInfoList.size() > 0) {
						NS_LOG_INFO(this << "<phy2> Received DL-HARQ feedback");
						m_dlInfoListBuffered2.insert(m_dlInfoListBuffered2.end(),
								params.m_dlInfoList.begin(), params.m_dlInfoList.end());
					}
				} else {
					if (params.m_dlInfoList.size() > 0) {
						m_dlInfoListBuffered2 = params.m_dlInfoList;
					}
				}
			//-----------------------------------
			if (m_harqOn == false) {
						// Ignore HARQ feedback

						m_dlInfoListBuffered2.clear();
						//------------------------------
					}

			NS_LOG_FUNCTION("<mohamed> -------------- check point 1 ----------------- <mohamed>");
		//-----------------------------------------
			//--->>second phy. list
				for (uint16_t i = 0; i < m_dlInfoListBuffered2.size(); i++){
					std::set<uint16_t>::iterator itRnti = rntiAllocated2.find(m_dlInfoListBuffered2.at(i).m_rnti);
					NS_LOG_FUNCTION("<mohamed> -------------- 6 ----------------- <mohamed>");
					if (itRnti != rntiAllocated2.end()) {
						// RNTI already allocated for retx
						NS_LOG_FUNCTION("<mohamed> -------------- 7 ----------------- <mohamed>");
						continue;
					}


					uint8_t nLayers = m_dlInfoListBuffered2.at(i).m_harqStatus.size();
					std::vector<bool> retx;
					NS_LOG_INFO(this << "<phy2> Processing DLHARQ feedback");
					if (nLayers == 1) {
						retx.push_back(
								m_dlInfoListBuffered2.at(i).m_harqStatus.at(0)
										== DlInfoListElement_s::NACK);
						retx.push_back(false);
					} else {
						retx.push_back(
								m_dlInfoListBuffered2.at(i).m_harqStatus.at(0)
										== DlInfoListElement_s::NACK);
						retx.push_back(
								m_dlInfoListBuffered2.at(i).m_harqStatus.at(1)
										== DlInfoListElement_s::NACK);
					}


					if (retx.at(0) || retx.at(1)) {
						// retrieve HARQ process information
						uint16_t rnti = m_dlInfoListBuffered2.at(i).m_rnti;
						uint8_t harqId = m_dlInfoListBuffered2.at(i).m_harqProcessId;
						NS_LOG_INFO(
								this << "<phy2> HARQ retx RNTI " << rnti << " harqId " << (uint16_t)harqId);
						std::map<uint16_t, DlHarqProcessesDciBuffer_t>::iterator itHarq =
								m_dlHarqProcessesDciBuffer2.find(rnti);
						if (itHarq == m_dlHarqProcessesDciBuffer2.end()) {
							NS_FATAL_ERROR("<phy2> No info find in HARQ buffer for UE " << rnti);
						}

						DlDciListElement_s dci = (*itHarq).second.at(harqId);
						int rv = 0;
						if (dci.m_rv.size() == 1) {
							rv = dci.m_rv.at(0);
						} else {
							rv = (dci.m_rv.at(0) > dci.m_rv.at(1) ?
									dci.m_rv.at(0) : dci.m_rv.at(1));
						}

						if (rv == 3) {
							// maximum number of retx reached -> drop process
							NS_LOG_INFO(
									"<phy2> Max number of retransmissions reached -> drop process");
							std::map<uint16_t, DlHarqProcessesStatus_t>::iterator it =
									m_dlHarqProcessesStatus2.find(rnti);
							if (it == m_dlHarqProcessesStatus2.end()) {
								NS_LOG_ERROR(
										"<phy2> No info find in HARQ buffer for UE (might change eNB) " << m_dlInfoListBuffered2.at (i).m_rnti);	// modified
							}
							(*it).second.at(harqId) = 0;
							std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
									m_dlHarqProcessesRlcPduListBuffer2.find(rnti);
							if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer2.end()) {
								NS_FATAL_ERROR(
										"<phy2> Unable to find RlcPdcList in HARQ buffer for RNTI " << m_dlInfoListBuffered2.at (i).m_rnti);	// modified
							}
							for (uint16_t k = 0; k < (*itRlcPdu).second.size(); k++) {
								(*itRlcPdu).second.at(k).at(harqId).clear();
							}
							continue;
						}


						// check the feasibility of retransmitting on the same RBGs
						// translate the DCI to Spectrum framework
						std::vector<int> dciRbg;
						uint32_t mask = 0x1;
						NS_LOG_INFO(
								"<phy2>  Original RBGs " << dci.m_rbBitmap << " rnti " << dci.m_rnti);
						for (int j = 0; j < 32; j++) {
							if (((dci.m_rbBitmap & mask) >> j) == 1) {
								dciRbg.push_back(j);
								NS_LOG_INFO("\t" << j);
							}
							mask = (mask << 1);
						}
						bool free = true;
						for (uint8_t j = 0; j < dciRbg.size(); j++) {
							if (rbgMap.at(dciRbg.at(j)) == true) {
								free = false;
								break;
							}
						}
						if (free) {
							// use the same RBGs for the retx
							// reserve RBGs
							for (uint8_t j = 0; j < dciRbg.size(); j++) {
								rbgMap.at(dciRbg.at(j)) = true;
								NS_LOG_INFO("<phy2>      RBG " << dciRbg.at (j) << " assigned");
								rbgAllocatedNum2++;
							}

							NS_LOG_INFO(this << "<phy2>  Send retx in the same RBGs");
						} else {     // not free

							// find RBGs for sending HARQ retx
							uint8_t j = 0;
							uint8_t rbgId = (dciRbg.at(dciRbg.size() - 1) + 1) % rbgNum2;
							uint8_t startRbg = dciRbg.at(dciRbg.size() - 1);
							std::vector<bool> rbgMapCopy = rbgMap2;
							while ((j < dciRbg.size()) && (startRbg != rbgId)) {
								if (rbgMapCopy.at(rbgId) == false) {
									rbgMapCopy.at(rbgId) = true;
									dciRbg.at(j) = rbgId;
									j++;
								}
								rbgId = (rbgId + 1) % rbgNum2;
							}
							if (j == dciRbg.size()) {
								// find new RBGs -> update DCI map
								uint32_t rbgMask = 0;
								for (uint16_t k = 0; k < dciRbg.size(); k++) {
									rbgMask = rbgMask + (0x1 << dciRbg.at(k));
									NS_LOG_INFO(
											this << "<phy2> New allocated RBG " << dciRbg.at (k));
									rbgAllocatedNum2++;
								}
								dci.m_rbBitmap = rbgMask;
								rbgMap2 = rbgMapCopy;
							} else {
								// HARQ retx cannot be performed on this TTI -> store it
								dlInfoListUntxed.push_back(m_dlInfoListBuffered2.at(i));	// modified
								NS_LOG_INFO(this << "<phy2> No resource for this retx -> buffer it");
							}
						}
						// retrieve RLC PDU list for retx TBsize and update DCI
						BuildDataListElement_s newEl;
						std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
								m_dlHarqProcessesRlcPduListBuffer2.find(rnti);
						if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer2.end()) {
							NS_FATAL_ERROR(
									"<phy2> Unable to find RlcPdcList in HARQ buffer for RNTI " << rnti);
						}

						for (uint8_t j = 0; j < nLayers; j++) {
							if (retx.at(j)) {
								if (j >= dci.m_ndi.size()) {
									// for avoiding errors in MIMO transient phases
									dci.m_ndi.push_back(0);
									dci.m_rv.push_back(0);
									dci.m_mcs.push_back(0);
									dci.m_tbsSize.push_back(0);
									NS_LOG_INFO(
											this << "<phy2>  layer " << (uint16_t)j << " no txed (MIMO transition)");

								} else {
									dci.m_ndi.at(j) = 0;
									dci.m_rv.at(j)++;(
			                        *itHarq).second.at (harqId).m_rv.at (j)++;
									NS_LOG_INFO(
											this << "<phy2>  layer " << (uint16_t)j << " RV " << (uint16_t)dci.m_rv.at (j));
								}
							} else {
								// empty TB of layer j
								dci.m_ndi.at(j) = 0;
								dci.m_rv.at(j) = 0;
								dci.m_mcs.at(j) = 0;
								dci.m_tbsSize.at(j) = 0;
								NS_LOG_INFO(this << "<phy2>  layer " << (uint16_t)j << " no retx");
							}
						}

						for (uint16_t k = 0;k < (*itRlcPdu).second.at(0).at(dci.m_harqProcess).size();k++) {
							std::vector<struct RlcPduListElement_s> rlcPduListPerLc;
							for (uint8_t j = 0; j < nLayers; j++) {
								if (retx.at(j)) {
									if (j < dci.m_ndi.size()) {
										rlcPduListPerLc.push_back(
												(*itRlcPdu).second.at(j).at(
														dci.m_harqProcess).at(k));
									}
								}
							}

							if (rlcPduListPerLc.size() > 0) {
								newEl.m_rlcPduList.push_back(rlcPduListPerLc);
							}
						}
						newEl.m_rnti = rnti;
						newEl.m_dci = dci;
						(*itHarq).second.at(harqId).m_rv = dci.m_rv;

						// refresh timer
						std::map<uint16_t, DlHarqProcessesTimer_t>::iterator itHarqTimer =
								m_dlHarqProcessesTimer2.find(rnti);
						if (itHarqTimer == m_dlHarqProcessesTimer2.end()) {
							NS_FATAL_ERROR(
									"<phy2>  Unable to find HARQ timer for RNTI " << (uint16_t)rnti);
						}
						(*itHarqTimer).second.at(harqId) = 0;
						ret.m_buildDataList.push_back(newEl);
						rntiAllocated2.insert(rnti);
					} else {
						// update HARQ process status
						NS_LOG_INFO(
								this << "<phy2>  HARQ ACK UE " << m_dlInfoListBuffered2.at (i).m_rnti);
						std::map<uint16_t, DlHarqProcessesStatus_t>::iterator it =
								m_dlHarqProcessesStatus2.find(
										m_dlInfoListBuffered2.at(i).m_rnti);
						if (it == m_dlHarqProcessesStatus2.end()) {
							NS_FATAL_ERROR(
									"No info find in HARQ buffer for UE " << m_dlInfoListBuffered2.at (i).m_rnti);
						}
						(*it).second.at(m_dlInfoListBuffered2.at(i).m_harqProcessId) = 0;
						std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
								m_dlHarqProcessesRlcPduListBuffer2.find(
										m_dlInfoListBuffered2.at(i).m_rnti);
						if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer2.end()) {
							NS_FATAL_ERROR(
									"<phy2>  Unable to find RlcPdcList in HARQ buffer for RNTI " << m_dlInfoListBuffered2.at (i).m_rnti);
						}
						for (uint16_t k = 0; k < (*itRlcPdu).second.size(); k++) {
							(*itRlcPdu).second.at(k).at(
									m_dlInfoListBuffered2.at(i).m_harqProcessId).clear();
						}
					}
				}
				NS_LOG_FUNCTION("<mohamed> -------------- check point 2 ----------------- <mohamed>");
				//-------------------------------------------------------------------

				//---------------------
				m_dlInfoListBuffered2.clear();
				m_dlInfoListBuffered2 = dlInfoListUntxed;
				//--------------------------------------------
					if (rbgAllocatedNum2 == rbgNum2) {
						NS_LOG_FUNCTION("<mohamed> -------------- line 2126 ----------------- <mohamed>");
							// all the RBGs are already allocated -> exit
							if ((ret.m_buildDataList.size() > 0)
									|| (ret.m_buildRarList.size() > 0)) {
								NS_LOG_FUNCTION("<mohamed> -------------- line 2130 ----------------- <mohamed>");
								m_schedSapUser2->SchedDlConfigInd(ret);
							}
							NS_LOG_FUNCTION("<mohamed> -------------- line 2133 ----------------- <mohamed>");
							return;
						}

					NS_LOG_FUNCTION("<mohamed> -------------- check point 3 line 2137 ----------------- <mohamed>");
			  //---------------------------------------------
					//-----------------------------------------------------------------------
						for (it = m_rlcBufferReq2.begin(); it != m_rlcBufferReq2.end(); it++){
							NS_LOG_FUNCTION("<mohamed> -------------- line 2141 ----------------- <mohamed>");
							// remove old entries of this UE-LC
							std::set<uint16_t>::iterator itRnti = rntiAllocated2.find((*it).m_rnti);
							if ((((*it).m_rlcTransmissionQueueSize > 0)
									|| ((*it).m_rlcRetransmissionQueueSize > 0)
									|| ((*it).m_rlcStatusPduSize > 0))
									&& (itRnti == rntiAllocated2.end()) // UE must not be allocated for HARQ retx
									&& (HarqProcessAvailability((*it).m_rnti))) // UE needs HARQ proc free

									{
								NS_LOG_LOGIC(
										this << "<phy2>  User " << (*it).m_rnti << " LC " << (uint16_t)(*it).m_logicalChannelIdentity << " is active, status  " << (*it).m_rlcStatusPduSize << " retx " << (*it).m_rlcRetransmissionQueueSize << " tx " << (*it).m_rlcTransmissionQueueSize);
								std::map<uint16_t, uint8_t>::iterator itCqi = m_p10CqiRxed2.find((*it).m_rnti);
								uint8_t cqi = 0;
								if (itCqi != m_p10CqiRxed2.end()) {
									cqi = (*itCqi).second;
								} else {
									cqi = 1; // lowest value for  trying a transmission
								}
								if (cqi != 0) {
									// CQI == 0 means "out of range" (see table 7.2.3-1 of 36.213)
									nflows2++;
									itLcRnti2 = lcActivesPerRnti2.find((*it).m_rnti);
									if (itLcRnti2 != lcActivesPerRnti2.end()) {
										(*itLcRnti2).second++;
									} else {
										lcActivesPerRnti2.insert(
												std::pair<uint16_t, uint8_t>((*it).m_rnti, 1));
										nTbs2++;
									}

								}
							}
						}
						NS_LOG_FUNCTION("<mohamed> -------------- check point 4 line 2175 ----------------- <mohamed>");
		 //--------------------------------------------------
						if (nflows2 == 0) {
									if ((ret.m_buildDataList.size() > 0)
											|| (ret.m_buildRarList.size() > 0)) {
										NS_LOG_FUNCTION("<mohamed> -------------- (a) line 2180 ----------------- <mohamed>");
										m_schedSapUser2->SchedDlConfigInd(ret);
										NS_LOG_FUNCTION("<mohamed> -------------- (b) line 2182 ----------------- <mohamed>");
									}
									NS_LOG_FUNCTION("<mohamed> -------------- (c) line 2184 ----------------- <mohamed>");
									return;
								}
						NS_LOG_FUNCTION("<mohamed> -------------- check point 5 line 2184 ----------------- <mohamed>");
							//--------------------------------------------
						// Divide the resource equally among the active users according to
							// Resource allocation type 0 (see sec 7.1.6.1 of 36.213)
							int rbgPerTb2 = (nTbs2 > 0) ? ((rbgNum2 - rbgAllocatedNum2) / nTbs2) : INT_MAX;
							NS_LOG_INFO(this << "<phy2> Flows to be transmitted " << nflows2 << " rbgPerTb " << rbgPerTb2);

							//------------------------------
							if (rbgPerTb2 == 0) {
									rbgPerTb2 = 1;         // at least 1 rbg per TB (till available resource)
								}
							//---
							int rbgAllocated2 = 0;
							//----------------------------
							// round robin assignment to all UEs registered starting from the subsequent of the one
								// served last scheduling trigger event

								//--------------------------------------------------
								if (m_nextRntiDl2 != 0) {
										NS_LOG_DEBUG("<phy2> Start from the successive of " << (uint16_t) m_nextRntiDl2);
										for (it2 = m_rlcBufferReq2.begin(); it2 != m_rlcBufferReq2.end(); it2++) {
											if ((*it2).m_rnti == m_nextRntiDl2) {
												// select the next RNTI to starting
												it2++;
												if (it2 == m_rlcBufferReq2.end()) {
													it2 = m_rlcBufferReq2.begin();
												}
												m_nextRntiDl2 = (*it2).m_rnti;
												break;
											}
										}

										if (it2 == m_rlcBufferReq2.end()) {
											NS_LOG_ERROR(this << "<phy2>  no user found");
										}
									} else {
										it2 = m_rlcBufferReq2.begin();
										m_nextRntiDl2 = (*it2).m_rnti;
										do {
												itLcRnti = lcActivesPerRnti2.find((*it2).m_rnti);
												std::set<uint16_t>::iterator itRnti = rntiAllocated2.find((*it2).m_rnti);
												if ((itLcRnti == lcActivesPerRnti2.end())
														|| (itRnti != rntiAllocated2.end())) {
													// skip this RNTI (no active queue or yet allocated for HARQ)
													uint16_t rntiDiscared = (*it2).m_rnti;
													while (it2 != m_rlcBufferReq2.end()) {
														if ((*it2).m_rnti != rntiDiscared) {
															break;
														}
														it2++;
													}

													if (it2 == m_rlcBufferReq2.end()) {
														// restart from the first
														it2 = m_rlcBufferReq2.begin();
													}
													continue;
												}
												itTxMode = m_uesTxMode2.find((*it2).m_rnti);
												if (itTxMode == m_uesTxMode2.end()) {
													NS_FATAL_ERROR(
															"<phy2>  No Transmission Mode info on user " << (*it2).m_rnti);
												}
												int nLayer = TransmissionModesLayers::TxMode2LayerNum(
														(*itTxMode).second);
												int lcNum = (*itLcRnti2).second;
												// create new BuildDataListElement_s for this RNTI
												BuildDataListElement_s newEl;
												newEl.m_rnti = (*it2).m_rnti;
												// create the DlDciListElement_s
												DlDciListElement_s newDci;
												newDci.m_rnti = (*it2).m_rnti;
												newDci.m_harqProcess = UpdateHarqProcessId((*it2).m_rnti , params.m_phyId);
												newDci.m_resAlloc = 0;
												newDci.m_rbBitmap = 0;
												std::map<uint16_t, uint8_t>::iterator itCqi = m_p10CqiRxed2.find(
														newEl.m_rnti);
												for (uint8_t i = 0; i < nLayer; i++) {
													if (itCqi == m_p10CqiRxed2.end()) {
														newDci.m_mcs.push_back(0); // no info on this user -> lowest MCS
													} else {
														newDci.m_mcs.push_back(m_amc2->GetMcsFromCqi((*itCqi).second));
													}
												}
												int tbSize = (m_amc2->GetTbSizeFromMcs(newDci.m_mcs.at(0),
														rbgPerTb2 * rbgSize2) / 8);
												uint16_t rlcPduSize = tbSize / lcNum;
												while ((*it2).m_rnti == newEl.m_rnti) {
													if (((*it2).m_rlcTransmissionQueueSize > 0)
															|| ((*it2).m_rlcRetransmissionQueueSize > 0)
															|| ((*it2).m_rlcStatusPduSize > 0)) {
														std::vector<struct RlcPduListElement_s> newRlcPduLe;
														for (uint8_t j = 0; j < nLayer; j++) {
															RlcPduListElement_s newRlcEl;
															newRlcEl.m_logicalChannelIdentity =
																	(*it2).m_logicalChannelIdentity;
															NS_LOG_INFO(
																	this << "<phy2>   LCID " << (uint32_t) newRlcEl.m_logicalChannelIdentity << " size " << rlcPduSize << " ID " << (*it2).m_rnti << " layer " << (uint16_t)j);
															newRlcEl.m_size = rlcPduSize;
//															UpdateDlRlcBufferInfo2((*it2).m_rnti,newRlcEl.m_logicalChannelIdentity, rlcPduSize);
															newRlcPduLe.push_back(newRlcEl);

															if (m_harqOn == true) {
																// store RLC PDU list for HARQ
																std::map<uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =
																		m_dlHarqProcessesRlcPduListBuffer2.find(
																				(*it2).m_rnti);
																if (itRlcPdu
																		== m_dlHarqProcessesRlcPduListBuffer2.end()) {
																	NS_FATAL_ERROR(
																			"<PHY2> Unable to find RlcPdcList in HARQ buffer for RNTI " << (*it).m_rnti);
																}
																(*itRlcPdu).second.at(j).at(newDci.m_harqProcess).push_back(
																		newRlcEl);
															}

														}
														newEl.m_rlcPduList.push_back(newRlcPduLe);
														lcNum--;
													}
													it2++;
													if (it2 == m_rlcBufferReq2.end()) {
														// restart from the first
														it2 = m_rlcBufferReq2.begin();
														break;
													}
												}
												uint32_t rbgMask = 0;
												uint16_t i = 0;
												NS_LOG_INFO(
														this << "<PHY2>----  DL - Allocate user " << newEl.m_rnti << " LCs " << (uint16_t)(*itLcRnti).second << " bytes " << tbSize << " mcs " << (uint16_t) newDci.m_mcs.at (0) << " harqId " << (uint16_t)newDci.m_harqProcess << " layers " << nLayer);
												NS_LOG_INFO("RBG:");
												while (i < rbgPerTb2) {
													if (rbgMap2.at(rbgAllocated2) == false) {
														rbgMask = rbgMask + (0x1 << rbgAllocated2);
														NS_LOG_INFO("\t " <<"<PHY2>..... " << rbgAllocated2);
														i++;
														rbgMap.at(rbgAllocated2) = true;
														rbgAllocatedNum2++;
													}
													rbgAllocated2++;
												}
												newDci.m_rbBitmap = rbgMask; // (32 bit bitmap see 7.1.6 of 36.213)

												for (int i = 0; i < nLayer; i++) {
													newDci.m_tbsSize.push_back(tbSize);
													newDci.m_ndi.push_back(1);
													newDci.m_rv.push_back(0);
												}

												newDci.m_tpc = 1; //1 is mapped to 0 in Accumulated Mode and to -1 in Absolute Mode

												newEl.m_dci = newDci;
												if (m_harqOn == true) {
													// store DCI for HARQ
													std::map<uint16_t, DlHarqProcessesDciBuffer_t>::iterator itDci =
															m_dlHarqProcessesDciBuffer2.find(newEl.m_rnti);
													if (itDci == m_dlHarqProcessesDciBuffer2.end()) {
														NS_FATAL_ERROR(
																"<phy2>  Unable to find RNTI entry in DCI HARQ buffer for RNTI " << newEl.m_rnti);
													}
													(*itDci).second.at(newDci.m_harqProcess) = newDci;
													// refresh timer
													std::map<uint16_t, DlHarqProcessesTimer_t>::iterator itHarqTimer =
															m_dlHarqProcessesTimer2.find(newEl.m_rnti);
													if (itHarqTimer == m_dlHarqProcessesTimer2.end()) {
														NS_FATAL_ERROR(
																"<phy2> Unable to find HARQ timer for RNTI " << (uint16_t)newEl.m_rnti);
													}
													(*itHarqTimer).second.at(newDci.m_harqProcess) = 0;
												}
												// ...more parameters -> ignored in this version

												ret.m_buildDataList.push_back(newEl);
												if (rbgAllocatedNum2 == rbgNum2) {
													m_nextRntiDl2 = newEl.m_rnti; // store last RNTI served
													break;                       // no more RGB to be allocated
												}
											}while ((*it2).m_rnti != m_nextRntiDl2);
			//---------------------------------------------------------------------------------------------------
										//---------------------
											ret.m_nrOfPdcchOfdmSymbols = 1;
											m_schedSapUser2->SchedDlConfigInd(ret);
										    //-----------------------------------
									}


	} break ;

	}

	//-----------------------------
	std::clog << this << " RrFfMacScheduler::DoSchedDlTriggerReq <mohamed> ---------- end of Do schedDltriggerreq --------- <mohamed>" << std::endl;
	return;
}
//------MOd. end------------------------------------
//=======================================================================================


//----Modified <Abdelrhman>------------------------
void RrFfMacScheduler::DoSchedDlRachInfoReq(
		const struct FfMacSchedSapProvider::SchedDlRachInfoReqParameters& params ) {
	NS_LOG_FUNCTION(this << "phyId " << params.m_phyId);
  if (params.m_phyId == 1){
	  m_rachList = params.m_rachList;
  }
  else if (params.m_phyId ==2){
	  m_rachList2 = params.m_rachList;
  }


	return;
}
//------MOd. end------------------------------------
//=======================================================================================

//---modified <Doha>
/*                        ^^^^^^^^^^^^^^^^FIZO^^^^^^^^^^^^^^^^^^^^^
 * for loop 3la l CQI list element ( byb2a fiha RNTI , Rank indication , CQI type , wb_CQI : The reported wideband CQI value per codeword,
 * wb_pmi The reported wideband precoding matrix index , m_sbMeasResult : field discrimination is based on cqiType)
 * recall : PMI = Precoding Matrix Indicator , check the screenshots
 * then bn-check 3l type lw P10 bn3ml iterator it w bndwr 3l RNTI f m_p10CqiRxed (Map of UE's DL CQI P01 received)
 * lw not found bn3ml new entery fiha kol l info. bta3t l CQI of this  RNTI
 * else lw found l it dlw2ti fi l RNTI l bndwr 3leh bn3ml update ll entery d
 *                        ^^^^^^^^^^^^^^^^FIZO^^^^^^^^^^^^^^^^^^^^^
 */
void
RrFfMacScheduler::DoSchedDlCqiInfoReq (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params)
{
	NS_LOG_FUNCTION(this) ;
		std::map <uint16_t,uint8_t>::iterator it;
		for (unsigned int i = 0; i < params.m_cqiList.size (); i++)
		    {
		      if ( params.m_cqiList.at (i).m_cqiType == CqiListElement_s::P10 )
		        {
		          NS_LOG_LOGIC ("wideband CQI " <<  (uint32_t) params.m_cqiList.at (i).m_wbCqi.at (0) << " reported");
		          std::map <uint16_t,uint8_t>::iterator it;
		          uint16_t rnti = params.m_cqiList.at (i).m_rnti;

		          if (params.m_phyId == 1)
		          {
		        	  it = m_p10CqiRxed.find (rnti);
		        	  if (it == m_p10CqiRxed.end ())
		        	 {
		        	 	// create the new entry
		        	    m_p10CqiRxed.insert ( std::pair<uint16_t, uint8_t > (rnti, params.m_cqiList.at (i).m_wbCqi.at (0)) ); // only codeword 0 at this stage (SISO)
		        	 	// generate correspondent timer
		        	 	m_p10CqiTimers.insert ( std::pair<uint16_t, uint32_t > (rnti, m_cqiTimersThreshold));
		        	 }
		        	 else
		        	 {
		        	 	 // update the CQI value
		        	 	 (*it).second = params.m_cqiList.at (i).m_wbCqi.at (0);
		        	 	 // update correspondent timer
		        	 	 std::map <uint16_t,uint32_t>::iterator itTimers;
		        	 	 itTimers = m_p10CqiTimers.find (rnti);
		        	 	 (*itTimers).second = m_cqiTimersThreshold;
		        	 }
		          }
		          else if (params.m_phyId == 2)
		          {
		        	  it = m_p10CqiRxed2.find (rnti);
		        	  if (it == m_p10CqiRxed2.end ())
		        	 {
		        	 	// create the new entry
		        	    m_p10CqiRxed2.insert ( std::pair<uint16_t, uint8_t > (rnti, params.m_cqiList.at (i).m_wbCqi.at (0)) ); // only codeword 0 at this stage (SISO)
		        	   // generate correspondent timer
		        	   m_p10CqiTimers2.insert ( std::pair<uint16_t, uint32_t > (rnti, m_cqiTimersThreshold2));
		        	 }
		        	 else
		             {
		        	 	// update the CQI value
		        	 	(*it).second = params.m_cqiList.at (i).m_wbCqi.at (0);
		        	 	// update correspondent timer
		        	 	std::map <uint16_t,uint32_t>::iterator itTimers;
		        	 	itTimers = m_p10CqiTimers2.find (rnti);
		        	 	(*itTimers).second = m_cqiTimersThreshold2;
		        	  }
		          }

		        }
		      else if ( params.m_cqiList.at (i).m_cqiType == CqiListElement_s::A30 )
		        {
		          // subband CQI reporting high layer configured
		          // Not used by RR Scheduler
		        }
		      else
		        {
		          NS_LOG_ERROR (this << " CQI type unknown");
		        }
		    }
		 return ;
	}


//---Mod. ends ------------------------------------
//=================================================

//------Modified <Doha> ----------------------------------
//--------------------------------------------------------


void
RrFfMacScheduler::DoSchedUlTriggerReq (const struct FfMacSchedSapProvider::SchedUlTriggerReqParameters& params)
{
	NS_LOG_FUNCTION (this << "For Phy" << params.m_phyId << " UL - Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf) << " size " << params.m_ulInfoList.size ());
		  RefreshUlCqiMaps (params.m_phyId);
		  // Generate RBs map
		  FfMacSchedSapUser::SchedUlConfigIndParameters ret;
		  ret.m_phyId = params.m_phyId;
		  std::vector <bool> rbMap;
		  uint16_t rbAllocatedNum = 0;
		  std::set <uint16_t> rntiAllocated;
		  std::vector <uint16_t> rbgAllocationMap;
		  if (params.m_phyId == 1)
		  {
			  // update with RACH allocation map
			  rbgAllocationMap = m_rachAllocationMap;
			  //rbgAllocationMap.resize (m_cschedCellConfig.m_ulBandwidth, 0);
			  m_rachAllocationMap.clear ();
			  m_rachAllocationMap.resize (m_cschedCellConfig.m_ulBandwidth, 0);
	     	  rbMap.resize (m_cschedCellConfig.m_ulBandwidth, false);
			  // remove RACH allocation
			  for (uint16_t i = 0; i < m_cschedCellConfig.m_ulBandwidth; i++)
			  {
			  	  if (rbgAllocationMap.at (i) != 0)
			  	  {
			  	      rbMap.at (i) = true;
			  	      NS_LOG_DEBUG (this << " <Phy1> :  Allocated for RACH " << i);
			  	  }
			  }
		  }
		  else if (params.m_phyId == 2)
		  {
			  // update with RACH allocation map
			  rbgAllocationMap = m_rachAllocationMap2;
			  //rbgAllocationMap.resize (m_cschedCellConfig.m_ulBandwidth, 0);
			  m_rachAllocationMap2.clear ();
			  m_rachAllocationMap2.resize (m_cschedCellConfig2.m_ulBandwidth, 0);
	     	  rbMap.resize (m_cschedCellConfig2.m_ulBandwidth, false);
			  // remove RACH allocation
			  for (uint16_t i = 0; i < m_cschedCellConfig2.m_ulBandwidth; i++)
			  {
			  	  if (rbgAllocationMap.at (i) != 0)
			  	  {
			  	      rbMap.at (i) = true;
			  	      NS_LOG_DEBUG (this << " <Phy2> :  Allocated for RACH " << i);
			  	  }
			  }
		  }

		  if (m_harqOn == true)
		    {
		      //   Process UL HARQ feedback
		      for (uint16_t i = 0; i < params.m_ulInfoList.size (); i++)
		        {
		          if (params.m_ulInfoList.at (i).m_receptionStatus == UlInfoListElement_s::NotOk)
		            {
		              // retx correspondent block: retrieve the UL-DCI
		              uint16_t rnti = params.m_ulInfoList.at (i).m_rnti;
		              // ---------------------------------
		              if (params.m_phyId == 1)
		              {
		            	  std::map <uint16_t, uint8_t>::iterator itProcId = m_ulHarqCurrentProcessId.find (rnti);
		            	  if (itProcId == m_ulHarqCurrentProcessId.end ())
		            	  {
		            	 	  NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << rnti);
		            	  }
		            	 uint8_t harqId = (uint8_t)((*itProcId).second - HARQ_PERIOD) % HARQ_PROC_NUM;
		            	 NS_LOG_INFO (this << " UL-HARQ retx RNTI " << rnti << " harqId " << (uint16_t)harqId);
		            	 std::map <uint16_t, UlHarqProcessesDciBuffer_t>::iterator itHarq = m_ulHarqProcessesDciBuffer.find (rnti);
		            	 if (itHarq == m_ulHarqProcessesDciBuffer.end ())
		            	 {
		            	 	  NS_LOG_ERROR ("No info find in UL-HARQ buffer for UE (might change eNB) " << rnti);
		            	 }
		            	 UlDciListElement_s dci = (*itHarq).second.at (harqId);
		            	 std::map <uint16_t, UlHarqProcessesStatus_t>::iterator itStat = m_ulHarqProcessesStatus.find (rnti);
		            	 if (itStat == m_ulHarqProcessesStatus.end ())
		            	 {
		            	 	 NS_LOG_ERROR (" <Phy1> : No info find in HARQ buffer for UE (might change eNB) " << rnti);
		            	 }
		            	 if ((*itStat).second.at (harqId) > 3)
		            	 {
		            	      NS_LOG_INFO ("Max number of retransmissions reached (UL)-> drop process");
		            	      continue;
		            	 }
		            	 bool free = true;
		            	 for (int j = dci.m_rbStart; j < dci.m_rbStart + dci.m_rbLen; j++)
		            	 {
		            	 if (rbMap.at (j) == true)
		            	 {
		            	         free = false;
		            	 	     NS_LOG_INFO (this << " BUSY " << j);
		                 }
		            	 }
		            	 if (free)
		            	 {
		            	         // retx on the same RBs
		            	         for (int j = dci.m_rbStart; j < dci.m_rbStart + dci.m_rbLen; j++)
		            	         {
		            	              rbMap.at (j) = true;
		            	              rbgAllocationMap.at (j) = dci.m_rnti;
		            	              NS_LOG_INFO ("\tRB " << j);
		            	              rbAllocatedNum++;
		            	          }
		            	          NS_LOG_INFO (this << " Send retx in the same RBGs " << (uint16_t)dci.m_rbStart << " to " << dci.m_rbStart + dci.m_rbLen << " RV " << (*itStat).second.at (harqId) + 1);
		            	   }
		            	   else
		            	   {
		            	         NS_LOG_INFO ("Cannot allocate retx due to RACH allocations for UE " << rnti);
		            	         continue;
		                   }
	   	 	              dci.m_ndi = 0;
	  	 	              // Update HARQ buffers with new HarqId
	         	 	      (*itStat).second.at ((*itProcId).second) = (*itStat).second.at (harqId) + 1;
		            	  (*itStat).second.at (harqId) = 0;
		            	  (*itHarq).second.at ((*itProcId).second) = dci;
		            	   ret.m_dciList.push_back (dci);
		            	   rntiAllocated.insert (dci.m_rnti);
		              }
		              //---------------------------------------
		              else if (params.m_phyId == 2)
		              {
		            	  std::map <uint16_t, uint8_t>::iterator itProcId = m_ulHarqCurrentProcessId2.find (rnti);
		            	  if (itProcId == m_ulHarqCurrentProcessId2.end ())
		            	  {
		            	 	  NS_LOG_ERROR ("<Phy2> : No info find in HARQ buffer for UE (might change eNB) " << rnti);
		            	  }
		            	 uint8_t harqId = (uint8_t)((*itProcId).second - HARQ_PERIOD) % HARQ_PROC_NUM;
		            	 NS_LOG_INFO (this << " UL-HARQ retx RNTI " << rnti << " harqId " << (uint16_t)harqId);
		            	 std::map <uint16_t, UlHarqProcessesDciBuffer_t>::iterator itHarq = m_ulHarqProcessesDciBuffer2.find (rnti);
		            	 if (itHarq == m_ulHarqProcessesDciBuffer2.end ())
		            	 {
		            	 	  NS_LOG_ERROR ("No info find in UL-HARQ buffer for UE (might change eNB) " << rnti);
		            	 }
		            	 UlDciListElement_s dci = (*itHarq).second.at (harqId);
		            	 std::map <uint16_t, UlHarqProcessesStatus_t>::iterator itStat = m_ulHarqProcessesStatus2.find (rnti);
		            	 if (itStat == m_ulHarqProcessesStatus2.end ())
		            	 {
		            	 	 NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << rnti);
		            	 }
		            	 if ((*itStat).second.at (harqId) > 3)
		            	 {
		            	      NS_LOG_INFO ("Max number of retransmissions reached (UL)-> drop process");
		            	      continue;
		            	 }
		            	 bool free = true;
		            	 for (int j = dci.m_rbStart; j < dci.m_rbStart + dci.m_rbLen; j++)
		            	 {
		            	 if (rbMap.at (j) == true)
		            	 {
		            	         free = false;
		            	 	     NS_LOG_INFO (this << " BUSY " << j);
		                 }
		                 }
		            	 if (free)
		            	 {
		            	     // retx on the same RBs
		            	     for (int j = dci.m_rbStart; j < dci.m_rbStart + dci.m_rbLen; j++)
		            	       {
		            	          rbMap.at (j) = true;
		            	          rbgAllocationMap.at (j) = dci.m_rnti;
		            	          NS_LOG_INFO ("\tRB " << j);
		            	          rbAllocatedNum++;
		            	       }
		                    NS_LOG_INFO (this << " Send retx in the same RBGs " << (uint16_t)dci.m_rbStart << " to " << dci.m_rbStart + dci.m_rbLen << " RV " << (*itStat).second.at (harqId) + 1);
		            	   }
		            	   else
		            	   {
		            	         NS_LOG_INFO ("Cannot allocate retx due to RACH allocations for UE " << rnti);
		            	         continue;
		                   }
	   	 	              dci.m_ndi = 0;
	  	 	              // Update HARQ buffers with new HarqId
	         	 	      (*itStat).second.at ((*itProcId).second) = (*itStat).second.at (harqId) + 1;
		            	  (*itStat).second.at (harqId) = 0;
		            	  (*itHarq).second.at ((*itProcId).second) = dci;
		            	   ret.m_dciList.push_back (dci);
		            	   rntiAllocated.insert (dci.m_rnti);
		              }
		            }
		        }
		    }

		  std::map <uint16_t,uint32_t>::iterator it;
		  int nflows = 0;
		  //-----------------------------------------
		  if (params.m_phyId == 1)
		  {
			  for (it = m_ceBsrRxed.begin (); it != m_ceBsrRxed.end (); it++)
			    {
			      std::set <uint16_t>::iterator itRnti = rntiAllocated.find ((*it).first);
			      // select UEs with queues not empty and not yet allocated for HARQ
			      NS_LOG_INFO (this << " UE " << (*it).first << " queue " << (*it).second);
			      if (((*it).second > 0)&&(itRnti == rntiAllocated.end ()))
			        {
			          nflows++;
			        }
			    }

			  if (nflows == 0)
			    {
			      if (ret.m_dciList.size () > 0)
			        {
			          m_allocationMaps.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap));
			          m_schedSapUser->SchedUlConfigInd (ret);
			        }
			      return;  // no flows to be scheduled
			    }

			  // Divide the remaining resources equally among the active users starting from the subsequent one served last scheduling trigger
			  uint16_t rbPerFlow = (m_cschedCellConfig.m_ulBandwidth) / (nflows + rntiAllocated.size ());
			  if (rbPerFlow < 3)
			    {
			      rbPerFlow = 3;  // at least 3 rbg per flow (till available resource) to ensure TxOpportunity >= 7 bytes
			    }
			  uint16_t rbAllocated = 0;

			  if (m_nextRntiUl != 0)
			    {
			      for (it = m_ceBsrRxed.begin (); it != m_ceBsrRxed.end (); it++)
			        {
			          if ((*it).first == m_nextRntiUl)
			            {
			              break;
			            }
			        }
			      if (it == m_ceBsrRxed.end ())
			        {
			          NS_LOG_ERROR (this << " no user found");
			        }
			    }
			  else
			    {
			      it = m_ceBsrRxed.begin ();
			      m_nextRntiUl = (*it).first;
			    }
			  NS_LOG_INFO (this << " NFlows " << nflows << " RB per Flow " << rbPerFlow);
			  do
			    {
			      std::set <uint16_t>::iterator itRnti = rntiAllocated.find ((*it).first);
			      if ((itRnti != rntiAllocated.end ())||((*it).second == 0))
			        {
			          // UE already allocated for UL-HARQ -> skip it
			          it++;
			          if (it == m_ceBsrRxed.end ())
			            {
			              // restart from the first
			              it = m_ceBsrRxed.begin ();
			            }
			          continue;
			        }
			      if (rbAllocated + rbPerFlow - 1 > m_cschedCellConfig.m_ulBandwidth)
			        {
			          // limit to physical resources last resource assignment
			          rbPerFlow = m_cschedCellConfig.m_ulBandwidth - rbAllocated;
			          // at least 3 rbg per flow to ensure TxOpportunity >= 7 bytes
			          if (rbPerFlow < 3)
			            {
			              // terminate allocation
			              rbPerFlow = 0;
			            }
			        }
			      NS_LOG_INFO (this << " try to allocate " << (*it).first);
			      UlDciListElement_s uldci;
			      uldci.m_rnti = (*it).first;
			      uldci.m_rbLen = rbPerFlow;
			      bool allocated = false;
			      NS_LOG_INFO (this << " RB Allocated " << rbAllocated << " rbPerFlow " << rbPerFlow << " flows " << nflows);
			      while ((!allocated)&&((rbAllocated + rbPerFlow - m_cschedCellConfig.m_ulBandwidth) < 1) && (rbPerFlow != 0))
			        {
			          // check availability
			          bool free = true;
			          for (uint16_t j = rbAllocated; j < rbAllocated + rbPerFlow; j++)
			            {
			              if (rbMap.at (j) == true)
			                {
			                  free = false;
			                  break;
			                }
			            }
			          if (free)
			            {
			              uldci.m_rbStart = rbAllocated;

			              for (uint16_t j = rbAllocated; j < rbAllocated + rbPerFlow; j++)
			                {
			                  rbMap.at (j) = true;
			                  // store info on allocation for managing ul-cqi interpretation
			                  rbgAllocationMap.at (j) = (*it).first;
			                  NS_LOG_INFO ("\t " << j);
			                }
			              rbAllocated += rbPerFlow;
			              allocated = true;
			              break;
			            }
			          rbAllocated++;
			          if (rbAllocated + rbPerFlow - 1 > m_cschedCellConfig.m_ulBandwidth)
			            {
			              // limit to physical resources last resource assignment
			              rbPerFlow = m_cschedCellConfig.m_ulBandwidth - rbAllocated;
			              // at least 3 rbg per flow to ensure TxOpportunity >= 7 bytes
			              if (rbPerFlow < 3)
			                {
			                  // terminate allocation
			                  rbPerFlow = 0;
			                }
			            }
			        }
			      if (!allocated)
			        {
			          // unable to allocate new resource: finish scheduling
			          m_nextRntiUl = (*it).first;
			          if (ret.m_dciList.size () > 0)
			            {
			        	  NS_LOG_FUNCTION("<mohamed> ---- line 2802 ----- <mohamed>");
			              m_schedSapUser->SchedUlConfigInd (ret);
			            }
			          m_allocationMaps.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap));
			          NS_LOG_FUNCTION("<mohamed> ---- line 2806 ----- <mohamed>");
			          return;
			        }
			      std::map <uint16_t, std::vector <double> >::iterator itCqi = m_ueCqi.find ((*it).first);
			      NS_LOG_FUNCTION("<mohamed> ---- line 2810 ----- <mohamed>");
			      int cqi = 0;
			      if (itCqi == m_ueCqi.end ())
			        {
			          // no cqi info about this UE
			          uldci.m_mcs = 0; // MCS 0 -> UL-AMC TBD
			          NS_LOG_INFO (this << " UE does not have ULCQI " << (*it).first );
			        }
			      else
			        {
			          // take the lowest CQI value (worst RB)
			          double minSinr = (*itCqi).second.at (uldci.m_rbStart);
			          for (uint16_t i = uldci.m_rbStart; i < uldci.m_rbStart + uldci.m_rbLen; i++)
			            {
			              if ((*itCqi).second.at (i) < minSinr)
			                {
			                  minSinr = (*itCqi).second.at (i);
			                }
			            }
			          // translate SINR -> cqi: WILD ACK: same as DL
			          double s = log2 ( 1 + (
			                                 std::pow (10, minSinr / 10 )  /
			                                 ( (-std::log (5.0 * 0.00005 )) / 1.5) ));


			          cqi = m_amc->GetCqiFromSpectralEfficiency (s);
			          if (cqi == 0)
			            {
			              it++;
			              if (it == m_ceBsrRxed.end ())
			                {
			                  // restart from the first
			                  it = m_ceBsrRxed.begin ();
			                }
			              NS_LOG_DEBUG (this << " UE discared for CQI=0, RNTI " << uldci.m_rnti);
			              // remove UE from allocation map
			              for (uint16_t i = uldci.m_rbStart; i < uldci.m_rbStart + uldci.m_rbLen; i++)
			                {
			                  rbgAllocationMap.at (i) = 0;
			                }
			              continue; // CQI == 0 means "out of range" (see table 7.2.3-1 of 36.213)
			            }
			          NS_LOG_FUNCTION("<mohamed> ---- line 2852 ----- <mohamed>");
			          uldci.m_mcs = m_amc->GetMcsFromCqi (cqi);
			        }
			      uldci.m_tbSize = (m_amc->GetTbSizeFromMcs (uldci.m_mcs, rbPerFlow) / 8); // MCS 0 -> UL-AMC TBD
			      NS_LOG_FUNCTION("<mohamed> ---- line 2856 ----- phyId" << params.m_phyId << " <mohamed>");

			      UpdateUlRlcBufferInfo (uldci.m_rnti, uldci.m_tbSize , params.m_phyId);
			      uldci.m_ndi = 1;
			      uldci.m_cceIndex = 0;
			      uldci.m_aggrLevel = 1;
			      uldci.m_ueTxAntennaSelection = 3; // antenna selection OFF
			      uldci.m_hopping = false;
			      uldci.m_n2Dmrs = 0;
			      uldci.m_tpc = 0; // no power control
			      uldci.m_cqiRequest = false; // only period CQI at this stage
			      uldci.m_ulIndex = 0; // TDD parameter
			      uldci.m_dai = 1; // TDD parameter
			      uldci.m_freqHopping = 0;
			      uldci.m_pdcchPowerOffset = 0; // not used
			      ret.m_dciList.push_back (uldci);
			      // store DCI for HARQ_PERIOD
			      uint8_t harqId = 0;
			      if (m_harqOn == true)
			        {
			          std::map <uint16_t, uint8_t>::iterator itProcId;
			          itProcId = m_ulHarqCurrentProcessId.find (uldci.m_rnti);
			          if (itProcId == m_ulHarqCurrentProcessId.end ())
			            {
			              NS_FATAL_ERROR ("No info find in HARQ buffer for UE " << uldci.m_rnti);
			            }
			          harqId = (*itProcId).second;
			          std::map <uint16_t, UlHarqProcessesDciBuffer_t>::iterator itDci = m_ulHarqProcessesDciBuffer.find (uldci.m_rnti);
			          if (itDci == m_ulHarqProcessesDciBuffer.end ())
			            {
			              NS_FATAL_ERROR ("Unable to find RNTI entry in UL DCI HARQ buffer for RNTI " << uldci.m_rnti);
			            }
			          (*itDci).second.at (harqId) = uldci;
			          // Update HARQ process status (RV 0)
			          std::map <uint16_t, UlHarqProcessesStatus_t>::iterator itStat = m_ulHarqProcessesStatus.find (uldci.m_rnti);
			          if (itStat == m_ulHarqProcessesStatus.end ())
			            {
			              NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << uldci.m_rnti);
			            }
			          (*itStat).second.at (harqId) = 0;
			        }

			      NS_LOG_INFO (this << " UL Allocation - UE " << (*it).first << " startPRB " << (uint32_t)uldci.m_rbStart << " nPRB " << (uint32_t)uldci.m_rbLen << " CQI " << cqi << " MCS " << (uint32_t)uldci.m_mcs << " TBsize " << uldci.m_tbSize << " harqId " << (uint16_t)harqId);

			      it++;
			      if (it == m_ceBsrRxed.end ())
			        {
			          // restart from the first
			          it = m_ceBsrRxed.begin ();
			        }
			      if ((rbAllocated == m_cschedCellConfig.m_ulBandwidth) || (rbPerFlow == 0))
			        {
			          // Stop allocation: no more PRBs
			          m_nextRntiUl = (*it).first;
			          break;
			        }
			    }
			  while (((*it).first != m_nextRntiUl)&&(rbPerFlow!=0));

			  m_allocationMaps.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap));

			  m_schedSapUser->SchedUlConfigInd (ret);
		  }
		  //--------------------------------------------------
		  else if (params.m_phyId == 2)
		  {
			  for (it = m_ceBsrRxed2.begin (); it != m_ceBsrRxed2.end (); it++)
			    {
			      std::set <uint16_t>::iterator itRnti = rntiAllocated.find ((*it).first);
			      // select UEs with queues not empty and not yet allocated for HARQ
			      NS_LOG_INFO (this << " UE " << (*it).first << " queue " << (*it).second);
			      if (((*it).second > 0)&&(itRnti == rntiAllocated.end ()))
			        {
			          nflows++;
			        }
			    }

			  if (nflows == 0)
			    {
			      if (ret.m_dciList.size () > 0)
			        {
			          m_allocationMaps2.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap));
			          m_schedSapUser2->SchedUlConfigInd (ret);
			        }
			      return;  // no flows to be scheduled
			    }
			  // Divide the remaining resources equally among the active users starting from the subsequent one served last scheduling trigger
			  uint16_t rbPerFlow = (m_cschedCellConfig2.m_ulBandwidth) / (nflows + rntiAllocated.size ());
			  if (rbPerFlow < 3)
			    {
			      rbPerFlow = 3;  // at least 3 rbg per flow (till available resource) to ensure TxOpportunity >= 7 bytes
			    }
			  uint16_t rbAllocated = 0;

			  if (m_nextRntiUl2 != 0)
			    {
			      for (it = m_ceBsrRxed2.begin (); it != m_ceBsrRxed2.end (); it++)
			        {
			          if ((*it).first == m_nextRntiUl2)
			            {
			              break;
			            }
			        }
			      if (it == m_ceBsrRxed2.end ())
			        {
			          NS_LOG_ERROR (this << " no user found");
			        }
			    }
			  else
			    {
			      it = m_ceBsrRxed2.begin ();
			      m_nextRntiUl2 = (*it).first;
			    }
			  NS_LOG_INFO (this << " NFlows " << nflows << " RB per Flow " << rbPerFlow);
			  do
			    {
			      std::set <uint16_t>::iterator itRnti = rntiAllocated.find ((*it).first);
			      if ((itRnti != rntiAllocated.end ())||((*it).second == 0))
			        {
			          // UE already allocated for UL-HARQ -> skip it
			          it++;
			          if (it == m_ceBsrRxed2.end ())
			            {
			              // restart from the first
			              it = m_ceBsrRxed2.begin ();
			            }
			          continue;
			        }
			      if (rbAllocated + rbPerFlow - 1 > m_cschedCellConfig2.m_ulBandwidth)
			        {
			          // limit to physical resources last resource assignment
			          rbPerFlow = m_cschedCellConfig2.m_ulBandwidth - rbAllocated;
			          // at least 3 rbg per flow to ensure TxOpportunity >= 7 bytes
			          if (rbPerFlow < 3)
			            {
			              // terminate allocation
			              rbPerFlow = 0;
			            }
			        }
			      NS_LOG_INFO (this << " try to allocate " << (*it).first);
			      UlDciListElement_s uldci;
			      uldci.m_rnti = (*it).first;
			      uldci.m_rbLen = rbPerFlow;
			      bool allocated = false;
			      NS_LOG_INFO (this << " RB Allocated " << rbAllocated << " rbPerFlow " << rbPerFlow << " flows " << nflows);
			      while ((!allocated)&&((rbAllocated + rbPerFlow - m_cschedCellConfig2.m_ulBandwidth) < 1) && (rbPerFlow != 0))
			        {
			          // check availability
			          bool free = true;
			          for (uint16_t j = rbAllocated; j < rbAllocated + rbPerFlow; j++)
			            {
			              if (rbMap.at (j) == true)
			                {
			                  free = false;
			                  break;
			                }
			            }
			          if (free)
			            {
			              uldci.m_rbStart = rbAllocated;

			              for (uint16_t j = rbAllocated; j < rbAllocated + rbPerFlow; j++)
			                {
			                  rbMap.at (j) = true;
			                  // store info on allocation for managing ul-cqi interpretation
			                  rbgAllocationMap.at (j) = (*it).first;
			                  NS_LOG_INFO ("\t " << j);
			                }
			              rbAllocated += rbPerFlow;
			              allocated = true;
			              break;
			            }
			          rbAllocated++;
			          if (rbAllocated + rbPerFlow - 1 > m_cschedCellConfig2.m_ulBandwidth)
			            {
			              // limit to physical resources last resource assignment
			              rbPerFlow = m_cschedCellConfig2.m_ulBandwidth - rbAllocated;
			              // at least 3 rbg per flow to ensure TxOpportunity >= 7 bytes
			              if (rbPerFlow < 3)
			                {
			                  // terminate allocation
			                  rbPerFlow = 0;
			                }
			            }
			        }
			      if (!allocated)
			        {
			          // unable to allocate new resource: finish scheduling
			          m_nextRntiUl2 = (*it).first;
			          if (ret.m_dciList.size () > 0)
			            {
			              m_schedSapUser2->SchedUlConfigInd (ret);
			            }
			          m_allocationMaps2.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap));
			          return;
			        }
			      std::map <uint16_t, std::vector <double> >::iterator itCqi = m_ueCqi2.find ((*it).first);
			      int cqi = 0;
			      if (itCqi == m_ueCqi2.end ())
			        {
			          // no cqi info about this UE
			          uldci.m_mcs = 0; // MCS 0 -> UL-AMC TBD
			          NS_LOG_INFO (this << " UE does not have ULCQI " << (*it).first );
			        }
			      else
			        {
			          // take the lowest CQI value (worst RB)
			          double minSinr = (*itCqi).second.at (uldci.m_rbStart);
			          for (uint16_t i = uldci.m_rbStart; i < uldci.m_rbStart + uldci.m_rbLen; i++)
			            {
			              if ((*itCqi).second.at (i) < minSinr)
			                {
			                  minSinr = (*itCqi).second.at (i);
			                }
			            }
			          // translate SINR -> cqi: WILD ACK: same as DL
			          double s = log2 ( 1 + (
			                                 std::pow (10, minSinr / 10 )  /
			                                 ( (-std::log (5.0 * 0.00005 )) / 1.5) ));


			          cqi = m_amc2->GetCqiFromSpectralEfficiency (s);
			          if (cqi == 0)
			            {
			              it++;
			              if (it == m_ceBsrRxed2.end ())
			                {
			                  // restart from the first
			                  it = m_ceBsrRxed2.begin ();
			                }
			              NS_LOG_DEBUG (this << " UE discared for CQI=0, RNTI " << uldci.m_rnti);
			              // remove UE from allocation map
			              for (uint16_t i = uldci.m_rbStart; i < uldci.m_rbStart + uldci.m_rbLen; i++)
			                {
			                  rbgAllocationMap.at (i) = 0;
			                }
			              continue; // CQI == 0 means "out of range" (see table 7.2.3-1 of 36.213)
			            }
			          uldci.m_mcs = m_amc2->GetMcsFromCqi (cqi);
			        }
			      uldci.m_tbSize = (m_amc2->GetTbSizeFromMcs (uldci.m_mcs, rbPerFlow) / 8); // MCS 0 -> UL-AMC TBD

			      UpdateUlRlcBufferInfo (uldci.m_rnti, uldci.m_tbSize , params.m_phyId);
			      uldci.m_ndi = 1;
			      uldci.m_cceIndex = 0;
			      uldci.m_aggrLevel = 1;
			      uldci.m_ueTxAntennaSelection = 3; // antenna selection OFF
			      uldci.m_hopping = false;
			      uldci.m_n2Dmrs = 0;
			      uldci.m_tpc = 0; // no power control
			      uldci.m_cqiRequest = false; // only period CQI at this stage
			      uldci.m_ulIndex = 0; // TDD parameter
			      uldci.m_dai = 1; // TDD parameter
			      uldci.m_freqHopping = 0;
			      uldci.m_pdcchPowerOffset = 0; // not used
			      ret.m_dciList.push_back (uldci);
			      // store DCI for HARQ_PERIOD
			      uint8_t harqId = 0;
			      if (m_harqOn == true)
			        {
			          std::map <uint16_t, uint8_t>::iterator itProcId;
			          itProcId = m_ulHarqCurrentProcessId2.find (uldci.m_rnti);
			          if (itProcId == m_ulHarqCurrentProcessId2.end ())
			            {
			              NS_FATAL_ERROR ("No info find in HARQ buffer for UE " << uldci.m_rnti);
			            }
			          harqId = (*itProcId).second;
			          std::map <uint16_t, UlHarqProcessesDciBuffer_t>::iterator itDci = m_ulHarqProcessesDciBuffer2.find (uldci.m_rnti);
			          if (itDci == m_ulHarqProcessesDciBuffer2.end ())
			            {
			              NS_FATAL_ERROR ("Unable to find RNTI entry in UL DCI HARQ buffer for RNTI " << uldci.m_rnti);
			            }
			          (*itDci).second.at (harqId) = uldci;
			          // Update HARQ process status (RV 0)
			          std::map <uint16_t, UlHarqProcessesStatus_t>::iterator itStat = m_ulHarqProcessesStatus2.find (uldci.m_rnti);
			          if (itStat == m_ulHarqProcessesStatus2.end ())
			            {
			              NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << uldci.m_rnti);
			            }
			          (*itStat).second.at (harqId) = 0;
			        }

			      NS_LOG_INFO (this << " UL Allocation - UE " << (*it).first << " startPRB " << (uint32_t)uldci.m_rbStart << " nPRB " << (uint32_t)uldci.m_rbLen << " CQI " << cqi << " MCS " << (uint32_t)uldci.m_mcs << " TBsize " << uldci.m_tbSize << " harqId " << (uint16_t)harqId);

			      it++;
			      if (it == m_ceBsrRxed2.end ())
			        {
			          // restart from the first
			          it = m_ceBsrRxed2.begin ();
			        }
			      if ((rbAllocated == m_cschedCellConfig2.m_ulBandwidth) || (rbPerFlow == 0))
			        {
			          // Stop allocation: no more PRBs
			          m_nextRntiUl2 = (*it).first;
			          break;
			        }
			    }
			  while (((*it).first != m_nextRntiUl2)&&(rbPerFlow!=0));

			  m_allocationMaps2.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap));

			  m_schedSapUser2->SchedUlConfigInd (ret);
		  }
		  return;
	}


/////// fizo : modifications end here ^^^^^^^^^^^^^^^
//=========================================================
void RrFfMacScheduler::DoSchedUlNoiseInterferenceReq(
		const struct FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters& params) {
	NS_LOG_FUNCTION(this);
	return;
}

void RrFfMacScheduler::DoSchedUlSrInfoReq(
		const struct FfMacSchedSapProvider::SchedUlSrInfoReqParameters& params) {
	NS_LOG_FUNCTION(this);
	return;
}

/*                        ^^^^^^^^^^^^^^^^^^^^^FIZO^^^^^^^^^^^^^^^^^
 * m_macCeList : gwaha l rnti , mac ce type , mac ce value
 * mac ce type d wa7da mn 3 options : BSR, PHR, CRNTI
 * BSR : buffer status report d l mohtamen beeha hna
 * mac ce value : gwaha m_phr , m_crnti , m_bufferStatus da l e7na bnt3aml m3ah hna

 * bndwar fl list 3la l rnti , loop 3l lcg (logical channel group ) we have only 3 lcgs
 * BufferSizeLevelBsr::BsrId2BufferSize (bsrId); l function d btakhod l buffer id trga3 its size
 * b3d kda bndwar 3la l rnti f m_ceBsrRxed :   Map of UE's buffer status reports received
 * lw not found bn-create new entery else bn-update l entery bta3t l ue da
 *
 */

void
RrFfMacScheduler::DoSchedUlMacCtrlInfoReq (const struct FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this << "For Phy" << params.m_phyId << "Do Sched UL Mac Ctrl Info Req") ;

		  std::map <uint16_t,uint32_t>::iterator it;

		  for (unsigned int i = 0; i < params.m_macCeList.size (); i++)
		    {
		      if ( params.m_macCeList.at (i).m_macCeType == MacCeListElement_s::BSR )
		        {
		          // buffer status report
		          // note that this scheduler does not differentiate the
		          // allocation according to which LCGs have more/less bytes
		          // to send.
		          // Hence the BSR of different LCGs are just summed up to get
		          // a total queue size that is used for allocation purposes.

		          uint32_t buffer = 0;
		          for (uint8_t lcg = 0; lcg < 4; ++lcg)
		            {
		              uint8_t bsrId = params.m_macCeList.at (i).m_macCeValue.m_bufferStatus.at (lcg);
		              buffer += BufferSizeLevelBsr::BsrId2BufferSize (bsrId);
		            }

		          uint16_t rnti = params.m_macCeList.at (i).m_rnti;
		          if (params.m_phyId == 1)
		          {
		        	  it = m_ceBsrRxed.find (rnti);
		        	  if (it == m_ceBsrRxed.end ())
		        	  {
		        	  	  // create the new entry
		        	  	  m_ceBsrRxed.insert ( std::pair<uint16_t, uint32_t > (rnti, buffer));
		        	  	  NS_LOG_INFO (this << "<phy1>" << " Insert RNTI " << rnti << " queue " << buffer);
		        	   }
		        	  else
		        	  {
	             	       // update the buffer size value
		        	  	   (*it).second = buffer;
		        	  	    NS_LOG_INFO (this << "<phy1>" << " Update RNTI " << rnti << " queue " << buffer);
		        	  	 }
		          }
		          else if (params.m_phyId == 2)
		          {
		        	  it = m_ceBsrRxed2.find (rnti);
		        	  if (it == m_ceBsrRxed2.end ())
		        	  {
		        	 	   // create the new entry
		        	 	   m_ceBsrRxed2.insert ( std::pair<uint16_t, uint32_t > (rnti, buffer));
		        	 	   NS_LOG_INFO (this << "<phy2>" << " Insert RNTI " << rnti << " queue " << buffer);
		        	  }
		        	  else
		        	 {
		        	       // update the buffer size value
		        	 	   (*it).second = buffer;
		        	 	   NS_LOG_INFO (this << "<phy2>" << " Update RNTI " << rnti << " queue " << buffer);
		        	 }
		          }

		        }

		}
	         return ;
	    }



//===============================================================

//-----Modified <Doha>

void
RrFfMacScheduler::DoSchedUlCqiInfoReq (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params)

{
	 NS_LOG_FUNCTION (this << params.m_phyId);

	  switch (m_ulCqiFilter)
	    {
	    case FfMacScheduler::SRS_UL_CQI:
	      {
	        // filter all the CQIs that are not SRS based
	        if (params.m_ulCqi.m_type != UlCqi_s::SRS)
	          {
	            return;
	          }
	      }
	    break;
	    case FfMacScheduler::PUSCH_UL_CQI:
	      {
	        // filter all the CQIs that are not SRS based
	        if (params.m_ulCqi.m_type != UlCqi_s::PUSCH)
	          {
	            return;
	          }
	      }
	    case FfMacScheduler::ALL_UL_CQI:
	      break;

	    default:
	      NS_FATAL_ERROR ("Unknown UL CQI type");
	    }
	  switch (params.m_ulCqi.m_type)
	    {
	    case UlCqi_s::PUSCH:
	      {
	        std::map <uint16_t, std::vector <uint16_t> >::iterator itMap;
	        std::map <uint16_t, std::vector <double> >::iterator itCqi;
	        if (params.m_phyId == 1)
	        {
	    	        itMap = m_allocationMaps.find (params.m_sfnSf);
	    	        if (itMap == m_allocationMaps.end ())
	    	          {
	    	            NS_LOG_INFO (this << "<Phy1>" << " Does not find info on allocation, size : " << m_allocationMaps.size ());
	    	            return;
	    	          }
	    	        for (uint32_t i = 0; i < (*itMap).second.size (); i++)
	    	          {
	    	            // convert from fixed point notation Sxxxxxxxxxxx.xxx to double
	    	            double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (i));
	    	            itCqi = m_ueCqi.find ((*itMap).second.at (i));
	    	            if (itCqi == m_ueCqi.end ())
	    	              {
	    	                // create a new entry
	    	                std::vector <double> newCqi;
	    	                for (uint32_t j = 0; j < m_cschedCellConfig.m_ulBandwidth; j++)
	    	                  {
	    	                    if (i == j)
	    	                      {
	    	                        newCqi.push_back (sinr);
	    	                      }
	    	                    else
	    	                      {
	    	                        // initialize with NO_SINR value.
	    	                        newCqi.push_back (30.0);
	    	                      }

	    	                  }
	    	                m_ueCqi.insert (std::pair <uint16_t, std::vector <double> > ((*itMap).second.at (i), newCqi));
	    	                // generate correspondent timer
	    	                m_ueCqiTimers.insert (std::pair <uint16_t, uint32_t > ((*itMap).second.at (i), m_cqiTimersThreshold));
	    	                std::clog << "<mai>" << m_ueCqiTimers.size()<< std::endl;
	    	              }
	    	            else
	    	              {
	    	                // update the value
	    	                (*itCqi).second.at (i) = sinr;
	    	                // update correspondent timer
	    	                std::map <uint16_t, uint32_t>::iterator itTimers;
	    	               itTimers = m_ueCqiTimers.find ((*itMap).second.at (i));
	    	                (*itTimers).second = m_cqiTimersThreshold;

	    	              }

	    	          }
	    	        // remove obsolete info on allocation
	    	        m_allocationMaps.erase (itMap);
	        }
	        else if(params.m_phyId == 2)
	        {
    	        itMap = m_allocationMaps2.find (params.m_sfnSf);
    	        if (itMap == m_allocationMaps2.end ())
    	          {
    	            NS_LOG_INFO (this << "<Phy2>" << " Does not find info on allocation, size : " << m_allocationMaps2.size ());
    	            return;
    	          }
    	        for (uint32_t i = 0; i < (*itMap).second.size (); i++)
    	          {
    	            // convert from fixed point notation Sxxxxxxxxxxx.xxx to double
    	            double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (i));
    	            itCqi = m_ueCqi2.find ((*itMap).second.at (i));
    	            if (itCqi == m_ueCqi2.end ())
    	              {
    	                // create a new entry
    	                std::vector <double> newCqi;
    	                for (uint32_t j = 0; j < m_cschedCellConfig2.m_ulBandwidth; j++)
    	                  {
    	                    if (i == j)
    	                      {
    	                        newCqi.push_back (sinr);
    	                      }
    	                    else
    	                      {
    	                        // initialize with NO_SINR value.
    	                        newCqi.push_back (30.0);
    	                      }

    	                  }
    	                m_ueCqi2.insert (std::pair <uint16_t, std::vector <double> > ((*itMap).second.at (i), newCqi));
    	                // generate correspondent timer
    	                m_ueCqiTimers2.insert (std::pair <uint16_t, uint32_t > ((*itMap).second.at (i), m_cqiTimersThreshold2));
    	              }
    	            else
    	              {
    	                // update the value
    	                (*itCqi).second.at (i) = sinr;
    	                // update correspondent timer
    	                std::map <uint16_t, uint32_t>::iterator itTimers;
    	               itTimers = m_ueCqiTimers2.find ((*itMap).second.at (i));
    	                (*itTimers).second = m_cqiTimersThreshold2;

    	              }

    	          }
    	        // remove obsolete info on allocation
    	        m_allocationMaps2.erase (itMap);
	        }
	   	      }
	      break;

	    case UlCqi_s::SRS:
	      {
	        // get the RNTI from vendor specific parameters
	        uint16_t rnti = 0;
	        NS_ASSERT (params.m_vendorSpecificList.size () > 0);
	        for (uint16_t i = 0; i < params.m_vendorSpecificList.size (); i++)
	          {
	            if (params.m_vendorSpecificList.at (i).m_type == SRS_CQI_RNTI_VSP)
	              {
	                Ptr<SrsCqiRntiVsp> vsp = DynamicCast<SrsCqiRntiVsp> (params.m_vendorSpecificList.at (i).m_value);
	                rnti = vsp->GetRnti ();
	              }
	          }
	        std::map <uint16_t, std::vector <double> >::iterator itCqi;
	        if (params.m_phyId == 1)
	        {
	        	itCqi = m_ueCqi.find (rnti);
	            if (itCqi == m_ueCqi.end ())
	            {
	        		// create a new entry
	        		std::vector <double> newCqi;
	        		for (uint32_t j = 0; j < m_cschedCellConfig.m_ulBandwidth; j++)
	        		{
	        		    double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (j));
	        		    newCqi.push_back (sinr);
	        		    NS_LOG_INFO (this << "<Phy1>" << "RNTI " << rnti << " new SRS-CQI for RB  " << j << " value " << sinr);
	        		 }
	        		m_ueCqi.insert (std::pair <uint16_t, std::vector <double> > (rnti, newCqi));
	        		// generate correspondent timer
	        		m_ueCqiTimers.insert (std::pair <uint16_t, uint32_t > (rnti, m_cqiTimersThreshold));
	        	}
	        	else
	        	{
	        		// update the values
	        		for (uint32_t j = 0; j < m_cschedCellConfig.m_ulBandwidth; j++)
	        		{
	        		   double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (j));
	        		   (*itCqi).second.at (j) = sinr;
	        		    NS_LOG_INFO (this << "<Phy1>" <<"RNTI " << rnti << " update SRS-CQI for RB  " << j << " value " << sinr);
	        	     }
	        	// update correspondent timer
	        	std::map <uint16_t, uint32_t>::iterator itTimers;
	        	itTimers = m_ueCqiTimers.find (rnti);
	        	(*itTimers).second = m_cqiTimersThreshold;
	             }
	        }
	        else if (params.m_phyId == 2)
	        {
	        	itCqi = m_ueCqi2.find (rnti);
	            if (itCqi == m_ueCqi2.end ())
	            {
	        		// create a new entry
	        		std::vector <double> newCqi;
	        		for (uint32_t j = 0; j < m_cschedCellConfig2.m_ulBandwidth; j++)
	        		{
	        		    double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (j));
	        		    newCqi.push_back (sinr);
	        		    NS_LOG_INFO (this << "<Phy2>" << "RNTI " << rnti << " new SRS-CQI for RB  " << j << " value " << sinr);
	        		 }
	        		m_ueCqi2.insert (std::pair <uint16_t, std::vector <double> > (rnti, newCqi));
	        		// generate correspondent timer
	        		m_ueCqiTimers2.insert (std::pair <uint16_t, uint32_t > (rnti, m_cqiTimersThreshold2));
	        	}
	        	else
	        	{
	        		// update the values
	        		for (uint32_t j = 0; j < m_cschedCellConfig2.m_ulBandwidth; j++)
	        		{
	        		   double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (j));
	        		   (*itCqi).second.at (j) = sinr;
	        		    NS_LOG_INFO (this << "<Phy2>" <<"RNTI " << rnti << " update SRS-CQI for RB  " << j << " value " << sinr);
	        	     }
	        	// update correspondent timer
	        	std::map <uint16_t, uint32_t>::iterator itTimers;
	        	itTimers = m_ueCqiTimers2.find (rnti);
	        	(*itTimers).second = m_cqiTimersThreshold2;
	             }
	        }

	      }
	    break;
	    case UlCqi_s::PUCCH_1:
	    case UlCqi_s::PUCCH_2:
	    case UlCqi_s::PRACH:
	     {
	        NS_FATAL_ERROR ("PfFfMacScheduler supports only PUSCH and SRS UL-CQIs");
	      }
	      break;
	      default:
	     NS_FATAL_ERROR ("Unknown type of UL-CQI");
	   }
       return;

}


//====================================================

//-----------modified <Abdelrhman> ----------------//
//-------------------------------------------------//
void RrFfMacScheduler::RefreshUlCqiMaps(uint16_t id) {
	// refresh UL CQI  Map
	std::map<uint16_t, uint32_t>::iterator itUl = m_ueCqiTimers.begin();
	NS_LOG_FUNCTION("<mohamed> phyId " << id << "<mohamed>");
	switch(id)
	{
	case 1:
	{
		NS_LOG_FUNCTION("<mohamed> line 3507 <mohamed>");
		while (itUl != m_ueCqiTimers.end()) {
			NS_LOG_INFO(this << " phy" << id << " UL-CQI for user " << (*itUl).first << " is " << (uint32_t)(*itUl).second << " thr " << (uint32_t)m_cqiTimersThreshold);
			if ((*itUl).second == 0) {
				NS_LOG_FUNCTION("<mohamed> line 3511 <mohamed>");
				// delete correspondent entries
				std::map<uint16_t, std::vector<double> >::iterator itMap =
						m_ueCqi.find((*itUl).first);
				NS_ASSERT_MSG(itMap != m_ueCqi.end()," <phy1> Does not find CQI report for user " << (*itUl).first);
				NS_LOG_INFO(this << "<phy1> UL-CQI exired for user " << (*itUl).first);
				(*itMap).second.clear();
				m_ueCqi.erase(itMap);
				std::map<uint16_t, uint32_t>::iterator temp = itUl;
				itUl++;
				m_ueCqiTimers.erase(temp);
			} else {
				NS_LOG_FUNCTION("<mohamed> line 3524 <mohamed>");
				(*itUl).second--;
				itUl++;
			   }
			NS_LOG_FUNCTION("<mohamed> line 3527 <mohamed>");
		}  // close of while
	} break ;
//--------------------------------------------------------------
	case 2:
	{
	 itUl = m_ueCqiTimers2.begin();

		while (itUl != m_ueCqiTimers2.end()) {
			NS_LOG_INFO(this << "phy" << id << " UL-CQI for user " << (*itUl).first << " is " << (uint32_t)(*itUl).second << " thr " << (uint32_t)m_cqiTimersThreshold);
			if ((*itUl).second == 0) {
				// delete correspondent entries
				std::map<uint16_t, std::vector<double> >::iterator itMap = m_ueCqi2.find((*itUl).first);
				NS_ASSERT_MSG(itMap != m_ueCqi2.end()," <phy1> Does not find CQI report for user " << (*itUl).first);
				NS_LOG_INFO(this << "<phy2> UL-CQI exired for user " << (*itUl).first);
				(*itMap).second.clear();
				m_ueCqi2.erase(itMap);
				std::map<uint16_t, uint32_t>::iterator temp = itUl;
				itUl++;
				m_ueCqiTimers2.erase(temp);
			} else {
		        (*itUl).second--;
				itUl++;
			   }

		}  // close of while
	}break;


	}
	return;
}
	//------Mod end-----------------------------------------------
	//============================================================



	//----MOd <Abdelrhman> ----------------------
void
RrFfMacScheduler::RefreshDlCqiMaps (uint16_t id)
{
//  NS_LOG_FUNCTION(this << "<Abdelrhman> PhyId  is " << id );
	switch (id)
	{
	case 1:
	 {
		  NS_LOG_FUNCTION (this <<"Id= " << id << "m_p10CqiTimers"<<m_p10CqiTimers.size ());
		  // refresh DL CQI P01 Map
		  std::map <uint16_t,uint32_t>::iterator itP10 = m_p10CqiTimers.begin ();
		  while (itP10 != m_p10CqiTimers.end ())
			{
			  NS_LOG_FUNCTION(this<<id << m_p10CqiTimers.begin()->second);
			  NS_LOG_INFO (this << " P10-CQI for user " << (*itP10).first << " is " << (uint32_t)(*itP10).second << " thr " << (uint32_t)m_cqiTimersThreshold);
			  if ((*itP10).second == 0)
				{
				  // delete correspondent entries
				  std::map <uint16_t,uint8_t>::iterator itMap = m_p10CqiRxed.find ((*itP10).first);
				  NS_ASSERT_MSG (itMap != m_p10CqiRxed.end (), " Does not find CQI report for user " << (*itP10).first);
				  NS_LOG_INFO (this << " P10-CQI exired for user " << (*itP10).first);
				  m_p10CqiRxed.erase (itMap);
				  std::map <uint16_t,uint32_t>::iterator temp = itP10;
				  itP10++;
				  m_p10CqiTimers.erase (temp);
				}
			  else
				{
				  (*itP10).second--;
				  itP10++;
				}
			}
		}break;
	case 2:
	{
		NS_LOG_FUNCTION (this <<"Id= " << id << "m_p10CqiTimers" <<  m_p10CqiTimers2.size ());
		  // refresh DL CQI P01 Map
		  std::map <uint16_t,uint32_t>::iterator itP10 = m_p10CqiTimers2.begin ();
		  std::clog << "<mohamed> " << this << "Id=2 ------------- 1 --------- <mohamed>" << std::endl;
		  while (itP10 != m_p10CqiTimers2.end ())
			{
			  std::clog << "<mohamed> " << this << "Id=2 ------------- 2 --------- <mohamed>" << std::endl;
			  NS_LOG_FUNCTION(this<< id << m_p10CqiTimers.begin()->second);
			  NS_LOG_INFO (this << " P10-CQI for user " << (*itP10).first << " is " << (uint32_t)(*itP10).second << " thr " << (uint32_t)m_cqiTimersThreshold2);
			  std::clog << "<mohamed> " << this << "Id=2 ------------- 3 --------- <mohamed>" << std::endl;
			  if ((*itP10).second == 0)
				{
				  std::clog << "<mohamed> " << this << "Id=2 ------------- 4 --------- <mohamed>" << std::endl;
				  // delete correspondent entries
				  std::map <uint16_t,uint8_t>::iterator itMap = m_p10CqiRxed2.find ((*itP10).first);
				  NS_ASSERT_MSG (itMap != m_p10CqiRxed2.end (), " Does not find CQI report for user " << (*itP10).first);
				  std::clog << "<mohamed> " << this << "Id=2 ------------- 5 --------- <mohamed>" << std::endl;
				  NS_LOG_INFO (this << " P10-CQI exired for user " << (*itP10).first);
				  m_p10CqiRxed2.erase (itMap);
				  std::map <uint16_t,uint32_t>::iterator temp = itP10;
				  itP10++;
				  m_p10CqiTimers2.erase (temp);
				  std::clog << "<mohamed> " << this << "Id=2 ------------- 6 --------- <mohamed>" << std::endl;
				}
			  else
				{
				  std::clog << "<mohamed> " << this << "Id=2 ------------- 7 --------- <mohamed>" << std::endl;
				  (*itP10).second--;
				  itP10++;
				}
			}

	}break;

	}


	std::clog << "<mohamed> " << this << "Id=2 ------------- 8 --------- <mohamed>" << std::endl;
  return;
}



	//----------MOd end-----------------------------
	//============================================================





void RrFfMacScheduler::UpdateDlRlcBufferInfo(uint16_t rnti, uint8_t lcid,uint16_t size , uint16_t id) {

	switch(id)
	{
	case 1:
	{
	NS_LOG_FUNCTION(this);
	std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it;
	for (it = m_rlcBufferReq.begin(); it != m_rlcBufferReq.end(); it++) {
		if (((*it).m_rnti == rnti)
				&& ((*it).m_logicalChannelIdentity == lcid)) {
			NS_LOG_INFO(
					this << " UE " << rnti << " LC " << (uint16_t)lcid << " txqueue " << (*it).m_rlcTransmissionQueueSize << " retxqueue " << (*it).m_rlcRetransmissionQueueSize << " status " << (*it).m_rlcStatusPduSize << " decrease " << size);
			// Update queues: RLC tx order Status, ReTx, Tx
			// Update status queue
			if (((*it).m_rlcStatusPduSize > 0)
					&& (size >= (*it).m_rlcStatusPduSize)) {
				(*it).m_rlcStatusPduSize = 0;
			} else if (((*it).m_rlcRetransmissionQueueSize > 0)
					&& (size >= (*it).m_rlcRetransmissionQueueSize)) {
				(*it).m_rlcRetransmissionQueueSize = 0;
			} else if ((*it).m_rlcTransmissionQueueSize > 0) {
				uint32_t rlcOverhead;
				if (lcid == 1) {
					// for SRB1 (using RLC AM) it's better to
					// overestimate RLC overhead rather than
					// underestimate it and risk unneeded
					// segmentation which increases delay
					rlcOverhead = 4;
				} else {
					// minimum RLC overhead due to header
					rlcOverhead = 2;
				}
				// update transmission queue
				if ((*it).m_rlcTransmissionQueueSize <= size - rlcOverhead) {
					(*it).m_rlcTransmissionQueueSize = 0;
				} else {
					(*it).m_rlcTransmissionQueueSize -= size - rlcOverhead;
				}
			}
			return;
		}
	}

 } break;
//----------------------------------------

	case 2:
	{
		NS_LOG_FUNCTION(this);
		std::list<FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it;
		for (it = m_rlcBufferReq2.begin(); it != m_rlcBufferReq2.end(); it++) {
			if (((*it).m_rnti == rnti)
					&& ((*it).m_logicalChannelIdentity == lcid)) {
				NS_LOG_INFO(
						this << "<phy2>  UE " << rnti << " LC " << (uint16_t)lcid << " txqueue " << (*it).m_rlcTransmissionQueueSize << " retxqueue " << (*it).m_rlcRetransmissionQueueSize << " status " << (*it).m_rlcStatusPduSize << " decrease " << size);
				// Update queues: RLC tx order Status, ReTx, Tx
				// Update status queue
				if (((*it).m_rlcStatusPduSize > 0)
						&& (size >= (*it).m_rlcStatusPduSize)) {
					(*it).m_rlcStatusPduSize = 0;
				} else if (((*it).m_rlcRetransmissionQueueSize > 0)
						&& (size >= (*it).m_rlcRetransmissionQueueSize)) {
					(*it).m_rlcRetransmissionQueueSize = 0;
				} else if ((*it).m_rlcTransmissionQueueSize > 0) {
					uint32_t rlcOverhead;
					if (lcid == 1) {
						// for SRB1 (using RLC AM) it's better to
						// overestimate RLC overhead rather than
						// underestimate it and risk unneeded
						// segmentation which increases delay
						rlcOverhead = 4;
					} else {
						// minimum RLC overhead due to header
						rlcOverhead = 2;
					}
					// update transmission queue
					if ((*it).m_rlcTransmissionQueueSize <= size - rlcOverhead) {
						(*it).m_rlcTransmissionQueueSize = 0;
					} else {
						(*it).m_rlcTransmissionQueueSize -= size - rlcOverhead;
					}
				}
				return;
			}
		}
	}break;

	}


}
//==================================




void RrFfMacScheduler::UpdateUlRlcBufferInfo(uint16_t rnti, uint16_t size , uint16_t id )
{
	NS_LOG_FUNCTION("<mohamed> phyId " << id << " <mohamed>");
	size = size - 2; // remove the minimum RLC overhead
	if (id == 1)
	{
		std::map<uint16_t, uint32_t>::iterator it = m_ceBsrRxed.find(rnti);
	    if (it != m_ceBsrRxed.end())
	    {
		NS_LOG_INFO(this << " <Phy1> :  Update RLC BSR UE " << rnti << " size " << size << " BSR " << (*it).second);
		if ((*it).second >= size)
		{
			(*it).second -= size;
		}
		else
		{
			(*it).second = 0;
		}
		}
		else
		{
			NS_LOG_ERROR(this << " Does not find BSR report info of UE " << rnti);
		}
	}
	else if (id == 2)
	{
		std::map<uint16_t, uint32_t>::iterator it = m_ceBsrRxed2.find(rnti);
		if (it != m_ceBsrRxed2.end())
		{
		NS_LOG_INFO(this << "<phy2> : Update RLC BSR UE " << rnti << " size " << size << " BSR " << (*it).second);
	    if ((*it).second >= size)
	    {
			(*it).second -= size;
		}
	    else
	    {
			(*it).second = 0;
		}
		}
		else
		{
			NS_LOG_ERROR(this << "<phy2> : Does not find BSR report info of UE " << rnti);
		}

	}
}

//=============================================


void RrFfMacScheduler::TransmissionModeConfigurationUpdate(uint16_t rnti,
		uint8_t txMode , uint16_t id) {
	    NS_LOG_FUNCTION(this << id << " RNTI "  << rnti << " txMode " << (uint16_t)txMode);
		FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params;
		params.m_rnti = rnti;
		params.m_transmissionMode = txMode;
		if (id == 1)
		{
			m_cschedSapUser->CschedUeConfigUpdateInd(params);
		}
		else if (id == 2)
		{
			m_cschedSapUser2->CschedUeConfigUpdateInd(params);
		}
		return ;
}



//-----Added <Abdelrhman>--------------
/*void RrFfMacScheduler::TransmissionModeConfigurationUpdate2(uint16_t rnti,uint8_t txMode) {
	NS_LOG_FUNCTION(this << "<phy2>....... RNTI " << rnti << " txMode " << (uint16_t)txMode);
	FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params;
	params.m_rnti = rnti;
	params.m_transmissionMode = txMode;
	m_cschedSapUser2->CschedUeConfigUpdateInd(params);
    }*/


//=========================================

}
