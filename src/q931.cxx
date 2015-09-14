/*
 * q931.cxx
 *
 * Q.931 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Contributor(s): ______________________________________.
 *
 * $id $
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "q931.h"
#endif

#include "q931.h"
#include "openh323buildopts.h"

#include <ptclib/random.h>

#ifndef _Ios_Fmtflags
  #define _Ios_Fmtflags ios::fmtflags
#endif


#define new PNEW


ostream & operator<<(ostream & strm, Q931::InformationElementCodes ie)
{
  static POrdinalToString::Initialiser IENamesInit[] = {
    { Q931::BearerCapabilityIE,     "Bearer-Capability"     },
    { Q931::CauseIE,                "Cause"                 },
    { Q931::FacilityIE,             "Facility"              },
    { Q931::ProgressIndicatorIE,    "Progress-Indicator"    },
    { Q931::CallStateIE,            "Call-State"            },
    { Q931::DisplayIE,              "Display"               },
    { Q931::SignalIE,               "Signal"                },
    { Q931::KeypadIE,               "Keypad"                },
    { Q931::ConnectedNumberIE,      "Connected-Number"      },
    { Q931::CallingPartyNumberIE,   "Calling-Party-Number"  },
    { Q931::CalledPartyNumberIE,    "Called-Party-Number"   },
    { Q931::RedirectingNumberIE,    "Redirecting-Number"    },
    { Q931::ChannelIdentificationIE,"Channel-Identification"},
    { Q931::UserUserIE,             "User-User"             } 
  };
  static const POrdinalToString IENames(PARRAYSIZE(IENamesInit), IENamesInit);

  if (IENames.Contains((PINDEX)ie))
    strm << IENames[ie];
  else
    strm << "0x" << hex << (unsigned)ie << dec << " (" << (unsigned)ie << ')';

  return strm;
}


ostream & operator<<(ostream & strm, Q931::CauseValues cause)
{
  static POrdinalToString::Initialiser CauseNamesInit[] = {
    { Q931::UnallocatedNumber,           "Unallocated number"              },
    { Q931::NoRouteToNetwork,            "No route to network"             },
    { Q931::NoRouteToDestination,        "No route to destination"         },
    { Q931::SendSpecialTone,             "Send special tone"               },
    { Q931::MisdialledTrunkPrefix,       "Misdialled trunk prefix"         },
    { Q931::ChannelUnacceptable,         "Channel unacceptable"            },
    { Q931::NormalCallClearing,          "Normal call clearing"            },
    { Q931::UserBusy,                    "User busy"                       },
    { Q931::NoResponse,                  "No response"                     },
    { Q931::NoAnswer,                    "No answer"                       },
    { Q931::SubscriberAbsent,            "Subscriber absent"               },
    { Q931::CallRejected,                "Call rejected"                   },
    { Q931::NumberChanged,               "Number changed"                  },
    { Q931::Redirection,                 "Redirection"                     },
    { Q931::ExchangeRoutingError,        "Exchange routing error"          },
    { Q931::NonSelectedUserClearing,     "Non selected user clearing"      },
    { Q931::DestinationOutOfOrder,       "Destination out of order"        },
    { Q931::InvalidNumberFormat,         "Invalid number format"           },
    { Q931::FacilityRejected,            "Facility rejected"               },
    { Q931::StatusEnquiryResponse,       "Status enquiry response"         },
    { Q931::NormalUnspecified,           "Normal unspecified"              },
    { Q931::NoCircuitChannelAvailable,   "No circuit/channel available"    },
    { Q931::NetworkOutOfOrder,           "Network out of order"            },
    { Q931::TemporaryFailure,            "Temporary failure"               },
    { Q931::Congestion,                  "Congestion"                      },
    { Q931::RequestedCircuitNotAvailable,"RequestedCircuitNotAvailable"    },
    { Q931::ResourceUnavailable,         "Resource unavailable"            },
    { Q931::ServiceOptionNotAvailable,   "Service or option not available" },
    { Q931::InvalidCallReference,        "Invalid call reference"          },
    { Q931::IncompatibleDestination,     "Incompatible destination"        },
    { Q931::IENonExistantOrNotImplemented,"IE non-existent or not implemented" },
    { Q931::TimerExpiry,                 "Recovery from timer expiry"      },
    { Q931::ProtocolErrorUnspecified,    "Protocol error, unspecified"     },
    { Q931::InterworkingUnspecified,     "Interworking, unspecified"       },
    { Q931::CallAwarded,                 "Call awarded, delivered via established channel" },
    { Q931::Preemption,                  "Call is being preempted" },
    { Q931::PreemptionCircuitReserved,   "Preemption - circuit reserved for reuse" },
    { Q931::CallQueued,                  "Call queued" },
    { Q931::FrameModeOOS,                "Frame mode out of service" },
    { Q931::FrameModeOperational,        "Frame mode operational" },
    { Q931::AccessInformationDiscarded,  "Access information discarded" },
    { Q931::PrecedenceCallBlocked,       "Precedence Call Blocked" },
    { Q931::QoSNotAvailable,             "QoS not available" },
    { Q931::RequestedFacilityNotSubscribed,   "Requested facility not subscribed"  },
    { Q931::OutgoingCallsBarred,              "Outgoing calls barred"              },
    { Q931::OutgoingCallsBarredInCUG,         "Outgoing calls barred in CUG"       },
    { Q931::IncomingCallsBarred,              "Incoming calls barred"              },
    { Q931::IncomingCallsBarredInCUG,         "Incoming calls barred in CUG"       },
    { Q931::BearerCapNotAuthorised,           "Bearer cap not authorised"          },
    { Q931::BearerCapNotPresentlyAvailable,   "Bearer cap not presently available" },
    { Q931::InconsistentOutgoingIE,           "Inconsistent outgoing information element" },
    { Q931::BearerCapNotImplemented,          "Bearer cap not implemented"        },
    { Q931::ChannelTypeNotImplemented,        "Channel type not implemented"      },
    { Q931::RequestedFacilityNotImplemented,  "Requested facility not implemented" },
    { Q931::OnlyRestrictedDigitalBearerCapAvailable, "Only restricted digital bearer cap available" },
    { Q931::ServiceOrOptionNotImplemented,    "Service or option not implemented" },
    { Q931::IdentifiedChannelNonExistent,     "IdentifiedChannelNonExistent"      },
    { Q931::CallIdentifyNotSuspendedCall,     "CallIdentifyNotSuspendedCall"      }, 
    { Q931::CallIdentifyInUse,                "CallIdentifyInUse"                 },
    { Q931::NoCallSuspended,                  "NoCallSuspended"                   },
    { Q931::ClearedRequestedCallIdentity,     "ClearedRequestedCallIdentity"      },
    { Q931::UserNotInCUG,                     "UserNotInCUG"                      },
  };
  static const POrdinalToString CauseNames(PARRAYSIZE(CauseNamesInit), CauseNamesInit);

  if (CauseNames.Contains((PINDEX)cause))
    strm << CauseNames[cause];
  else if (cause < Q931::ErrorInCauseIE)
    strm << "0x" << hex << (unsigned)cause << dec << " (" << (unsigned)cause << ')';
  else
    strm << "N/A";

  return strm;
}


///////////////////////////////////////////////////////////////////////////////

Q931::Q931()
{
  protocolDiscriminator = 8;  // Q931 always has 00001000
  messageType = NationalEscapeMsg;
  fromDestination = FALSE;
  callReference = 0;
}


Q931::Q931(const Q931 & other)
{
  operator=(other);
}


Q931 & Q931::operator=(const Q931 & other)
{
  callReference = other.callReference;
  fromDestination = other.fromDestination;
  protocolDiscriminator = other.protocolDiscriminator;
  messageType = other.messageType;

  informationElements.RemoveAll();
  for (PINDEX i = 0; i < other.informationElements.GetSize(); i++)
    informationElements.SetAt(other.informationElements.GetKeyAt(i), new PBYTEArray(other.informationElements.GetDataAt(i)));

  return *this;
}


void Q931::BuildFacility(int callRef, PBoolean fromDest)
{
  messageType = FacilityMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
  PBYTEArray data;
  SetIE(FacilityIE, data);
}


void Q931::BuildInformation(int callRef, PBoolean fromDest)
{
  messageType = InformationMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
}


void Q931::BuildProgress(int callRef,
                         PBoolean fromDest,
                         unsigned description,
                         unsigned codingStandard,
                         unsigned location)
{
  messageType = ProgressMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
  SetProgressIndicator(description, codingStandard, location);
}


void Q931::BuildNotify(int callRef, PBoolean fromDest)
{
  messageType = NotifyMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
}


void Q931::BuildSetupAcknowledge(int callRef)
{
  messageType = SetupAckMsg;
  callReference = callRef;
  fromDestination = TRUE;
  informationElements.RemoveAll();
}


void Q931::BuildCallProceeding(int callRef)
{
  messageType = CallProceedingMsg;
  callReference = callRef;
  fromDestination = TRUE;
  informationElements.RemoveAll();
}


void Q931::BuildAlerting(int callRef)
{
  messageType = AlertingMsg;
  callReference = callRef;
  fromDestination = TRUE;
  informationElements.RemoveAll();
}


void Q931::BuildSetup(int callRef)
{
  messageType = SetupMsg;
  if (callRef < 0)
    callReference = GenerateCallReference();
  else
    callReference = callRef;
  fromDestination = FALSE;
  informationElements.RemoveAll();
  SetBearerCapabilities(TransferSpeech, 1);
}


void Q931::BuildConnect(int callRef)
{
  messageType = ConnectMsg;
  callReference = callRef;
  fromDestination = TRUE;
  informationElements.RemoveAll();
  //SetBearerCapabilities(TransferSpeech, 1); <- Codian interop issue - SH
}

void Q931::BuildConnectAck(int callRef, PBoolean fromDest)
{
  messageType = ConnectAckMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
}


void Q931::BuildStatus(int callRef, PBoolean fromDest)
{
  messageType = StatusMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
  SetCallState(CallState_Active);
  // Cause field as per Q.850
  SetCause(StatusEnquiryResponse);
}


void Q931::BuildStatusEnquiry(int callRef, PBoolean fromDest)
{
  messageType = StatusEnquiryMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
}


void Q931::BuildReleaseComplete(int callRef, PBoolean fromDest)
{
  messageType = ReleaseCompleteMsg;
  callReference = callRef;
  fromDestination = fromDest;
  informationElements.RemoveAll();
}


PBoolean Q931::Decode(const PBYTEArray & data)
{
  // Clear all existing data before reading new
  informationElements.RemoveAll();

  if (data.GetSize() < 5) // Packet too short
    return FALSE;

  protocolDiscriminator = data[0];

  unsigned callRefLen = data[1];
  if (callRefLen > 2) // Call reference is usually 2 bytes long, Innovaphone sends 0 for H.460.17
    return FALSE;

  if (callRefLen == 2) {
    callReference = ((data[2]&0x7f) << 8) | data[3];
    fromDestination = (data[2]&0x80) != 0;
  } else {
    callReference = 0;
    fromDestination = false;
  }

  messageType = (MsgTypes)data[2+callRefLen];

  // Have preamble, start getting the informationElements into buffers
  PINDEX offset = 3+callRefLen;
  while (offset < data.GetSize()) {
    // Get field discriminator
    int discriminator = data[offset++];

    PBYTEArray * item = new PBYTEArray;

    // For discriminator with high bit set there is no data
    if ((discriminator&0x80) == 0) {
      int len = data[offset++];

      if (discriminator == UserUserIE) {
        // Special case of User-user field. See 7.2.2.31/H.225.0v4.
        len <<= 8;
        len |= data[offset++];

        // we also have a protocol discriminator, which we ignore
        offset++;

        // before decrementing the length, make sure it is not zero
        if (len == 0) {
          delete item;
          return FALSE;
        }

        // adjust for protocol discriminator
        len--;
      }

      if (offset + len > data.GetSize()) {
        delete item;
        return FALSE;
      }

      memcpy(item->GetPointer(len), (const BYTE *)data+offset, len);
      offset += len;
    }

    informationElements.SetAt(discriminator, item);
  }

  return TRUE;
}


PBoolean Q931::Encode(PBYTEArray & data) const
{
  PINDEX totalBytes = 5;
  unsigned discriminator;
  for (discriminator = 0; discriminator < 256; discriminator++) {
    if (informationElements.Contains(discriminator)) {
      if (discriminator < 128)
        totalBytes += informationElements[discriminator].GetSize() +
                            (discriminator != UserUserIE ? 2 : 4);
      else
        totalBytes++;
    }
  }

  if (!data.SetMinSize(totalBytes))
    return FALSE;

  // Put in Q931 header
  PAssert(protocolDiscriminator < 256, PInvalidParameter);
  data[0] = (BYTE)protocolDiscriminator;
  data[1] = 2; // Length of call reference
  data[2] = (BYTE)(callReference >> 8);
  if (fromDestination)
    data[2] |= 0x80;
  data[3] = (BYTE)callReference;
  PAssert(messageType < 256, PInvalidParameter);
  data[4] = (BYTE)messageType;

  // The following assures disciminators are in ascending value order
  // as required by Q931 specification
  PINDEX offset = 5;
  for (discriminator = 0; discriminator < 256; discriminator++) {
    if (informationElements.Contains(discriminator)) {
      if (discriminator < 128) {
        int len = informationElements[discriminator].GetSize();

        if (discriminator != UserUserIE) {
          data[offset++] = (BYTE)discriminator;
          data[offset++] = (BYTE)len;
        }
        else {
          len++; // Allow for protocol discriminator
          data[offset++] = (BYTE)discriminator;
          data[offset++] = (BYTE)(len >> 8);
          data[offset++] = (BYTE)len;
          len--; // Then put the length back again
          // We shall assume that the user-user field is an ITU protocol block (5)
          data[offset++] = 5;
        }

        memcpy(&data[offset], (const BYTE *)informationElements[discriminator], len);
        offset += len;
      }
      else
        data[offset++] = (BYTE)discriminator;
    }
  }

  return data.SetSize(offset);
}


void Q931::PrintOn(ostream & strm) const
{
  int indent = (int)strm.precision() + 2;
  _Ios_Fmtflags flags = strm.flags();

  strm << "{\n"
       << setw(indent+24) << "protocolDiscriminator = " << protocolDiscriminator << '\n'
       << setw(indent+16) << "callReference = " << callReference << '\n'
       << setw(indent+7)  << "from = " << (fromDestination ? "destination" : "originator") << '\n'
       << setw(indent+14) << "messageType = " << GetMessageTypeName() << '\n';

  for (unsigned discriminator = 0; discriminator < 256; discriminator++) {
    if (informationElements.Contains(discriminator)) {
      strm << setw(indent+4) << "IE: " << (InformationElementCodes)discriminator;
      if (discriminator == CauseIE) {
        if (informationElements[discriminator].GetSize() > 1)
          strm << " - " << (CauseValues)(informationElements[discriminator][1]&0x7f);
      }
      strm << " = {\n"
           << hex << setfill('0') << resetiosflags(ios::floatfield)
           << setprecision(indent+2) << setw(16);

      PBYTEArray value = informationElements[discriminator];
      if (value.GetSize() <= 32 || (flags&ios::floatfield) != ios::fixed)
        strm << value;
      else {
        PBYTEArray truncatedArray(value, 32);
        strm << truncatedArray << '\n'
             << setfill(' ')
             << setw(indent+5) << "...";
      }

      strm << dec << setfill(' ')
           << '\n'
           << setw(indent+2) << "}\n";
    }
  }

  strm << setw(indent-1) << "}";

  strm.flags(flags);
}


PString Q931::GetMessageTypeName() const
{
  switch (messageType) {
    case AlertingMsg :
      return "Alerting";
    case CallProceedingMsg :
      return "CallProceeding";
    case ConnectMsg :
      return "Connect";
    case ConnectAckMsg :
      return "ConnectAck";
    case ProgressMsg :
      return "Progress";
    case SetupMsg :
      return "Setup";
    case SetupAckMsg :
      return "SetupAck";
    case FacilityMsg :
      return "Facility";
    case ReleaseCompleteMsg :
      return "ReleaseComplete";
    case StatusEnquiryMsg :
      return "StatusEnquiry";
    case StatusMsg :
      return "Status";
    case InformationMsg :
      return "Information";
    case NationalEscapeMsg :
      return "Escape";
    case NotifyMsg :
      return "NotifyMsg";
    case ResumeMsg :
      return "ResumeMsg";
    case ResumeAckMsg :
      return "ResumeAckMsg";
    case ResumeRejectMsg :
      return "ResumeRejectMsg";
    case SuspendMsg :
      return "SuspendMsg";
    case SuspendAckMsg :
      return "SuspendAckMsg";
    case SuspendRejectMsg :
      return "SuspendRejectMsg";
    case UserInformationMsg :
      return "UserInformationMsg";
    case DisconnectMsg :
      return "DisconnectMsg";
    case ReleaseMsg :
      return "ReleaseMsg";
    case RestartMsg :
      return "RestartMsg";
    case RestartAckMsg :
      return "RestartAckMsg";
    case SegmentMsg :
      return "SegmentMsg";
    case CongestionCtrlMsg :
      return "CongestionCtrlMsg";
    default :
      break;
  }

  return psprintf("<%u>", messageType);
}


unsigned Q931::GenerateCallReference()
{
  static unsigned LastCallReference;
  static PMutex mutex;
  PWaitAndSignal wait(mutex);

  if (LastCallReference == 0)
    LastCallReference = PRandom::Number();
  else
    LastCallReference++;

  LastCallReference &= 0x7fff;

  if (LastCallReference == 0)
    LastCallReference = 1;

  return LastCallReference;
}


PBoolean Q931::HasIE(InformationElementCodes ie) const
{
  return informationElements.Contains(POrdinalKey(ie));
}


PBYTEArray Q931::GetIE(InformationElementCodes ie) const
{
  if (informationElements.Contains(POrdinalKey(ie)))
    return informationElements[ie];

  return PBYTEArray();
}


void Q931::SetIE(InformationElementCodes ie, const PBYTEArray & userData)
{
  informationElements.SetAt(ie, new PBYTEArray(userData));
}

void Q931::RemoveIE(InformationElementCodes ie)
{
  informationElements.RemoveAt(ie);
}

unsigned Q931::SetBearerTransferRate(unsigned bitrate)
{
   unsigned rawTransferRate = bitrate/64000;

   if      (rawTransferRate <= 2)  return rawTransferRate;
   else if (rawTransferRate <= 6)  return  6;
   else if (rawTransferRate <= 24) return 24;
   else if (rawTransferRate >= 30) return 30;
   else return 32;  // Never called. Bug with Polycom RMX
}

void Q931::SetBearerCapabilities(InformationTransferCapability capability,
                                 unsigned transferRate,
                                 unsigned codingStandard,
                                 unsigned userInfoLayer1)
{
  BYTE data[4];
  PINDEX size = 1;
  data[0] = (BYTE)(0x80 | ((codingStandard&3) << 5) | (capability&31));

  switch (codingStandard) {
    case 0 :  // ITU-T standardized coding
      size = 3;

      // Note this is always "Circuit Mode"
      switch (transferRate) {
        case 1 :
          data[1] = 0x90;
          break;
        case 2 :
          data[1] = 0x91;
          break;
        case 6 :
          data[1] = 0x93;
          break;
        case 24 :
          data[1] = 0x95;
          break;
        case 30 :
          data[1] = 0x97;
          break;
        case 32 :
          data[1] = 0x98;
          data[2] = 0xA0;
          size = 4;
          break;
        default :
          PAssert(transferRate > 0 && transferRate < 128, PInvalidParameter);
          data[1] = 0x18;
          data[2] = (BYTE)(0x80 | transferRate);
          size = 4;
      }

      PAssert(userInfoLayer1 >= 2 && userInfoLayer1 <= 5, PInvalidParameter);
      data[size-1] = (BYTE)(0x80 | (1<<5) | userInfoLayer1);
      break;

    case 1 : // Other international standard
      size = 2;
      data[1] = 0x80; // Call independent signalling connection
      break;

    default :
      break;
  }

  SetIE(BearerCapabilityIE, PBYTEArray(data, size));
}


PBoolean Q931::GetBearerCapabilities(InformationTransferCapability & capability,
                                 unsigned & transferRate,
                                 unsigned * codingStandard,
                                 unsigned * userInfoLayer1) const
{
  if (!HasIE(BearerCapabilityIE))
    return FALSE;

  PBYTEArray data = GetIE(BearerCapabilityIE);
  if (data.GetSize() < 2)
    return FALSE;

  capability = (InformationTransferCapability)data[0];
  if (codingStandard != NULL)
    *codingStandard = (data[0] >> 5)&3;

  PINDEX nextByte = 2;
  switch (data[1]) {
    case 0x90 :
      transferRate = 1;
      break;
    case 0x91 :
      transferRate = 2;
      break;
    case 0x93 :
      transferRate = 6;
      break;
    case 0x95 :
      transferRate = 24;
      break;
    case 0x97 :
      transferRate = 30;
      break;
    case 0x18 :
      if (data.GetSize() < 3)
        return FALSE;
      transferRate = data[2]&0x7f;
      nextByte = 3;
      break;
    default :
      return FALSE;
  }

  if (userInfoLayer1 != NULL)
    *userInfoLayer1 = data.GetSize() > nextByte && ((data[nextByte]>>5)&3) == 1 ? (data[nextByte]&0x1f) : 0;

  return TRUE;
}


void Q931::SetCause(CauseValues value, unsigned standard, unsigned location)
{
  PBYTEArray data(2);
  data[0] = (BYTE)(0x80 | ((standard&3) << 5) | (location&15));
  data[1] = (BYTE)(0x80 | value);
  SetIE(CauseIE, data);
}


Q931::CauseValues Q931::GetCause(unsigned * standard, unsigned * location) const
{
  if (!HasIE(CauseIE))
    return ErrorInCauseIE;

  PBYTEArray data = GetIE(CauseIE);
  if (data.GetSize() < 2)
    return ErrorInCauseIE;

  if (standard != NULL)
    *standard = (data[0] >> 5)&3;
  if (location != NULL)
    *location = data[0]&15;

  if ((data[0]&0x80) != 0)
    return (CauseValues)(data[1]&0x7f);

  // Allow for optional octet
  if (data.GetSize() < 3)
    return ErrorInCauseIE;

  return (CauseValues)(data[2]&0x7f);
}


void Q931::SetCallState(CallStates value, unsigned standard)
{
  if (value >= CallState_ErrorInIE)
    return;

  // Call State as per Q.931 section 4.5.7
  PBYTEArray data(1);
  data[0] = (BYTE)(((standard&3) << 6) | value);
  SetIE(CallStateIE, data);
}


Q931::CallStates Q931::GetCallState(unsigned * standard) const
{
  if (!HasIE(CallStateIE))
    return CallState_ErrorInIE;

  PBYTEArray data = GetIE(CallStateIE);
  if (data.IsEmpty())
    return CallState_ErrorInIE;

  if (standard != NULL)
    *standard = (data[0] >> 6)&3;

  return (CallStates)(data[0]&0x3f);
}


void Q931::SetSignalInfo(SignalInfo value)
{
  PBYTEArray data(1);
  data[0] = (BYTE)value;
  SetIE(SignalIE, data);
}


Q931::SignalInfo Q931::GetSignalInfo() const
{
  if (!HasIE(SignalIE))
    return SignalErrorInIE;

  PBYTEArray data = GetIE(SignalIE);
  if (data.IsEmpty())
    return SignalErrorInIE;

  return (SignalInfo)data[0];
}


void Q931::SetKeypad(const PString & digits)
{
  PBYTEArray bytes((const BYTE *)(const char *)digits, digits.GetLength()+1);
  SetIE(KeypadIE, bytes);
}


PString Q931::GetKeypad() const
{
  if (!HasIE(Q931::KeypadIE))
    return PString();

  PBYTEArray digits = GetIE(Q931::KeypadIE);
  if (digits.IsEmpty())
    return PString();

  return PString((const char *)(const BYTE *)digits, digits.GetSize());
}


void Q931::SetProgressIndicator(unsigned description,
                                unsigned codingStandard,
                                unsigned location)
{
  PBYTEArray data(2);
  data[0] = (BYTE)(0x80+((codingStandard&0x03)<<5)+(location&0x0f));
  data[1] = (BYTE)(0x80+(description&0x7f));
  SetIE(ProgressIndicatorIE, data);
}


PBoolean Q931::GetProgressIndicator(unsigned & description,
                                unsigned * codingStandard,
                                unsigned * location) const
{
  if (!HasIE(ProgressIndicatorIE))
    return FALSE;

  PBYTEArray data = GetIE(ProgressIndicatorIE);
  if (data.GetSize() < 2)
    return FALSE;

  if (codingStandard != NULL)
    *codingStandard = (data[0]>>5)&0x03;
  if (location != NULL)
    *location = data[0]&0x0f;
  description = data[1]&0x7f;

  return TRUE;
}


void Q931::SetDisplayName(const PString & name)
{
  PBYTEArray bytes((const BYTE *)(const char *)name, name.GetLength()+1);
  if (name.GetLength() == 0)
    RemoveIE(DisplayIE);
  else
    SetIE(DisplayIE, bytes);
}


PString Q931::GetDisplayName() const
{
  if (!HasIE(Q931::DisplayIE))
    return PString();

  PBYTEArray display = GetIE(Q931::DisplayIE);
  if (display.IsEmpty())
    return PString();

  return PString((const char *)(const BYTE *)display, display.GetSize());
}


static PBYTEArray SetNumberIE(const PString & number,
                              unsigned plan,
                              unsigned type,
                              int presentation,
                              int screening,
                              int reason)
{
  PBYTEArray bytes;

  PINDEX len = number.GetLength();

  if (reason == -1) {
    if (presentation == -1 || screening == -1) {
      bytes.SetSize(len+1);
      bytes[0] = (BYTE)(0x80|((type&7)<<4)|(plan&15));
      memcpy(bytes.GetPointer()+1, (const char *)number, len);
    }
    else {
      bytes.SetSize(len+2);
      bytes[0] = (BYTE)(((type&7)<<4)|(plan&15));
      bytes[1] = (BYTE)(0x80|((presentation&3)<<5)|(screening&3));
      memcpy(bytes.GetPointer()+2, (const char *)number, len);
    }
  } 
  else {
    // If octet 3b is present, then octet 3a must also be present!
    if (presentation == -1 || screening == -1) {
      // This situation should never occur!!!
      bytes.SetSize(len+1);
      bytes[0] = (BYTE)(0x80|((type&7)<<4)|(plan&15));
      memcpy(bytes.GetPointer()+1, (const char *)number, len);
    }
    else {
      bytes.SetSize(len+3);
      bytes[0] = (BYTE)(0x80|((type&7)<<4)|(plan&15));
      bytes[1] = (BYTE)(0x80|((presentation&3)<<5)|(screening&3));
      bytes[2] = (BYTE)(0x80|(reason&15));
      memcpy(bytes.GetPointer()+3, (const char *)number, len);
    }
  }

  return bytes;
}


static PBoolean GetNumberIE(const PBYTEArray & bytes,
                        PString  & number,
                        unsigned * plan,
                        unsigned * type,
                        unsigned * presentation,
                        unsigned * screening,
                        unsigned * reason,
                        unsigned   defPresentation,
                        unsigned   defScreening,
                        unsigned   defReason)
{
  number = PString();

  if (bytes.IsEmpty())
    return FALSE;

  if (plan != NULL)
    *plan = bytes[0]&15;

  if (type != NULL)
    *type = (bytes[0]>>4)&7;

  PINDEX offset;
  if ((bytes[0] & 0x80) != 0) {  // Octet 3a not provided, set defaults
    if (presentation != NULL)
      *presentation = defPresentation;

    if (screening != NULL)
      *screening = defScreening;

    offset = 1;
  }
  else {
    if (bytes.GetSize() < 2)
      return FALSE;

    if (presentation != NULL)
      *presentation = (bytes[1]>>5)&3;

    if (screening != NULL)
      *screening = bytes[1]&3;

    if ((bytes[1] & 0x80) != 0) { // Octet 3b not provided, set defaults
      if (reason != NULL)
        *reason = defReason;

      offset = 2;
    }
    else {
      if (bytes.GetSize() < 3)
        return FALSE;

      if (reason != NULL)
        *reason = bytes[2]&15;

      offset = 3;
    }
  }

  if (bytes.GetSize() < offset)
    return FALSE;

  PINDEX len = bytes.GetSize()-offset;

  if (len > 0)
#if PTLIB_VER >= 2110
    memcpy(number.GetPointerAndSetLength(len), ((const BYTE *)bytes)+offset, len);
#else
    memcpy(number.GetPointer(len+1), ((const BYTE *)bytes)+offset, len);
#endif

  return !number;
}


void Q931::SetCallingPartyNumber(const PString & number,
                                 unsigned plan,
                                 unsigned type,
                                 int presentation,
                                 int screening)
{
  SetIE(CallingPartyNumberIE,
        SetNumberIE(number, plan, type, presentation, screening, -1));
}


PBoolean Q931::GetCallingPartyNumber(PString  & number,
                                 unsigned * plan,
                                 unsigned * type,
                                 unsigned * presentation,
                                 unsigned * screening,
                                 unsigned   defPresentation,
                                 unsigned   defScreening) const
{
  return GetNumberIE(GetIE(CallingPartyNumberIE), number,
                     plan, type, presentation, screening, NULL,
                     defPresentation, defScreening, 0);
}

void Q931::SetCallingPartySubAddress(const PString & number, unsigned plan, unsigned type)
{
  SetIE(CallingPartySubAddressIE,
        SetNumberIE(number, plan, type, -1, -1, -1));
}

PBoolean Q931::GetCallingPartySubAddress(PString & number, unsigned * plan, unsigned * type) const
{
  return GetNumberIE(GetIE(CallingPartySubAddressIE),
                     number, plan, type, NULL, NULL, NULL, 0, 0, 0);
}

void Q931::SetCalledPartyNumber(const PString & number, unsigned plan, unsigned type)
{
  SetIE(CalledPartyNumberIE,
        SetNumberIE(number, plan, type, -1, -1, -1));
}


PBoolean Q931::GetCalledPartyNumber(PString & number, unsigned * plan, unsigned * type) const
{
  return GetNumberIE(GetIE(CalledPartyNumberIE),
                     number, plan, type, NULL, NULL, NULL, 0, 0, 0);
}

void Q931::SetCalledPartySubAddress(const PString & number, unsigned plan, unsigned type)
{
  SetIE(CalledPartySubAddressIE,
        SetNumberIE(number, plan, type, -1, -1, -1));
}

PBoolean Q931::GetCalledPartySubAddress(PString & number, unsigned * plan, unsigned * type) const
{
  return GetNumberIE(GetIE(CalledPartySubAddressIE),
                     number, plan, type, NULL, NULL, NULL, 0, 0, 0);
}

void Q931::SetRedirectingNumber(const PString & number,
                                unsigned plan,
                                unsigned type,
                                int presentation,
                                int screening,
                                int reason)
{
  SetIE(RedirectingNumberIE,
        SetNumberIE(number, plan, type, presentation, screening, reason));
}


PBoolean Q931::GetRedirectingNumber(PString  & number,
                                unsigned * plan,
                                unsigned * type,
                                unsigned * presentation,
                                unsigned * screening,
                                unsigned * reason,
                                unsigned   defPresentation,
                                unsigned   defScreening,
                                unsigned   defReason) const
{
  return GetNumberIE(GetIE(RedirectingNumberIE),
                     number, plan, type, presentation, screening, reason,
                     defPresentation, defScreening, defReason);
}


PBoolean Q931::GetConnectedNumber(PString  & number,
                              unsigned * plan,
                              unsigned * type,
                              unsigned * presentation,
                              unsigned * screening,
                              unsigned * reason,
                              unsigned   defPresentation,
                              unsigned   defScreening,
                              unsigned   defReason) const
{
  return GetNumberIE(GetIE(ConnectedNumberIE), number,
                     plan, type, presentation, screening, reason,
                     defPresentation, defScreening, defReason);
}


void Q931::SetConnectedNumber(const PString & number,
                              unsigned plan,
                              unsigned type,
                              int presentation,
                              int screening,
                              int reason)
{
  SetIE(ConnectedNumberIE,
        SetNumberIE(number, plan, type, presentation, screening, reason));
}


void Q931::SetChannelIdentification(unsigned interfaceType,
                                    unsigned preferredOrExclusive,
                                    int      channelNumber)
{
  // Known limitations:
  //  - the interface identifier cannot be specified
  //  - channel in PRI can only be indicated by number and cannot be indicated by map
  //  - one and only one channel can be indicated
  //  - the coding standard is always ITU Q.931

  PBYTEArray bytes;
  bytes.SetSize(1);

  PAssert(interfaceType < 2, PInvalidParameter);

  if (interfaceType == 0) { // basic rate
    if (channelNumber == -1) { // any channel
      bytes[0] = 0x80 | 0x03;
    }
    if (channelNumber == 0) { // D channel
      bytes[0] = 0x80 | 0x04;
    }    
    if (channelNumber > 0) { // B channel
      bytes[0] = (BYTE)(0x80 | ((preferredOrExclusive & 0x01) << 3) | (channelNumber & 0x03));
    }
  }

  if (interfaceType == 1) { // primary rate
    if (channelNumber == -1) { // any channel
      bytes[0] = 0x80 | 0x20 | 0x03;
      bytes[1] = 0x80 | 0x03;      // dummy octet 3.2
      bytes[2] = (BYTE)(0x80 | 1); // dummy octet 3.3
    }
    if (channelNumber == 0) { // D channel
      bytes[0] = 0x80 | 0x04 | 0x20;
    }    
    if (channelNumber > 0) { // B channel
      bytes.SetSize(3);

      bytes[0] = (BYTE)(0x80 | 0x20 | ((preferredOrExclusive & 0x01) << 3) | 0x01);
      bytes[1] = 0x80 | 0x03;
      bytes[2] = (BYTE)(0x80 | channelNumber);
    }
  }

  SetIE(ChannelIdentificationIE, bytes);
}


PBoolean Q931::GetChannelIdentification(unsigned * interfaceType,
                                    unsigned * preferredOrExclusive,
                                    int      * channelNumber) const
{
  if (!HasIE(ChannelIdentificationIE))
    return FALSE;

  PBYTEArray bytes = GetIE(ChannelIdentificationIE);
  if (bytes.GetSize() < 1)
    return FALSE;

  *interfaceType        = (bytes[0]>>5) & 0x01;
  *preferredOrExclusive = (bytes[0]>>3) & 0x01;

  if (*interfaceType == 0) {  // basic rate
    if ( (bytes[0] & 0x04) == 0x04 ) {  // D Channel
      *channelNumber = 0;
    }
    else {
      if ( (bytes[0] & 0x03) == 0x03 ) {  // any channel
        *channelNumber = -1;
      }
      else { // B Channel
        *channelNumber = (bytes[0] & 0x03);
      }
    }
  }

  if (*interfaceType == 1) {  // primary rate
    if ( (bytes[0] & 0x04) == 0x04 ) {  // D Channel
      *channelNumber = 0;
    }
    else {
      if ( (bytes[0] & 0x03) == 0x03 ) {  // any channel
        *channelNumber = -1;
      }
      else { // B Channel
        if (bytes.GetSize() < 3)
          return FALSE;

        if (bytes[1] != 0x83)
          return FALSE;

        *channelNumber = bytes[2] & 0x7f;
      }
    }
  }

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
