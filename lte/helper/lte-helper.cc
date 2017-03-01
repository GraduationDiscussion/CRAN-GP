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
 * Author: Nicola Baldo <nbaldo@cttc.es> (re-wrote from scratch this helper)
 *         Giuseppe Piro <g.piro@poliba.it> (parts of the PHY & channel  creation & configuration copied from the GSoC 2011 code)
 */


#include "lte-helper.h"

#include <ns3/string.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/epc-ue-nas.h>
#include <ns3/epc-enb-application.h>
#include <ns3/lte-ue-rrc.h>
#include <ns3/lte-ue-mac.h>
#include <ns3/lte-enb-mac.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-enb-phy.h>
#include <ns3/lte-ue-phy.h>
#include <ns3/lte-spectrum-phy.h>
#include <ns3/lte-chunk-processor.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/friis-spectrum-propagation-loss.h>
#include <ns3/trace-fading-loss-model.h>
#include <ns3/isotropic-antenna-model.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/ff-mac-scheduler.h>
#include <ns3/lte-ffr-algorithm.h>
#include <ns3/lte-handover-algorithm.h>
#include <ns3/lte-anr.h>
#include <ns3/lte-rlc.h>
#include <ns3/lte-rlc-um.h>
#include <ns3/lte-rlc-am.h>
#include <ns3/epc-enb-s1-sap.h>
#include <ns3/lte-rrc-protocol-ideal.h>
#include <ns3/lte-rrc-protocol-real.h>
#include <ns3/mac-stats-calculator.h>
#include <ns3/phy-stats-calculator.h>
#include <ns3/phy-tx-stats-calculator.h>
#include <ns3/phy-rx-stats-calculator.h>
#include <ns3/epc-helper.h>
#include <iostream>
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/lte-spectrum-value-helper.h>
#include <ns3/epc-x2.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteHelper");

NS_OBJECT_ENSURE_REGISTERED (LteHelper);

LteHelper::LteHelper (void)
  : m_fadingStreamsAssigned (false),
    m_imsiCounter (0),
    m_cellIdCounter (0)
{
  NS_LOG_FUNCTION (this);
  m_enbNetDeviceFactory.SetTypeId (LteEnbNetDevice::GetTypeId ());
  m_enbAntennaModelFactory.SetTypeId (IsotropicAntennaModel::GetTypeId ());
  m_ueNetDeviceFactory.SetTypeId (LteUeNetDevice::GetTypeId ());
  m_ueAntennaModelFactory.SetTypeId (IsotropicAntennaModel::GetTypeId ());
  m_channelFactory.SetTypeId (MultiModelSpectrumChannel::GetTypeId ());
}

void 
LteHelper::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_downlinkChannel = m_channelFactory.Create<SpectrumChannel> ();
  m_uplinkChannel = m_channelFactory.Create<SpectrumChannel> ();
  m_downlinkPathlossModel = m_dlPathlossModelFactory.Create ();
  Ptr<SpectrumPropagationLossModel> dlSplm = m_downlinkPathlossModel->GetObject<SpectrumPropagationLossModel> ();

  //------------------added
  m_downlinkChannel2 = m_channelFactory.Create<SpectrumChannel> ();
  m_uplinkChannel2 = m_channelFactory.Create<SpectrumChannel> ();
  m_downlinkPathlossModel2 = m_dlPathlossModelFactory.Create ();
  Ptr<SpectrumPropagationLossModel> dlSplm2 = m_downlinkPathlossModel2->GetObject<SpectrumPropagationLossModel> ();
  //------------------added

  if (dlSplm != 0 && dlSplm2 != 0)	// dlSplm2 is -------------added
    {
      //---------------added
	  NS_LOG_FUNCTION(this << "<mohamed> DL spectrum propagation loss model is [set] <moahmed>");
	  //---------------added

	  NS_LOG_LOGIC (this << " using a SpectrumPropagationLossModel in DL");
      m_downlinkChannel->AddSpectrumPropagationLossModel (dlSplm);

      //----------------------added
      m_downlinkChannel2->AddSpectrumPropagationLossModel (dlSplm);
      // -----------------------added
    }
  else
    {
	  //---------added
	  NS_LOG_FUNCTION(this << "<mohamed> DL spectrum propagation loss model is [NOT set] <mohamed> ");
	  //---------added

	  NS_LOG_LOGIC (this << " using a PropagationLossModel in DL<pathloss model is not set>");
      Ptr<PropagationLossModel> dlPlm = m_downlinkPathlossModel->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (dlPlm != 0, " " << m_downlinkPathlossModel << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_downlinkChannel->AddPropagationLossModel (dlPlm);

      //-----------------added
      Ptr<PropagationLossModel> dlPlm2 = m_downlinkPathlossModel2->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (dlPlm2 != 0, " " << m_downlinkPathlossModel2 << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_downlinkChannel2->AddPropagationLossModel (dlPlm2);
      //----------------added

    }

  m_uplinkPathlossModel = m_ulPathlossModelFactory.Create ();
  Ptr<SpectrumPropagationLossModel> ulSplm = m_uplinkPathlossModel->GetObject<SpectrumPropagationLossModel> ();

  //-------------------added
  m_uplinkPathlossModel2 = m_ulPathlossModelFactory.Create ();
  Ptr<SpectrumPropagationLossModel> ulSplm2 = m_uplinkPathlossModel2->GetObject<SpectrumPropagationLossModel> ();
  //-------------------added

  if (ulSplm != 0 && ulSplm2 != 0)	//ulSplm2 is-------added
    {
      //---------added
	  NS_LOG_FUNCTION("<mohamed> UL spectrum propagation loss model is [set] <mohamed>");
	  //---------added

	  NS_LOG_LOGIC (this << " using a SpectrumPropagationLossModel in UL");
      m_uplinkChannel->AddSpectrumPropagationLossModel (ulSplm);

      //--------------added
      m_uplinkChannel2->AddSpectrumPropagationLossModel (ulSplm);
      //-------------added
    }
  else
    {
	  //---------added
	  NS_LOG_FUNCTION("<mohamed> UL spectrum propagation loss model is [not set] <mohamed>");
	  //---------added

      NS_LOG_LOGIC (this << " using a PropagationLossModel in UL");
      Ptr<PropagationLossModel> ulPlm = m_uplinkPathlossModel->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (ulPlm != 0, " " << m_uplinkPathlossModel << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_uplinkChannel->AddPropagationLossModel (ulPlm);

      //------------added
      Ptr<PropagationLossModel> ulPlm2 = m_uplinkPathlossModel2->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (ulPlm2 != 0, " " << m_uplinkPathlossModel2 << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_uplinkChannel2->AddPropagationLossModel (ulPlm2);
      //------------added
    }
  if (!m_fadingModelType.empty ())
    {
	  //------------added
	  NS_LOG_FUNCTION("L:174, LteHelper CLASS ,DoInitialze method ,If fading model type is [not set] it will be initialized");
	  //------------added

      m_fadingModule = m_fadingModelFactory.Create<SpectrumPropagationLossModel> ();
      m_fadingModule->Initialize ();
      m_downlinkChannel->AddSpectrumPropagationLossModel (m_fadingModule);
      m_uplinkChannel->AddSpectrumPropagationLossModel (m_fadingModule);
      //--------------added
      m_downlinkChannel2->AddSpectrumPropagationLossModel (m_fadingModule);
      m_uplinkChannel2->AddSpectrumPropagationLossModel (m_fadingModule);
      //-------------added
    }
  else
  {
	  NS_LOG_FUNCTION(this << m_fadingModelType << "<mohamed> No Fading model exists yet <mohamed>");
  }
  m_phyStats = CreateObject<PhyStatsCalculator> ();
  m_phyTxStats = CreateObject<PhyTxStatsCalculator> ();
  m_phyRxStats = CreateObject<PhyRxStatsCalculator> ();
  m_macStats = CreateObject<MacStatsCalculator> ();
  Object::DoInitialize ();

}

LteHelper::~LteHelper (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId LteHelper::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::LteHelper")
    .SetParent<Object> ()
    .AddConstructor<LteHelper> ()
    .AddAttribute ("Scheduler",
                   "The type of scheduler to be used for eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::FfMacScheduler.",
                   StringValue ("ns3::PfFfMacScheduler"),
                   MakeStringAccessor (&LteHelper::SetSchedulerType,
                                       &LteHelper::GetSchedulerType),
                   MakeStringChecker ())
    .AddAttribute ("FfrAlgorithm",
                   "The type of FFR algorithm to be used for eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::LteFfrAlgorithm.",
                   StringValue ("ns3::LteFrNoOpAlgorithm"),
                   MakeStringAccessor (&LteHelper::SetFfrAlgorithmType,
                                       &LteHelper::GetFfrAlgorithmType),
                   MakeStringChecker ())
    .AddAttribute ("HandoverAlgorithm",
                   "The type of handover algorithm to be used for eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::LteHandoverAlgorithm.",
                   StringValue ("ns3::NoOpHandoverAlgorithm"),
                   MakeStringAccessor (&LteHelper::SetHandoverAlgorithmType,
                                       &LteHelper::GetHandoverAlgorithmType),
                   MakeStringChecker ())
    .AddAttribute ("PathlossModel",
                   "The type of pathloss model to be used. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::PropagationLossModel.",
                   StringValue ("ns3::FriisPropagationLossModel"),
                   MakeStringAccessor (&LteHelper::SetPathlossModelType),
                   MakeStringChecker ())
    .AddAttribute ("FadingModel",
                   "The type of fading model to be used."
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::SpectrumPropagationLossModel."
                   "If the type is set to an empty string, no fading model is used.",
                   StringValue (""),
                   MakeStringAccessor (&LteHelper::SetFadingModel),
                   MakeStringChecker ())
    .AddAttribute ("UseIdealRrc",
                   "If true, LteRrcProtocolIdeal will be used for RRC signaling. "
                   "If false, LteRrcProtocolReal will be used.",
                   BooleanValue (true), 
                   MakeBooleanAccessor (&LteHelper::m_useIdealRrc),
                   MakeBooleanChecker ())
    .AddAttribute ("AnrEnabled",
                   "Activate or deactivate Automatic Neighbour Relation function",
                   BooleanValue (true),
                   MakeBooleanAccessor (&LteHelper::m_isAnrEnabled),
                   MakeBooleanChecker ())
    .AddAttribute ("UsePdschForCqiGeneration",
                   "If true, DL-CQI will be calculated from PDCCH as signal and PDSCH as interference "
                   "If false, DL-CQI will be calculated from PDCCH as signal and PDCCH as interference  ",
                   BooleanValue (true),
                   MakeBooleanAccessor (&LteHelper::m_usePdschForCqiGeneration),
                   MakeBooleanChecker ())
  ;
  return tid;
}

void
LteHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_downlinkChannel = 0;
  m_uplinkChannel = 0;
  Object::DoDispose ();
}


void 
LteHelper::SetEpcHelper (Ptr<EpcHelper> h)
{
  NS_LOG_FUNCTION (this << h);
  m_epcHelper = h;
}

void 
LteHelper::SetSchedulerType (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  m_schedulerFactory = ObjectFactory ();
  m_schedulerFactory.SetTypeId (type);
}

std::string
LteHelper::GetSchedulerType () const
{
  return m_schedulerFactory.GetTypeId ().GetName ();
} 

void 
LteHelper::SetSchedulerAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_schedulerFactory.Set (n, v);
}

std::string
LteHelper::GetFfrAlgorithmType () const
{
  return m_ffrAlgorithmFactory.GetTypeId ().GetName ();
}

void
LteHelper::SetFfrAlgorithmType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_ffrAlgorithmFactory = ObjectFactory ();
  m_ffrAlgorithmFactory.SetTypeId (type);
}

