#include "mod_pgengine.h"
#include "mod_pgengine_dbd_mysql.h"

static void register_hooks(apr_pool_t *pool);
module AP_MODULE_DECLARE_DATA pgengine_dbd_mysql_module =
  {
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    register_hooks   /* Our hook registering function */
  };

static void pg_mysql_init(dbd_t *dbd)
{
  if (dbd)
    dbd->conn = mysql_init(NULL);
}

static void pg_mysql_connect(dbd_t *dbd, const char* host, 
                             const char* user, const char* passwd, 
                             const char* db, const char* port,
                             const char* unix_socket, unsigned long client_flag) 
{
  if (dbd)
    dbd->conn = mysql_real_connect(dbd->conn, host, 
                                   user, passwd, db, port, unix_socket, 
                                   client_flag);
}

static int pg_mysql_query(dbd_t *dbd, const char* query)
{
  if (dbd && dbd->conn && query)
    return mysql_query(dbd->conn, query);
}

void pg_mysql_store_result(dbd_t *dbd)
{
  if (dbd && dbd->conn)
    dbd->res = mysql_store_result(dbd->conn);
}

int pg_mysql_fetch_row(dbd_t *dbd)
{
  if (!dbd || !dbd->res)
    return 0;
  dbd->row = mysql_fetch_row(dbd->res);
  if (dbd->row)
    return 1;
  else
    return 0;
}

const char* pg_mysql_get_field(dbd_t *dbd, int idx)
{
  if (dbd && dbd->row)
    return ((MYSQL_ROW)dbd->row)[idx];
  else
    return NULL;
}

static void pg_mysql_close(dbd_t* dbd)
{
  if (dbd && dbd->conn)
    mysql_close(dbd->conn);
}

static dbd_sql_ops_t mysql_ops = {
  .init = pg_mysql_init,
  .connect = pg_mysql_connect,
  .query = pg_mysql_query,
  .close = pg_mysql_close,
  .store_result = pg_mysql_store_result,
  .fetch_row = pg_mysql_fetch_row,
  .get_field = pg_mysql_get_field
};

static dbd_t mysql_dbd;
dbd_t *mysql_db_driver()
{
  mysql_dbd.type = DBD_MYSQL;
  mysql_dbd.conn = NULL;
  mysql_dbd.ops.sql = &mysql_ops;
  return &mysql_dbd;
}

static void register_hooks(apr_pool_t *pool)
{}
