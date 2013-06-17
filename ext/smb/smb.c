#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
/* Includes */
#include <ruby.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libsmbclient.h"
#include "ruby.h"

/* Setup types */
/* Try not to clash with other definitions of bool... */
typedef int rubber_bool;
#define bool rubber_bool

/* Prototypes */
static VALUE _gcpool_Credentials = Qnil;
static void __gcpool_Credentials_add(VALUE val);
static void __gcpool_Credentials_del(VALUE val);
#define CREDENTIALS_ADD(val) __gcpool_Credentials_add(val)
#define CREDENTIALS_DEL(val) __gcpool_Credentials_del(val)
static VALUE mSMB;
static VALUE
SMB_CLASS_init(int __p_argc, VALUE *__p_argv, VALUE self);
static VALUE enumType;


static VALUE enumBaseClass;

typedef struct {
	int value;
	char *name;
	char *fullname;
} EnumData;

static VALUE make_enum_value(VALUE klass, int value, char *name, char *fullname)
{
	EnumData *data = NULL;
	
	data = ALLOC(EnumData);
	data->value = value;
	data->name = name;
	data->fullname = fullname;
	
	return Data_Wrap_Struct(klass, NULL, free, data);
}
static int enum_value_to_int(VALUE value, VALUE klass)
{
	switch (TYPE(value))
	{
	case T_FIXNUM:
	case T_FLOAT:
		return NUM2INT(value);
	break;
	case T_DATA:
		if (rb_obj_is_kind_of(value, enumBaseClass))
		{
			EnumData *data = NULL;
			
			if ((klass != Qnil) && (!rb_obj_is_kind_of(value, klass)))
			{
				rb_raise(rb_eTypeError, "Wrong type of enum  %s (%s required)", rb_obj_classname(value), rb_class2name(klass));
			}
			
			Data_Get_Struct(value, EnumData, data);
			return data->value;
		}
	break;
	}
	return 0;
	
}

static VALUE rubber_enum_inspect(VALUE value)
{
	EnumData *data = NULL;
	volatile VALUE str = rb_str_new("#<", 2);
	char number[16] = ""; 
	
	Data_Get_Struct(value, EnumData, data);
	
	rb_str_cat2(str, rb_obj_classname(value));
	rb_str_cat2(str, " - ");
	rb_str_cat2(str, data->name);
	rb_str_cat2(str, "(");
	sprintf(number, "%i", data->value);
	rb_str_cat2(str, number);
	rb_str_cat2(str, ")>");
	
	return str;
}

static VALUE rubber_enum_to_s(VALUE value)
{
	EnumData *data = NULL;
	
	Data_Get_Struct(value, EnumData, data);
	
	return rb_str_new2(data->fullname);
}
static VALUE rubber_enum_name(VALUE value)
{
	EnumData *data = NULL;
	
	Data_Get_Struct(value, EnumData, data);
	
	return rb_str_new2(data->name);
}

static VALUE rubber_enum_cmp(VALUE value, VALUE other)
{
	VALUE a,b;
	a = rb_funcall(value, rb_intern("to_i"), 0);
	b = rb_funcall(other, rb_intern("to_i"), 0);
	return rb_num_coerce_cmp(a, b);
}

static VALUE rubber_enum_to_i(VALUE value)
{
	EnumData *data = NULL;
	
	Data_Get_Struct(value, EnumData, data);
	
	return INT2FIX(data->value);
}

static VALUE rubber_enum_coerce(VALUE value, VALUE other)
{
	EnumData *data = NULL;
	
	Data_Get_Struct(value, EnumData, data);
	
	switch(TYPE(other))
	{
	case T_FIXNUM:
	case T_BIGNUM:
		return INT2FIX(data->value);
	case T_FLOAT:
		return rb_float_new(data->value);
	default:
		return Qnil;
	}
}