void
LteHelper::SetFfrAlgorithmAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_ffrAlgorithmFactory.Set (n, v);
}

std::string
LteHelper::GetHandoverAlgorithmType () const
{
  return m_handoverAlgorithmFactory.GetTypeId ().GetName ();
}

void
LteHelper::SetHandoverAlgorithmType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_handoverAlgorithmFactory = ObjectFactory ();
  m_handoverAlgorithmFactory.SetTypeId (type);
}

void
LteHelper::SetHandoverAlgorithmAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_handoverAlgorithmFactory.Set (n, v);
}


void 
LteHelper::SetPathlossModelType (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  m_dlPathlossModelFactory = ObjectFactory ();
  m_dlPathlossModelFactory.SetTypeId (type);
  m_ulPathlossModelFactory = ObjectFactory ();
  m_ulPathlossModelFactory.SetTypeId (type);
}

void 
LteHelper::SetPathlossModelAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_dlPathlossModelFactory.Set (n, v);
  m_ulPathlossModelFactory.Set (n, v);
}

void
LteHelper::SetEnbDeviceAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_enbNetDeviceFactory.Set (n, v);
}


void 
LteHelper::SetEnbAntennaModelType (std::string type)
{
  NS_LOG_FUNCTION (this);
  m_enbAntennaModelFactory.SetTypeId (type);
}

void 
LteHelper::SetEnbAntennaModelAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_enbAntennaModelFactory.Set (n, v);
}

void
LteHelper::SetUeDeviceAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_ueNetDeviceFactory.Set (n, v);
}

void 
LteHelper::SetUeAntennaModelType (std::string type)
{
  NS_LOG_FUNCTION (this);
  m_ueAntennaModelFactory.SetTypeId (type);
}

void 
LteHelper::SetUeAntennaModelAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_ueAntennaModelFactory.Set (n, v);
}

void 
LteHelper::SetFadingModel (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  m_fadingModelType = type;
  if (!type.empty ())
    {
      m_fadingModelFactory = ObjectFactory ();
      m_fadingModelFactory.SetTypeId (type);
    }
}

void 
LteHelper::SetFadingModelAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION(this << n <<"<mohamed> L:419,LteHelper::SetFadingModelAttribute, set attribute to the fading model <mohamed>");
  m_fadingModelFactory.Set (n, v);
}

void 
LteHelper::SetSpectrumChannelType (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  m_channelFactory.SetTypeId (type);
}

void 
LteHelper::SetSpectrumChannelAttribute (std::string n, const AttributeValue &v)
{
  m_channelFactory.Set (n, v);
}


NetDeviceContainer
LteHelper::InstallEnbDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this << "L:444,LteHelper Class, InstallEnbDevice Method,installing the net device in a node container");
  Initialize ();  // will run DoInitialize () if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
	  NS_LOG_FUNCTION (this << "<moh>L:449,LteHelper Class, InstallEnbDevice Method,for loop to install net devices to all containers one at a time <moh>");
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleEnbDevice (node);
      devices.Add (device);
    }
  return devices;
}

NetDeviceContainer
LteHelper::InstallUeDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this << "<mohamed>***********************install UE device************************* <moahmed>");
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleUeDevice (node);
      devices.Add (device);
    }
  NS_LOG_FUNCTION (this << "<mohamed>*********************install UE device*************************** <moahmed>");
  return devices;
}


