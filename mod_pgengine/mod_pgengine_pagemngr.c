#include "mod_pgengine.h"

page_tbl_t *global_page_tbl = NULL;

void register_page_table(page_tbl_t *page_tbl)
{
  global_page_tbl = page_tbl;
}

int output_page(request_rec *r) {
  if (!r)
    goto out_err;
  if (!r->path_info)
    goto out_err;
  if (!global_page_tbl)
    goto out_err;

  for (int i = 0; i < 1024 && global_page_tbl[i].path != NULL; i++) {
    if (!strcmp(r->path_info, global_page_tbl[i].path)) {
      ap_set_content_type(r, global_page_tbl[i].type);
      if (global_page_tbl[i].usetech == TECH_ROUTINE)
        return global_page_tbl[i].routine(r);
    }
  }

 out_err:
  return HTTP_NOT_FOUND;
}
