#include <sdk/types.h>

#include "jansson/jansson_api.h"

const jansson_api_t jansson_api = {
	.sos_api = {
		.name = "jansson",
		.version = 0x0001,
		.git_hash = SOS_GIT_HASH,
	},
	.create_object = json_object,
	.create_array = json_array,
	.create_string = json_string,
	.create_stringn = json_stringn,
	.create_string_nocheck = json_string_nocheck,
	.create_stringn_nocheck = json_stringn_nocheck,
	.create_integer = json_integer,
	.create_real = json_real,
	.create_true = json_true,
	.create_false = json_false,
	.create_null = json_null,
	.remove = json_delete,
	.object_seed = json_object_seed,
	.object_size = json_object_size,
	.object_get = json_object_get,
	.object_set = json_object_set,
	//.object_set_new_nocheck = json_object_set_new_nocheck,
	.object_del = json_object_del,
	.object_clear = json_object_clear,
	.object_update = json_object_update,
	.object_update_existing = json_object_update_existing,
	.object_update_missing = json_object_update_missing,
	.object_iter = json_object_iter,
	.object_iter_at = json_object_iter_at,
	.object_key_to_iter = json_object_key_to_iter,
	.object_iter_next = json_object_iter_next,
	.object_iter_key = json_object_iter_key,
	.object_iter_value = json_object_iter_value,
	.object_iter_set_new = json_object_iter_set_new,
	.array_size = json_array_size,
	.array_get = json_array_get,
	.array_set = json_array_set,
	.array_append = json_array_append,
	.array_insert = json_array_insert,
	.array_remove = json_array_remove,
	.array_clear = json_array_clear,
	.array_extend = json_array_extend,
	.string_value = json_string_value,
	.string_length = json_string_length,
	.integer_value = json_integer_value,
	.real_value = json_real_value,
	.number_value = json_number_value,
	.string_set = json_string_set,
	.string_setn = json_string_setn,
	.string_set_nocheck = json_string_set_nocheck,
	.string_setn_nocheck = json_string_setn_nocheck,
	.integer_set = json_integer_set,
	.real_set = json_real_set,
	.pack = json_pack,
	.pack_ex = json_pack_ex,
	.vpack_ex = json_vpack_ex,
	.unpack = json_unpack,
	.unpack_ex = json_unpack_ex,
	.vunpack_ex = json_vunpack_ex,
	.sprintf = json_sprintf,
	.vsprintf = json_vsprintf,
	.equal = json_equal,
	.copy = json_copy,
	.deep_copy = json_deep_copy,
	.loads = json_loads,
	.loadb = json_loadb,
	.loadf = json_loadf,
	.loadfd = json_loadfd,
	.load_file = json_load_file,
	.load_callback = json_load_callback,
	.dumps = json_dumps,
	.dumpb = json_dumpb,
	.dumpf = json_dumpf,
	.dumpfd = json_dumpfd,
	.dump_file = json_dump_file,
	.dump_callback = json_dump_callback,
	.decref = json_decref,
	.decrefp = json_decrefp,
	.incref = json_incref
};