Ptr<NetDevice>
LteHelper::InstallSingleEnbDevice (Ptr<Node> n)
{

  NS_ABORT_MSG_IF (m_cellIdCounter == 65535, "max num eNBs exceeded");
  uint16_t cellId = ++m_cellIdCounter;
/*
 * ---------------------------mohamed----------------------------
 * LteSpectrum defines the spectrum of the channel so he defines 2 specrums
 * one for the Dl and the other for the UL.
 * So the following 2 lines instantiate the spectrum of the channel for the Dl,UL ->
 * (propagation loss model,propagation delay model) , sets the device connected to
 * this channel , and also sets the mobility but It still not attached to the phy
 * of the eNB and other things
 * modification process : repeat these lines
 * ---------------------------mohamed--------------------------------
 */
  //------------added
  NS_LOG_FUNCTION("L490 ,LteHelper::InstallSingleEnbDevice , initializing the spectrum for both PHYs");
  //------------added

  Ptr<LteSpectrumPhy> dlPhy = CreateObject<LteSpectrumPhy> ();
  Ptr<LteSpectrumPhy> ulPhy = CreateObject<LteSpectrumPhy> ();

  //---------------added
  Ptr<LteSpectrumPhy> dlPhy2 = CreateObject<LteSpectrumPhy> ();
  Ptr<LteSpectrumPhy> ulPhy2 = CreateObject<LteSpectrumPhy> ();
  //---------------added


 /*
 * ---------------------mohamed-------------------------------
 * the following line instantiate an instance of the eNB phy and sets this phy to
 * the previously defines parameters in Dl,Ul
 * It sets Tx power , Rx power , returns pointers to the contained saps to provide
 * them to the mac and RRC layer and also requires (ask for) pointers to the saps in the
 * MAC and RRC
 * modification process: repeat these lines , Add ID to to each reapeted one.
 * ----------------------mohamed---------------------------------
 */

  Ptr<LteEnbPhy> phy = CreateObject<LteEnbPhy> (dlPhy, ulPhy);
  //----------------------------------added
  Ptr<LteEnbPhy> phy2 = CreateObject<LteEnbPhy> (dlPhy2, ulPhy2);
  //----------------------------------added

 /*
 * ------------------------------mohamed-------------------------
 * Instantiate a HARQ then setting the dl,ul and eNB phy to this harq
 * modification process: repeat them then pass them to phy2
 * ------------------------------mohamed----------------------------
 */
  NS_LOG_FUNCTION("start Of HARQ section inside this method");
  Ptr<LteHarqPhy> harq = Create<LteHarqPhy> ();
  dlPhy->SetHarqPhyModule (harq);
  ulPhy->SetHarqPhyModule (harq);
  phy->SetHarqPhyModule (harq);
  NS_LOG_FUNCTION("End Of HARQ 1 inside this method");

  //------------------------------------added
  Ptr<LteHarqPhy> harq2 = Create<LteHarqPhy> ();
  dlPhy2->SetHarqPhyModule (harq2);
  ulPhy2->SetHarqPhyModule (harq2);
  phy2->SetHarqPhyModule (harq2);
  NS_LOG_FUNCTION("End Of HARQ 2 inside this method");
  //------------------------------------added

// --------------------------- >>>>>>>>>>>>>>>> check call backs first <<<<<<<<<<<<<<<<<<<<-------------------------------
  Ptr<LteChunkProcessor> pCtrl = Create<LteChunkProcessor> ();
  pCtrl->AddCallback (MakeCallback (&LteEnbPhy::GenerateCtrlCqiReport, phy));
  ulPhy->AddCtrlSinrChunkProcessor (pCtrl); // for evaluating SRS UL-CQI

  Ptr<LteChunkProcessor> pData = Create<LteChunkProcessor> ();
  pData->AddCallback (MakeCallback (&LteEnbPhy::GenerateDataCqiReport, phy));
  pData->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, ulPhy));
  ulPhy->AddDataSinrChunkProcessor (pData); // for evaluating PUSCH UL-CQI

  Ptr<LteChunkProcessor> pInterf = Create<LteChunkProcessor> ();
  pInterf->AddCallback (MakeCallback (&LteEnbPhy::ReportInterference, phy));
  ulPhy->AddInterferenceDataChunkProcessor (pInterf); // for interference power tracing

  //---------------------------------added
  Ptr<LteChunkProcessor> pCtrl2 = Create<LteChunkProcessor> ();
  pCtrl2->AddCallback (MakeCallback (&LteEnbPhy::GenerateCtrlCqiReport, phy2));
  ulPhy2->AddCtrlSinrChunkProcessor (pCtrl2); // for evaluating SRS UL-CQI

  Ptr<LteChunkProcessor> pData2 = Create<LteChunkProcessor> ();
  pData2->AddCallback (MakeCallback (&LteEnbPhy::GenerateDataCqiReport, phy2));
  pData2->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, ulPhy2));
  ulPhy2->AddDataSinrChunkProcessor (pData2); // for evaluating PUSCH UL-CQI

  Ptr<LteChunkProcessor> pInterf2 = Create<LteChunkProcessor> ();
  pInterf2->AddCallback (MakeCallback (&LteEnbPhy::ReportInterference, phy2));
  ulPhy2->AddInterferenceDataChunkProcessor (pInterf2); // for interference power tracing
  //----------------------------------added
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  dlPhy->SetChannel (m_downlinkChannel);
  ulPhy->SetChannel (m_uplinkChannel);

  //------------------------------------added
  dlPhy2->SetChannel (m_downlinkChannel2);
  ulPhy2->SetChannel (m_uplinkChannel2);
  //------------------------------------added

