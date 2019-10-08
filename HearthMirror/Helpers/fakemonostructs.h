//
//  fakemonostructs.h
//  HearthMirror
//
//  Created by Istvan Fehervari on 2017-10-24.
//  Copyright © 2017 com.ifehervari. All rights reserved.
//

#ifndef fakemonostructs_h
#define fakemonostructs_h

typedef enum {
    MONO_TYPE_END        = 0x00,
    MONO_TYPE_VOID       = 0x01,
    MONO_TYPE_BOOLEAN    = 0x02,
    MONO_TYPE_CHAR       = 0x03,
    MONO_TYPE_I1         = 0x04,
    MONO_TYPE_U1         = 0x05,
    MONO_TYPE_I2         = 0x06,
    MONO_TYPE_U2         = 0x07,
    MONO_TYPE_I4         = 0x08,
    MONO_TYPE_U4         = 0x09,
    MONO_TYPE_I8         = 0x0a,
    MONO_TYPE_U8         = 0x0b,
    MONO_TYPE_R4         = 0x0c,
    MONO_TYPE_R8         = 0x0d,
    MONO_TYPE_STRING     = 0x0e,
    MONO_TYPE_PTR        = 0x0f,
    MONO_TYPE_BYREF      = 0x10,
    MONO_TYPE_VALUETYPE  = 0x11,
    MONO_TYPE_CLASS      = 0x12,
    MONO_TYPE_VAR         = 0x13,
    MONO_TYPE_ARRAY      = 0x14,
    MONO_TYPE_GENERICINST= 0x15,
    MONO_TYPE_TYPEDBYREF = 0x16,
    MONO_TYPE_I          = 0x18,
    MONO_TYPE_U          = 0x19,
    MONO_TYPE_FNPTR      = 0x1b,
    MONO_TYPE_OBJECT     = 0x1c,
    MONO_TYPE_SZARRAY    = 0x1d,
    MONO_TYPE_MVAR         = 0x1e,
    MONO_TYPE_CMOD_REQD  = 0x1f,
    MONO_TYPE_CMOD_OPT   = 0x20,
    MONO_TYPE_INTERNAL   = 0x21,
    
    MONO_TYPE_MODIFIER   = 0x40,
    MONO_TYPE_SENTINEL   = 0x41,
    MONO_TYPE_PINNED     = 0x45,
    
    MONO_TYPE_ENUM       = 0x55
} MonoTypeEnum;

#ifdef __APPLE__

#include <pthread.h>

typedef uint32_t ptr32_t;
typedef uint64_t ptr64_t;

typedef enum {
    MONO_TABLE_MODULE,
    MONO_TABLE_TYPEREF,
    MONO_TABLE_TYPEDEF,
    MONO_TABLE_FIELD_POINTER,
    MONO_TABLE_FIELD,
    MONO_TABLE_METHOD_POINTER,
    MONO_TABLE_METHOD,
    MONO_TABLE_PARAM_POINTER,
    MONO_TABLE_PARAM,
    MONO_TABLE_INTERFACEIMPL,
    MONO_TABLE_MEMBERREF,
    MONO_TABLE_CONSTANT,
    MONO_TABLE_CUSTOMATTRIBUTE,
    MONO_TABLE_FIELDMARSHAL,
    MONO_TABLE_DECLSECURITY,
    MONO_TABLE_CLASSLAYOUT,
    MONO_TABLE_FIELDLAYOUT,
    MONO_TABLE_STANDALONESIG,
    MONO_TABLE_EVENTMAP,
    MONO_TABLE_EVENT_POINTER,
    MONO_TABLE_EVENT,
    MONO_TABLE_PROPERTYMAP,
    MONO_TABLE_PROPERTY_POINTER,
    MONO_TABLE_PROPERTY,
    MONO_TABLE_METHODSEMANTICS,
    MONO_TABLE_METHODIMPL,
    MONO_TABLE_MODULEREF,
    MONO_TABLE_TYPESPEC,
    MONO_TABLE_IMPLMAP,
    MONO_TABLE_FIELDRVA,
    MONO_TABLE_UNUSED6,
    MONO_TABLE_UNUSED7,
    MONO_TABLE_ASSEMBLY,
    MONO_TABLE_ASSEMBLYPROCESSOR,
    MONO_TABLE_ASSEMBLYOS,
    MONO_TABLE_ASSEMBLYREF,
    MONO_TABLE_ASSEMBLYREFPROCESSOR,
    MONO_TABLE_ASSEMBLYREFOS,
    MONO_TABLE_FILE,
    MONO_TABLE_EXPORTEDTYPE,
    MONO_TABLE_MANIFESTRESOURCE,
    MONO_TABLE_NESTEDCLASS,
    MONO_TABLE_GENERICPARAM,
    MONO_TABLE_METHODSPEC,
    MONO_TABLE_GENERICPARAMCONSTRAINT
    
#define MONO_TABLE_LAST MONO_TABLE_GENERICPARAMCONSTRAINT
#define MONO_TABLE_NUM (MONO_TABLE_LAST + 1)
    
} FakeMonoMetaTableEnum;

