#include "atheme.h"
#include "pmodule.h"

DECLARE_MODULE_V1 (
	"freenode/log_sasl_fail", FALSE, _modinit, _moddeinit,
	"", "freenode <http://www.freenode.net>"
);

void (*old_encap_handler)(sourceinfo_t *, int, char *[]) = 0;

static void encap_handler(sourceinfo_t *si, int parc, char *parv[])
{
    if (!irccasecmp(parv[1], "SASLFAIL"))
    {
	if (parc < 6) return;
	slog(LG_CMD_REQUEST, "SASL login failure by %s from %s(%s)", parv[2], parv[5], parv[6]);
    }
    if (old_encap_handler)
	old_encap_handler(si, parc, parv);
}

void _modinit(module_t *m)
{
    pcommand_t *old_encap = pcommand_find("ENCAP");
    if (old_encap) old_encap_handler = old_encap->handler;
    pcommand_delete("ENCAP");
    pcommand_add("ENCAP", encap_handler, 2, MSRC_USER | MSRC_SERVER);
}

void _moddeinit(module_unload_intent_t intentvoid)
{
    pcommand_delete("ENCAP");
    if (old_encap_handler) pcommand_add("ENCAP", old_encap_handler, 2, MSRC_USER | MSRC_SERVER);
}