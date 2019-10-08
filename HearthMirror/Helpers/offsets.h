//
//  HMOffsets.h
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef offsets_h
#define offsets_h

#include "fakemonostructs.h"

#ifdef __APPLE__

#define kMonoDomainDomainAssemblies offsetof(FakeMonoDomain, domain_assemblies) //0x88
#define kMonoDomainDomainAssemblies64 offsetof(FakeMonoDomain64, domain_assemblies) //0x??

#define kMonoAssemblyName offsetof(FakeMonoAssembly, aname) // 0x8
#define kMonoAssemblyName64 offsetof(FakeMonoAssembly64, aname)

#define kMonoAssemblyImage offsetof(FakeMonoAssembly, image) // 0x40
#define kMonoAssemblyImage64 offsetof(FakeMonoAssembly64, image)

#define kMonoImageClassCache offsetof(FakeMonoImage, class_cache) // 0x2a0
#define kMonoImageClassCache64 offsetof(FakeMonoImage64, class_cache)

#define kMonoInternalHashTableSize offsetof(FakeMonoInternalHashTable, size) // 0xc
#define kMonoInternalHashTableSize64 offsetof(FakeMonoInternalHashTable64, size)

#define kMonoInternalHashTableNumEntries offsetof(FakeMonoInternalHashTable, num_entries) // 0x10
#define kMonoInternalHashTableNumEntries64 offsetof(FakeMonoInternalHashTable64, num_entries)

#define kMonoInternalHashTableTable offsetof(FakeMonoInternalHashTable, table) // 0x14
#define kMonoInternalHashTableTable64 offsetof(FakeMonoInternalHashTable64, table)

#define kMonoClassNextClassCache offsetof(FakeMonoClass, next_class_cache) // 0x98
#define kMonoClassNextClassCache64 offsetof(FakeMonoClass64, next_class_cache)

#define kMonoClassName offsetof(FakeMonoClass, name) // 0x28
#define kMonoClassName64 offsetof(FakeMonoClass64, name)

#define kMonoClassNameSpace offsetof(FakeMonoClass, name_space) //0x2c
#define kMonoClassNameSpace64 offsetof(FakeMonoClass64, name_space)

#define kMonoClassNestedIn offsetof(FakeMonoClass, nested_in) // 0x20
#define kMonoClassNestedIn64 offsetof(FakeMonoClass64, nested_in)

#define kMonoClassRuntimeInfo offsetof(FakeMonoClass, runtime_info)  // 0x94
#define kMonoClassRuntimeInfo64 offsetof(FakeMonoClass64, runtime_info)

#define kMonoClassRuntimeInfoDomainVtables offsetof(FakeMonoClassRuntimeInfo, domain_vtables) // 0x4
#define kMonoClassRuntimeInfoDomainVtables64 offsetof(FakeMonoClassRuntimeInfo64, domain_vtables) 

// bitfield address depends on compiler optimizer, thus they are computed on the fly
//#define kMonoClassBitfields offsetof(FakeMonoClass, inited) // 0x14
//#define kMonoClassBitfields64 offsetof(FakeMonoClass64, inited)

#define kMonoClassSizes offsetof(FakeMonoClass, sizes) // 0x50
#define kMonoClassSizes64 offsetof(FakeMonoClass64, sizes)

#define kMonoClassParent offsetof(FakeMonoClass, parent) // 0x1c
#define kMonoClassParent64 offsetof(FakeMonoClass64, parent)

#define kMonoClassByvalArg offsetof(FakeMonoClass, byval_arg)// 0x7c
#define kMonoClassByvalArg64 offsetof(FakeMonoClass64, byval_arg)

#define kMonoClassFields offsetof(FakeMonoClass, fields)// 0x6c
#define kMonoClassFields64 offsetof(FakeMonoClass64, fields)

#define kMonoClassFieldCount offsetof(FakeMonoClass, field) + offsetof(FakeMonoElement, count) // 0x5c
#define kMonoClassFieldCount64 offsetof(FakeMonoClass64, field) + offsetof(FakeMonoElement64, count)

#define kMonoClassFieldSizeof sizeof(FakeMonoClassField) // 0x10
#define kMonoClassFieldSizeof64 sizeof(FakeMonoClassField64)

#define kMonoClassFieldType offsetof(FakeMonoClassField, type) // 0x0
#define kMonoClassFieldType64 offsetof(FakeMonoClassField64, type)

#define kMonoClassFieldName offsetof(FakeMonoClassField, name) // 0x4
#define kMonoClassFieldName64 offsetof(FakeMonoClassField64, name)

#define kMonoClassFieldParent offsetof(FakeMonoClassField, parent) // 0x8
#define kMonoClassFieldParent64 offsetof(FakeMonoClassField64, parent)

#define kMonoClassFieldOffset offsetof(FakeMonoClassField, offset) // 0xc
#define kMonoClassFieldOffset64 offsetof(FakeMonoClassField64, offset)