struct FakeMonoTableInfo {
    const ptr32_t base;
    unsigned int       rows     : 24;
    unsigned int       row_size : 8;
    uint32_t   size_bitfield;
};

struct FakeMonoTableInfo64 {
    const ptr64_t base;
    unsigned int       rows     : 24;
    unsigned int       row_size : 8;
    uint32_t   size_bitfield;
};

struct FakeMonoStreamHeader {
    const ptr32_t data;
    uint32_t  size;
};

struct FakeMonoStreamHeader64 {
    const ptr64_t data;
    uint32_t  size;
};

struct FakeMonoInternalHashTable
{
    ptr32_t hash_func;
    ptr32_t key_extract;
    ptr32_t next_value;
    int size;
    int num_entries;
    ptr32_t table;
};

struct FakeMonoInternalHashTable64
{
    ptr64_t hash_func;
    ptr64_t key_extract;
    ptr64_t next_value;
    int size;
    int num_entries;
    ptr64_t table;
};

typedef struct _WapiCriticalSection
{
    uint32_t depth;
    pthread_mutex_t mutex;
} CRITICAL_SECTION;

//typedef mono_mutex_t CRITICAL_SECTION;

struct FakeMonoImage {
    int   ref_count;
    ptr32_t raw_data_handle;
    ptr32_t raw_data;
    uint32_t raw_data_len;
    uint8_t raw_buffer_used    : 1;
    uint8_t raw_data_allocated : 1;
    
#ifdef USE_COREE
    uint8_t is_module_handle : 1;
    uint8_t has_entry_point : 1;
#endif
    uint8_t dynamic : 1;
    uint8_t ref_only : 1;
    uint8_t uncompressed_metadata : 1;
    uint8_t checked_module_cctor : 1;
    uint8_t has_module_cctor : 1;
    uint8_t idx_string_wide : 1;
    uint8_t idx_guid_wide : 1;
    uint8_t idx_blob_wide : 1;
    uint8_t core_clr_platform_code : 1;
    ptr32_t name;
    const ptr32_t assembly_name;
    const ptr32_t module_name;
    ptr32_t version;
    int16_t md_version_major, md_version_minor;
    ptr32_t guid;
    ptr32_t image_info;
    ptr32_t mempool;
    ptr32_t raw_metadata;
    FakeMonoStreamHeader     heap_strings;
    FakeMonoStreamHeader     heap_us;
    FakeMonoStreamHeader     heap_blob;
    FakeMonoStreamHeader     heap_guid;
    FakeMonoStreamHeader     heap_tables;
    const ptr32_t tables_base;
    FakeMonoTableInfo        tables [MONO_TABLE_LAST + 1];
    ptr32_t references;
    ptr32_t modules;
    uint32_t module_count;
    ptr32_t modules_loaded;
    ptr32_t files;
    ptr32_t aot_module;
    ptr32_t assembly;
    ptr32_t method_cache;
    FakeMonoInternalHashTable class_cache;
    ptr32_t methodref_cache;
    ptr32_t field_cache;
    ptr32_t typespec_cache;
    ptr32_t memberref_signatures;
    ptr32_t helper_signatures;
    ptr32_t method_signatures;
    ptr32_t name_cache;
    ptr32_t array_cache;
    ptr32_t ptr_cache;
    
