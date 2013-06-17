%name smb

%include libsmbclient.h
%include ruby.h
%lib smbclient

%{

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
    return Data_Wrap_Struct(cStat, NULL, free, nst);
}

%}

%pre_init{
/*	rb_cStat = rb_eval_string("File::Stat");*/
%}

%map ruby - (char *) => (VALUE): rb_str_new2(#0)
%map string - (VALUE) => (char *): StringValuePtr(%%)
%map VALUE > off_t : NUM2OFFT(%%)

%option glib=no

gcpool Credentials

module SMB
	enum Type (SMBC_WORKGROUP, SMBC_SERVER, SMBC_FILE_SHARE, SMBC_PRINTER_SHARE, SMBC_COMMS_SHARE, SMBC_IPC_SHARE, SMBC_DIR, SMBC_FILE, SMBC_LINK)
	class CTX
		pre_func SMBCCTX *ctx; Data_Get_Struct(self, SMBCCTX, ctx);
		def self.default
			return Data_Wrap_Struct(self, NULL, smbc_free_context, smbc_set_context(NULL));
		end
		def self.__alloc__
			SMBCCTX *ctx = smbc_new_context();
			volatile VALUE obj;
			obj = Data_Wrap_Struct(self, NULL, smbc_free_context, ctx);
			return obj;
		end
		def initialize()
			smbc_init_context(ctx);
		end
		class SmbFile
			
		end
		def file_creat(char *url, int mode = 0666)
			volatile VALUE rfile;
			SMBCFILE *file = (smbc_getFunctionCreat(ctx))(ctx, url, mode);
			rfile = Data_Wrap_Struct(cSmbFile, NULL, NULL, file);
			return rfile;
		end
		def file_open(char *url, int flags = O_RDONLY, int mode = 0666)
			volatile VALUE rfile;
			SMBCFILE *file = ctx->open(ctx, url, flags, mode);
			rfile = Data_Wrap_Struct(cSmbFile, NULL, NULL, file);
			return rfile;
		end
		def long:file_write(T_DATA sfile, T_STRING data)
			SMBCFILE *file; Data_Get_Struct(sfile, SMBCFILE, file);
			StringValue(data);
			return ctx->write(ctx, file, RSTRING_PTR(data), RSTRING_LEN(data));
		end
		def file_close(T_DATA sfile)
			SMBCFILE *file; Data_Get_Struct(sfile, SMBCFILE, file);
			ctx->close_fn(ctx, file);
		end
	end
	
	
	def self.init(int debug = 0,&block)
		auth_block = block;
		
		rb_gc_register_address(&auth_block);
		
		if (smbc_init(smbc_get_auth_data, debug) < 0)
		{
			rb_sys_fail("smbc_init failed");
		}
	end
	
	
	struct DirEntry (type,comment,name);
	
	class Dir
		def self.new(char *url)
			volatile VALUE val;
			struct rb_smbprivate *dir;
			int handle;
			
			if((handle = smbc_opendir(url)) < 0)
			{
				rb_sys_fail("Unable to open dir URL");
			}
			
			val = Data_Make_Struct(self, struct rb_smbprivate, 0, smbprivate_free, dir);
			dir->handle = handle;

			rb_obj_call_init(val, 0, NULL);
			return val;
		end
		def self.rmdir(char *url)
			if (smbc_rmdir(url) < 0)
				rb_sys_fail(url);
		end
		def self.mkdir(char *url, int mode)
			if (smbc_mkdir(url, mode) < 0)
				rb_sys_fail(url);
		end

		pre_func rb_smbprivate *dir; Data_Get_Struct(self, rb_smbprivate, dir);

		def initialize()
		end
		def read_entry
			 struct smbc_dirent* de;
			 de = smbc_readdir(dir->handle);
			 if(de)
			 {
			 	return rb_struct_new(structDirEntry, 
					<{int>VALUE:de->smbc_type}>,
					<{char*>VALUE:de->comment}>,
					<{char*>VALUE:de->name}>);
			 }
		end
		def char*:read
			 struct smbc_dirent* de;
			 de = smbc_readdir(dir->handle);
			 if(de)
			 	return de->name;
		end
		def int:tell()
			return smbc_telldir(dir->handle);
		end
		def char *:url
			return dir->url;
		end
		def close()
			smbc_closedir(dir->handle);
			dir->handle = 0;
		end
	end

  class Stat
  pre_func struct stat *s; Data_Get_Struct(self, struct stat, s);
    def bool:exist?
      return (!!s);
    end
    def long:dev
      return s->st_dev;
    end
    def long:ino
      return s->st_ino;
    end
    def long:mode
      return s->st_mode;
    end
    def long:nlink
      return s->st_nlink;
    end

    def long:uid
      return s->st_uid;
    end
    def long:gid
      return s->st_gid;
    end
    def long:rdev
       return s->st_rdev;
    end

    def long:blksize
       return s->st_blksize;
    end
    def long:blocks
       return s->st_blocks;
    end

    def long:size
      return s->st_size;
    end

    def bool:directory?
      return S_ISDIR(s->st_mode);
    end
    def bool:file?
      return S_ISREG(s->st_mode);
    end
    def bool:regular?
      return S_ISREG(s->st_mode);
    end
    def bool:symlink?
      return S_ISLNK(s->st_mode);
    end
    def bool:fifo?
      return S_ISFIFO(s->st_mode);
    end
  
    def bool:zero?
      return (s->st_size == 0);
    end
  end
	
	class File
		def self.new(char *url, int flags, int mode)
			volatile VALUE val;
			struct rb_smbprivate *file;
			int handle = 0;
			
			if ((handle = smbc_open(url, flags, mode)) < 0)
			{
				rb_sys_fail(url);
			}
			
			
			val =  Data_Make_Struct(self, struct rb_smbprivate, 0, smbprivate_free, file);
			file->handle = handle;
			file->url = strdup(url);
			
			rb_obj_call_init(val, 0, NULL);
			return val;
		end
		def self.create(char *url, int mode)
			volatile VALUE val;
			struct rb_smbprivate *file;
			int handle = 0;
			
			if ((handle = smbc_creat(url, mode)) < 0)
			{
				rb_sys_fail(url);
			}
			
			
			val =  Data_Make_Struct(self, struct rb_smbprivate, 0, smbprivate_free, file);
			file->handle = handle;
			file->url = strdup(url);
			
			rb_obj_call_init(val, 0, NULL);
			return val;
		end
		def self.unlink(char *url)
			if (smbc_unlink(url) < 0)
				rb_sys_fail(url);
		end
		def self.rename(char *ourl, char *nurl)
			if (smbc_rename(ourl, nurl) < 0)
				rb_sys_fail(ourl);
		end
		def self.stat(char *url)
			struct stat s;
			if (smbc_stat(url, &s) < 0)
				rb_sys_fail(url);
			return stat_new(&s);
		end
		
		pre_func rb_smbprivate *file; Data_Get_Struct(self, rb_smbprivate, file);

		def initialize()
		end
		def stat()
			struct stat s;
			if (smbc_fstat(file->handle, &s) < 0)
				rb_sys_fail(file->url);
			return stat_new(&s);
		end
		def char *:url
			return file->url;
		end
		def seek(off_t offset, int whence)
			return INT2NUM(smbc_lseek(file->handle, offset, whence));
		end
		def read(int bytes_to_read=-1)
			size_t read;
			struct stat s;
			char buf[4096];
			volatile VALUE str = Qnil;
			int next_read = 0;
			 
			if(bytes_to_read == -1)
			{
				if (smbc_fstat(file->handle, &s) < 0)
					rb_sys_fail(file->url);
			
				bytes_to_read = s.st_size;
			}
			
			if(bytes_to_read > sizeof(buf))
				next_read = sizeof(buf);
			else
				next_read = bytes_to_read;
			 
			while((read = smbc_read(file->handle, buf, next_read))!=0)
			{
				if(str == Qnil)
					str = rb_str_new("",0);
				rb_str_cat(str, buf, read);
				bytes_to_read -= read;
				if(bytes_to_read == 0)
					break;
			
				if(bytes_to_read > sizeof(buf))
					next_read = sizeof(buf);
				else
					next_read = bytes_to_read;
			}
			 
			 return str;
		end
		def long:write(T_STRING buf)
			int i;
			StringValue(buf);
			i = smbc_write(file->handle, RSTRING_PTR(buf), RSTRING_LEN(buf));
			if (i < 0)
				rb_sys_fail("write");
			return i;
		end
		def close()
			smbc_close(file->handle);
			file->handle = 0;
		end
		
		def self.read(char *url)
			size_t read;
			struct stat s;
			char buf[4096];
			volatile VALUE str;
			int next_read = 0, handle, bytes_to_read;
			 
			if ((handle = smbc_open(url, O_RDONLY, 0)) < 0)
				rb_sys_fail(url);
			
			if (smbc_fstat(handle, &s) < 0)
				rb_sys_fail(url);
			bytes_to_read = s.st_size;
			
			if(bytes_to_read > sizeof(buf))
			 	next_read = sizeof(buf);
			else
				next_read = bytes_to_read;
			 
			str = rb_str_new("",0);
			
			while((read = smbc_read(handle, buf, next_read))!=0)
			{
				rb_str_cat(str, buf, read);
				bytes_to_read -= read;
				if(bytes_to_read == 0)
					break;
				
				if(bytes_to_read > sizeof(buf))
					next_read = sizeof(buf);
				else
					next_read = bytes_to_read;
			}
               
			smbc_close(handle);			
			
			return str;
		end
	end
end
