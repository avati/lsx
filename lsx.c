#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <alloca.h>
#include <attr/xattr.h>


int
process_xattr (const char *path, const char *key)
{
	ssize_t ret;
	char *value;

	ret = lgetxattr (path, key, NULL, 0);
	if (ret < 0)
		return ret;

	value = alloca (ret + 1);
	value[ret] = 0;
	ret = lgetxattr (path, key, value, ret);
	if (ret < 0)
		return ret;
	return 0;
}


int
process_xattrs (const char *path)
{
	ssize_t ret;
	char *keys;
	char *key;

	ret = llistxattr (path, NULL, 0);
	if (ret < 0)
		return ret;

	keys = alloca (ret + 1);
	ret = llistxattr (path, keys, ret);
	if (ret < 0)
		return ret;

	for (key = keys; key - keys < ret; key += (strlen (key) + 1)) {
		ret = process_xattr (path, key);
		if (ret < 0)
			break;
	}

	return ret;
}


int
process_entry (const char *dir, const char *name)
{
	struct stat stbuf;
	int ret;
	char *path;

	path = alloca (strlen (dir) + strlen (name) + 2);
	sprintf (path, "%s/%s", dir, name);

	ret = lstat (path, &stbuf);
	if (ret != 0)
		return ret;

	ret = process_xattrs (path);

	return ret;
}


int
main (int argc, char *argv[])
{
	DIR *d;
	struct dirent entry, *res;

	if (argc != 2) {
		fprintf (stderr, "Usage: %s <dir>\n", argv[0]);
		return 1;
	}

	d = opendir (argv[1]);
	if (!d) {
		fprintf (stderr, "%s: %s\n", argv[1], strerror (errno));
		return 1;
	}

	while (readdir_r (d, &entry, &res) == 0) {
		if (!res)
			break;
		process_entry (argv[1], res->d_name);
	}

	closedir (d);
	return 0;
}