    ptr32_t szarray_cache;
    CRITICAL_SECTION szarray_cache_lock;
    ptr32_t delegate_begin_invoke_cache;
    ptr32_t delegate_end_invoke_cache;
    ptr32_t delegate_invoke_cache;
    ptr32_t runtime_invoke_cache;
    ptr32_t delegate_abstract_invoke_cache;
    ptr32_t runtime_invoke_direct_cache;
    ptr32_t runtime_invoke_vcall_cache;
    ptr32_t managed_wrapper_cache;
    ptr32_t native_wrapper_cache;
    ptr32_t native_wrapper_aot_cache;
    ptr32_t remoting_invoke_cache;
    ptr32_t synchronized_cache;
    ptr32_t unbox_wrapper_cache;
    ptr32_t cominterop_invoke_cache;
    ptr32_t cominterop_wrapper_cache;
    ptr32_t thunk_invoke_cache;
    ptr32_t ldfld_wrapper_cache;
    ptr32_t ldflda_wrapper_cache;
    ptr32_t stfld_wrapper_cache;
    ptr32_t isinst_cache;
    ptr32_t castclass_cache;
    ptr32_t proxy_isinst_cache;
    ptr32_t rgctx_template_hash;
    ptr32_t generic_class_cache;
    ptr32_t property_hash;
    ptr32_t reflection_info;
    ptr32_t user_info;
    ptr32_t dll_map;
    ptr32_t interface_bitset;
    ptr32_t reflection_info_unregister_classes;
    CRITICAL_SECTION    lock;
};

struct FakeMonoImage64 {
    int   ref_count;
    ptr64_t raw_data_handle;
    ptr64_t raw_data;
    uint32_t raw_data_len;
    uint8_t raw_buffer_used    : 1;
    uint8_t raw_data_allocated : 1;
    
#ifdef USE_COREE
    uint8_t is_module_handle : 1;
    uint8_t has_entry_point : 1;
#endif
    uint8_t dynamic : 1;
    uint8_t ref_only : 1;
    uint8_t uncompressed_metadata : 1;
    uint8_t checked_module_cctor : 1;
    uint8_t has_module_cctor : 1;
    uint8_t idx_string_wide : 1;
    uint8_t idx_guid_wide : 1;
    uint8_t idx_blob_wide : 1;
    uint8_t core_clr_platform_code : 1;
    
    ptr64_t name;
    const ptr64_t assembly_name;
    const ptr64_t module_name;
    ptr64_t version;
    int16_t md_version_major, md_version_minor;
    ptr64_t guid; // char*
    ptr64_t image_info;
    ptr64_t mempool;
    ptr64_t raw_metadata; // char*
    FakeMonoStreamHeader64     heap_strings;
    FakeMonoStreamHeader64     heap_us;
    FakeMonoStreamHeader64     heap_blob;
    FakeMonoStreamHeader64     heap_guid;
    FakeMonoStreamHeader64     heap_tables;
    const ptr64_t tables_base;
    FakeMonoTableInfo64        tables [MONO_TABLE_LAST + 1];
    ptr64_t references;
    ptr64_t modules;
    uint32_t module_count;
    ptr64_t modules_loaded;
    ptr64_t files;
    ptr64_t aot_module;
    ptr64_t assembly;
    ptr64_t method_cache;
    FakeMonoInternalHashTable64 class_cache;
    ptr64_t methodref_cache;
    ptr64_t field_cache;
    ptr64_t typespec_cache;
    ptr64_t memberref_signatures;
    ptr64_t helper_signatures;
    ptr64_t method_signatures;
    ptr64_t name_cache;
    ptr64_t array_cache;
    ptr64_t ptr_cache;
    
