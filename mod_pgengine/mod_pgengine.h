#ifndef MOD_PGENGINE_H
#define MOD_PGENGINE_H
#include "httpd.h"
#include "http_config.h" 
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "apr.h"
#include "util_script.h"
#include "apr_strings.h"

typedef enum {
  TECH_ROUTINE,
  TECH_TEMPLATE
} USE_TECH_t;
typedef struct {
  const char *path;
  const char *type;
  const USE_TECH_t usetech;
  union {
   int (*routine)(request_rec* r);
    const char* template;
  }
} page_tbl_t;
void register_page_table(page_tbl_t *page_tbl);

typedef enum {
  DBD_MYSQL
} DBD_TYPE;
typedef struct _dbd_sql_ops_t dbd_sql_ops_t;
typedef struct {
  DBD_TYPE type;
  void *conn;
  void *res;
  void *row;
  union {
    dbd_sql_ops_t *sql;
  } ops;
} dbd_t;
typedef struct _dbd_sql_ops_t{
  void (*init)(dbd_t *dbd);
  void (*connect)(dbd_t *dbd, const char* host, 
                  const char* user, const char* passwd, 
                  const char* db, const char* port,
                  const char* unix_socket, unsigned long client_flag);
  int (*query)(dbd_t *dbd, const char* query);
  void (*store_result)(dbd_t *dbd);
  int (*fetch_row)(dbd_t *dbd);
  const char* (*get_field)(dbd_t *dbd, int idx);
  void (*close)(dbd_t *dbd);
} dbd_sql_ops_t;
void register_dbd_ops(const dbd_sql_ops_t* ops);
void dbd_init(dbd_t *dbd);
void dbd_connect(dbd_t *dbd, const char* host, 
            const char* user, const char* passwd, 
            const char* db, const char* port,
            const char* unix_socket, unsigned long client_flag);
int dbd_query(dbd_t *dbd, const char* query);
void dbd_store_result(dbd_t *dbd);
int dbd_fetch_row(dbd_t *dbd);
const char* dbd_get_field(dbd_t *dbd, int idx);
void dbd_close(dbd_t *dbd);

int output_page(request_rec* r);
int read_post(request_rec *r, apr_table_t **tab);

char *addslashes(request_rec *r, char *str);

#endif /* MOD_PGENGINE_H */