static VALUE enumType_SMBC_WORKGROUP = Qnil;
static VALUE enumType_SMBC_SERVER = Qnil;
static VALUE enumType_SMBC_FILE_SHARE = Qnil;
static VALUE enumType_SMBC_PRINTER_SHARE = Qnil;
static VALUE enumType_SMBC_COMMS_SHARE = Qnil;
static VALUE enumType_SMBC_IPC_SHARE = Qnil;
static VALUE enumType_SMBC_DIR = Qnil;
static VALUE enumType_SMBC_FILE = Qnil;
static VALUE enumType_SMBC_LINK = Qnil;
typedef int Type;
#ifdef __GNUC__
// No point in declaring these unless we're using GCC
// They're ahead of any code that uses them anyway.
static VALUE enum_Type_to_ruby(int value)
__attribute__ ((unused))
;
static int enum_ruby_to_Type(VALUE val)
__attribute__ ((unused))
;
#endif

static VALUE enum_Type_to_ruby(int value) { switch(value) {
    case SMBC_WORKGROUP: return enumType_SMBC_WORKGROUP;
    case SMBC_SERVER: return enumType_SMBC_SERVER;
    case SMBC_FILE_SHARE: return enumType_SMBC_FILE_SHARE;
    case SMBC_PRINTER_SHARE: return enumType_SMBC_PRINTER_SHARE;
    case SMBC_COMMS_SHARE: return enumType_SMBC_COMMS_SHARE;
    case SMBC_IPC_SHARE: return enumType_SMBC_IPC_SHARE;
    case SMBC_DIR: return enumType_SMBC_DIR;
    case SMBC_FILE: return enumType_SMBC_FILE;
    case SMBC_LINK: return enumType_SMBC_LINK;
}; return Qnil; }
static int enum_ruby_to_Type(VALUE val) { return enum_value_to_int(val, enumType); }
static VALUE cCTX;
static VALUE
CTX_CLASS_default(VALUE self);
static VALUE
CTX_CLASS___alloc__(VALUE self);
static VALUE
CTX_initialize(VALUE self);
static VALUE
CTX_file_creat(int __p_argc, VALUE *__p_argv, VALUE self);
static VALUE
CTX_file_open(int __p_argc, VALUE *__p_argv, VALUE self);
static VALUE
CTX_file_write(VALUE self, VALUE sfile, VALUE data);
static VALUE
CTX_file_close(VALUE self, VALUE sfile);
static VALUE cSmbFile;
static VALUE structDirEntry;
static VALUE cDir;
static VALUE
Dir_CLASS_new(VALUE self, VALUE __v_url);
static VALUE
Dir_CLASS_rmdir(VALUE self, VALUE __v_url);
static VALUE
Dir_CLASS_mkdir(VALUE self, VALUE __v_url, VALUE __v_mode);
static VALUE
Dir_initialize(VALUE self);
static VALUE
Dir_read_entry(VALUE self);
static VALUE
Dir_read(VALUE self);
static VALUE
Dir_tell(VALUE self);
static VALUE
Dir_url(VALUE self);
static VALUE
Dir_close(VALUE self);
static VALUE cFile;
static VALUE
File_CLASS_new(VALUE self, VALUE __v_url, VALUE __v_flags, VALUE __v_mode);
static VALUE
File_CLASS_create(VALUE self, VALUE __v_url, VALUE __v_mode);
static VALUE
File_CLASS_unlink(VALUE self, VALUE __v_url);
static VALUE
File_CLASS_rename(VALUE self, VALUE __v_ourl, VALUE __v_nurl);
static VALUE
File_CLASS_stat(VALUE self, VALUE __v_url);
static VALUE
File_initialize(VALUE self);
static VALUE
File_stat(VALUE self);
static VALUE
File_url(VALUE self);
static VALUE
File_seek(VALUE self, VALUE __v_offset, VALUE __v_whence);
static VALUE
File_read(int __p_argc, VALUE *__p_argv, VALUE self);
static VALUE
File_write(VALUE self, VALUE buf);
static VALUE
File_close(VALUE self);
static VALUE
File_CLASS_read(VALUE self, VALUE __v_url);

/* Inline C code */


#define TO_STRING(v) ((v) ? rb_str_new2((v)) : Qnil)