    ptr64_t szarray_cache;
    CRITICAL_SECTION szarray_cache_lock;
    ptr64_t delegate_begin_invoke_cache;
    ptr64_t delegate_end_invoke_cache;
    ptr64_t delegate_invoke_cache;
    ptr64_t runtime_invoke_cache;
    ptr64_t delegate_abstract_invoke_cache;
    ptr64_t runtime_invoke_direct_cache;
    ptr64_t runtime_invoke_vcall_cache;
    ptr64_t managed_wrapper_cache;
    ptr64_t native_wrapper_cache;
    ptr64_t native_wrapper_aot_cache;
    ptr64_t remoting_invoke_cache;
    ptr64_t synchronized_cache;
    ptr64_t unbox_wrapper_cache;
    ptr64_t cominterop_invoke_cache;
    ptr64_t cominterop_wrapper_cache;
    ptr64_t thunk_invoke_cache;
    ptr64_t ldfld_wrapper_cache;
    ptr64_t ldflda_wrapper_cache;
    ptr64_t stfld_wrapper_cache;
    ptr64_t isinst_cache;
    ptr64_t castclass_cache;
    ptr64_t proxy_isinst_cache;
    ptr64_t rgctx_template_hash;
    ptr64_t generic_class_cache;
    ptr64_t property_hash;
    ptr64_t reflection_info;
    ptr64_t user_info;
    ptr64_t dll_map;
    ptr64_t interface_bitset;
    ptr64_t reflection_info_unregister_classes;
    CRITICAL_SECTION    lock;
};

typedef struct {
    unsigned int required : 1;
    unsigned int token    : 31;
} FakeMonoCustomMod;

#ifndef MONO_ZERO_LEN_ARRAY
#ifdef __GNUC__
#define MONO_ZERO_LEN_ARRAY 0
#else
#define MONO_ZERO_LEN_ARRAY 1
#endif
#endif

typedef struct {
    unsigned int required : 1;
    unsigned int token    : 31;
} MonoCustomMod;

#if defined(_MSC_VER)
#   define USE_UINT8_BIT_FIELD(type, field) uint8 field
#else
#   define USE_UINT8_BIT_FIELD(type, field) type field
#endif

struct FakeMonoClass;

