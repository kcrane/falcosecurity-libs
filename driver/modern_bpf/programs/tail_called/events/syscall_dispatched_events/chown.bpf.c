/*
 * Copyright (C) 2022 The Falco Authors.
 *
 * This file is dual licensed under either the MIT or GPL 2. See MIT.txt
 * or GPL2.txt for full copies of the license.
 */

#include <helpers/interfaces/fixed_size_event.h>
#include <helpers/interfaces/variable_size_event.h>

/*=============================== ENTER EVENT ===========================*/

SEC("tp_btf/sys_enter")
int BPF_PROG(chown_e,
	     struct pt_regs *regs,
	     long id)
{
	struct ringbuf_struct ringbuf;
	if(!ringbuf__reserve_space(&ringbuf, CHOWN_E_SIZE))
	{
		return 0;
	}

	ringbuf__store_event_header(&ringbuf, PPME_SYSCALL_CHOWN_E);

	/*=============================== COLLECT PARAMETERS  ===========================*/

	// Here we have no parameters to collect.

	/*=============================== COLLECT PARAMETERS  ===========================*/

	ringbuf__submit_event(&ringbuf);

	return 0;
}

/*=============================== ENTER EVENT ===========================*/

/*=============================== EXIT EVENT ===========================*/

SEC("tp_btf/sys_exit")
int BPF_PROG(chown_x,
	     struct pt_regs *regs,
	     long ret)
{
	struct auxiliary_map *auxmap = auxmap__get();
	if(!auxmap)
	{
		return 0;
	}

	auxmap__preload_event_header(auxmap, PPME_SYSCALL_CHOWN_X);

	/*=============================== COLLECT PARAMETERS  ===========================*/

	/* Parameter 1: res (type: PT_ERRNO) */
	auxmap__store_s64_param(auxmap, ret);

	/* Parameter 2: filename (type: PT_FSPATH) */
	unsigned long path_pointer = extract__syscall_argument(regs, 0);
	auxmap__store_charbuf_param(auxmap, path_pointer, MAX_PATH, USER);

	/* Parameter 3: uid (type: PT_UINT32) */
	unsigned long uid = extract__syscall_argument(regs, 1);
	auxmap__store_u32_param(auxmap, uid);

	/* Parameter 4: gid (type: PT_UINT32) */
	unsigned long gid = extract__syscall_argument(regs, 2);
	auxmap__store_u32_param(auxmap, gid);

	/*=============================== COLLECT PARAMETERS  ===========================*/

	auxmap__finalize_event_header(auxmap);

	auxmap__submit_event(auxmap);

	return 0;
}

/*=============================== EXIT EVENT ===========================*/