#define kMonoTypeData offsetof(FakeMonoType, data)
#define kMonoTypeData64 offsetof(FakeMonoType64, data)

// bitfield address depends on compiler optimizer, thus they are computed on the fly
//#define kMonoTypeAttrs offsetof(FakeMonoType, attrs) //0x4
//#define kMonoTypeAttrs64 offsetof(FakeMonoType64, attrs)

#define kMonoTypeType offsetof(FakeMonoType, type) // 0x6
#define kMonoTypeType64 offsetof(FakeMonoType64, type)

#define kMonoTypeSizeof offsetof(FakeMonoType, modifiers) // 0x8
#define kMonoTypeSizeof64 offsetof(FakeMonoType64, modifiers)

#define kMonoVTableData offsetof(FakeMonoVTable, data) // 0xc
#define kMonoVTableData64 offsetof(FakeMonoVTable64, data)

#define kMonoStringLength offsetof(FakeMonoString, length)
#define kMonoStringLength64 offsetof(FakeMonoString64, length)

#define kMonoStringChars offsetof(FakeMonoString, chars)
#define kMonoStringChars64 offsetof(FakeMonoString64, chars)

#define kMonoArrayMaxLength offsetof(FakeMonoArray, max_length)
#define kMonoArrayMaxLength64 offsetof(FakeMonoArray64, max_length)

#define kMonoArrayVector offsetof(FakeMonoArray, vector)
#define kMonoArrayVector64 offsetof(FakeMonoArray64, vector)

#else

#define kImageDosHeader_e_lfanew 0x3c
#define kImageNTHeadersSignature 0x0
#define kImageNTHeadersMachine 0x4
#define kImageNTHeadersExportDirectoryAddress 0x78
#define kImageExportDirectoryNumberOfFunctions 0x14
#define kImageExportDirectoryAddressOfFunctions 0x1c
#define kImageExportDirectoryAddressOfNames 0x20
#define kMonoDomainSizeof 0x144
#define kMonoDomainDomainAssemblies 0x70
#define kMonoAssemblyName 0x8
#define kMonoAssemblyImage 0x40

#define kMonoImageClassCache 0x2a0
#define kMonoInternalHashTableSize 0xc
#define kMonoInternalHashTableTable 0x14

#define kMonoClassParent 0x24
#define kMonoClassName 0x34
#define kMonoClassNameSpace 0x38
#define kMonoClassNestedIn 0x2c
#define kMonoClassRuntimeInfo 0xa8
#define kMonoClassRuntimeInfoDomainVtables 0x4
#define kMonoClassBitfields  0x14
#define kMonoClassSizes 0x5c
#define kMonoClassByvalArg 0x8c
#define kMonoClassFields 0x78
#define kMonoClassNextClassCache 0xac

#define kMonoClassFieldCount 0x64
#define kMonoClassFieldSizeof 0x10
#define kMonoClassFieldType 0x0
#define kMonoClassFieldName 0x4
#define kMonoClassFieldParent 0x8
#define kMonoClassFieldOffset 0xc

#define kMonoTypeAttrs 0x4
//#define kMonoTypeType 0x6
#define kMonoTypeSizeof 0x8
#define kMonoVTableData 0xc

// 64 bit values are currently unknown

#define kMonoDomainSizeof64 0x144
#define kMonoDomainDomainAssemblies64 0x70
#define kMonoAssemblyName64 0x8
#define kMonoAssemblyImage64 0x40

#define kMonoImageClassCache64 0x2a0
#define kMonoInternalHashTableSize64 0xc
#define kMonoInternalHashTableTable64 0x14

#define kMonoClassParent64 0x24
#define kMonoClassName64 0x34
#define kMonoClassNameSpace64 0x38
#define kMonoClassNestedIn64 0x2c
#define kMonoClassRuntimeInfo64 0xa8
#define kMonoClassRuntimeInfoDomainVtables64 0x4
#define kMonoClassBitfields64  0x14
#define kMonoClassSizes64 0x5c
#define kMonoClassByvalArg64 0x8c
#define kMonoClassFields64 0x78
#define kMonoClassNextClassCache64 0xac

#define kMonoClassFieldCount64 0x64
#define kMonoClassFieldSizeof64 0x10
#define kMonoClassFieldType64 0x0
#define kMonoClassFieldName64 0x4
#define kMonoClassFieldParent64 0x8
#define kMonoClassFieldOffset64 0xc

#define kMonoStringChars 0xC
#define kMonoStringChars64 0xC

#define kMonoStringLength 0x8
#define kMonoStringLength64 0x8

#define kMonoArrayMaxLength 0xC
#define kMonoArrayMaxLength64 0xC

#define kMonoArrayVector 0x10
#define kMonoArrayVector64 0x10

#define kMonoVTableData64 0xc

#endif // APPLE

#endif /* offsets_h */
