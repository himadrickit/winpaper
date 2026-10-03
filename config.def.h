/* winpapers config.def.h
 * Copy of this file becomes config.h on first `make` (edit config.h, not this).
 */
#ifndef CONFIG_H
#define CONFIG_H

/* Desktop wallpaper fit style, written to
 * HKCU\Control Panel\Desktop\WallpaperStyle / TileWallpaper
 * before SPI_SETDESKWALLPAPER is applied.
 *
 *   "0"  Center       "1"  Tile
 *   "2"  Stretch      "6"  Fit
 *   "10" Fill         "22" Span (multi-monitor)
 */
#define WALLPAPER_STYLE "10" /* Fill */
#define WALLPAPER_TILE  "0"

#endif /* CONFIG_H */