struct FakeMonoType {
    union {
        ptr32_t klass; // for VALUETYPE and CLASS
        ptr32_t type;   // for PTR
        ptr32_t array; // for ARRAY
        ptr32_t method;
        ptr32_t generic_param; // for VAR and MVAR
        ptr32_t generic_class; // for GENERICINST
    } data;
    uint32_t attrs    : 16;
    MonoTypeEnum type     : 8;
    unsigned int num_mods : 6;
    unsigned int byref    : 1;
    unsigned int pinned   : 1;
    MonoCustomMod modifiers [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoType64 {
    union {
        ptr64_t klass;
        ptr64_t type;
        ptr64_t array;
        ptr64_t method;
        ptr64_t generic_param;
        ptr64_t generic_class;
    } data;
    uint32_t attrs    : 16;
    MonoTypeEnum type     : 8;
    unsigned int num_mods : 6;
    unsigned int byref    : 1;
    unsigned int pinned   : 1;
    MonoCustomMod modifiers [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoVTable {
    ptr32_t klass;
    ptr32_t gc_descr;
    ptr32_t domain;
    ptr32_t    data;
    ptr32_t    type;
    ptr32_t interface_bitmap;
    uint16_t     max_interface_id;
    uint8_t      rank;
    USE_UINT8_BIT_FIELD(unsigned int, remote      : 1);
    USE_UINT8_BIT_FIELD(unsigned int, initialized : 1);
    USE_UINT8_BIT_FIELD(unsigned int, init_failed : 1);
    uint32_t     imt_collisions_bitmap;
    ptr32_t runtime_generic_context;
    ptr32_t    vtable [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoVTable64 {
    ptr64_t klass;
    ptr64_t gc_descr;
    ptr64_t domain;
    ptr64_t    data;
    ptr64_t    type;
    ptr64_t interface_bitmap;
    uint16_t     max_interface_id;
    uint8_t      rank;
    USE_UINT8_BIT_FIELD(unsigned int, remote      : 1);
    USE_UINT8_BIT_FIELD(unsigned int, initialized : 1);
    USE_UINT8_BIT_FIELD(unsigned int, init_failed : 1);
    uint32_t     imt_collisions_bitmap;
    ptr64_t runtime_generic_context;
    ptr64_t    vtable [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoClassRuntimeInfo {
    uint16_t max_domain;
    FakeMonoVTable *domain_vtables [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoClassRuntimeInfo64 {
    uint16_t max_domain;
    FakeMonoVTable64 *domain_vtables [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoElement {
    uint32_t first, count;
};

struct FakeMonoElement64 {
    uint32_t first, count;
};

struct FakeMonoClass {
    ptr32_t element_class;
    ptr32_t cast_class;
    ptr32_t supertypes;
    uint16_t     idepth;
    uint8_t     rank;
    int        instance_size;
    USE_UINT8_BIT_FIELD(uint32_t, inited          : 1);
    USE_UINT8_BIT_FIELD(uint32_t, init_pending    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, size_inited     : 1);
    USE_UINT8_BIT_FIELD(uint32_t, valuetype       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, enumtype        : 1);
    USE_UINT8_BIT_FIELD(uint32_t, blittable       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, unicode         : 1);
    USE_UINT8_BIT_FIELD(uint32_t, wastypebuilder  : 1);
    uint8_t min_align;
    USE_UINT8_BIT_FIELD(uint32_t, packing_size    : 4);
    USE_UINT8_BIT_FIELD(uint32_t, ghcimpl         : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_finalize    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, marshalbyref    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, contextbound    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, delegate        : 1);
    USE_UINT8_BIT_FIELD(uint32_t, gc_descr_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_cctor       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_references  : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_static_refs : 1);
    USE_UINT8_BIT_FIELD(uint32_t, no_special_static_fields : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_com_object   : 1);
    USE_UINT8_BIT_FIELD(uint32_t, nested_classes_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, interfaces_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, simd_type       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_generic      : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_inflated     : 1);
    
    uint8_t     exception_type;
    
    ptr32_t  parent;
    ptr32_t  nested_in;
    
    ptr32_t cattrs;
    
    ptr32_t image;
    ptr32_t name;
    ptr32_t name_space;
    
    uint32_t    type_token;
    int        vtable_size;
    
    uint16_t      interface_count;
    uint16_t     interface_id;
    uint16_t     max_interface_id;
    
    uint16_t     interface_offsets_count;
    ptr32_t interfaces_packed;
    ptr32_t interface_offsets_packed;
    ptr32_t interface_bitmap;
    
    ptr32_t interfaces;
    
    union {
        int class_size;
        int element_size;
        int generic_param_token;
    } sizes;
    
    uint32_t    flags;
    struct {
        uint32_t first, count;
    } field, method;
    
    ptr32_t marshal_info;
    
    ptr32_t fields;
    
    ptr32_t methods;
    
    FakeMonoType this_arg;
    FakeMonoType byval_arg;
    
    ptr32_t generic_class;
    ptr32_t generic_container;
    
    ptr32_t reflection_info;
    
    ptr32_t gc_descr;
    
    ptr32_t runtime_info;
    
    ptr32_t next_class_cache;
    ptr32_t vtable;
    ptr32_t ext;
    
    ptr32_t user_data;
};

struct FakeMonoClass64 {
    ptr64_t element_class;
    ptr64_t cast_class;
    ptr64_t supertypes;
    uint16_t     idepth;
    uint8_t     rank;
    int        instance_size;
    USE_UINT8_BIT_FIELD(uint32_t, inited          : 1);
    USE_UINT8_BIT_FIELD(uint32_t, init_pending    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, size_inited     : 1);
    USE_UINT8_BIT_FIELD(uint32_t, valuetype       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, enumtype        : 1);
    USE_UINT8_BIT_FIELD(uint32_t, blittable       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, unicode         : 1);
    USE_UINT8_BIT_FIELD(uint32_t, wastypebuilder  : 1);
    uint8_t min_align;
    USE_UINT8_BIT_FIELD(uint32_t, packing_size    : 4);
    USE_UINT8_BIT_FIELD(uint32_t, ghcimpl         : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_finalize    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, marshalbyref    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, contextbound    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, delegate        : 1);
    USE_UINT8_BIT_FIELD(uint32_t, gc_descr_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_cctor       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_references  : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_static_refs : 1);
    USE_UINT8_BIT_FIELD(uint32_t, no_special_static_fields : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_com_object   : 1);
    USE_UINT8_BIT_FIELD(uint32_t, nested_classes_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, interfaces_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, simd_type       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_generic      : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_inflated     : 1);
    
    uint8_t     exception_type;
    
    ptr64_t  parent;
    ptr64_t  nested_in;
    
    ptr64_t  cattrs;

    ptr64_t image;
    const ptr64_t name; // char*
    const ptr64_t name_space; // char*
    
    uint32_t    type_token;
    int        vtable_size;
    
    uint16_t      interface_count;
    uint16_t     interface_id;
    uint16_t     max_interface_id;
    
    uint16_t     interface_offsets_count;
    ptr64_t interfaces_packed;
    ptr64_t interface_offsets_packed;
    ptr64_t interface_bitmap;
    
    ptr64_t interfaces;
    
    union {
        int class_size;
        int element_size;
        int generic_param_token;
    } sizes;
    
    uint32_t    flags;
    struct {
        uint32_t first, count;
    } field, method;
    
    ptr64_t marshal_info;
    
    ptr64_t fields;
    
    ptr64_t methods;
    
    FakeMonoType64 this_arg;
    FakeMonoType64 byval_arg;
    
    ptr64_t generic_class;
    ptr64_t generic_container;
    
    ptr64_t reflection_info;
    
    ptr64_t gc_descr;
    
    ptr64_t runtime_info;
    
    ptr64_t next_class_cache;
    ptr64_t vtable;
    ptr64_t ext;
    
    ptr64_t user_data;
};

struct FakeMonoClassField {
    ptr32_t type; // MonoType*
    ptr32_t name; // char*
    ptr32_t parent; // MonoClass*
    int     offset;
};

struct FakeMonoClassField64 {
    ptr64_t type;
    ptr64_t name;
    ptr64_t parent;
    int     offset;
};

#define MONO_PUBLIC_KEY_TOKEN_LENGTH    17

struct FakeMonoAssemblyName {
    const ptr32_t name;
    const ptr32_t culture;
    const ptr32_t hash_value;
    const ptr32_t public_key;
    unsigned char public_key_token [MONO_PUBLIC_KEY_TOKEN_LENGTH];
    uint32_t hash_alg;
    uint32_t hash_len;
    uint32_t flags;
    uint16_t major, minor, build, revision;
};

struct FakeMonoAssemblyName64 {
    const ptr64_t name;
    const ptr64_t culture;
    const ptr64_t hash_value;
    const ptr64_t public_key;
    unsigned char public_key_token [MONO_PUBLIC_KEY_TOKEN_LENGTH];
    uint32_t hash_alg;
    uint32_t hash_len;
    uint32_t flags;
    uint16_t major, minor, build, revision;
};

struct FakeMonoAssembly {
    int ref_count;
    ptr32_t basedir;
    FakeMonoAssemblyName aname;
    ptr32_t image;
    ptr32_t friend_assembly_names;
    uint8_t friend_assembly_names_inited;
    uint8_t in_gac;
    uint8_t dynamic;
    uint8_t corlib_internal;
    int ref_only;
    uint32_t ecma:2;
    uint32_t aptc:2;
    uint32_t fulltrust:2;
    uint32_t unmanaged:2;
    uint32_t skipverification:2;
};

struct FakeMonoAssembly64 {
    int ref_count;
    ptr64_t basedir;
    FakeMonoAssemblyName64 aname;
    ptr64_t image;
    ptr64_t friend_assembly_names;
    uint8_t friend_assembly_names_inited;
    uint8_t in_gac;
    uint8_t dynamic;
    uint8_t corlib_internal;
    int ref_only;
    uint32_t ecma:2;
    uint32_t aptc:2;
    uint32_t fulltrust:2;
    uint32_t unmanaged:2;
    uint32_t skipverification:2;
};

struct FakeMonoDomain {
    CRITICAL_SECTION    lock;
    ptr32_t             mp;
    ptr32_t             code_mp;
    
#define MONO_DOMAIN_FIRST_OBJECT setup
    ptr32_t             setup;
    ptr32_t             domain;
    ptr32_t             default_context;
    ptr32_t             out_of_memory_ex;
    ptr32_t             null_reference_ex;
    ptr32_t             stack_overflow_ex;
    ptr32_t             divide_by_zero_ex;
    
    ptr32_t             typeof_void;
    ptr32_t             empty_string;
    
#define MONO_DOMAIN_FIRST_GC_TRACKED env
    ptr32_t             env;
    ptr32_t             ldstr_table;
    ptr32_t             type_hash;
    ptr32_t             refobject_hash;
    ptr32_t             static_data_array;
    ptr32_t             type_init_exception_hash;
    ptr32_t             delegate_hash_table;
#define MONO_DOMAIN_LAST_GC_TRACKED delegate_hash_table
    uint32_t             state;
    int32_t              domain_id;
    int32_t              shadow_serial;
    ptr32_t             domain_assemblies;
    ptr32_t             entry_assembly;
    ptr32_t             friendly_name;
    ptr32_t             class_vtable_hash;
    ptr32_t             proxy_vtable_hash;
    FakeMonoInternalHashTable jit_code_hash;
    CRITICAL_SECTION    jit_code_hash_lock;
    int                 num_jit_info_tables;
    ptr32_t
    volatile            jit_info_table;
    ptr32_t             jit_info_free_queue;
    ptr32_t             search_path;
    ptr32_t             private_bin_path;
    ptr32_t             create_proxy_for_type_method;
    ptr32_t             private_invoke_method;
    ptr32_t             special_static_fields;
    ptr32_t             finalizable_objects_hash;
#ifndef HAVE_SGEN_GC
    ptr32_t             track_resurrection_objects_hash;
    ptr32_t             track_resurrection_handles_hash;
#endif
    CRITICAL_SECTION    finalizable_objects_hash_lock;
    CRITICAL_SECTION    assemblies_lock;
    
    ptr32_t             method_rgctx_hash;
    
    ptr32_t             generic_virtual_cases;
    ptr32_t             thunk_free_lists;
    ptr32_t             class_custom_attributes;
    ptr32_t            runtime_info;
    int                 threadpool_jobs;
    ptr32_t             cleanup_semaphore;
    ptr32_t            finalize_runtime_invoke;
    ptr32_t            capture_context_runtime_invoke;
    ptr32_t            capture_context_method;
    ptr32_t             socket_assembly;
    ptr32_t             sockaddr_class;
    ptr32_t             sockaddr_data_field;
    ptr32_t             static_data_class_array;
};

struct FakeMonoDomain64 {
    CRITICAL_SECTION    lock;
    ptr64_t             mp;
    ptr64_t             code_mp;
    
#define MONO_DOMAIN_FIRST_OBJECT setup
    ptr64_t             setup;
    ptr64_t             domain;
    ptr64_t             default_context;
    ptr64_t             out_of_memory_ex;
    ptr64_t             null_reference_ex;
    ptr64_t             stack_overflow_ex;
    ptr64_t             divide_by_zero_ex;
    
    ptr64_t             typeof_void;
    ptr64_t             empty_string;
    
#define MONO_DOMAIN_FIRST_GC_TRACKED env
    ptr64_t             env;
    ptr64_t             ldstr_table;
    ptr64_t             type_hash;
    ptr64_t             refobject_hash;
    ptr64_t             static_data_array;
    ptr64_t             type_init_exception_hash;
    ptr64_t             delegate_hash_table;
#define MONO_DOMAIN_LAST_GC_TRACKED delegate_hash_table
    uint32_t            state;
    int32_t             domain_id;
    int32_t             shadow_serial;
    ptr64_t             domain_assemblies;
    ptr64_t             entry_assembly;
    ptr64_t             friendly_name;
    ptr64_t             class_vtable_hash;
    ptr64_t             proxy_vtable_hash;
    FakeMonoInternalHashTable64 jit_code_hash;
    CRITICAL_SECTION    jit_code_hash_lock;
    int                 num_jit_info_tables;
    ptr64_t
    volatile            jit_info_table;
    ptr64_t             jit_info_free_queue;
    ptr64_t             search_path;
    ptr64_t             private_bin_path;
    ptr64_t             create_proxy_for_type_method;
    ptr64_t             private_invoke_method;
    ptr64_t             special_static_fields;
    ptr64_t             finalizable_objects_hash;
#ifndef HAVE_SGEN_GC
    ptr64_t             track_resurrection_objects_hash;
    ptr64_t             track_resurrection_handles_hash;
#endif
    CRITICAL_SECTION    finalizable_objects_hash_lock;
    CRITICAL_SECTION    assemblies_lock;
    
    ptr64_t             method_rgctx_hash;
    
    ptr64_t             generic_virtual_cases;
    ptr64_t             thunk_free_lists;
    ptr64_t             class_custom_attributes;
    ptr64_t             runtime_info;
    int                 threadpool_jobs;
    ptr64_t             cleanup_semaphore; // !!!! Inaccurate, it was HANDLE
    ptr64_t             finalize_runtime_invoke;
    ptr64_t             capture_context_runtime_invoke;
    ptr64_t             capture_context_method;
    ptr64_t             socket_assembly;
    ptr64_t             sockaddr_class;
    ptr64_t             sockaddr_data_field;
    ptr64_t             static_data_class_array;
};

struct FakeMonoObject {
    ptr32_t vtable;
    ptr32_t synchronisation;
};

struct FakeMonoObject64 {
    ptr64_t vtable;
    ptr64_t synchronisation;
};

struct FakeMonoString {
    FakeMonoObject object;
    int32_t length;
    uint16_t chars [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoString64 {
    FakeMonoObject64 object;
    int32_t length;
    uint16_t chars [MONO_ZERO_LEN_ARRAY];
};

#ifdef MONO_BIG_ARRAYS
typedef uint64_t mono_array_size_t;
typedef uint64_t mono_array_lower_bound_t;
#define MONO_ARRAY_MAX_INDEX G_MAXINT64
#define MONO_ARRAY_MAX_SIZE  G_MAXUINT64
#else
typedef uint32_t mono_array_size_t;
typedef uint32_t mono_array_lower_bound_t;
#define MONO_ARRAY_MAX_INDEX ((gint32) 0x7fffffff)
#define MONO_ARRAY_MAX_SIZE  ((guint32) 0xffffffff)
#endif

struct FakeMonoArray {
    FakeMonoObject obj;
    /* bounds is NULL for szarrays */
    ptr32_t bounds;
    /* total number of elements of the array */
    mono_array_size_t max_length;
    /* we use double to ensure proper alignment on platforms that need it */
    double vector [MONO_ZERO_LEN_ARRAY];
};

struct FakeMonoArray64 {
    FakeMonoObject64 obj;
    /* bounds is NULL for szarrays */
    ptr64_t bounds;
    /* total number of elements of the array */
    mono_array_size_t max_length;
    /* we use double to ensure proper alignment on platforms that need it */
    double vector [MONO_ZERO_LEN_ARRAY];
};

#endif // __APPLE__

#endif /* fakemonostructs_h */