static volatile VALUE auth_block = Qnil;

static void smbc_get_auth_data(const char *srv, const char *shr, 
	char *wg, int wglen, 
	char *un, int unlen, 
	char *pw, int pwlen)
{
	volatile VALUE ret= Qnil;
	
	
	ret = rb_funcall(auth_block, rb_intern("call"), 5, TO_STRING(srv), TO_STRING(shr), 
			TO_STRING(wg), TO_STRING(un), TO_STRING(pw));
	
	if(TYPE(ret) == T_ARRAY && RARRAY_LEN(ret) == 3)
	{
		char * tmp;
		volatile VALUE tmpv;
		
		tmpv = RARRAY_PTR(ret)[0];
		tmp = StringValuePtr(tmpv);
		strncpy(wg, tmp, wglen);
		
		tmpv = RARRAY_PTR(ret)[1];
		tmp = StringValuePtr(tmpv);
		strncpy(un, tmp, unlen);
		
		tmpv = RARRAY_PTR(ret)[2];
		tmp = StringValuePtr(tmpv);
		strncpy(pw, tmp, pwlen);

    CREDENTIALS_ADD(ret);
	}
}

/*static VALUE rb_cStat;*/

typedef struct rb_smbprivate {
	int handle;
	char *url;
} rb_smbprivate;

static void smbprivate_free(rb_smbprivate *p)
{
  if (p) {
		if(p->url)
			free(p->url);
		free(p);
	}
}

static VALUE stat_new(struct stat *st)
{
    struct stat *nst = 0;

    if (st) {
	nst = ALLOC(struct stat);
	*nst = *st;
    }
    return Data_Wrap_Struct(rb_cStat, NULL, free, nst);
}


/* Code */
static void __gcpool_Credentials_add(VALUE val)
    {
      if (_gcpool_Credentials == Qnil)
      {
        _gcpool_Credentials = rb_ary_new3(1, val);
      }
      else
      {
        rb_ary_push(_gcpool_Credentials, val);
      }
    }
    
    static void __gcpool_Credentials_del(VALUE val)
    {
      if (_gcpool_Credentials == Qnil)
      {
        rb_warn("Trying to remove object from empty GC queue Credentials");
        return;
      }
      rb_ary_delete(_gcpool_Credentials, val);
      // If nothing is referenced, don't keep an empty array in the pool...
      if (RARRAY_LEN(_gcpool_Credentials) == 0)
        _gcpool_Credentials = Qnil;
    }
    
static VALUE
SMB_CLASS_init(int __p_argc, VALUE *__p_argv, VALUE self)
{
  VALUE __v_debug = Qnil;
  int debug; int __orig_debug;
  VALUE block = rb_block_proc();

  /* Scan arguments */
  rb_scan_args(__p_argc, __p_argv, "01&",&__v_debug, &block);

  /* Set defaults */
  if (__p_argc > 0)
    __orig_debug = debug = NUM2INT(__v_debug);
  else
    debug = 0;


#line 130 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  auth_block = block;
  rb_gc_register_address(&auth_block);
  if (smbc_init(smbc_get_auth_data, debug) < 0) { rb_sys_fail("smbc_init failed");
  }
  return Qnil;
}

