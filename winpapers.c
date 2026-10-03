/* winpapers - minimal CLI wallpaper setter for Windows
 *
 *   wp <path>       set desktop wallpaper
 *   wp -l <path>    set lock screen wallpaper (needs admin)
 *
 * Single file, no deps beyond the Win32 API. See config.h to tweak
 * the desktop fit style (Fill/Fit/Stretch/...).
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "config.h"

static void
usage(const char *prog)
{
	fprintf(stderr, "usage: %s [-l] <image-path>\n", prog);
	fprintf(stderr, "  (no flag)   set desktop wallpaper\n");
	fprintf(stderr, "  -l          set lock screen wallpaper (requires admin)\n");
}

/* resolve a possibly-relative path to an absolute one and confirm it
 * points at an existing, non-directory file. SPI_SETDESKWALLPAPER and
 * the lockscreen policy key both need an absolute path to be reliable. */
static int
resolve_path(const char *in, char *out, DWORD outlen)
{
	DWORD n = GetFullPathNameA(in, outlen, out, NULL);
	if (n == 0 || n > outlen)
		return 0;

	DWORD attr = GetFileAttributesA(out);
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
		return 0;

	return 1;
}

static int
set_desktop_wallpaper(const char *path)
{
	HKEY hkey;

	/* fit style is best-effort; if it fails we still try to set the image */
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0,
	                   KEY_SET_VALUE, &hkey) == ERROR_SUCCESS) {
		RegSetValueExA(hkey, "WallpaperStyle", 0, REG_SZ,
		               (const BYTE *)WALLPAPER_STYLE,
		               (DWORD)strlen(WALLPAPER_STYLE) + 1);
		RegSetValueExA(hkey, "TileWallpaper", 0, REG_SZ,
		               (const BYTE *)WALLPAPER_TILE,
		               (DWORD)strlen(WALLPAPER_TILE) + 1);
		RegCloseKey(hkey);
	}

	if (!SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)path,
	                            SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) {
		fprintf(stderr, "error: SystemParametersInfoA failed (%lu)\n",
		        GetLastError());
		return 1;
	}

	printf("desktop wallpaper set: %s\n", path);
	return 0;
}

static int
set_lockscreen_wallpaper(const char *path)
{
	HKEY hkey;
	LONG rc = RegCreateKeyExA(
	    HKEY_LOCAL_MACHINE,
	    "SOFTWARE\\Policies\\Microsoft\\Windows\\Personalization",
	    0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hkey, NULL);

	if (rc != ERROR_SUCCESS) {
		fprintf(stderr,
		        "error: cannot open registry key (%ld) - run as administrator\n",
		        rc);
		return 1;
	}

	rc = RegSetValueExA(hkey, "LockScreenImage", 0, REG_SZ,
	                     (const BYTE *)path, (DWORD)strlen(path) + 1);
	RegCloseKey(hkey);

	if (rc != ERROR_SUCCESS) {
		fprintf(stderr, "error: RegSetValueExA failed (%ld)\n", rc);
		return 1;
	}

	printf("lock screen wallpaper set: %s\n", path);
	return 0;
}

int
main(int argc, char **argv)
{
	int lock = 0;
	const char *arg_path = NULL;
	int i;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-l") == 0) {
			lock = 1;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			usage(argv[0]);
			return 0;
		} else {
			arg_path = argv[i];
		}
	}

	if (!arg_path) {
		usage(argv[0]);
		return 1;
	}

	char full[MAX_PATH];
	if (!resolve_path(arg_path, full, MAX_PATH)) {
		fprintf(stderr, "error: file not found or invalid: %s\n", arg_path);
		return 1;
	}

	return lock ? set_lockscreen_wallpaper(full) : set_desktop_wallpaper(full);
}
