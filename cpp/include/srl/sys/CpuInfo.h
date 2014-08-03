/*********************************************************************************
* Copyright (C) 1999,2006 by Ian C. Starnes   All Rights Reserved.        
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*******************************************************************************/

#ifndef __SRL_CPU_INFO__
#define __SRL_CPU_INFO__

#include "srl/sys/System.h"
#include "srl/Singleton.h"
#include "srl/util/Vector.h"

namespace SRL
{
    namespace System
    {
        /** cpuid helper functions */
        namespace CpuInfo
        {
            extern SRL_API bool SupportsCpuid();
            extern SRL_API bool SupportsMultiThreading();
            extern SRL_API bool IsIntel();
            extern SRL_API bool IsAmd();
            
            extern SRL_API String GetVendorId();
            extern SRL_API String GetModelName();
            extern SRL_API String GetSignature();
            
            extern SRL_API uint32 LogicalProcessorsPerPackage();
            extern SRL_API uint32 CoresPerPackage();
            
            extern SRL_API uint32 NumberOfLogicalProcessors();
            extern SRL_API uint32 NumberOfPhysicalProcessors();            
            
            extern SRL_API SRL::byte GetActiveApicId();
            extern SRL_API SRL::byte GetActivePackageId(SRL::byte &apic_id, uint32 &logical_per_package);
            extern SRL_API SRL::byte GetActiveCoreId(SRL::byte &apic_id, uint32 &logical_per_package, uint32 &max_logical_per_core);
            extern SRL_API SRL::byte GetActiveSmtId(SRL::byte &apic_id, uint32 &max_logical_per_core);
        }
    }   
}

#endif