/*--------------------------------------------mohamed---------------------------
 * you need to set the mobiltity model to constant and set the different phys in different postions.
 * ------------------------------------------mohamed----------------------------
 */
  // mm will store the mobility model attribute set by the user in the script
  Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
  Ptr<MobilityModel> mm2 = n->GetObject<MobilityModel> ();
  NS_LOG_FUNCTION("mobility model type:" << mm->GetTypeId() << "	" << mm2->GetTypeId());
  // assert_msg checks the size of mm so if the size is zero will print the assert msg.
  NS_ASSERT_MSG (mm,"MobilityModel needs to be set on node before calling LteHelper::InstallUeDevice ()");
  mm -> SetPosition(Vector(0,0,0));
  dlPhy->SetMobility (mm);
  ulPhy->SetMobility (mm);

  //-------------------------------------added <there is a problem here that the mobility model contains the position so the 2 positions are the same>
  mm2 -> SetPosition(Vector(50,50,50));
  dlPhy2->SetMobility (mm2);
  ulPhy2->SetMobility (mm2);
  //-------------------------------------added

  /*--------------------------------------------mohamed---------------------------
   * you need to set 2 antennas now.
   * ------------------------------------------mohamed----------------------------
   */

  Ptr<AntennaModel> antenna = (m_enbAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
  NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
  dlPhy->SetAntenna (antenna);
  ulPhy->SetAntenna (antenna);

  //----------------------------added
  dlPhy2->SetAntenna (antenna);
  ulPhy2->SetAntenna (antenna);
  //----------------------------added

  /*--------------------------------------------mohamed---------------------------
   * No need to be changed.
   * ------------------------------------------mohamed----------------------------
   */

  Ptr<LteEnbMac> mac = CreateObject<LteEnbMac> ();
  Ptr<FfMacScheduler> sched = m_schedulerFactory.Create<FfMacScheduler> ();
  Ptr<LteFfrAlgorithm> ffrAlgorithm = m_ffrAlgorithmFactory.Create<LteFfrAlgorithm> ();
  Ptr<LteHandoverAlgorithm> handoverAlgorithm = m_handoverAlgorithmFactory.Create<LteHandoverAlgorithm> ();
  Ptr<LteEnbRrc> rrc = CreateObject<LteEnbRrc> ();

  /*--------------------------------------------mohamed---------------------------
   * RRC is illustrated in p.17 doha's part
   *  1- LteEnbRrcProtocolIdeal = Lte eNB RRC Ideal protocol  --> used with control msgs
   *  Models the transmission of RRC messages from the UE to the eNB in
   * an ideal fashion, without errors and without consuming any radio
   * resources.
   *
   *  2-LteEnbRrcProtocolReal
   *  It's used with Data traffic and resources are allocated to this type during tXion.
   * ------------------------------------------mohamed----------------------------
   */

  if (m_useIdealRrc)
    {
	  NS_LOG_FUNCTION(this << "<mohamed>L:628,LteHelper::InstallSingleEnbDevice, Ideal mode RRC (no resource allocation is done)<mohamed>");
	   /*------------------------------------------------mohamed----------------------------------------------------------------
	   * 		||																				-----------------------------------
	   *        ----																			|			<eNB>				  |
	   *        |UE| UeRrcSapProvider------msg direction-------> GetUeRrcSapProvider(rnti)------->LteEnbRrcProtocolIdeal(sap user)|
	   *        |  |											 								|			A					  |
	   *        |  |																			|			|					  |
	   *        ----																			|			V					  |
	   *        																				|	LteEnbRrc (sap provider)	  |
	   *																						-----------------------------------
	   *
	   * <LteEnbRrcProtocolIdeal>[sap user] GetLteEnbRrcSapUser <------msg direction ------- SetLteEnbRrcSapUser [sap provider] <LteEnbRrc>
	   *                                    SetLteEnbRrcSapProvider -------msg direction --> GetLteEnbRrcSapProvider
	   *
	   *illustration: within this scope the predefined RRC (in line 498) sap is connected to the rrcProtocol sap
	   * ---------------------------------------------mohamed------------------------------------------------------------------------
	   */

      Ptr<LteEnbRrcProtocolIdeal> rrcProtocol = CreateObject<LteEnbRrcProtocolIdeal> ();
      rrcProtocol->SetLteEnbRrcSapProvider (rrc->GetLteEnbRrcSapProvider ());
      rrc->SetLteEnbRrcSapUser (rrcProtocol->GetLteEnbRrcSapUser ());
      /*xxxxxxxxxxxxxxxxxxxxxx mohamed xxxxxxxxxxxxxxxxxxxx
       * The following 2 lines don't print their logs
       * I donno what is the problem here
       * xxxxxxxxxxxxxxxxxxxxx mohamed xxxxxxxxxxxxxxxxxxxx
       */
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }
  else
    {
	  NS_LOG_FUNCTION(this << "<mohamed>L:628,LteHelper::InstallSingleEnbDevice, Real mode RRC (resources allocated)<mohamed>");

	  /*-------------------------------mohamed-------------------------------------------------
	   * same as above but rrc operates in the real protocol mode
	   * -----------------------------mohamed-------------------------------------------------
	   */
      Ptr<LteEnbRrcProtocolReal> rrcProtocol = CreateObject<LteEnbRrcProtocolReal> ();
      rrcProtocol->SetLteEnbRrcSapProvider (rrc->GetLteEnbRrcSapProvider ());
      rrc->SetLteEnbRrcSapUser (rrcProtocol->GetLteEnbRrcSapUser ());
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }

  if (m_epcHelper != 0)
    {
	  NS_LOG_FUNCTION(this << "<mohamed> L:674 ,LteHelper::InstallSingleEnbDevice , CORE NETWORK EXISTS <mohamed>");
	  /*-----------------------------------------------mohamed------------------------------------------------------
	   * EnumValue is a class used to define some types of the attributes
	   * ---------------------------------------------mohamed-------------------------------------------------------
	   */
      EnumValue epsBearerToRlcMapping;
      rrc->GetAttribute ("EpsBearerToRlcMapping", epsBearerToRlcMapping);
      // it does not make sense to use RLC/SM when also using the EPC
      /*--------------------------------------------------mohamed----------------------------------------
       * as mentioned above "it does not make sense to use RLC/SM when also using the EPC" so if the RLC is set to SM mode we
       * will change it to UM mode
       * -------------------------------------------------mohamed----------------------------------------
       */
      if (epsBearerToRlcMapping.Get () == LteEnbRrc::RLC_SM_ALWAYS)
        {
          rrc->SetAttribute ("EpsBearerToRlcMapping", EnumValue (LteEnbRrc::RLC_UM_ALWAYS));
        }
    }
  else
  {
	  NS_LOG_FUNCTION(this << "<mohamed> L:693 ,LteHelper::InstallSingleEnbDevice , NO CORE NETWORK <mohamed>");
  }
  /*-------------------------------------------mohamed---------------------------------------------------
   * Bet. the MAC layer (not the scheduler) and the RRC layer (not the RRC protocol)
   * notice that It's clear that there is no RRC in user plane so It's obvious that this connection is in control plane (^v^)
   * provider: from RRC --towards--> MAC
   * User: all the way around
   * ------------------------------------------mohamed---------------------------------------------------
   */
  rrc->SetLteEnbCmacSapProvider (mac->GetLteEnbCmacSapProvider ());
  mac->SetLteEnbCmacSapUser (rrc->GetLteEnbCmacSapUser ());
  rrc->SetLteMacSapProvider (mac->GetLteMacSapProvider ());
  /*-------------------------------------------mohamed---------------------------------------------------
   * We are not using any handover during this phase so no worry about this part
   * first let me say that this part is illustrated in the UE RRC Measurements Model
   * https://www.nsnam.org/docs/models/html/lte-design.html#ue-rrc-measurements-model
   * this layer is transmited using the RRC ideal mode so It's txed directly between the UE and the eNB RRC layer without allocating any
   * resources
   * Future plans: RRC layer must be able to Identify its UEs through the physical layer they are connected on and their ID
   * + phy ID
   * + UE ID
   */
  rrc->SetLteHandoverManagementSapProvider (handoverAlgorithm->GetLteHandoverManagementSapProvider ());
  handoverAlgorithm->SetLteHandoverManagementSapUser (rrc->GetLteHandoverManagementSapUser ());

  /*---------------------------------------------mohamed------------------------------------------------------------
   * Both are from the MAC layer --towards--> MAC scheduler
   * sched : is used in the data Plane
   * Csched: is used in the control plane
   * I don't think It needs to be changed but I think the scheduler needs to be changed to meet the new added features(multiple phys)
   * --------------------------------------------mohamed-------------------------------------------------------------
   */

  mac->SetFfMacSchedSapProvider (sched->GetFfMacSchedSapProvider ());
  mac->SetFfMacCschedSapProvider (sched->GetFfMacCschedSapProvider ());
  /*--------------------------------------------mohamed------------------------------------------------------
   *all the way around the previous comment
   * --------------------------------------------mohamed--------------------------------------------------
   */
  sched->SetFfMacSchedSapUser (mac->GetFfMacSchedSapUser ());
  sched->SetFfMacCschedSapUser (mac->GetFfMacCschedSapUser ());
  /*---------------------------------------------mohamed------------------------------------------------------
   * connects the Saps between teh PHY and MAC layers
   * provider: from MAC ---towards---> PHY
   * User all the way around
   * modification:as we will have multiple phys so we need to connect the multiple phys we have to the only existing MAC
   * --------------------------------------------mohamed------------------------------------------------------
   */

  /*----------------------------mohamed
   * da lazm yt3dl 3la asas ele badria w doha 3mleno
   * ---------------------------mohamed
   */
  phy->SetLteEnbPhySapUser (mac->GetLteEnbPhySapUser ());
  mac->SetLteEnbPhySapProvider (phy->GetLteEnbPhySapProvider ());

  //----------------------added
  phy2->SetLteEnbPhySapUser(mac->GetLteEnbPhy2SapUser());
  mac->SetLteEnbPhy2SapProvider (phy2->GetLteEnbPhySapProvider ());
  //----------------------added

  /*-------------------------------------------mohamed--------------------------------------------------------
   * connects the Saps between RRC and PHY
   * provider: from RRC --towards--> PHY
   * user: all the way around
   * modification: connect all PHYs to the RRC So increase RRC saps and instantiate more phys
   * ------------------------------------------mohamed--------------------------------------------------------
   */

  phy->SetLteEnbCphySapUser (rrc->GetLteEnbCphySapUser ());
  rrc->SetLteEnbCphySapProvider (phy->GetLteEnbCphySapProvider ());

  //------------added
  phy2->SetLteEnbCphySapUser (rrc->GetLteEnbCphy2SapUser ());
  rrc->SetLteEnbCphy2SapProvider (phy2->GetLteEnbCphySapProvider ());
  //------------added

   /*-------------------------------------------mohamed------------------------------------------------------
    * Didn't check this section but this is about Frequency reuse which we won't use for now
    * so for future plane is using any frequency reuse algorithms check this section for modification
    * ------------------------------------------mohamed-----------------------------------------------------
    */
  //FFR SAP
  sched->SetLteFfrSapProvider (ffrAlgorithm->GetLteFfrSapProvider ());
  ffrAlgorithm->SetLteFfrSapUser (sched->GetLteFfrSapUser ());

  rrc->SetLteFfrRrcSapProvider (ffrAlgorithm->GetLteFfrRrcSapProvider ());
  ffrAlgorithm->SetLteFfrRrcSapUser (rrc->GetLteFfrRrcSapUser ());
  //FFR SAP END

  /*--------------------------------------------mohamed---------------------------------------------------------
   * setting the attributes to the dev
   * dev is the return value of Install method
   * -------------------------------------------mohamed---------------------------------------------------------
   */
  Ptr<LteEnbNetDevice> dev = m_enbNetDeviceFactory.Create<LteEnbNetDevice> ();
  dev->SetNode (n);
  /* xxxxxxxxxxxxxxxxxxxxxx mohamed xxxxxxxxxxxxxxx
   * can't check this part as printing no logs
   * xxxxxxxxxxxxxxxxxxxxxx mohamed xxxxxxxxxxxxxxx
   */
  dev->SetAttribute ("CellId", UintegerValue (cellId)); 
  dev->SetAttribute ("LteEnbPhy", PointerValue (phy));
  //--------------------------------added <not sure about how this part sets the phy2>
  dev->SetAttribute ("LteEnbPhy2", PointerValue (phy2));
  //--------------------------------added
  dev->SetAttribute ("LteEnbMac", PointerValue (mac));
  dev->SetAttribute ("FfMacScheduler", PointerValue (sched));
  dev->SetAttribute ("LteEnbRrc", PointerValue (rrc)); 
  dev->SetAttribute ("LteHandoverAlgorithm", PointerValue (handoverAlgorithm));
  dev->SetAttribute ("LteFfrAlgorithm", PointerValue (ffrAlgorithm));
// xxxxxxxxxxxx mohamed <end of the part> xxxxxxxxxxxx

  if (m_isAnrEnabled)
    {
	  NS_LOG_FUNCTION("<mohamed> L:809, LteHelper::InstallSingleEnbDevice, ANR (automatic neighbor relation) is enabled <mohamed>");
      Ptr<LteAnr> anr = CreateObject<LteAnr> (cellId);
      rrc->SetLteAnrSapProvider (anr->GetLteAnrSapProvider ());
      anr->SetLteAnrSapUser (rrc->GetLteAnrSapUser ());
      dev->SetAttribute ("LteAnr", PointerValue (anr));
    }
  else
    {
	  NS_LOG_FUNCTION("<mohamed> L:817, LteHelper::InstallSingleEnbDevice, ANR (automatic neighbor relation) is not enabled <mohamed>");
    }

  /*------------------------------------------------mohamed-------------------------------------------------
   * setting the phy (which is of type LteEnbPhy) to point to the connected device
   * dlPhy & ulPhy (which are of type LteSpectrumPhy) to point to the connected device
   * -----------------------------------------------mohamed-------------------------------------------------
   */
  phy->SetDevice (dev);

  //------------------added
  phy2->SetDevice(dev);
  //-----------------added

  dlPhy->SetDevice (dev);
  ulPhy->SetDevice (dev);

  //-----------------added
  dlPhy2->SetDevice (dev);
  ulPhy2->SetDevice (dev);
  //-----------------added

  /*-----------------------------------------------mohamed-------------------------------------------------
   * SetLtePhyRxData/Ctrl/UlHarqEndOKCallback: set the callback for the successful end of a RX, as part of the
   * interconnections between the PHY and the MAC (between MAC and Phy So callback)
   * <Data for data traffic , ctrl for control traffic , UlHarq fo the uplink Harq>
   *
   * MakeCallback: Build Callbacks for class method members which take varying numbers of arguments
   * and potentially returning a value.
   * MakeCallback(1st,2nd) --> 1st: return value
   * 						   2nd: arguement
   * SetForwardUpCallback : set the callback used to forward data packets up the stack
   *
   * PhyPduReceived: the Phy to notifies the MAC of the reception of a new PHY-PDU
   * ReceiveLteControlMessageList: notifies the MAC or reception of a new control msgs
   * ReceiveLteUlHarqFeedback: Notify the HARQ on the UL tranmission status
   * Receive (in the SetForwardUpCallback method): receive a packet from the lower layers in order to forward it to the upper layers
   * modification: no change to 1st line
   * 			   repeat the following 3 lines
   *               <not sure> but I think no change
   * ----------------------------------------------mohamed-------------------------------------------------
   */
  n->AddDevice (dev);
  ulPhy->SetLtePhyRxDataEndOkCallback (MakeCallback (&LteEnbPhy::PhyPduReceived, phy));
  ulPhy->SetLtePhyRxCtrlEndOkCallback (MakeCallback (&LteEnbPhy::ReceiveLteControlMessageList, phy));
  ulPhy->SetLtePhyUlHarqFeedbackCallback (MakeCallback (&LteEnbPhy::ReceiveLteUlHarqFeedback, phy));
  rrc->SetForwardUpCallback (MakeCallback (&LteEnbNetDevice::Receive, dev));

  //----------------added
  ulPhy2->SetLtePhyRxDataEndOkCallback (MakeCallback (&LteEnbPhy::PhyPduReceived, phy2));
  ulPhy2->SetLtePhyRxCtrlEndOkCallback (MakeCallback (&LteEnbPhy::ReceiveLteControlMessageList, phy2));
  ulPhy2->SetLtePhyUlHarqFeedbackCallback (MakeCallback (&LteEnbPhy::ReceiveLteUlHarqFeedback, phy2));
  //----------------added

  /*--------------------------------------moahmed---------------------------------
   * I need to make sure that this part should be repeated using different freq. for both antennas
   * ------------------------------------mohamed-----------------------------------
   */
  NS_LOG_LOGIC ("set the propagation model frequencies");
  double dlFreq = LteSpectrumValueHelper::GetCarrierFrequency (dev->GetDlEarfcn ());
  NS_LOG_LOGIC ("DL freq: " << dlFreq);
  bool dlFreqOk = m_downlinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (dlFreq));
  if (!dlFreqOk)
    {
      NS_LOG_WARN ("DL propagation model does not have a Frequency attribute");
    }
  double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (dev->GetUlEarfcn ());
  NS_LOG_LOGIC ("UL freq: " << ulFreq);
  bool ulFreqOk = m_uplinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
  if (!ulFreqOk)
    {
      NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
    }

  /* ----------------------mohamed--------------------
   *	setting the carrier freq. for the 2nd path loss model
   * ---------------------mohamed--------------------
   */
  //------------------added
  NS_LOG_LOGIC ("<mohamed> set the propagation model frequencies for the second PHY <mohamed>");
  NS_LOG_LOGIC ("DL freq2: " << dlFreq);
  dlFreqOk = m_downlinkPathlossModel2->SetAttributeFailSafe ("Frequency", DoubleValue (dlFreq));
  if (!dlFreqOk)
    {
      NS_LOG_WARN ("DL propagation model does not have a Frequency attribute");
    }
  NS_LOG_LOGIC ("UL freq2: " << ulFreq);
  ulFreqOk = m_uplinkPathlossModel2->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
  if (!ulFreqOk)
    {
      NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
    }
  //-------------------added

  /*-----------------------------------------------mohamed-----------------------------------------
   * Invoke DoInitialize on all Objects aggregated to this one.
   *
   * This method calls the virtual DoInitialize() method on all the Objects
   * aggregated to this Object. DoInitialize() will be called only once over
   * the lifetime of an Object, just like DoDispose() is called only
   * once.
   * modification: I don't think It will be modified as we have only one devise.
   * ----------------------------------------------mohamed---------------------------------------
   */
  dev->Initialize ();
   /*----------------------------------------------mohamed---------------------------------------
    * AddRx: attaches the spectrum Phy to the spectrum channel so the phy is able to receive packets sent on the channel.
    * m_uplinkchannel : is the uL channel
    *    mobile ---towards---> eNB
    * ---------------------------------------------mohamed----------------------------------------
    */
  m_uplinkChannel->AddRx (ulPhy);

  if (m_epcHelper != 0)
    {
      NS_LOG_INFO ("adding this eNB to the EPC");
      m_epcHelper->AddEnb (n, dev, dev->GetCellId ());
      Ptr<EpcEnbApplication> enbApp = n->GetApplication (0)->GetObject<EpcEnbApplication> ();
      NS_ASSERT_MSG (enbApp != 0, "cannot retrieve EpcEnbApplication");

      // S1 SAPs
      rrc->SetS1SapProvider (enbApp->GetS1SapProvider ());
      enbApp->SetS1SapUser (rrc->GetS1SapUser ());

      // X2 SAPs
      Ptr<EpcX2> x2 = n->GetObject<EpcX2> ();
      x2->SetEpcX2SapUser (rrc->GetEpcX2SapUser ());
      rrc->SetEpcX2SapProvider (x2->GetEpcX2SapProvider ());
    }

  return dev;
}

