///////////////////////////////////////////////////////////////////////////////
//
// File           : $Workfile: ManualResetEvent.cpp $
// Version        : $Revision: 3 $
// Function       :
//
// Author         : $Author: Len $
// Date           : $Date: 20/05/02 23:17 $
//
// Notes          :
//
// Modifications  :
//
// $Log: /Clients/PayPoint/e-Voucher/JetByteTools/Win32Tools/ManualResetEvent.cpp $
//
// 3     20/05/02 23:17 Len
// Updated copyright and disclaimers.
//
// 2     10/05/02 19:25 Len
// Lint options and code cleaning.
//
// 1     9/05/02 18:47 Len
//
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1997 - 2002 JetByte Limited.
//
// JetByte Limited grants you ("Licensee") a non-exclusive, royalty free,
// licence to use, modify and redistribute this software in source and binary
// code form, provided that i) this copyright notice and licence appear on all
// copies of the software; and ii) Licensee does not utilize the software in a
// manner which is disparaging to JetByte Limited.
//
// This software is provided "as is" without a warranty of any kind. All
// express or implied conditions, representations and warranties, including
// any implied warranty of merchantability, fitness for a particular purpose
// or non-infringement, are hereby excluded. JetByte Limited and its licensors
// shall not be liable for any damages suffered by licensee as a result of
// using, modifying or distributing the software or its derivatives. In no
// event will JetByte Limited be liable for any lost revenue, profit or data,
// or for direct, indirect, special, consequential, incidental or punitive
// damages, however caused and regardless of the theory of liability, arising
// out of the use of or inability to use software, even if JetByte Limited
// has been advised of the possibility of such damages.
//
// This software is not designed or intended for use in on-line control of
// aircraft, air traffic, aircraft navigation or aircraft communications; or in
// the design, construction, operation or maintenance of any nuclear
// facility. Licensee represents and warrants that it will not use or
// redistribute the Software for such purposes.
//
///////////////////////////////////////////////////////////////////////////////

#include "ManualResetEvent.h"

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Member not defined
//lint -esym(1526, CManualResetEvent::CManualResetEvent)
//lint -esym(1526, CManualResetEvent::operator=)
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools
{
namespace Win32
{

///////////////////////////////////////////////////////////////////////////////
// CManualResetEvent
///////////////////////////////////////////////////////////////////////////////

CManualResetEvent::CManualResetEvent(
    bool initialState /* = false */)
    :  CEvent(0, true, initialState)
{

}

CManualResetEvent::CManualResetEvent(
    const _tstring &name,
    bool initialState /* = false */)
    :  CEvent(0, true, initialState, name)
{

}

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Win32
} // End of namespace JetByteTools

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////

