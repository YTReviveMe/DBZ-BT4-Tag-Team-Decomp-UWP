#ifdef BT3_UWP
#include "net_update.h"
#include "../../uwp/http.h"
#include "../../include/bt4_version.h"
#include <SDL3/SDL.h>
#include <string.h>

static SDL_AtomicInt sOnlineRequired;

const char *Update_Version(void) { return BT4_CANONICAL_VERSION; }
void Update_Cleanup(void) {}
void Update_Check(void) {}
void Update_Recheck(void) {}
void Update_Status(UpdateStatus *out) {
    if (!out) return;
    memset(out, 0, sizeof(*out));
    out->state = UPD_NONE;
    out->current = Update_Version();
}
const char *Update_Notes(void) { return ""; }
void Update_NotesFor(const char *locale, char *out, size_t size) {
    (void)locale;
    if (out && size) out[0] = 0;
}
void Update_Download(void) {}
void Update_Install(void) {}
int Update_StartTextureMaintenance(int locale) { (void)locale; return 0; }
int Update_Relaunch(void) { return 0; }
int Update_FetchOnlineRoute(char *out, size_t capacity) {
    static const char url[] = "https://raw.githubusercontent.com/NauTaro/DBZ-BT4-Tag-Team-Decomp/online-routing/policy.json";
    return PortUwp_HttpRequest(url, NULL, NULL, out, capacity);
}
void Update_SetOnlineRequired(int required) {
    SDL_SetAtomicInt(&sOnlineRequired, required != 0);
}
int Update_OnlineRequired(void) { return SDL_GetAtomicInt(&sOnlineRequired) != 0; }
int Update_BlocksOnline(void) { return Update_OnlineRequired(); }
const char *Update_OnlineNote(void) {
    return Update_OnlineRequired() ? "A newer BT4 UWP package is required for online play." : "";
}
int Update_LastResult(const char **version) {
    if (version) *version = "";
    return 0;
}
#endif
