/* A test harness for libr_asm. Persistent mode is supported.
 * Input is read from stdin. */

#define PERSIST_MAX 10000
static unsigned int persist_cnt = 0;

#include <r_asm.h>

/* Walks the list of plugins, writes each one into the plugins array, returns
 * the number of plugins available.
 *
 * plugins must point to an array of 256 elements. */
int gather_plugins(RAsm *rasm, RAsmPlugin **plugins) {
	RList *list = r_asm_get_plugins(rasm);
	RListIter *it;
	RAsmPlugin *plugin;
	int index = 0;

	r_list_foreach(list, it, plugin) {
		plugins[index++] = plugin;

		if (index == 256) {
			printf("too many plugins");
			abort();
		}
	}

	/* TODO: Do I need to free anything? */

	return index;
}

int main(void) {
	ut8 buf[64];
	RAsmOp op;
	RAsm *rasm = r_asm_new();
	RAsmPlugin *plugins[256];
	int nplugins = gather_plugins(rasm, plugins);

try_again:

	/* reset stuff */
	memset(buf, 0, sizeof buf);
	memset(&op, 0, sizeof op);

	/* Let's hope that r_asm_disassemble modifies only the pc */
	r_asm_set_pc(rasm, 0x1000);

	/* read the input */
	read(0, buf, sizeof buf);

	/* choose a plugin to test */
	if (buf[0] >= nplugins)
		goto nothing_to_do;
	r_asm_use(rasm, plugins[buf[0]]->name);


	/* test libr_asm */
	int res = r_asm_disassemble(rasm, &op, buf+1, sizeof(buf) - 1);


nothing_to_do:
	/* restart the loop, if it makes sense. */
	if (getenv("AFL_PERSISTENT") && persist_cnt++ < PERSIST_MAX) {
		raise(SIGSTOP);
		goto try_again;
	}

	/* release resources */
	r_asm_free(rasm);

	/* we're done, for now. */
	return 0;
}