Ptr<NetDevice>
LteHelper::InstallSingleUeDevice (Ptr<Node> n)
{
  NS_LOG_FUNCTION (this);
  Ptr<LteSpectrumPhy> dlPhy = CreateObject<LteSpectrumPhy> ();
  Ptr<LteSpectrumPhy> ulPhy = CreateObject<LteSpectrumPhy> ();

  Ptr<LteUePhy> phy = CreateObject<LteUePhy> (dlPhy, ulPhy);

  Ptr<LteHarqPhy> harq = Create<LteHarqPhy> ();
  dlPhy->SetHarqPhyModule (harq);
  ulPhy->SetHarqPhyModule (harq);
  phy->SetHarqPhyModule (harq);

  Ptr<LteChunkProcessor> pRs = Create<LteChunkProcessor> ();
  pRs->AddCallback (MakeCallback (&LteUePhy::ReportRsReceivedPower, phy));
  dlPhy->AddRsPowerChunkProcessor (pRs);

  Ptr<LteChunkProcessor> pInterf = Create<LteChunkProcessor> ();
  pInterf->AddCallback (MakeCallback (&LteUePhy::ReportInterference, phy));
  dlPhy->AddInterferenceCtrlChunkProcessor (pInterf); // for RSRQ evaluation of UE Measurements

  Ptr<LteChunkProcessor> pCtrl = Create<LteChunkProcessor> ();
  pCtrl->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, dlPhy));
  dlPhy->AddCtrlSinrChunkProcessor (pCtrl);

  Ptr<LteChunkProcessor> pData = Create<LteChunkProcessor> ();
  pData->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, dlPhy));
  dlPhy->AddDataSinrChunkProcessor (pData);

  if (m_usePdschForCqiGeneration)
    {
      // CQI calculation based on PDCCH for signal and PDSCH for interference
      pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateMixedCqiReport, phy));
      Ptr<LteChunkProcessor> pDataInterf = Create<LteChunkProcessor> ();      
      pDataInterf->AddCallback (MakeCallback (&LteUePhy::ReportDataInterference, phy));
      dlPhy->AddInterferenceDataChunkProcessor (pDataInterf);
    }
  else
    {
      // CQI calculation based on PDCCH for both signal and interference
      pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateCtrlCqiReport, phy));
    }



  dlPhy->SetChannel (m_downlinkChannel);
  ulPhy->SetChannel (m_uplinkChannel);

  Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
  NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling LteHelper::InstallUeDevice ()");
  dlPhy->SetMobility (mm);
  ulPhy->SetMobility (mm);

  Ptr<AntennaModel> antenna = (m_ueAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
  NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
  dlPhy->SetAntenna (antenna);
  ulPhy->SetAntenna (antenna);

  Ptr<LteUeMac> mac = CreateObject<LteUeMac> ();
  Ptr<LteUeRrc> rrc = CreateObject<LteUeRrc> ();

  if (m_useIdealRrc)
    {
      Ptr<LteUeRrcProtocolIdeal> rrcProtocol = CreateObject<LteUeRrcProtocolIdeal> ();
      rrcProtocol->SetUeRrc (rrc);
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (rrc->GetLteUeRrcSapProvider ());
      rrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }
  else
    {
      Ptr<LteUeRrcProtocolReal> rrcProtocol = CreateObject<LteUeRrcProtocolReal> ();
      rrcProtocol->SetUeRrc (rrc);
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (rrc->GetLteUeRrcSapProvider ());
      rrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }

  if (m_epcHelper != 0)
    {
      rrc->SetUseRlcSm (false);
    }
  Ptr<EpcUeNas> nas = CreateObject<EpcUeNas> ();
 
  nas->SetAsSapProvider (rrc->GetAsSapProvider ());
  rrc->SetAsSapUser (nas->GetAsSapUser ());

  rrc->SetLteUeCmacSapProvider (mac->GetLteUeCmacSapProvider ());
  mac->SetLteUeCmacSapUser (rrc->GetLteUeCmacSapUser ());
  rrc->SetLteMacSapProvider (mac->GetLteMacSapProvider ());

  phy->SetLteUePhySapUser (mac->GetLteUePhySapUser ());
  mac->SetLteUePhySapProvider (phy->GetLteUePhySapProvider ());

  phy->SetLteUeCphySapUser (rrc->GetLteUeCphySapUser ());
  rrc->SetLteUeCphySapProvider (phy->GetLteUeCphySapProvider ());

  NS_ABORT_MSG_IF (m_imsiCounter >= 0xFFFFFFFF, "max num UEs exceeded");
  uint64_t imsi = ++m_imsiCounter;

  Ptr<LteUeNetDevice> dev = m_ueNetDeviceFactory.Create<LteUeNetDevice> ();
  dev->SetNode (n);
  dev->SetAttribute ("Imsi", UintegerValue (imsi));
  dev->SetAttribute ("LteUePhy", PointerValue (phy));
  dev->SetAttribute ("LteUeMac", PointerValue (mac));
  dev->SetAttribute ("LteUeRrc", PointerValue (rrc));
  dev->SetAttribute ("EpcUeNas", PointerValue (nas));

  phy->SetDevice (dev);
  dlPhy->SetDevice (dev);
  ulPhy->SetDevice (dev);
  nas->SetDevice (dev);

  n->AddDevice (dev);
  dlPhy->SetLtePhyRxDataEndOkCallback (MakeCallback (&LteUePhy::PhyPduReceived, phy));
  dlPhy->SetLtePhyRxCtrlEndOkCallback (MakeCallback (&LteUePhy::ReceiveLteControlMessageList, phy));
  dlPhy->SetLtePhyRxPssCallback (MakeCallback (&LteUePhy::ReceivePss, phy));
  dlPhy->SetLtePhyDlHarqFeedbackCallback (MakeCallback (&LteUePhy::ReceiveLteDlHarqFeedback, phy));
  nas->SetForwardUpCallback (MakeCallback (&LteUeNetDevice::Receive, dev));

  if (m_epcHelper != 0)
    {
      m_epcHelper->AddUe (dev, dev->GetImsi ());
    }

  dev->Initialize ();

  return dev;
}