static VALUE
CTX_CLASS_default(VALUE self)
{
  VALUE __p_retval = Qnil;

#line 90 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  do { __p_retval = Data_Wrap_Struct(self, NULL, smbc_free_context, smbc_set_context(NULL)); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
CTX_CLASS___alloc__(VALUE self)
{
  VALUE __p_retval = Qnil;

#line 93 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  SMBCCTX * ctx  =
 smbc_new_context();
  volatile VALUE  obj  ;
 rb_p(self);
  obj = Data_Wrap_Struct(self, NULL, smbc_free_context, ctx);
  do { __p_retval = obj; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
CTX_initialize(VALUE self)
{
SMBCCTX *ctx; Data_Get_Struct(self, SMBCCTX, ctx);

#line 100 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  smbc_init_context(ctx);
 
  return Qnil;
}

static VALUE
CTX_file_creat(int __p_argc, VALUE *__p_argv, VALUE self)
{
  VALUE __p_retval = Qnil;
  VALUE __v_url = Qnil;
  char * url; char * __orig_url;
  VALUE __v_mode = Qnil;
  int mode; int __orig_mode;
SMBCCTX *ctx; Data_Get_Struct(self, SMBCCTX, ctx);

  /* Scan arguments */
  rb_scan_args(__p_argc, __p_argv, "11",&__v_url, &__v_mode);

  /* Set defaults */
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );

  if (__p_argc > 1)
    __orig_mode = mode = NUM2INT(__v_mode);
  else
    mode = 0666;


#line 106 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  volatile VALUE  rfile  ;
 SMBCFILE * file  =
 (smbc_getFunctionCreat(ctx))(ctx, url, mode);
  rfile = Data_Wrap_Struct(cSmbFile, NULL, NULL, file);
  do { __p_retval = rfile; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
CTX_file_open(int __p_argc, VALUE *__p_argv, VALUE self)
{
  VALUE __p_retval = Qnil;
  VALUE __v_url = Qnil;
  char * url; char * __orig_url;
  VALUE __v_flags = Qnil;
  int flags; int __orig_flags;
  VALUE __v_mode = Qnil;
  int mode; int __orig_mode;
SMBCCTX *ctx; Data_Get_Struct(self, SMBCCTX, ctx);

  /* Scan arguments */
  rb_scan_args(__p_argc, __p_argv, "12",&__v_url, &__v_flags, &__v_mode);

  /* Set defaults */
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );

  if (__p_argc > 1)
    __orig_flags = flags = NUM2INT(__v_flags);
  else
    flags = O_RDONLY;

  if (__p_argc > 2)
    __orig_mode = mode = NUM2INT(__v_mode);
  else
    mode = 0666;


#line 112 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  volatile VALUE  rfile  ;
 SMBCFILE * file  =
 ctx->open(ctx, url, flags, mode);
  rfile = Data_Wrap_Struct(cSmbFile, NULL, NULL, file);
  do { __p_retval = rfile; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
CTX_file_write(VALUE self, VALUE sfile, VALUE data)
{
  VALUE __p_retval = Qnil;
SMBCCTX *ctx; Data_Get_Struct(self, SMBCCTX, ctx);
  Check_Type(sfile, T_DATA);
  Check_Type(data, T_STRING);

#line 118 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  SMBCFILE * file  ;
 Data_Get_Struct(sfile, SMBCFILE, file);
  StringValue(data);
  do { __p_retval =  LONG2NUM(ctx->write(ctx, file, RSTRING_PTR(data), RSTRING_LEN(data))); goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
CTX_file_close(VALUE self, VALUE sfile)
{
SMBCCTX *ctx; Data_Get_Struct(self, SMBCCTX, ctx);
  Check_Type(sfile, T_DATA);

#line 123 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  SMBCFILE * file  ;
 Data_Get_Struct(sfile, SMBCFILE, file);
  ctx->close_fn(ctx, file);
 

  } while(0);

  return Qnil;
}

static VALUE
Dir_CLASS_new(VALUE self, VALUE __v_url)
{
  VALUE __p_retval = Qnil;
  char * url; char * __orig_url;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );

#line 145 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  volatile VALUE  val  ;
 struct rb_smbprivate * dir  ;
 int  handle  ;
 if((handle = smbc_opendir(url)) < 0) { rb_sys_fail("Unable to open dir URL");
  } val = Data_Make_Struct(self, struct rb_smbprivate, 0, smbprivate_free, dir);
  dir->handle = handle;
  rb_obj_call_init(val, 0, NULL);
  do { __p_retval = val; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
Dir_CLASS_rmdir(VALUE self, VALUE __v_url)
{
  char * url; char * __orig_url;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );

#line 161 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  if (smbc_rmdir(url) < 0) rb_sys_fail(url);
 
  return Qnil;
}

static VALUE
Dir_CLASS_mkdir(VALUE self, VALUE __v_url, VALUE __v_mode)
{
  char * url; char * __orig_url;
  int mode; int __orig_mode;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );
  __orig_mode = mode = NUM2INT(__v_mode);

#line 165 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  if (smbc_mkdir(url, mode) < 0) rb_sys_fail(url);
 
  return Qnil;
}

static VALUE
Dir_initialize(VALUE self)
{
rb_smbprivate *dir; Data_Get_Struct(self, rb_smbprivate, dir);

#line 172 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  
  return Qnil;
}

static VALUE
Dir_read_entry(VALUE self)
{
  VALUE __p_retval = Qnil;
rb_smbprivate *dir; Data_Get_Struct(self, rb_smbprivate, dir);

#line 174 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  struct smbc_dirent*  de  ;
 de = smbc_readdir(dir->handle);
  if(de) { do { __p_retval = rb_struct_new(structDirEntry,  INT2NUM(de->smbc_type),  rb_str_new2(de->comment),  rb_str_new2(de->name)); goto out; } while(0); }

  } while(0);

out:
  return __p_retval;
}

static VALUE
Dir_read(VALUE self)
{
  VALUE __p_retval = Qnil;
rb_smbprivate *dir; Data_Get_Struct(self, rb_smbprivate, dir);

#line 185 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  struct smbc_dirent*  de  ;
 de = smbc_readdir(dir->handle);
  if(de) do { __p_retval =  rb_str_new2(de->name); goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
Dir_tell(VALUE self)
{
  VALUE __p_retval = Qnil;
rb_smbprivate *dir; Data_Get_Struct(self, rb_smbprivate, dir);

#line 191 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  do { __p_retval =  INT2NUM(smbc_telldir(dir->handle)); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Dir_url(VALUE self)
{
  VALUE __p_retval = Qnil;
rb_smbprivate *dir; Data_Get_Struct(self, rb_smbprivate, dir);

#line 194 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  do { __p_retval =  rb_str_new2(dir->url); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Dir_close(VALUE self)
{
rb_smbprivate *dir; Data_Get_Struct(self, rb_smbprivate, dir);

#line 197 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  smbc_closedir(dir->handle);
  dir->handle = 0;
 
  return Qnil;
}

static VALUE
File_CLASS_new(VALUE self, VALUE __v_url, VALUE __v_flags, VALUE __v_mode)
{
  VALUE __p_retval = Qnil;
  char * url; char * __orig_url;
  int flags; int __orig_flags;
  int mode; int __orig_mode;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );
  __orig_flags = flags = NUM2INT(__v_flags);
  __orig_mode = mode = NUM2INT(__v_mode);

#line 204 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  volatile VALUE  val  ;
 struct rb_smbprivate * file  ;
 int  handle  =
 0;
  if ((handle = smbc_open(url, flags, mode)) < 0) { rb_sys_fail(url);
  } val = Data_Make_Struct(self, struct rb_smbprivate, 0, smbprivate_free, file);
  file->handle = handle;
  file->url = strdup(url);
  rb_obj_call_init(val, 0, NULL);
  do { __p_retval = val; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
File_CLASS_create(VALUE self, VALUE __v_url, VALUE __v_mode)
{
  VALUE __p_retval = Qnil;
  char * url; char * __orig_url;
  int mode; int __orig_mode;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );
  __orig_mode = mode = NUM2INT(__v_mode);

#line 222 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  volatile VALUE  val  ;
 struct rb_smbprivate * file  ;
 int  handle  =
 0;
  if ((handle = smbc_creat(url, mode)) < 0) { rb_sys_fail(url);
  } val = Data_Make_Struct(self, struct rb_smbprivate, 0, smbprivate_free, file);
  file->handle = handle;
  file->url = strdup(url);
  rb_obj_call_init(val, 0, NULL);
  do { __p_retval = val; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
File_CLASS_unlink(VALUE self, VALUE __v_url)
{
  char * url; char * __orig_url;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );

#line 240 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  if (smbc_unlink(url) < 0) rb_sys_fail(url);
 
  return Qnil;
}

static VALUE
File_CLASS_rename(VALUE self, VALUE __v_ourl, VALUE __v_nurl)
{
  char * ourl; char * __orig_ourl;
  char * nurl; char * __orig_nurl;
  __orig_ourl = ourl = ( NIL_P(__v_ourl) ? NULL : StringValuePtr(__v_ourl) );
  __orig_nurl = nurl = ( NIL_P(__v_nurl) ? NULL : StringValuePtr(__v_nurl) );

#line 244 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  if (smbc_rename(ourl, nurl) < 0) rb_sys_fail(ourl);
 
  return Qnil;
}

static VALUE
File_CLASS_stat(VALUE self, VALUE __v_url)
{
  VALUE __p_retval = Qnil;
  char * url; char * __orig_url;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );

#line 248 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  struct stat  s  ;
 if (smbc_stat(url, &s) < 0) rb_sys_fail(url);
  do { __p_retval = stat_new(&s); goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
File_initialize(VALUE self)
{
rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);

#line 257 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  
  return Qnil;
}

static VALUE
File_stat(VALUE self)
{
  VALUE __p_retval = Qnil;
rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);

#line 259 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  struct stat  s  ;
 if (smbc_fstat(file->handle, &s) < 0) rb_sys_fail(file->url);
  do { __p_retval = stat_new(&s); goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
File_url(VALUE self)
{
  VALUE __p_retval = Qnil;
rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);

#line 265 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  do { __p_retval =  rb_str_new2(file->url); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
File_seek(VALUE self, VALUE __v_offset, VALUE __v_whence)
{
  VALUE __p_retval = Qnil;
  off_t offset; off_t __orig_offset;
  int whence; int __orig_whence;
rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);
  __orig_offset = offset = NUM2OFFT(__v_offset);
  __orig_whence = whence = NUM2INT(__v_whence);

#line 268 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  do { __p_retval = INT2NUM(smbc_lseek(file->handle, offset, whence)); goto out; } while(0);
out:
;
  return __p_retval;
}

static VALUE
File_read(int __p_argc, VALUE *__p_argv, VALUE self)
{
  VALUE __p_retval = Qnil;
  VALUE __v_bytes_to_read = Qnil;
  int bytes_to_read; int __orig_bytes_to_read;
rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);

  /* Scan arguments */
  rb_scan_args(__p_argc, __p_argv, "01",&__v_bytes_to_read);

  /* Set defaults */
  if (__p_argc > 0)
    __orig_bytes_to_read = bytes_to_read = NUM2INT(__v_bytes_to_read);
  else
    bytes_to_read = -1;


#line 271 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  size_t  read  ;
 struct stat  s  ;
 char  buf [4096] ;
 volatile VALUE  str  =
 Qnil;
  int next_read = 0;
  if(bytes_to_read == -1) { if (smbc_fstat(file->handle, &s) < 0) rb_sys_fail(file->url);
  bytes_to_read = s.st_size;
  } if(bytes_to_read > sizeof(buf)) next_read = sizeof(buf);
  else next_read = bytes_to_read;
  while((read = smbc_read(file->handle, buf, next_read))!=0) { if(str == Qnil) str = rb_str_new("",0);
  rb_str_cat(str, buf, read);
  bytes_to_read -= read;
  if(bytes_to_read == 0) break;
  if(bytes_to_read > sizeof(buf)) next_read = sizeof(buf);
  else next_read = bytes_to_read;
  } do { __p_retval = str; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
File_write(VALUE self, VALUE buf)
{
  VALUE __p_retval = Qnil;
rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);
  Check_Type(buf, T_STRING);

#line 308 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  int  i  ;
 StringValue(buf);
  i = smbc_write(file->handle, RSTRING_PTR(buf), RSTRING_LEN(buf));
  if (i < 0) rb_sys_fail("write");
  do { __p_retval =  LONG2NUM(i); goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static VALUE
File_close(VALUE self)
{
rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);

#line 316 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"
  smbc_close(file->handle);
  file->handle = 0;
 
  return Qnil;
}

static VALUE
File_CLASS_read(VALUE self, VALUE __v_url)
{
  VALUE __p_retval = Qnil;
  char * url; char * __orig_url;
  __orig_url = url = ( NIL_P(__v_url) ? NULL : StringValuePtr(__v_url) );

#line 321 "/home/geoff/Projects/smb-ruby/ext/smb/smb.cr"

  do {
  size_t  read  ;
 struct stat  s  ;
 char  buf [4096] ;
 volatile VALUE  str  ;
 int next_read = 0, handle, bytes_to_read;
  if ((handle = smbc_open(url, O_RDONLY, 0)) < 0) rb_sys_fail(url);
  if (smbc_fstat(handle, &s) < 0) rb_sys_fail(url);
  bytes_to_read = s.st_size;
  if(bytes_to_read > sizeof(buf)) next_read = sizeof(buf);
  else next_read = bytes_to_read;
  str = rb_str_new("",0);
  while((read = smbc_read(handle, buf, next_read))!=0) { rb_str_cat(str, buf, read);
  bytes_to_read -= read;
  if(bytes_to_read == 0) break;
  if(bytes_to_read > sizeof(buf)) next_read = sizeof(buf);
  else next_read = bytes_to_read;
  } smbc_close(handle);
  do { __p_retval = str; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

/* Init */
void
Init_smb(void)
{

/*	rb_cStat = rb_eval_string("File::Stat");*/
rb_gc_register_address(&_gcpool_Credentials);
  mSMB = rb_define_module("SMB");
  rb_define_singleton_method(mSMB, "init", SMB_CLASS_init, -1);
  enumBaseClass = rb_define_class("Enum", rb_cObject);
    rb_define_method(enumBaseClass, "inspect", rubber_enum_inspect, 0);
    rb_define_method(enumBaseClass, "to_i", rubber_enum_to_i, 0);
    rb_define_method(enumBaseClass, "coerce", rubber_enum_coerce, 1);
    rb_define_method(enumBaseClass, "to_s", rubber_enum_to_s, 0);
    rb_define_method(enumBaseClass, "to_str", rubber_enum_to_s, 0);
    rb_define_method(enumBaseClass, "fullname", rubber_enum_to_s, 0);
    rb_define_method(enumBaseClass, "name", rubber_enum_name, 0);
    rb_define_method(enumBaseClass, "<=>", rubber_enum_cmp, 0);
    
  enumType = rb_define_class_under(mSMB, "Type", enumBaseClass);
    enumType_SMBC_WORKGROUP = make_enum_value(enumType, SMBC_WORKGROUP, "workgroup", "SMBC_WORKGROUP");
    rb_obj_freeze(enumType_SMBC_WORKGROUP);
    rb_define_const(enumType, "WORKGROUP", enumType_SMBC_WORKGROUP);
    enumType_SMBC_SERVER = make_enum_value(enumType, SMBC_SERVER, "server", "SMBC_SERVER");
    rb_obj_freeze(enumType_SMBC_SERVER);
    rb_define_const(enumType, "SERVER", enumType_SMBC_SERVER);
    enumType_SMBC_FILE_SHARE = make_enum_value(enumType, SMBC_FILE_SHARE, "file-share", "SMBC_FILE_SHARE");
    rb_obj_freeze(enumType_SMBC_FILE_SHARE);
    rb_define_const(enumType, "FILE_SHARE", enumType_SMBC_FILE_SHARE);
    enumType_SMBC_PRINTER_SHARE = make_enum_value(enumType, SMBC_PRINTER_SHARE, "printer-share", "SMBC_PRINTER_SHARE");
    rb_obj_freeze(enumType_SMBC_PRINTER_SHARE);
    rb_define_const(enumType, "PRINTER_SHARE", enumType_SMBC_PRINTER_SHARE);
    enumType_SMBC_COMMS_SHARE = make_enum_value(enumType, SMBC_COMMS_SHARE, "comms-share", "SMBC_COMMS_SHARE");
    rb_obj_freeze(enumType_SMBC_COMMS_SHARE);
    rb_define_const(enumType, "COMMS_SHARE", enumType_SMBC_COMMS_SHARE);
    enumType_SMBC_IPC_SHARE = make_enum_value(enumType, SMBC_IPC_SHARE, "ipc-share", "SMBC_IPC_SHARE");
    rb_obj_freeze(enumType_SMBC_IPC_SHARE);
    rb_define_const(enumType, "IPC_SHARE", enumType_SMBC_IPC_SHARE);
    enumType_SMBC_DIR = make_enum_value(enumType, SMBC_DIR, "dir", "SMBC_DIR");
    rb_obj_freeze(enumType_SMBC_DIR);
    rb_define_const(enumType, "DIR", enumType_SMBC_DIR);
    enumType_SMBC_FILE = make_enum_value(enumType, SMBC_FILE, "file", "SMBC_FILE");
    rb_obj_freeze(enumType_SMBC_FILE);
    rb_define_const(enumType, "FILE", enumType_SMBC_FILE);
    enumType_SMBC_LINK = make_enum_value(enumType, SMBC_LINK, "link", "SMBC_LINK");
    rb_obj_freeze(enumType_SMBC_LINK);
    rb_define_const(enumType, "LINK", enumType_SMBC_LINK);
  cCTX = rb_define_class_under(mSMB, "CTX", rb_cObject);
  rb_define_singleton_method(cCTX, "default", CTX_CLASS_default, 0);
 rb_define_alloc_func(cCTX, CTX_CLASS___alloc__);
  rb_define_method(cCTX, "initialize", CTX_initialize, 0);
  rb_define_method(cCTX, "file_creat", CTX_file_creat, -1);
  rb_define_method(cCTX, "file_open", CTX_file_open, -1);
  rb_define_method(cCTX, "file_write", CTX_file_write, 2);
  rb_define_method(cCTX, "file_close", CTX_file_close, 1);
  cSmbFile = rb_define_class_under(cCTX, "SmbFile", rb_cObject);
  structDirEntry = rb_struct_define("DirEntry", "type", "comment", "name", NULL);
    rb_define_const(mSMB, "DirEntry", structDirEntry);
  cDir = rb_define_class_under(mSMB, "Dir", rb_cObject);
  rb_define_singleton_method(cDir, "new", Dir_CLASS_new, 1);
  rb_define_singleton_method(cDir, "rmdir", Dir_CLASS_rmdir, 1);
  rb_define_singleton_method(cDir, "mkdir", Dir_CLASS_mkdir, 2);
  rb_define_method(cDir, "initialize", Dir_initialize, 0);
  rb_define_method(cDir, "read_entry", Dir_read_entry, 0);
  rb_define_method(cDir, "read", Dir_read, 0);
  rb_define_method(cDir, "tell", Dir_tell, 0);
  rb_define_method(cDir, "url", Dir_url, 0);
  rb_define_method(cDir, "close", Dir_close, 0);
  cFile = rb_define_class_under(mSMB, "File", rb_cObject);
  rb_define_singleton_method(cFile, "new", File_CLASS_new, 3);
  rb_define_singleton_method(cFile, "create", File_CLASS_create, 2);
  rb_define_singleton_method(cFile, "unlink", File_CLASS_unlink, 1);
  rb_define_singleton_method(cFile, "rename", File_CLASS_rename, 2);
  rb_define_singleton_method(cFile, "stat", File_CLASS_stat, 1);
  rb_define_method(cFile, "initialize", File_initialize, 0);
  rb_define_method(cFile, "stat", File_stat, 0);
  rb_define_method(cFile, "url", File_url, 0);
  rb_define_method(cFile, "seek", File_seek, 2);
  rb_define_method(cFile, "read", File_read, -1);
  rb_define_method(cFile, "write", File_write, 1);
  rb_define_method(cFile, "close", File_close, 0);
  rb_define_singleton_method(cFile, "read", File_CLASS_read, 1);
}
