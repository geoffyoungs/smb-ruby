require 'smb'
require 'uri'
require 'io/nonblock'

module SMB
class Dir
	include Enumerable
	def each(&block)
		while entry = read()
			yield(entry)
		end
	end
	def self.entries(url)
		list = []
		d = SMB::Dir.new(url)
		begin
			while entry = d.read
				list << entry
			end
		ensure
			d.close
		end
		list
	end
	def self.foreach(url)
		d = SMB::Dir.new(url)
		begin
			while entry = d.read
				yield(entry)
			end
		ensure
			d.close
		end
	end
end
end

#module Ambit
module VFS
	SCHEMES = {
		'smb'=> {
			:dir_new => lambda { |uri| SMB::Dir.new(uri.to_s) },
			:file_new => lambda { |uri,flags,mode| SMB::File.new(uri.to_s,flags,mode) },
			:dir_entries => lambda { |uri| SMB::Dir.entries(uri.to_s) },
			:dir_foreach => lambda { |uri,block| SMB::Dir.foreach(uri.to_s, &block) },
			:mkdir => lambda { |uri,mode| SMB::Dir.mkdir(uri.to_s,mode) },
			:rmdir => lambda { |uri| SMB::Dir.rmdir(uri.to_s) },
			:stat => lambda { |uri| SMB::File.stat(uri.to_s) },
			:unlink => lambda { |uri| SMB::File.unlink(uri.to_s) },
			:file_read => lambda { |uri| SMB::File.read(uri.to_s) },
			
			
			:init => lambda { |auth| SMB::init { |srv,shr,wrk,uid,pwd| auth.call(URI.parse("smb://#{srv}/#{shr}"),{:server=>srv, :share=>shr, :workgroup=>wrk, :username=>uid, :password=>pwd}) } },
		},
		'file'=> {
			:dir_new => lambda { |uri| ::Dir.new(unescape(uri.path)) },
			:file_new => lambda { |uri,flags,mode| ::File.new(unescape(uri.path),flags,mode) },
			:dir_entries => lambda { |uri| ::Dir.entries(unescape(uri.path)) },
			:dir_foreach => lambda { |uri,block| ::Dir.foreach(unescape(uri.path), &block) },
			:mkdir => lambda { |uri,mode| ::Dir.mkdir(unescape(uri.path),mode) },
			:rmdir => lambda { |uri| ::Dir.rmdir(unescape(uri.path)) },
			:stat => lambda { |uri| ::File.stat(unescape(uri.path)) },
			:unlink => lambda { |uri| ::File.unlink(unescape(uri.path)) },
			:file_read => lambda { |uri| ::File.read(unescape(uri.path)) },
		}
	}
	def self.init(&auth_callback)
		SCHEMES.each do |key,val|
			if val[:init]
				val[:init].call(auth_callback)
			end
		end
	end
	def self.escape(str)
		str.gsub(/([^-A-Za-z0-9.\/])/) {|i| sprintf("%%%02X",i[0]) }
	end
	def self.unescape(str)
		str.gsub(/(\%[A-Fa-f0-9]{2})/) {|i| d=" "; d[0]=Integer("0x"+i[1..-1]); d }
	end
	def self.path2uri(path)
		"file://" + escape(if path[0] == ?/
			path
		else
			::File::expand_path(path)
		end)
	end
	def self.io_copy(in_fp, out_fp, block_size = (4096 * 16))
		rfp, wfp = IO::pipe

		reader = fork {
			rfp.close()
			while str = in_fp.read(block_size)
				wfp.write(str)
				break if in_fp.respond_to?(:eof?) and in_fp.eof?
			end
			wfp.close
			exit(1)
		}
		writer = fork {
			wfp.close()
			while str = rfp.read(block_size)
				out_fp.write(str)
				break if rfp.closed? or rfp.eof? or str.empty?
			end
			rfp.close
			exit(1)
		}
		wfp.close()
		rfp.close()

		ar = aw = nil
		until ar and aw
			ar = Process::waitpid2(reader, Process::WNOHANG) unless ar
			aw = Process::waitpid2(writer, Process::WNOHANG) unless aw
		end
	end
	
	module Dir
		def self.open(uri)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:dir_new][uri]
		end
		def self.entries(uri)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:dir_entries][uri]
		end
		def self.mkdir(uri,mode=0777)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:mkdir][uri,mode]
		end
		def self.rmdir(uri)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:rmdir][uri]
		end
		def self.foreach(uri, &block)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:dir_foreach][uri,block]
		end
		def self.copy(from_uri, to_uri)
			File::mkpath(to_uri)
			Dir.foreach(from_uri) do |file|
				next if file[0] == ?.
				s = File::stat(furi = File::join(from_uri, VFS::escape(file)))
				if s.directory?
					Dir::copy(furi, File::join(to_uri, VFS::escape(file)))
				elsif s.file?
					File::copy(furi, File::join(to_uri, VFS::escape(file)))
				end
			end
		end
	end
	module FileTest
		def self.exists?(uri)
			begin
				File::stat(uri)
				true
			rescue Errno::ENOENT, Errno::EINVAL
				false
			end
		end
		def self.directory?(uri)
			File::stat(uri).directory?
		end
	end
	module File
		def self.open(uri, flags=nil, mode=nil)
			uri = URI.parse(uri)
			mode ||= 0644
			unless flags.kind_of?(Integer)
				flags = {
					'w' => ::File::WRONLY|::File::CREAT|::File::TRUNC,
					'r' => ::File::RDONLY,
					'a' => ::File::RDWR|::File::CREAT|::File::APPEND
				}[flags && flags[0...1]] || ::File::RDONLY
			end
			SCHEMES[uri.scheme][:file_new][uri, flags, mode]
		end
		
		def self.copy(in_uri,out_uri)
			in_f, out_f = File.open(in_uri), File.open(out_uri,'w')
			VFS::io_copy(in_f, out_f)
		end
		def self.mkpath(uri)
			if FileTest.exists?(uri)
				false
			else
				dir_uri, base = File::split(uri)
				mkpath(dir_uri)
				Dir::mkdir(uri,0777)
				true
			end
		end
		def self.stat(uri)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:stat][uri]
		end
		def self.unlink(uri)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:unlinks][uri]
		end
		def self.read(uri)
			uri = URI.parse(uri)
			SCHEMES[uri.scheme][:file_read][uri]
		end
		def self.join(*args)
			::File::join(*args)
		end
		def self.split(*args)
			::File::split(*args)
		end
	end
end
#end


