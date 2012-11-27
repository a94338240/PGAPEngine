#include "mod_pgengine.h"

char *addslashes(request_rec *r, char *str)
{
  /* maximum string length, worst case situation */
  char *new_str;
  char *source, *target;
  char *end;
  int new_length;
  int length = strlen(str);

  if (length == 0) {
    char *p = (char *) apr_pcalloc(r->pool, 1);
    if (p == NULL) {
      return p;
    }
    p[0] = 0;
    return p;
  }

  new_str = (char *) apr_pcalloc(r->pool, 2 * length + 1);
  if (new_str == NULL) {
    return new_str;
  }
  source = str;
  end = source + length;
  target = new_str;
    
  while (source < end) {
    switch (*source) {
    case '\0':
      *target++ = '\\';
      *target++ = '0';
      break;
    case '\'':
    case '\"':
    case '\\':
      *target++ = '\\';
      /* break is missing *intentionally* */
    default:
      *target++ = *source;
      break;
    }
    source++;
  }
    
  *target = 0;
  new_length = target - new_str;
  // new_str = (char *) arp_prealloc(new_str, new_length + 1);
  return new_str;
}

static int util_read(request_rec *r, const char **rbuf)
{
  int rc;
  if ((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)) != OK) {
    return rc;
  }

  if (ap_should_client_block(r)) {
    char argsbuffer[HUGE_STRING_LEN];
    int rsize, len_read, rpos=0;
    long length = r->remaining;
    *rbuf = apr_pcalloc(r->pool, length + 1);
    while ((len_read =
            ap_get_client_block(r, argsbuffer, sizeof(argsbuffer))) > 0) { 
      if ((rpos + len_read) > length) {
        rsize = length - rpos;
      }
      else {
        rsize = len_read;
      }
      memcpy((char*)*rbuf + rpos, argsbuffer, rsize);
      rpos += rsize;
    }
  }
}

static int util_read_multipart(request_rec *r, const char* boundary, 
                               const char **rbuf, int *rbuf_len, 
                               const char **headers)
{
  int rc;
  if ((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)) != OK) {
    return rc;
  }
  if (ap_should_client_block(r)) {
    char argsbuffer[HUGE_STRING_LEN];
    char *tmpbuf;
    char *header_cs = NULL, *header_ce = NULL;
    char *data_cs = NULL, *data_ce = NULL, *tmp_data_ce = NULL;
    int rsize, len_read, rpos=0;
    long length = r->remaining;
    char *bufend = NULL;

    *rbuf = apr_pcalloc(r->pool, length + 1);
    *headers = apr_pcalloc(r->pool, 500);
    tmpbuf = apr_pcalloc(r->pool, length + 1);
    bufend = tmpbuf + length;

    while ((len_read =
            ap_get_client_block(r, argsbuffer, sizeof(argsbuffer))) > 0) { 
      if ((rpos + len_read) > length) {
        rsize = length - rpos;
      }
      else {
        rsize = len_read;
      }
      memcpy(tmpbuf + rpos, argsbuffer, rsize);
      rpos += rsize;
    }

    header_cs = strstr(tmpbuf, boundary);
    if (!header_cs)
      return -1;
    header_cs += strlen(boundary);

    header_ce = strstr(header_cs, "\r\n\r\n");
    strncpy(*headers, header_cs, header_ce - header_cs);
    
    data_cs = header_ce + 4;
    tmp_data_ce = data_cs;

    for (;tmp_data_ce < bufend;) {
      data_ce = strstr(tmp_data_ce, boundary);
      if (!data_ce)
        tmp_data_ce += strlen(tmp_data_ce) + 1;
      else 
        break;
    }

    data_ce -= 4;
    memcpy(*rbuf, data_cs, data_ce - data_cs);
    *rbuf_len = data_ce - data_cs;
  }

  return rc;
}

int read_post(request_rec *r, apr_table_t **tab)
{
  const char *data, *headers;
  const char *key, *val, *type;
  char boundary[200] = { 0 };
  char *boundary_cs = NULL;
  char *boundary_ce = NULL;
  int data_len = 0;
  int rc = OK;

  if(r->method_number != M_POST) {
    return rc;
  }

  type = apr_table_get(r->headers_in, "Content-Type");

  if (!type)
    return DECLINED;

  if(strstr(type, "application/x-www-form-urlencoded")) {
    if((rc = util_read(r, &data)) != OK) {
      return rc;
    }

    if(*tab) {
      apr_table_clear(*tab);
    }
    else {
      *tab = apr_table_make(r->pool, 8);
    }

    while(*data && (val = ap_getword(r->pool, &data, '&'))) { 
      key = ap_getword(r->pool, &val, '=');
      ap_unescape_url((char*)key);
      ap_unescape_url((char*)val);
      apr_table_merge(*tab, key, val);
    }
  }
  else if (strstr(type, "multipart/form-data")) {
    boundary_cs = strstr(type, "boundary=");
    if (!boundary_cs)
      return DECLINED;

    boundary_cs += 9;
    boundary_ce = strstr(boundary_cs, ";");
    if (!boundary_ce) {
      boundary_ce = boundary_cs;
      while (*boundary_ce)
        boundary_ce++;
    }
    
    strncpy(boundary, boundary_cs, (boundary_ce - boundary_cs));

    if((rc = util_read_multipart(r, boundary, &data, 
                                 &data_len, &headers)) != OK) {
      return rc;
    }
    
    
    /* TODO */
  }
  else {
    return DECLINED;
  }
  
  return OK;
}
