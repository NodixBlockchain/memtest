#include <base/std_def.h>
#include <base/std_mem.h>
#include <base/mem_base.h>
#include <base/std_str.h>

#include <strs.h>
#include <connect.h>
#include <mem_stream.h>
#include <tpo_mod.h>
#include <fsio.h>


#ifdef _DEBUG

#include "../libbase/include/parser.h"
#include "../libbase/include/tree.h"

C_IMPORT int C_API_FUNC app_init(mem_zone_ref_ptr params);
C_IMPORT int C_API_FUNC app_start(mem_zone_ref_ptr params);
C_IMPORT int C_API_FUNC app_loop(mem_zone_ref_ptr params);
C_IMPORT int C_API_FUNC app_stop(mem_zone_ref_ptr params);

C_IMPORT void C_API_FUNC tree_manager_init(size_t size, unsigned int flags);

#else
typedef int C_API_FUNC app_func(mem_zone_ref_ptr params);
typedef app_func						*app_func_ptr;

typedef void C_API_FUNC	tree_manager_init_func(size_t size,unsigned int flags);
typedef tree_manager_init_func			*tree_manager_init_func_ptr;

tree_manager_init_func_ptr			tree_manager_init = PTR_INVALID;

app_func_ptr						app_init = PTR_INVALID, app_start = PTR_INVALID, app_loop = PTR_INVALID, app_stop = PTR_INVALID;

#endif


typedef int C_API_FUNC init_func();
typedef init_func *init_func_ptr;

tpo_mod_file libbase_mod = { 0xDEF00FED };
tpo_mod_file memtest_mod = { 0xDEF00FED };
int main(int argc, const char **argv)
{
	

	init_mem_system();
	init_default_mem_area(24 * 1024 * 1024);
	set_exe_path();
	network_init();

	tpo_mod_init(&libbase_mod);
	tpo_mod_init(&memtest_mod);
	load_module("modz/libbase.tpo", "libbase", &libbase_mod);
	load_module("modz/memtest.tpo", "memtest", &memtest_mod);
	//load_module("modz/memtest2.tpo", "memtest2", &memtest_mod);

#ifndef _DEBUG
	tree_manager_init = (tree_manager_init_func_ptr)get_tpo_mod_exp_addr_name(&libbase_mod, "tree_manager_init", 0);;
#endif

	daemonize("testing");


#ifndef _DEBUG
	app_init = (app_func_ptr)get_tpo_mod_exp_addr_name(&memtest_mod, "app_init", 0);
	app_start = (app_func_ptr)get_tpo_mod_exp_addr_name(&memtest_mod, "app_start", 0);
	app_loop = (app_func_ptr)get_tpo_mod_exp_addr_name(&memtest_mod, "app_loop", 0);
	app_stop = (app_func_ptr)get_tpo_mod_exp_addr_name(&memtest_mod, "app_stop", 0);
#endif

	if (!app_init(PTR_NULL))
	{
		console_print("could not initialize app ");
		console_print(memtest_mod.name);
		console_print("\n");
		return 0;
	}

	if (!app_start(PTR_NULL))
	{
		console_print("could not start app ");
		console_print(memtest_mod.name);
		console_print("\n");
		return 0;
	}

	while (isRunning())
	{

		app_loop(PTR_NULL);
	}
}