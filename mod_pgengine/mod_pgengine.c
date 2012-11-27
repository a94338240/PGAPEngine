
#include "mod_pgengine.h"

static int pgengine_handler(request_rec *);
static void register_hooks(apr_pool_t *);
static const char *pgengine_set_enabled(cmd_parms *cmd, void *cfg, const char *arg1);

static const command_rec pgengine_directives[] =
  {
    AP_INIT_TAKE1("PGEngineEnabled", pgengine_set_enabled, NULL, RSRC_CONF, "Enable or disable mod_pgengine"),
    {NULL}
  };

module AP_MODULE_DECLARE_DATA pgengine_module =
  {
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    pgengine_directives,
    register_hooks   /* Our hook registering function */
  };

static void register_hooks(apr_pool_t *pool)
{
  ap_hook_handler(pgengine_handler, NULL, NULL, APR_HOOK_LAST);
}


static const char *pgengine_set_enabled(cmd_parms *cmd, void *cfg, const char *arg1)
{
  return NULL;
}

static int pgengine_handler(request_rec *r)
{
  if (!r->handler || strcmp(r->handler, "pgengine-handler")) 
    return (DECLINED);
  
  return output_page(r);
}