void
LteHelper::Attach (NetDeviceContainer ueDevices)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      Attach (*i);
    }
}

void
LteHelper::Attach (Ptr<NetDevice> ueDevice)
{
  NS_LOG_FUNCTION (this);

  if (m_epcHelper == 0)
    {
      NS_FATAL_ERROR ("This function is not valid without properly configured EPC");
    }

  Ptr<LteUeNetDevice> ueLteDevice = ueDevice->GetObject<LteUeNetDevice> ();
  if (ueLteDevice == 0)
    {
      NS_FATAL_ERROR ("The passed NetDevice must be an LteUeNetDevice");
    }

  // initiate cell selection
  Ptr<EpcUeNas> ueNas = ueLteDevice->GetNas ();
  NS_ASSERT (ueNas != 0);
  uint16_t dlEarfcn = ueLteDevice->GetDlEarfcn ();
  ueNas->StartCellSelection (dlEarfcn);

  // instruct UE to immediately enter CONNECTED mode after camping
  ueNas->Connect ();

  // activate default EPS bearer
  m_epcHelper->ActivateEpsBearer (ueDevice, ueLteDevice->GetImsi (),
                                  EpcTft::Default (),
                                  EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT));
}

void
LteHelper::Attach (NetDeviceContainer ueDevices, Ptr<NetDevice> enbDevice)
{
  NS_LOG_FUNCTION (this << "<mohamed *******************attach*********************** <mohamed>>");
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      Attach (*i, enbDevice);
    }
  NS_LOG_FUNCTION (this << "<mohamed *******************attach*********************** <mohamed>>");

}

void
LteHelper::Attach (Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice)
{
  NS_LOG_FUNCTION (this);
  //enbRrc->SetCellId (enbDevice->GetObject<LteEnbNetDevice> ()->GetCellId ());

  Ptr<LteUeNetDevice> ueLteDevice = ueDevice->GetObject<LteUeNetDevice> ();
  Ptr<LteEnbNetDevice> enbLteDevice = enbDevice->GetObject<LteEnbNetDevice> ();

  Ptr<EpcUeNas> ueNas = ueLteDevice->GetNas ();
  ueNas->Connect (enbLteDevice->GetCellId (), enbLteDevice->GetDlEarfcn ());

  if (m_epcHelper != 0)
    {
      // activate default EPS bearer
      m_epcHelper->ActivateEpsBearer (ueDevice, ueLteDevice->GetImsi (), EpcTft::Default (), EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT));
    }

  // tricks needed for the simplified LTE-only simulations 
  if (m_epcHelper == 0)
    {
      ueDevice->GetObject<LteUeNetDevice> ()->SetTargetEnb (enbDevice->GetObject<LteEnbNetDevice> ());
    }
}

void
LteHelper::AttachToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer enbDevices)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      AttachToClosestEnb (*i, enbDevices);
    }
}

