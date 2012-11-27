#include "mod_pgengine.h"

void dbd_init(dbd_t *dbd)
{
  if (dbd && dbd->ops.sql && dbd->ops.sql->init)
    dbd->ops.sql->init(dbd);
}
void dbd_connect(dbd_t *dbd, const char* host, 
            const char* user, const char* passwd, 
            const char* db, const char* port,
            const char* unix_socket, unsigned long client_flag)
{
  if (dbd && dbd->ops.sql && dbd->ops.sql->connect)
    dbd->ops.sql->connect(dbd, host, user, passwd, db, 
                            port, unix_socket, client_flag);
}

int dbd_query(dbd_t *dbd, const char* query)
{
  if (dbd && dbd->ops.sql && dbd->ops.sql->query)
    return dbd->ops.sql->query(dbd, query);
}

void dbd_close(dbd_t *dbd)
{
  if (dbd && dbd->ops.sql && dbd->ops.sql->close)
    dbd->ops.sql->close(dbd);
}

void dbd_store_result(dbd_t *dbd)
{
  if (dbd && dbd->ops.sql && dbd->ops.sql->store_result)
    dbd->ops.sql->store_result(dbd);
}

int dbd_fetch_row(dbd_t *dbd)
{
  if (dbd && dbd->ops.sql && dbd->ops.sql->store_result)
    return dbd->ops.sql->fetch_row(dbd);
}

const char* dbd_get_field(dbd_t *dbd, int idx)
{
  if (dbd && dbd->ops.sql && dbd->ops.sql->store_result)
    return dbd->ops.sql->get_field(dbd, idx);
}
