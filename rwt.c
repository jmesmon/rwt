#include <stdio.h>
#define NAME "rwtorrent"
static const char usage_str[] =
"usage: %s <torrent> <action> [options]\n"
"\n"
"actions: rm <regex>        - remove trackers that \n"
"                               match the given regex\n"
"         add <tracker url> - add the given tracker\n"
"         show              - show current trackers\n"
;

enum be_type {
	BE_STR,
	BE_DICT,
	BE_INT,
	BE_LIST
};

struct be_str {
	size_t len;
	char *str;
};

struct be_dict {
	char **key;
	be_node **val;
};

struct be_list {
	size_t len;
	struct be_node **nodes;
}

struct be_node {
	enum be_type type;
	union {
		long i;
		be_node **l;
		be_dict *d;
		be_str *s;
	}
};

struct be_list *bdecode_list(const char *estr, size_t len, const char **ep)
{
	struct be_list *l = malloc(sizeof(*l));

	/* assert(*estr == 'l'); */
	for(estr += 1; estr != 'e'; estr) {
		struct be_node *n = bdecode(estr + 1, &ep);
		if (n) {
		}		
	}
}

/* parses a bencoded str from the encoded string buffer.
 * on error : returns 0, estr = ep.
 */
struct be_str *bdecode_str(const char *estr, size_t len, const char **ep)
{
	if (len == 0) {
		*ep = estr;
		return 0;
	}

	const char *ppos = estr;
	size_t slen = *ppos - '0';
	for(;;) {
		ppos++;
		len--;
		if(len <= 0) {
			*ep = estr;
		} else if (*ppos == ':') {
			break;
		}

		slen *= 10;
		slen += *ppos - '0';
		/* TODO: detect overflow. */
	}

	/* ppos points to the ':' */
	if (slen == 0 || slen > len) {
		/* not a valid string. */
		*ep = estr;
		return 0;
	}

	char *str = malloc(len);
	if (!str) {
		*ep = estr;
		return 0;
	}

	memcpy(str, ppos + 1, len);

	struct be_str *bstr = malloc(sizeof(*bstr));
	if (!bstr) {
		*ep = estr;
		free(str);
		return 0;
	}

	bstr->len = slen;
	bstr->str = str;
	*ep = iep + 1 + slen;
	return bstr;
}

struct be_node *bdecode(const char *estr, size_t len, const char **ep)
{
	switch(*estr) {
	case 'd':
		return bdecode_dict(estr, len, ep);
	case 'i':
		return bdecode_int(estr, len, ep);
	case 'l':
		return bdecode_list(estr, len, ep);

	case '0'..'9':
		return bdecode_str(estr, len, ep);
	default:
		return 0;
	}
}

static void usage(char *name)
{
	fprintf(stderr, usage_str, name);
}

t_show(FILE *tf, int argc, char **argv)
{

}

int main(int argc, char **argv)
{
	if (argc < 3) {
		if (argc < 1)
			usage(NAME);
		else
			usage(argv[0]);
		return 1;
	}

	char *torrent = argv[1];
	FILE *tf = fopen(torrent, "r+b");
	if (!tf) {
		fprintf(stderr, "torrent \"%s\": fopen: %s\n", torrent, 
			strerror(errno));
		return 2;
	}

	char *act = argv[2];
	if (!strcmp(act, "rm")) {
		return t_rm(tf, argc - 1, argv + 1) + 5;
	} else if (!strcmp(act, "add")) {
		return t_add(tf, argc - 1, argv + 1) + 5;
	} else if (!strcmp(act, "show")) {
		return t_show(tf, argc - 1, argv + 1) + 5;
	} else {
		fprintf(stderr, "unknown action: \"%s\"\n",act);
		usage(argv[0]);
		return 4;
	}
}
