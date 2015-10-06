/* A test harness for libr_asm. Persistent mode is supported.
 * Input is read from stdin. */

#define PERSIST_MAX 10000
static unsigned int persist_cnt = 0;

#include "../afl.h"
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

/* Prints r2 commands that should reproduce the behavior of that input at
 * stdin. */
void explain(RAsmPlugin **plugins, int nplugins) {
	ut8 buf[64];

	memset(buf, 0, sizeof buf);
	read(0, buf, sizeof buf);

	if (buf[0] >= nplugins) {
		printf("# Invalid asm plugin %d\n", buf[0]);
		return;
	}

	RAsmPlugin *plugin = plugins[buf[0]];
	printf("# Run as: r2 -i THIS-FILE -\n");
	printf("e asm.arch=%s\t\t\t\t# %s\n", plugin->name, plugin->desc);

	for (int addr = 0; addr < sizeof buf - 1; addr += 16) {
		printf("wx ");
		for (int i = 0; i < 16; i++) {
			if (1 + addr + i < sizeof buf)
				printf("%02x", buf[1 + addr + i]);
			else
				printf("00");
		}
		printf(" @ %d\n", addr);
	}

	printf("pd 1\n");
}

int main(int argc, char **argv) {
	ut8 buf[64];
	RAsmOp op;
	RAsm *rasm = r_asm_new();
	RAsmPlugin *plugins[256];
	int nplugins = gather_plugins(rasm, plugins);

	/* If --explain if given on the command line, print a r2 script that
	 * can be used to reproduce the behavior of this input. Note that the
	 * input is still read from stdin. */
	if (argc == 2 && strcmp(argv[1], "--explain") == 0) {
		explain(plugins, nplugins);
		goto cleanup;
	}

	while (__AFL_LOOP(PERSIST_MAX)) {

		/* reset stuff */
		memset(buf, 0, sizeof buf);
		memset(&op, 0, sizeof op);

		/* Let's hope that r_asm_disassemble modifies only the pc */
		r_asm_set_pc(rasm, 0x1000);

		/* read the input */
		read(0, buf, sizeof buf);

		/* choose a plugin to test */
		if (buf[0] >= nplugins)
			continue;
		r_asm_use(rasm, plugins[buf[0]]->name);


		/* test libr_asm */
		int res = r_asm_disassemble(rasm, &op, buf+1, sizeof(buf) - 1);
	}

cleanup:
	/* release resources */
	r_asm_free(rasm);

	/* we're done, for now. */
	return 0;
}
