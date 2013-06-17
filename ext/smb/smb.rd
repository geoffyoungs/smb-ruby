= module SMB
--- SMB.init(Integer debug, block)


== enum SMB::Type
== class SMB::CTX
--- SMB::CTX.default()


--- SMB::CTX__alloc__()


--- SMB::CTX.new

--- SMB::CTX#file_creat(String url, Integer mode)


--- SMB::CTX#file_open(String url, Integer flags, Integer mode)


--- SMB::CTX#file_write(Data sfile, String data)


--- SMB::CTX#file_close(Data sfile)


=== class SMB::CTX::SmbFile
== enum SMB::DirEntry
== class SMB::Dir
--- SMB::Dir.new(String url)


--- SMB::Dir.rmdir(String url)


--- SMB::Dir.mkdir(String url, Integer mode)


--- SMB::Dir.new

--- SMB::Dir#read_entry

--- SMB::Dir#read

--- SMB::Dir#tell

--- SMB::Dir#url

--- SMB::Dir#close

== class SMB::Stat
--- SMB::Stat#exist?

--- SMB::Stat#dev

--- SMB::Stat#ino

--- SMB::Stat#mode

--- SMB::Stat#nlink

--- SMB::Stat#uid

--- SMB::Stat#gid

--- SMB::Stat#rdev

--- SMB::Stat#blksize

--- SMB::Stat#blocks

--- SMB::Stat#size

--- SMB::Stat#directory?

--- SMB::Stat#file?

--- SMB::Stat#regular?

--- SMB::Stat#symlink?

--- SMB::Stat#fifo?

== class SMB::File
--- SMB::File.new(String url, Integer flags, Integer mode)


--- SMB::File.create(String url, Integer mode)


--- SMB::File.unlink(String url)


--- SMB::File.rename(String ourl, String nurl)


--- SMB::File.stat(String url)


--- SMB::File.new

--- SMB::File#stat

--- SMB::File#url

--- SMB::File#seek(off_t offset, Integer whence)


--- SMB::File#read(Integer bytes_to_read)


--- SMB::File#write(String buf)


--- SMB::File#close

--- SMB::File.read(String url)


