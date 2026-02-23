#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x092a35a2, "_copy_to_user" },
	{ 0xd272d446, "__fentry__" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xe8213e80, "_printk" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0x5a844b26, "__x86_indirect_thunk_rbx" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x37031a65, "__register_chrdev" },
	{ 0x52b15b3b, "__unregister_chrdev" },
	{ 0xbebe66ff, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x092a35a2,
	0xd272d446,
	0xd272d446,
	0xe8213e80,
	0x092a35a2,
	0x5a844b26,
	0x90a48d82,
	0x37031a65,
	0x52b15b3b,
	0xbebe66ff,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"_copy_to_user\0"
	"__fentry__\0"
	"__x86_return_thunk\0"
	"_printk\0"
	"_copy_from_user\0"
	"__x86_indirect_thunk_rbx\0"
	"__ubsan_handle_out_of_bounds\0"
	"__register_chrdev\0"
	"__unregister_chrdev\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "1F6C7350426626591E751DC");