void
LteHelper::AttachToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer enbDevices)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (enbDevices.GetN () > 0, "empty enb device container");
  Vector uepos = ueDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
  double minDistance = std::numeric_limits<double>::infinity ();
  Ptr<NetDevice> closestEnbDevice;
  for (NetDeviceContainer::Iterator i = enbDevices.Begin (); i != enbDevices.End (); ++i)
    {
      Vector enbpos = (*i)->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      double distance = CalculateDistance (uepos, enbpos);
      if (distance < minDistance)
        {
          minDistance = distance;
          closestEnbDevice = *i;
        }
    }
  NS_ASSERT (closestEnbDevice != 0);
  Attach (ueDevice, closestEnbDevice);
}

uint8_t
LteHelper::ActivateDedicatedEpsBearer (NetDeviceContainer ueDevices, EpsBearer bearer, Ptr<EpcTft> tft)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      uint8_t bearerId = ActivateDedicatedEpsBearer (*i, bearer, tft);
      return bearerId;
    }
  return 0;
}


uint8_t
LteHelper::ActivateDedicatedEpsBearer (Ptr<NetDevice> ueDevice, EpsBearer bearer, Ptr<EpcTft> tft)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "dedicated EPS bearers cannot be set up when the EPC is not used");

  uint64_t imsi = ueDevice->GetObject<LteUeNetDevice> ()->GetImsi ();
  uint8_t bearerId = m_epcHelper->ActivateEpsBearer (ueDevice, imsi, tft, bearer);
  return bearerId;
}

/**
 * \ingroup lte
 *
 * DrbActivatior allows user to activate bearers for UEs
 * when EPC is not used. Activation function is hooked to
 * the Enb RRC Connection Estabilished trace source. When
 * UE change its RRC state to CONNECTED_NORMALLY, activation
 * function is called and bearer is activated.
*/
class DrbActivator : public SimpleRefCount<DrbActivator>
{
public:
  /**
  * DrbActivator Constructor
  *
  * \param ueDevice the UeNetDevice for which bearer will be activated
  * \param bearer the bearer configuration
  */
  DrbActivator (Ptr<NetDevice> ueDevice, EpsBearer bearer);

  /**
   * Function hooked to the Enb RRC Connection Established trace source
   * Fired upon successful RRC connection establishment.
   *
   * \param a DrbActivator object
   * \param context
   * \param imsi
   * \param cellId
   * \param rnti
   */
  static void ActivateCallback (Ptr<DrbActivator> a, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  /**
   * Procedure firstly checks if bearer was not activated, if IMSI
   * from trace source equals configured one and if UE is really
   * in RRC connected state. If all requirements are met, it performs
   * bearer activation.
   *
   * \param imsi
   * \param cellId
   * \param rnti
   */
  void ActivateDrb (uint64_t imsi, uint16_t cellId, uint16_t rnti);
private:
  /**
   * Bearer can be activated only once. This value stores state of
   * bearer. Initially is set to false and changed to true during
   * bearer activation.
   */
  bool m_active;
  /**
   * UeNetDevice for which bearer will be activated
   */
  Ptr<NetDevice> m_ueDevice;
  /**
   * Configuration of bearer which will be activated
   */
  EpsBearer m_bearer;
  /**
   * imsi the unique UE identifier
   */
  uint64_t m_imsi;
};

DrbActivator::DrbActivator (Ptr<NetDevice> ueDevice, EpsBearer bearer)
  : m_active (false),
    m_ueDevice (ueDevice),
    m_bearer (bearer),
    m_imsi (m_ueDevice->GetObject<LteUeNetDevice> ()->GetImsi ())
{
}

void
DrbActivator::ActivateCallback (Ptr<DrbActivator> a, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (a << context << imsi << cellId << rnti);
  a->ActivateDrb (imsi, cellId, rnti);
}

void
DrbActivator::ActivateDrb (uint64_t imsi, uint16_t cellId, uint16_t rnti)
{ 
  NS_LOG_FUNCTION (this << imsi << cellId << rnti << m_active);
  if ((!m_active) && (imsi == m_imsi))
    {
      Ptr<LteUeRrc> ueRrc = m_ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ();
      NS_ASSERT (ueRrc->GetState () == LteUeRrc::CONNECTED_NORMALLY);
      uint16_t rnti = ueRrc->GetRnti ();
      Ptr<LteEnbNetDevice> enbLteDevice = m_ueDevice->GetObject<LteUeNetDevice> ()->GetTargetEnb ();
      Ptr<LteEnbRrc> enbRrc = enbLteDevice->GetObject<LteEnbNetDevice> ()->GetRrc ();
      NS_ASSERT (ueRrc->GetCellId () == enbLteDevice->GetCellId ());
      Ptr<UeManager> ueManager = enbRrc->GetUeManager (rnti);
      NS_ASSERT (ueManager->GetState () == UeManager::CONNECTED_NORMALLY
                 || ueManager->GetState () == UeManager::CONNECTION_RECONFIGURATION);
      EpcEnbS1SapUser::DataRadioBearerSetupRequestParameters params;
      params.rnti = rnti;
      params.bearer = m_bearer;
      params.bearerId = 0;
      params.gtpTeid = 0; // don't care
      enbRrc->GetS1SapUser ()->DataRadioBearerSetupRequest (params);
      m_active = true;
    }
}


void 
LteHelper::ActivateDataRadioBearer (Ptr<NetDevice> ueDevice, EpsBearer bearer)
{
  NS_LOG_FUNCTION (this << ueDevice);
  NS_ASSERT_MSG (m_epcHelper == 0, "this method must not be used when the EPC is being used");

  // Normally it is the EPC that takes care of activating DRBs
  // when the UE gets connected. When the EPC is not used, we achieve
  // the same behavior by hooking a dedicated DRB activation function
  // to the Enb RRC Connection Established trace source


  Ptr<LteEnbNetDevice> enbLteDevice = ueDevice->GetObject<LteUeNetDevice> ()->GetTargetEnb ();

  std::ostringstream path;
  path << "/NodeList/" << enbLteDevice->GetNode ()->GetId () 
       << "/DeviceList/" << enbLteDevice->GetIfIndex ()
       << "/LteEnbRrc/ConnectionEstablished";
  Ptr<DrbActivator> arg = Create<DrbActivator> (ueDevice, bearer);
  Config::Connect (path.str (), MakeBoundCallback (&DrbActivator::ActivateCallback, arg));
}

void
LteHelper::AddX2Interface (NodeContainer enbNodes)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "X2 interfaces cannot be set up when the EPC is not used");

  for (NodeContainer::Iterator i = enbNodes.Begin (); i != enbNodes.End (); ++i)
    {
      for (NodeContainer::Iterator j = i + 1; j != enbNodes.End (); ++j)
        {
          AddX2Interface (*i, *j);
        }
    }
}

void
LteHelper::AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO ("setting up the X2 interface");

  m_epcHelper->AddX2Interface (enbNode1, enbNode2);
}

void
LteHelper::HandoverRequest (Time hoTime, Ptr<NetDevice> ueDev, Ptr<NetDevice> sourceEnbDev, Ptr<NetDevice> targetEnbDev)
{
  NS_LOG_FUNCTION (this << ueDev << sourceEnbDev << targetEnbDev);
  NS_ASSERT_MSG (m_epcHelper, "Handover requires the use of the EPC - did you forget to call LteHelper::SetEpcHelper () ?");
  Simulator::Schedule (hoTime, &LteHelper::DoHandoverRequest, this, ueDev, sourceEnbDev, targetEnbDev);
}

void
LteHelper::DoHandoverRequest (Ptr<NetDevice> ueDev, Ptr<NetDevice> sourceEnbDev, Ptr<NetDevice> targetEnbDev)
{
  NS_LOG_FUNCTION (this << ueDev << sourceEnbDev << targetEnbDev);

  uint16_t targetCellId = targetEnbDev->GetObject<LteEnbNetDevice> ()->GetCellId ();
  Ptr<LteEnbRrc> sourceRrc = sourceEnbDev->GetObject<LteEnbNetDevice> ()->GetRrc ();
  uint16_t rnti = ueDev->GetObject<LteUeNetDevice> ()->GetRrc ()->GetRnti ();
  sourceRrc->SendHandoverRequest (rnti, targetCellId);
}

void
LteHelper::DeActivateDedicatedEpsBearer (Ptr<NetDevice> ueDevice,Ptr<NetDevice> enbDevice, uint8_t bearerId)
{
  NS_LOG_FUNCTION (this << ueDevice << bearerId);
  NS_ASSERT_MSG (m_epcHelper != 0, "Dedicated EPS bearers cannot be de-activated when the EPC is not used");
  NS_ASSERT_MSG (bearerId != 1, "Default bearer cannot be de-activated until and unless and UE is released");

  DoDeActivateDedicatedEpsBearer (ueDevice, enbDevice, bearerId);
}

void
LteHelper::DoDeActivateDedicatedEpsBearer (Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice, uint8_t bearerId)
{
  NS_LOG_FUNCTION (this << ueDevice << bearerId);

  //Extract IMSI and rnti
  uint64_t imsi = ueDevice->GetObject<LteUeNetDevice> ()->GetImsi ();
  uint16_t rnti = ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ()->GetRnti ();


  Ptr<LteEnbRrc> enbRrc = enbDevice->GetObject<LteEnbNetDevice> ()->GetRrc ();

  enbRrc->DoSendReleaseDataRadioBearer (imsi,rnti,bearerId);
}


void 
LteHelper::ActivateDataRadioBearer (NetDeviceContainer ueDevices, EpsBearer bearer)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      ActivateDataRadioBearer (*i, bearer);
    }
}

void
LteHelper::EnableLogComponents (void)
{
  LogComponentEnable ("LteHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("LteEnbRrc", LOG_LEVEL_ALL);
  LogComponentEnable ("LteUeRrc", LOG_LEVEL_ALL);
  LogComponentEnable ("LteEnbMac", LOG_LEVEL_ALL);
  LogComponentEnable ("LteUeMac", LOG_LEVEL_ALL);
  LogComponentEnable ("LteRlc", LOG_LEVEL_ALL);
  LogComponentEnable ("LteRlcUm", LOG_LEVEL_ALL);
  LogComponentEnable ("LteRlcAm", LOG_LEVEL_ALL);
  LogComponentEnable ("RrFfMacScheduler", LOG_LEVEL_ALL);
  LogComponentEnable ("PfFfMacScheduler", LOG_LEVEL_ALL);

  LogComponentEnable ("LtePhy", LOG_LEVEL_ALL);
  LogComponentEnable ("LteEnbPhy", LOG_LEVEL_ALL);
  LogComponentEnable ("LteUePhy", LOG_LEVEL_ALL);
  LogComponentEnable ("LteSpectrumValueHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("LteSpectrumPhy", LOG_LEVEL_ALL);
  LogComponentEnable ("LteInterference", LOG_LEVEL_ALL);
  LogComponentEnable ("LteChunkProcessor", LOG_LEVEL_ALL);

  std::string propModelStr = m_dlPathlossModelFactory.GetTypeId ().GetName ().erase (0,5).c_str ();
  LogComponentEnable ("LteNetDevice", LOG_LEVEL_ALL);
  LogComponentEnable ("LteUeNetDevice", LOG_LEVEL_ALL);
  LogComponentEnable ("LteEnbNetDevice", LOG_LEVEL_ALL);

  LogComponentEnable ("RadioBearerStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("LteStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("MacStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("PhyTxStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("PhyRxStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("PhyStatsCalculator", LOG_LEVEL_ALL);


}

void
LteHelper::EnableTraces (void)
{
  EnablePhyTraces ();
  EnableMacTraces ();
  EnableRlcTraces ();
  EnablePdcpTraces ();
}

void
LteHelper::EnableRlcTraces (void)
{
  NS_ASSERT_MSG (m_rlcStats == 0, "please make sure that LteHelper::EnableRlcTraces is called at most once");
  m_rlcStats = CreateObject<RadioBearerStatsCalculator> ("RLC");
  m_radioBearerStatsConnector.EnableRlcStats (m_rlcStats);
}

int64_t
LteHelper::AssignStreams (NetDeviceContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  if ((m_fadingModule != 0) && (m_fadingStreamsAssigned == false))
    {
      Ptr<TraceFadingLossModel> tflm = m_fadingModule->GetObject<TraceFadingLossModel> ();
      if (tflm != 0)
        {
          currentStream += tflm->AssignStreams (currentStream);
          m_fadingStreamsAssigned = true;
        }
    }
  Ptr<NetDevice> netDevice;
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      netDevice = (*i);
      Ptr<LteEnbNetDevice> lteEnb = DynamicCast<LteEnbNetDevice> (netDevice);
      if (lteEnb)
        {
          Ptr<LteSpectrumPhy> dlPhy = lteEnb->GetPhy ()->GetDownlinkSpectrumPhy ();
          Ptr<LteSpectrumPhy> ulPhy = lteEnb->GetPhy ()->GetUplinkSpectrumPhy ();
          currentStream += dlPhy->AssignStreams (currentStream);
          currentStream += ulPhy->AssignStreams (currentStream);
        }
      Ptr<LteUeNetDevice> lteUe = DynamicCast<LteUeNetDevice> (netDevice);
      if (lteUe)
        {
          Ptr<LteSpectrumPhy> dlPhy = lteUe->GetPhy ()->GetDownlinkSpectrumPhy ();
          Ptr<LteSpectrumPhy> ulPhy = lteUe->GetPhy ()->GetUplinkSpectrumPhy ();
          Ptr<LteUeMac> ueMac = lteUe->GetMac ();
          currentStream += dlPhy->AssignStreams (currentStream);
          currentStream += ulPhy->AssignStreams (currentStream);
          currentStream += ueMac->AssignStreams (currentStream);
        }
    }
  return (currentStream - stream);
}


void
LteHelper::EnablePhyTraces (void)
{
  EnableDlPhyTraces ();
  EnableUlPhyTraces ();
  EnableDlTxPhyTraces ();
  EnableUlTxPhyTraces ();
  EnableDlRxPhyTraces ();
  EnableUlRxPhyTraces ();
}

void
LteHelper::EnableDlTxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbPhy/DlPhyTransmission",
                   MakeBoundCallback (&PhyTxStatsCalculator::DlPhyTransmissionCallback, m_phyTxStats));
}

void
LteHelper::EnableUlTxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/LteUePhy/UlPhyTransmission",
                   MakeBoundCallback (&PhyTxStatsCalculator::UlPhyTransmissionCallback, m_phyTxStats));
}

void
LteHelper::EnableDlRxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/LteUePhy/DlSpectrumPhy/DlPhyReception",
                   MakeBoundCallback (&PhyRxStatsCalculator::DlPhyReceptionCallback, m_phyRxStats));
}

void
LteHelper::EnableUlRxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbPhy/UlSpectrumPhy/UlPhyReception",
                   MakeBoundCallback (&PhyRxStatsCalculator::UlPhyReceptionCallback, m_phyRxStats));
}


void
LteHelper::EnableMacTraces (void)
{
  EnableDlMacTraces ();
  EnableUlMacTraces ();
}


void
LteHelper::EnableDlMacTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbMac/DlScheduling",
                   MakeBoundCallback (&MacStatsCalculator::DlSchedulingCallback, m_macStats));
}

void
LteHelper::EnableUlMacTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbMac/UlScheduling",
                   MakeBoundCallback (&MacStatsCalculator::UlSchedulingCallback, m_macStats));
}

void
LteHelper::EnableDlPhyTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/LteUePhy/ReportCurrentCellRsrpSinr",
                   MakeBoundCallback (&PhyStatsCalculator::ReportCurrentCellRsrpSinrCallback, m_phyStats));
}

void
LteHelper::EnableUlPhyTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbPhy/ReportUeSinr",
                   MakeBoundCallback (&PhyStatsCalculator::ReportUeSinr, m_phyStats));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbPhy/ReportInterference",
                   MakeBoundCallback (&PhyStatsCalculator::ReportInterference, m_phyStats));

}

Ptr<RadioBearerStatsCalculator>
LteHelper::GetRlcStats (void)
{
  return m_rlcStats;
}

void
LteHelper::EnablePdcpTraces (void)
{
  NS_ASSERT_MSG (m_pdcpStats == 0, "please make sure that LteHelper::EnablePdcpTraces is called at most once");
  m_pdcpStats = CreateObject<RadioBearerStatsCalculator> ("PDCP");
  m_radioBearerStatsConnector.EnablePdcpStats (m_pdcpStats);
}

Ptr<RadioBearerStatsCalculator>
LteHelper::GetPdcpStats (void)
{
  return m_pdcpStats;
}

} // namespace ns3
